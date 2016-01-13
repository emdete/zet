zet
===

Heavy duty harddisk check. A file in the filesystem, a
partition on a disk or even a hole disk (in fact any
file) will be written with crc-checked random-filled
buffers. in the second stage those blocks will be
randomly read from the file an the crc-sum checked.
also several times new blocks are written. errors are
logged. this programm simulates a usal harddiskacess
under a heavyly loaded system and brings up bugs in
the os, the hardware or even the firmware of the
harddisk.

