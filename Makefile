
.DUMMY: run

all:
	gcc -m32 -g -Wall -Wextra \
		-Wno-unused-variable -Wno-unused-but-set-variable \
		-I/usr/local/include/wine/windows \
		loader.c -o loader \
		-pthread

#all:
#	gcc -m32 -g -Wall -Wextra \
#		-I/usr/include/wine/windows \
#		-Wl,-Ttext-segment=0xe0000000 \
#		-Wl,--section-start,.xboxkrnl_flash=0xfffc0000 \
#		loader.c -o loader

run:
	gdb --args ./loader $(xbe)

