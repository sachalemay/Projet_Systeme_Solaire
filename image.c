#include "image.h"
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#define PNG_DEBUG 3
#include "png.h"
#include "jpeglib.h"

void image_create(struct Image * image, int width, int height) {
	image->width = width;
	image->height = height;
	image->pixels = calloc(width * height, sizeof(struct Pixel));
}

const char * image_read_from_png_after_header(struct Image * image, FILE * file) {
	// Initialize stuff
	png_structp png_ptr;
	png_infop info_ptr;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) return "png_create_read_struct failed";

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) return "png_create_info_struct failed";

	if (setjmp(png_jmpbuf(png_ptr))) return "error during init_io";

	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, 8);

	// Read the header and verify it
	png_read_info(png_ptr, info_ptr);
	image->width = png_get_image_width(png_ptr, info_ptr);
	image->height = png_get_image_height(png_ptr, info_ptr);
	int color_type = png_get_color_type(png_ptr, info_ptr);
	int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	if (image->width > 4000 || image->height > 4000 || color_type != PNG_COLOR_TYPE_RGB_ALPHA || bit_depth != 8) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return "file too big (w > 4000 or h > 4000), or wrong format (try 'convert image.png png32:converted-image.png')";
	}

	// Allocate memory
	int sizeInPixels = image->height * image->width;
	image->pixels = (struct Pixel *) malloc(sizeInPixels * sizeof(struct Pixel));

	// Prepare reading
	png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	if (setjmp(png_jmpbuf(png_ptr))) {
		image_free(image);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return "reading PNG data failed";
	}

	// Read the other lines
	for (int line = 0; line < image->height; line++) {
		unsigned char * lineData = (unsigned char *)&image->pixels[line * image->width];
		png_read_row(png_ptr, lineData, NULL);
	}

	// Free the memory
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return NULL;
}

jmp_buf jpeg_error_jmp_buf;

void jpeg_error_exit(j_common_ptr cinfo) {
	longjmp(jpeg_error_jmp_buf, 1);
}

const char * image_read_from_jpeg(struct Image * image, FILE * file) {
	// Set up error handling
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jerr.error_exit = jpeg_error_exit;

	if (setjmp(jpeg_error_jmp_buf)) {
		jpeg_destroy_decompress(&cinfo);
		if (jerr.msg_code < 1 || jerr.msg_code > jerr.last_jpeg_message) return "unknown JPEG decompression error";
		return jerr.jpeg_message_table[jerr.msg_code];
	}

	// Initialize a JPEG decompression object
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, file);

	// Read the header
	jpeg_read_header(&cinfo, TRUE);

	// Read the image
	jpeg_start_decompress(&cinfo);
	if (cinfo.output_components != 3 || cinfo.out_color_space != JCS_RGB) {
		jpeg_destroy_decompress(&cinfo);
		return "unsupported color space - only RGB is supported";
	}

	image->width = cinfo.output_width;
	image->height = cinfo.output_height;
	int sizeInPixels = image->height * image->width;
	image->pixels = (struct Pixel *) malloc(sizeInPixels * sizeof(struct Pixel));

	int rowStride = cinfo.output_width * cinfo.output_components;
	unsigned char * buffer = (unsigned char *)malloc(rowStride * sizeof(unsigned char));
	while (cinfo.output_scanline < cinfo.output_height) {
		int k = 0;
		int start = cinfo.output_scanline * cinfo.output_width;
		jpeg_read_scanlines(&cinfo, &buffer, 1);
		for (int i = 0; i < cinfo.output_width; i++) {
			image->pixels[start + i].r = buffer[k++];
			image->pixels[start + i].g = buffer[k++];
			image->pixels[start + i].b = buffer[k++];
			image->pixels[start + i].a = 255;
		}
	}

	jpeg_finish_decompress(&cinfo);
	free(buffer);

	// Clean up
	jpeg_destroy_decompress(&cinfo);
	if (jerr.num_warnings) return "corrupt-data warnings occured";

	// Check for warnings
	return NULL;
}

const char * image_read(struct Image * image, const char * fileName) {
	// Open file and test for it being a png
	FILE * file = fopen(fileName, "rb");
	if (!file) return "unable to open file for reading";

	// Read the file
	const char * result = "file format not recognized";
	unsigned char header[8];
	size_t bytesRead = fread(header, 1, 8, file);
	if (bytesRead == 8 && png_sig_cmp(header, 0, 8) == 0) {
		// It's a PNG file
		result = image_read_from_png_after_header(image, file);
	} else {
		// Treat it as JPEG file
		fseek(file, 0L, SEEK_SET);
		result = image_read_from_jpeg(image, file);
	}

	// Close the file and return
	fclose(file);
	return result;
}

const char * image_write_to_png(struct Image * image, const char * pngFileName) {
	// Open PNG
	FILE *pngFile = fopen(pngFileName, "wb");
	if (! pngFile) return "unable to open file for writing";

	// Initialize PNG
	png_structp png_ptr;
	png_infop info_ptr;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) return "png_create_write_struct failed";

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) return "png_create_info_struct failed";

	if (setjmp(png_jmpbuf(png_ptr))) return "error during init_io";
	png_init_io(png_ptr, pngFile);

	// Write header
	if (setjmp(png_jmpbuf(png_ptr))) return "error while writing header";
	png_set_IHDR(png_ptr, info_ptr, image->width, image->height,
	             8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);

	// Write all lines
	if (setjmp(png_jmpbuf(png_ptr))) return "error while writing bytes";
	for (int line = 0; line < image->height; line++) {
		unsigned char * lineData = (unsigned char *)&image->pixels[line * image->width];
		png_write_row(png_ptr, lineData);
	}

	// End write
	if (setjmp(png_jmpbuf(png_ptr))) return "error during end of write";
	png_write_end(png_ptr, NULL);

	// Cleanup
	png_free(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, NULL);
	fclose(pngFile);
	return NULL;
}

const char * image_write_to_jpeg(struct Image * image, const char * jpegFileName, int quality) {
	// Open the destination file
	FILE * outfile = fopen(jpegFileName, "wb");
	if (!outfile) return "unable to open file for writing";

	// Set up error handling
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jerr.error_exit = jpeg_error_exit;

	if (setjmp(jpeg_error_jmp_buf)) {
		fclose(outfile);
		jpeg_destroy_compress(&cinfo);
		if (jerr.msg_code < 1 || jerr.msg_code > jerr.last_jpeg_message) return "unknown JPEG compression error";
		return jerr.jpeg_message_table[jerr.msg_code];
	}

	// Initialize a JPEG compression object
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	// Set parameters for compression
	cinfo.image_width = image->width;
	cinfo.image_height = image->height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	// Configure the compression
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	// Write line by line
	jpeg_start_compress(&cinfo, TRUE);
	int rowStride = cinfo.image_width * cinfo.input_components;
	unsigned char * buffer = (unsigned char *)malloc(rowStride * sizeof(unsigned char));
	while (cinfo.next_scanline < cinfo.image_height) {
		int k = 0;
		int start = cinfo.next_scanline * cinfo.image_width;
		for (int i = 0; i < cinfo.image_width; i++) {
			buffer[k++] = image->pixels[start + i].r;
			buffer[k++] = image->pixels[start + i].g;
			buffer[k++] = image->pixels[start + i].b;
		}
		jpeg_write_scanlines(&cinfo, &buffer, 1);
	}
	jpeg_finish_compress(&cinfo);

	// Close the file and clean up
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
	return NULL;
}

void image_free(struct Image * image) {
	free(image->pixels);
	image->pixels = NULL;
}
