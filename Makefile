GCC = gcc
WINCC = x86_64-w64-mingw32-gcc
RM  = rm -f
CFLAGS = -march=x86-64 -mtune=generic -O3 -pipe -fno-plt
LDFLAGS = 

*nix: reversi.h reversi.c
	$(GCC) $(CFLAGS) $(LDFLAGS) reversi.c -o CReversi
win: reversi.h reversi.c
	$(WINCC) $(CFLAGS) $(LDFLAGS) reversi.c -o CReversi.exe

clean:
	$(RM) CReversi*