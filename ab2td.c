/*
 * ab2tc - convert arpabet pronunciations to tinydict
 *
 * Copyright (C) 2013 Ali Gholami Rudi
 *
 * This program is released under the Modified BSD license.
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define LEN(a)		(sizeof(a) / sizeof((a)[0]))

static int next(void)
{
	return fgetc(stdin);
}

static void back(int c)
{
	ungetc(c, stdin);
}

static void skip(void)
{
	int c = next();
	while (c != EOF && c != '\n')
		c = next();
}

static int nextword(char *d)
{
	int c = next();
	int l = 0;
	while (c != EOF && (c == ' ' || c == '\t'))
		c = next();
	while (c != EOF && !isspace(c)) {
		d[l++] = c == '_' ? ' ' : tolower(c);
		c = next();
	}
	d[l] = '\0';
	if (c != EOF)
		back(c);
	return !l;
}

static void trim_parens(char *s)
{
	int i = strlen(s) - 1;
	if (i > 0 && s[i] == ')') {
		while (i > 0 && s[i] != '(')
			i--;
		if (s[i] == '(')
			s[i] = '\0';
	}
}

static char *cmu2ipa(char *s);

static char *trim_stress(char *s)
{
	int i = strlen(s) - 1;
	if (s[i] == '1') {
		s[i] = '\0';
		return "ˈ";
	}
	if (s[i] == '2') {
		s[i] = '\0';
		return "ˌ";
	}
	if (s[i] == '0')
		s[i] = '\0';
	return "";
}

int main(void)
{
	int c;
	char word[512];
	char tok[512];
	char p1[512];
	char p2[512];
	while ((c = next()) != EOF) {
		if (c == '#' || c == ';') {
			skip();
			continue;
		}
		back(c);
		nextword(word);
		trim_parens(word);
		printf("%s\n", word);
		p1[0] = '\0';
		p2[0] = '\0';
		while (!nextword(tok)) {
			if (p1[0])
				strcat(p1, " ");
			strcat(p1, tok);
			if (tok[0] == 'a' && tok[1] == 'h' && tok[2] == '0')
				strcpy(tok, "ax0");
			if (tok[0] == 'e' && tok[1] == 'r' && tok[2] == '0')
				strcpy(tok, "axr0");
			strcat(p2, trim_stress(tok));
			strcat(p2, cmu2ipa(tok));
		}
		printf("%s \t\t [%s]\n%c", p2, p1, 0);
		skip();
	}
	return 0;
}

static char *ipa_map[][2] = {
	/* short vowels */
	{"eh", "ɛ"},
	{"ih", "ɪ"},
	{"uh", "ʊ"},
	{"ah", "ʌ"},
	{"oh", "ɒ"},
	{"ax", "ə"}, 
	{"ae", "æ"}, 
	/* long vowels */
	{"iy", "i"},		/* this could mean both i or iː */
	{"ao", "ɔː"},
	{"aa", "ɑː"},
	{"uw", "uː"},
	{"ea", "ɛː"}, 
	/* diphthongs */
	{"ey", "eɪ"}, 
	{"ay", "aɪ"},
	{"ow", "oʊ"},
	{"aw", "aʊ"},
	{"oy", "ɔɪ"},
	{"ua", "ʊə"}, 
	{"ia", "ɪə"},

	{"er", "ɝ"},
	{"axr", "ɚ"},

	/* consonants */
	{"p", "p"},
	{"b", "b"},
	{"t", "t"},
	{"d", "d"},
	{"k", "k"},
	{"g", "ɡ"},

	{"ch", "tʃ"},
	{"jh", "dʒ"},

	{"f", "f"},
	{"v", "v"},
	{"th", "θ"},
	{"dh", "ð"},
	{"s", "s"},
	{"z", "z"},
	{"sh", "ʃ"},
	{"zh", "ʒ"},
	{"hh", "h"},

	{"m", "m"},
	{"em", "m"},
	{"n", "n"},
	{"en", "n"},
	{"ng", "ŋ"},
	{"eng", "ŋ"},

	{"l", "l"},
	{"el", "ɫ"},
	{"r", "r"},
	{"dx", "ɾ"},
	{"nx", "ɾ"},

	{"y", "j"},
	{"w", "w"},
	{"q", "ʔ"},
};

static char *cmu2ipa(char *s)
{
	int i;
	for (i = 0; i < LEN(ipa_map); i++)
		if (!strcmp(ipa_map[i][0], s))
			return ipa_map[i][1];
	return s;
}
