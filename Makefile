CFLAGS=-O2 --static -Wall
DESTDIR=/usr/local

all: zet

run: zet
	touch /tmp/zet.blob
	./zet -f /tmp/zet.blob -l 100

dbg:

clean:
	rm -f zet

zet: zet.c

zet.c: Makefile

install: zet
	install -g root -o root -m 0755 zet $(DESTDIR)/sbin
	install -g root -o root -m 0644 zet.8 $(DESTDIR)/man/man8
	gzip $(DESTDIR)/man/man8/zet.8

