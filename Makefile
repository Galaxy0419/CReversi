# Copyright (c) 2019, William TANG <galaxyking0419@gmail.com>
GCC = gcc
WINCC = x86_64-w64-mingw32-gcc
RM  = rm -f
CFLAGS = -std=c18 -march=x86-64 -mtune=generic -O3 -pipe
LDFLAGS = -static -static-libgcc -lpthread

linux: src/reversi.c
	$(GCC) -o CReversi src/reversi.c $(CFLAGS) $(LDFLAGS)

win: src/reversi.c
	$(WINCC) -o CReversi.exe src/reversi.c -DWIN $(CFLAGS) $(LDFLAGS)

debug: src/reversi.c
	$(GCC) -o CReversi src/reversi.c -g -Wall $(LDFLAGS)

clean:
	$(RM) CReversi*