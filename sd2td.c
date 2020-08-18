/*
 * sd2tc - convert stardict dictionaries to tinydict
 *
 * Copyright (C) 2009 Ali Gholami Rudi
 *
 * This program is released under the Modified BSD license.
 */
#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXBYTES	(1 << 26)

static long readint(void *p)
{
	int32_t *i = p;
	return ntohl(*i);
}

static int xread(int fd, void *buf, int len)
{
	int nr = 0;
	while (nr < len) {
		int ret = read(fd, buf + nr, len - nr);
		if (ret <= 0)
			break;
		nr += ret;
	}
	return nr;
}

static char dict[MAXBYTES];
static char idx[MAXBYTES];

static void sd2td(int tdfd, int dfd, char *idxfile)
{
	int ifd = open(idxfile, O_RDONLY);
	char *idxeof;
	char *c = idx;
	char buf[1 << 20];
	long off, len;
	idxeof = idx + xread(ifd, idx, sizeof(idx));
	xread(dfd, dict, sizeof(dict));

	while (c < idxeof) {
		char *s = buf;
		strcpy(s, c);
		s = strchr(s, '\0');
		*s++ = '\n';
		c = strchr(c, '\0');
		off = readint(c + 1);
		len = readint(c + 5);
		c += 9;
		memcpy(s, dict + off, len);
		s[len++] = '\n';
		s[len] = '\0';
		write(tdfd, buf, strlen(buf) + 1);
	}

	close(ifd);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: sd2td sdict.idx <sdict >dict.td\n");
		return 1;
	}
	sd2td(1, 0, argv[1]);
	return 0;
}
