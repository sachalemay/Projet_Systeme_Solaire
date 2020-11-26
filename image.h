#ifndef SIE_IMAGE
#define SIE_IMAGE

struct Pixel {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

struct Image {
	int width;
	int height;
	struct Pixel * pixels;
};

void image_create(struct Image * image, int width, int height);
const char * image_read(struct Image * image, const char * pngFileName);
const char * image_write_to_png(struct Image * image, const char * pngFileName);
const char * image_write_to_jpeg(struct Image * image, const char * jpegFileName, int quality);
void image_free(struct Image * image);

#endif
