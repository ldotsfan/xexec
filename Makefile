
.DUMMY: gdb wait

all:
	gcc -m32 -g -Wall -Wextra \
		-Wno-unused-variable -Wno-unused-but-set-variable -Wno-pointer-to-int-cast \
		loader.c -o xexec \
		-pthread -lX11 -lGL -lGLEW

gdb:
	gdb --args ./xexec $(xbe)

wait:
	gdb --args ./xexec $(xbe) 1

