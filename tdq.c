/*
 * tdq - lookup tinydict dictionaries
 *
 * Copyright (C) 2009-2013 Ali Gholami Rudi <ali at rudi dot ir>
 *
 * This program is released under the Modified BSD license.
 */
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))
#define IDXCHARS	(32)
#define IDXCOUNT	(IDXCHARS * IDXCHARS)
#define MAXWORDS	(1 << 17)
#define MAXBYTES	(1 << 26)

#define TD_EXACT	0
#define TD_START	1
#define TD_OCCUR	2

#define DICTROOT	"/usr/local/share/tdq/dict"

static struct dictpath {
	char *td;	/* tinydict file */
	char *ti;	/* tinydict index; tdi output */
	char *sign;	/* dict sign characters */
} dicts[] = {
	{DICTROOT "etimology.td", DICTROOT "etimology.ti", "th"},
	{DICTROOT "foldoc.td", DICTROOT "foldoc.ti", "th"},
	{DICTROOT "gcide.td", DICTROOT "gcide.ti", "th"},
	{DICTROOT "gnulinux.td", DICTROOT "gnulinux.ti", "th"},
	{DICTROOT "jargon.td", DICTROOT "jargon.ti", "th"},
	{DICTROOT "ita.td", DICTROOT "ita.ti", "th"},
	{DICTROOT "engturk.td", DICTROOT "thesaurus.ti", "th"},
	{DICTROOT "turkeng.td", DICTROOT "turkeng.ti", "th"},
	{DICTROOT "thesaurus.td", DICTROOT "thesaurus.ti", "th"},
};

/* tinydict index file header */
struct tinydict {
	char sig[8];	/* "tinydict" */
	int version;	/* zero */
	int levels;	/* 2 */
};

static int dict_idx(int *idx, char *path)
{
	int fd = open(path, O_RDONLY);
	struct tinydict td;
	if (fd < 0 || read(fd, &td, sizeof(td)) != sizeof(td))
		return 1;
	if (read(fd, idx, IDXCOUNT * sizeof(*idx)) != IDXCOUNT * sizeof(*idx))
		return 1;
	close(fd);
	return 0;
}

static void dict_word(char *w, char *s)
{
	char *o = w;
	while (*s && *s != '\n' && *s != '(')
		*w++ = tolower(*s++);
	while (w > o && w[-1] == ' ')
		w--;
	*w = '\0';
}

static int charidx(int c)
{
	if (c < 'a')
		return 0;
	if (c > 'z')
		return IDXCHARS - 1;
	return c - 'a' + 1;
}

static void dict_region(char *word, int *s, int *e, int type)
{
	if (type == TD_OCCUR) {
		*s = 0;
		*e = IDXCOUNT;
	} else {
		*s = charidx(word[0]) * IDXCHARS + charidx(word[1]);
		*e = *s + 1;
	}
}

static void dict_find(char *q, char *s, char *e, int type)
{
	char word[1 << 10];
	while (s < e) {
		char *d = strchr(s, '\n');
		if (!d)
			break;
		dict_word(word, s);
		if (type == TD_EXACT && !strcmp(q, word))
			printf("%s", d + 1);
		if (type == TD_START && !strncmp(q, word, strlen(q)))
			printf("%s\n", word);
		if (type == TD_OCCUR && strstr(word, q))
			printf("%s\n", word);
		s = strchr(s, '\0') + 1;
	}
}

static char buf[MAXBYTES];
static int idx[IDXCOUNT];

int main(int argc, char *argv[])
{
	char *word;
	int si, ei;
	int mod = TD_EXACT;
	int sel = 'd';
	int fd;
	int i;
	if (argc < 2) {
		printf("usage: tdq [mode] word\n");
		return 1;
	}
	if (!strcmp("-v", argv[1])) {
		printf("tdq-0.2\n");
		return 1;
	}
	if (argc > 2)
		sel = argv[1][0];
	word = argv[argc > 2 ? 2 : 1];
	if (word[strlen(word) - 1] == '*') {
		mod = TD_START;
		word[strlen(word) - 1] = '\0';
	}
	if (word[0] == '*') {
		mod = TD_OCCUR;
		word++;
	}
	dict_region(word, &si, &ei, mod);
	for (i = 0; i < ARRAY_SIZE(dicts); i++) {
		char *name = strrchr(dicts[i].td, '/');
		int len;
		if (sel != '/' && !strchr(dicts[i].sign, sel))
			continue;
		fd = open(dicts[i].td, O_RDONLY);
		if (fd < 0 || dict_idx(idx, dicts[i].ti))
			continue;
		printf("------ %s ------\n", name ? name + 1 : dicts[i].td);
		lseek(fd, idx[si], 0);
		len = ei < IDXCOUNT ? idx[ei] - idx[si] : sizeof(buf);
		len = read(fd, buf, len);
		dict_find(word, buf, buf + len, mod);
		close(fd);
	}
	return 0;
}
