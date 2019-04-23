# Copyright (c) 2019, William TANG <galaxyking0419@gmail.com>
GCC = gcc
WINCC = x86_64-w64-mingw32-gcc
RM  = rm -f
CFLAGS = -march=x86-64 -mtune=generic -O3 -pipe -fno-plt
LDFLAGS = 

*nix: reversi.c
	$(GCC) $(CFLAGS) $(LDFLAGS) reversi.c -o CReversi
win: reversi.c
	$(WINCC) $(CFLAGS) $(LDFLAGS) reversi.c -o CReversi.exe

clean:
	$(RM) CReversi* reversi