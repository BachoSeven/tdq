/*
 * tdi - create tinydict index file
 *
 * Copyright (C) 2009-2011 Ali Gholami Rudi
 *
 * This program is released under the Modified BSD license.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXWORDS	(1 << 17)
#define MAXBYTES	(1 << 25)
#define IDXCHARS	(32)
#define IDXCOUNT	(IDXCHARS * IDXCHARS)

/* tinydict index file header */
struct tinydict {
	char sig[8];	/* "tinydict" */
	int version;	/* zero */
	int levels;	/* 2 */
};

static int charidx(int c)
{
	c = tolower(c);
	if (c < 'a')
		return 0;
	if (c > 'z')
		return IDXCHARS - 1;
	return c - 'a' + 1;
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

static int xwrite(int fd, void *buf, int len)
{
	int nw = 0;
	while (nw < len) {
		int ret = write(fd, buf + nw, len - nw);
		if (ret < 0)
			break;
		nw += ret;
	}
	return nw;
}

static int idx[IDXCOUNT];
static char buf[MAXBYTES];

static void tdi(int dfd, int ifd)
{
	struct tinydict td = {"tinydict", 0, 2};
	int i = -1, j, k;
	char *s, *eof;
	int len;

	len = xread(dfd, buf, sizeof(buf));
	eof = buf + len;
	s = buf;
	while (s < eof) {
		j = charidx(s[0]) * IDXCHARS + charidx(s[1]);
		if (i > j) {
			fprintf(stderr, "error: dict entries not sorted!\n");
			exit(1);
		}
		if (i < j) {
			idx[j] = s - buf;
			for (k = i + 1; k < j; k++)
				idx[k] = idx[j];
			i = j;
		}
		s = strchr(s, '\0') + 1;
	}
	for (k = i + 1; k < IDXCOUNT; k++)
		idx[k] = eof - buf;
	/* writing the index file */
	xwrite(ifd, &td, sizeof(td));
	xwrite(ifd, idx, sizeof(idx));
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		fprintf(stderr, "usage: tdi <dict >idx\n");
		return 1;
	}
	tdi(0, 1);
	return 0;
}
