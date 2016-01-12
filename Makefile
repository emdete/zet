CFLAGS=-O2 --static
DESTDIR=debian/tmp

zet: zet.c 

zet.c: Makefile

install: zet
	install -g root -o root -m 0755 zet /sbin
	install -g root -o root -m 0644 zet.8 /usr/man/man8
	gzip /usr/man/man8/zet.8
	
clean:
	rm zet

dbg:
	sdb zet

run: zet
	zet
