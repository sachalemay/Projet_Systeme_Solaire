# Si ça ne compile pas, vérifiez que libpng et libjpeg sont installées sur votre système.
# Sous Ubuntu, les paquets correspondants s'appellent "libpng12-dev" et "libjpeg-dev".

all: projetV3

projetV3: projetV3.c image.c image.h
	gcc -Wall -static -Iinclude -Llib -std=gnu11 projetV3.c image.c -ljpeg -lpng16 -lz -lm -o projetV3


