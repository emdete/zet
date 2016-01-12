/*
    harddisk check program 'zet'.
    Copyright (C) 1998 Michael Dietrich <mdt@mdt.in-berlin.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	"Keep it simple, as simple as possible, but no simpler" A.Einstein
*/

//#define fill(b, s) memset(b, 0xAA, s)
//#define check(b, s) test((void*)b, s)

#define _LARGEFILE64_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

unsigned int crctab[] = {
	0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
	0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
	0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0F00, 0xcFc1, 0xce81, 0x0e40,
	0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
	0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
	0x1e00, 0xdec1, 0xdF81, 0x1F40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
	0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
	0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
	0xF001, 0x30c0, 0x3180, 0xF141, 0x3300, 0xF3c1, 0xF281, 0x3240,
	0x3600, 0xF6c1, 0xF781, 0x3740, 0xF501, 0x35c0, 0x3480, 0xF441,
	0x3c00, 0xFcc1, 0xFd81, 0x3d40, 0xFF01, 0x3Fc0, 0x3e80, 0xFe41,
	0xFa01, 0x3ac0, 0x3b80, 0xFb41, 0x3900, 0xF9c1, 0xF881, 0x3840,
	0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
	0xee01, 0x2ec0, 0x2F80, 0xeF41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
	0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
	0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
	0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
	0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
	0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaF01, 0x6Fc0, 0x6e80, 0xae41,
	0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
	0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
	0xbe01, 0x7ec0, 0x7F80, 0xbF41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
	0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
	0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
	0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
	0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
	0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5F00, 0x9Fc1, 0x9e81, 0x5e40,
	0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
	0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
	0x4e00, 0x8ec1, 0x8F81, 0x4F40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
	0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
	0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

unsigned int crc(
	register char *buffer,
	register unsigned int count,
	register unsigned int code) {
	while (count--)
		code = (code>>8) ^ crctab[(code ^ (*buffer++)) & 0x00ff];
	return code;
}

off64_t tell64(int fd) {
	return lseek64(fd, 0L, SEEK_CUR);
}

typedef struct buftype {
	unsigned int crc;
	char data[0];
} buftype;

void randomize(char* buffer, int size) {
	while (size--)
		*buffer++ = rand();
}

void fill(struct buftype* buffer, int size) {
	randomize(buffer->data, size);
	buffer->crc = crc(buffer->data, size-sizeof(buftype), 0);
}

int check(struct buftype* buffer, int size) {
	return buffer->crc == crc(buffer->data, size-sizeof(buftype), 0);
}

int test(unsigned char* buffer, int size) {
int i;
int r = 1;
	for (i=0;i<size;i++)
		if (buffer[i] != 0xAA) {
			printf("error in buffer at %d (0x%02x)\n", i, (int)buffer[i]);
			r = 0;
		}
	return r;
}

int main(int argc, char* argv[])
{
int argi;
struct buftype* buffer = NULL;
int f = -1;
char* partitionname = NULL;
off64_t fsize = 0;
off64_t blocksize = 512 * 16;
int sync_mode = 0;
int sequencial_check_mode = 1;
int initmode = 1;
int randomer = 1;
/*	srand(ltime()); */
	for (argi=1;argi<argc;argi++)
		switch (argv[argi][0]) {
		case '-':
			switch (argv[argi][1]) {
			case 'f':
				if (++argi >= argc) {
					printf("%s: missing filename arg\n", argv[0]);
					goto help;
				}
				partitionname = argv[argi];
			break;
			case 'l':
				if (++argi >= argc) {
					printf("%s: missing size arg\n", argv[0]);
					goto help;
				}
				fsize = atoi(argv[argi]);
			break;
			case 'b':
				if (++argi >= argc) {
					printf("%s: missing blockcount arg\n", argv[0]);
					goto help;
				}
				blocksize = 512 * atoi(argv[argi]);
				if (blocksize == 0)
					blocksize = 512 * 16;
			break;
			case 's':
				sync_mode = !sync_mode;
				printf("sync now %s\n", sync_mode?"on":"off");
			break;
			case 'i':
				initmode = !initmode;
				printf("init now %s\n", initmode?"on":"off");
			break;
			case 'q':
				sequencial_check_mode = !sequencial_check_mode;
				printf("sequencial now %s\n", sequencial_check_mode?"on":"off");
			break;
			case 'r':
				randomer = !randomer;
				printf("randomer now %s\n", randomer?"on":"off");
			break;
			default:
				printf("wrong option %s\n", argv[argi]);
			case 'h':
				goto help;
			break;
			}
		break;
help:
		default:
			printf("\n\n"
"'zet' will check your harddisk. it does it by writing crc-checked blocks\n"
"onto a file. this file can either be a regular file in a filesystem, a\n"
"partition or a whole harddisk. if you decide to use a regular file,\n"
"you have to tell 'zet' the size you wish to use. without a given size\n"
"the size will be determined while writing (an eof tells 'zet', that the\n"
"disk or partition is full).\n"
"this 'initial' writing is just the first step of checking. the second\n"
"is to randomly read or write blocks at different locations in the\n"
"file.\n"
"so if you don't want to reinitilize a file again you just can skip the\n"
"first step. if you do so you have again to tell 'zet' the size of you\n"
"file.\n"
"\n"
"%s -f s [options]\n"
"\n"
"\t-f s\tpartitionname\n"
"\t-s\t(os-)sync each write\n"
"\t-l i\tsize of the file in count of blocks\n"
"\t-b i\tblocksize given in count of 512 byte\n"
"\t-i\ttoggle initilization (usefull if already done for this file)\n"
"\t-q\ttoggle inital sequencial check\n"
"\t-r\ttoggle random read/write run\n"
	, argv[0]);
			exit(-1);
		}
	if (partitionname==NULL) {
		printf("%s: missing partitionname\n", argv[0]);
		goto help;
	}
	if ((f = open64(partitionname, O_RDWR|(sync_mode?O_SYNC:0))) < 0) {
		printf("%s: open64: %s(%d) (be aware that zet won't create the file for you)\n", argv[0], strerror(errno), errno);
		exit(errno);
	}
	if ((buffer = malloc(blocksize)) == NULL) {
		printf("%s: malloc: %s(%d)\n", argv[0], strerror(ENOMEM), ENOMEM);
		exit(ENOMEM);
	}
	if (initmode) {
		if (lseek64(f, 0, SEEK_SET) < 0) {
			printf("%s: lseek64: %s(%d)\n", argv[0], strerror(errno), errno);
			exit(errno);
		}
		off64_t pos;
		do
		{
			pos = tell64(f);
			printf("write at %ld       \r", pos);
			fflush(stdout);
			if (fsize && pos > fsize * blocksize)
				break;
			fill(buffer, blocksize);
		} while (write(f, buffer, blocksize) == blocksize);
		fsize = pos / blocksize;
		printf("init done for %s\n", partitionname);
	}
	if (sequencial_check_mode) {
		if (lseek64(f, 0, SEEK_SET) < 0) {
			printf("%s: lseek64: %s(%d)\n", argv[0], strerror(errno), errno);
			exit(errno);
		}
		while (read(f, buffer, blocksize) == blocksize)
			if (check(buffer, blocksize)) {
				printf("read at %ld       \r", tell64(f));
				fflush(stdout);
				if (fsize && tell64(f) > fsize * blocksize)
					break;
			}
			else
				printf("check error at %ld in %s\n", tell64(f), partitionname);
		fsize = tell64(f) / blocksize;
		printf("sequencial done for %s\n", partitionname);
	}
	if (!fsize) { /* this does not work - i don't know why: */
		if (lseek64(f, 0, SEEK_END) < 0) {
			printf("%s: lseek64: %s(%d)\n", argv[0], strerror(errno), errno);
			exit(errno);
		}
		if (!fsize) {
			printf("size is zero, you have to specify one with -l\n");
			exit(-1);

		}
	}
	while (randomer) { /* for now we stop by pressing ^C. */
		if (lseek64(f, blocksize * (rand() % fsize), SEEK_SET) < 0) {
			printf("%s: lseek64: %s(%d)\n", argv[0], strerror(errno), errno);
			exit(errno);
		}
		else {
			if (rand() % 6 == 0) { /* every sixth time we write: */
				fill(buffer, blocksize);
				if (write(f, buffer, blocksize) == blocksize) {
					printf("write at %ld       \r", tell64(f));
					fflush(stdout);
				}
				else
					printf("write error %d at %ld\n", errno, tell64(f));
			}
			else
				if (read(f, buffer, blocksize) != blocksize)
					printf("read error %d at %ld\n", errno, tell64(f));
				else
					if (check(buffer, blocksize)) {
						printf("read at %ld       \r", tell64(f));
						fflush(stdout);
					}
					else
						printf("check error at %ld in %s\n", tell64(f), partitionname);
		}
	}
	close(f);
	free(buffer);
	return 0;
}
