SHELL := /bin/bash

.DUMMY: gdb wait

all:
	gcc -g -m32 -march=i686 -Wall -Wextra \
		-DXEXEC_VERSION="\"$(shell if which git >/dev/null 2>&1 && git rev-parse --short HEAD 2>/dev/null | tr -d '\n'; then if ! git diff --quiet HEAD &>/dev/null; then echo -n "-dirty"; fi; else echo -n 'unknown git revision'; fi)\"" \
		-Wno-unused-variable \
		-Wno-unused-but-set-variable \
		-Wno-pointer-to-int-cast \
		loader.c -o xexec \
		-pthread -lX11 -lGL -lGLEW

gdb:
	gdb --args ./xexec -dd $(xbe)

wait:
	gdb --args ./xexec -ddg $(xbe)

