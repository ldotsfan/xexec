
.DUMMY: gdb wait

all:
	gcc -g -m32 -march=i686 -Wall -Wextra \
		-Wno-unused-variable \
		-Wno-unused-but-set-variable \
		-Wno-pointer-to-int-cast \
		loader.c -o xexec \
		-pthread -lX11 -lGL -lGLEW

gdb:
	gdb --args ./xexec -dd $(xbe)

wait:
	gdb --args ./xexec -ddg $(xbe)

