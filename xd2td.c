/*
 * xd2tc - convert xdxf dictionaries to tinydict
 *
 * Copyright (C) 2011 Ali Gholami Rudi
 *
 * This program is released under the Modified BSD license.
 */
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

static char xd[MAXBYTES];
static char td[MAXBYTES];

static void xd2td(int xdfd, int tdfd)
{
	char tag[128];
	int len = xread(xdfd, xd, sizeof(xd));
	char *s = xd;
	char *d = td;
	xd[len] = '\0';

	while (s && *s) {
		char *r = strchr(s, '<');
		char *e = r ? r : strchr(s, '\0');
		int closing = 0;
		int i = 0;
		memcpy(d, s, e - s);
		d += e - s;
		if (!r)
			break;
		r++;
		if (*r == '/') {
			closing = 1;
			r++;
		}
		while (*r && isalpha(*r))
			tag[i++] = *r++;
		tag[i] = '\0';
		while (*r && *r != '>')
			r++;
		if (!closing && !strcmp("k", tag))
			*d++ = '\0';
		s = r && *r == '>' ? r + 1 : r;
	}
	*d++ = '\0';
	xwrite(tdfd, td, d - td);
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		fprintf(stderr, "usage: xd2td <xdxf_file >tinydict_file\n");
		return 1;
	}
	xd2td(0, 1);
	return 0;
}
