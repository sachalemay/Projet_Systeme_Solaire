# Si ça ne compile pas, vérifiez que libpng et libjpeg sont installées sur votre système.
# Sous Ubuntu, les paquets correspondants s'appellent "libpng12-dev" et "libjpeg-dev".

all: cc

cc: cc.c image.c image.h
	cc -Wall -std=gnu11 -Ilibjpeg -Ilibpng cc.c image.c libjpeg/libjpeg.a libpng/libpng16.a libpng/libz.a -lm -o cc
