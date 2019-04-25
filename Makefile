# Copyright (c) 2019, William TANG <galaxyking0419@gmail.com>
GCC = gcc
WINCC = x86_64-w64-mingw32-gcc
RM  = rm -f
CFLAGS = -march=x86-64 -mtune=generic -O3 -pipe -std=c18
LDFLAGS = -lpthread

linux: src/reversi.c
	$(GCC) $(CFLAGS) $(LDFLAGS) src/reversi.c -o CReversi
win: src/reversi.c
	$(WINCC) $(CFLAGS) $(LDFLAGS) -DWIN src/reversi.c -o CReversi.exe

debug: src/reversi.c
	$(GCC) $(LDFLAGS) -g src/reversi.c -o CReversi

clean:
	$(RM) CReversi*