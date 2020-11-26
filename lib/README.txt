These files were compiled using MinGW** from the following sources:
	zlib-1.2.11
	libpng-1.6.37
	libjpeg-9c

zlib was compiled as follows:
	make -fwin32/Makefile.gcc

libpng was compiled as follows:
	sh -c 'CPPFLAGS="-I../zlib-1.2.11" LDFLAGS="-L../zlib-1.2.11" configure'
	make

libjpeg was compiled as follows:
(a) Configure the package:
	sh configure
(b) Modify jconfig.h as follows:
	-  #undef HAVE_PROTOTYPES
	+  #define HAVE_PROTOTYPES 1
	-  #undef HAVE_STDLIB_H
	+  #define HAVE_STDLIB_H 1
(c) Compile:
	make

Finally, all *.a, *.la, *.dll, *.dll.a were copied into the "libs" folder,
and all relevant "*.h* files into the "include" folder.

---
2019-11

** I noticed that I also had an old version of MSYS installed, which provided a few necessary things like "sh", which are not in MinGW.

---
2019-11

libexif was compiled using msys32 as follows:
	./configure
	make

Finally, all *.a, *.la, *.dll, *.dll.a were copied into the "libs" folder,
and all relevant "*.h* files into the "include" folder.
