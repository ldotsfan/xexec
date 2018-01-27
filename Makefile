
.DUMMY: run gdb

all:
	gcc -m32 -g -Wall -Wextra \
		-Wno-unused-variable -Wno-unused-but-set-variable -Wno-pointer-to-int-cast \
		-I/usr/local/include/wine/windows \
		loader.c -o loader \
		-pthread -lX11 -lGL -lGLEW

#all:
#	gcc -m32 -g -Wall -Wextra \
#		-I/usr/include/wine/windows \
#		-Wl,-Ttext-segment=0xe0000000 \
#		-Wl,--section-start,.xboxkrnl_flash=0xfffc0000 \
#		loader.c -o loader

run:
	gdb --args ./loader $(xbe)

gdb:
	gdb --args ./loader $(xbe) 1
