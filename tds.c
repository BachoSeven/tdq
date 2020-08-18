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

#define MAXWORDS	(1 << 19)
#define MAXBYTES	(1 << 25)
#define IDXCHARS	(32)
#define IDXCOUNT	(IDXCHARS * IDXCHARS)

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

static char tdi[MAXBYTES];
static char tdo[MAXBYTES];
static char *words[MAXWORDS];

static int charidx(int c)
{
	c = tolower(c);
	if (c < 'a')
		return 0;
	if (c > 'z')
		return IDXCHARS - 1;
	return c - 'a' + 1;
}

static int wordpos(char *s)
{
	int c0 = s[0] != '\n' ? charidx(s[0]) : 0;
	int c1 = s[1] != '\n' ? charidx(s[1]) : 0;
	return (c0 * IDXCHARS) + c1;
}

static int wordcmp(void *v1, void *v2)
{
	return wordpos(*(char **) v1) - wordpos(*(char **) v2);
}

int main(int argc, char *argv[])
{
	int len;
	char *s, *r;
	int n = 0;
	int i;
	if (argc != 1) {
		printf("usage: tds <in.td >out.td\n");
		return 1;
	}
	len = xread(0, tdi, sizeof(tdi));
	s = tdi;
	while (s && s < tdi + len) {
		words[n++] = s;
		r = strchr(s, '\0');
		s = r ? r + 1 : 0;
	}

	qsort(words, n, sizeof(*words), (void *) wordcmp);

	s = tdo;
	for (i = 0; i < n; i++) {
		int len = strlen(words[i]);
		memcpy(s, words[i], len + 1);
		s += len + 1;
	}
	xwrite(1, tdo, s - tdo);
	return 0;
}
