#include "r.h"

char	outbuf[80];
int	outp	= 0;
int	cont	= 0;
int	contchar	= '&';

char	comment[320];
int	comptr	= 0;
int	indent	= 0;

void
outdon(void)
{
	outbuf[outp] = '\0';
	if (outp > 0)
		fprintf(outfil, "%s\n", outbuf);
	outp = cont = 0;
}

void
putcom(char *s)
{
	if (printcom) {
		ptc(uppercase? 'C' : 'c');
		outtab();
		pts(s);
		outdon();
	}
}

void
outcode(char *xp)
{
	int c, c1, j;
	char *q, *p, *s;

	p = xp;
	if (cont == 0 && comptr > 0)	/* flush comment if not on continuation */
		flushcom();
	while ((c = *p++) != '\0') {
		c1 = *p;
		if (isalpha(c) || isdigit(c)) {
			char *s = p-1;

			if (uppercase)
				s = foldup(p-1);
			pts(s);
			if (s != p-1)
				free(s);
			break;
		}
		s = NULL;	/* generally set to something like .ge. */
		switch(c) {
		case '"':
		case '\'':
			j = 0;
			for (q = p; *q; q++) {
				if (*q == '\\')
					q++;
				j++;
			}
			if (outp + j + 2 > 71)
				contcard();
			if (hollerith) {
				outnum(--j);
				ptc(uppercase ? 'H' : 'h');
			} else
				ptc(c);
			while (*p != c) {
				if (*p == '\\')
					p++;
				ptc(*p++);
			}
			if (!hollerith)
				ptc(c);
			p++;
			break;
		case '$':
		case '\\':
			if (strlen(p-1)+outp > 71)
				contcard();
			if (c1 == '"' || c1 == '\'') {
				ptc(c1);
				p++;
			} else
				for (p--; *p; p++)
					ptc(*p);
			break;
		case '%':
			outp = 0;
			while (*p)
				ptc(*p++);
			break;
		case '>':
			if (c1 == '=') {
				s = ".ge.";
				p++;
			} else
				s = ".gt.";
			break;
		case '<':
			if (c1 == '=') {
				s = ".le.";
				p++;
			} else if (c1 == '>') {
				s = ".ne.";
				p++;
			} else
				s = ".lt.";
			break;
		case '=':
			if (c1 == '=') {
				s = ".eq.";
				p++;
			} else
				s = "=";
			break;
		case '!': case '^':
			if (c1 == '=') {
				s = ".ne.";
				p++;
			} else
				s = ".not.";
			break;
		case '&':
			if (c1 == '&')
				p++;
			s = ".and.";
			break;
		case '|':
			if (c1 == '|')
				p++;
			s = ".or.";
			break;
		case '\t':
			outtab();
			break;
		case '\n':
			ptc(' ');
			break;
		default:
			ptc(c);
			break;
		}
		if (s != NULL) {
			char *p = s;

			if (uppercase)
				p = foldup(s);
			pts(p);
			if (p != s)
				free(p);
		}
	}
}

char *
foldup(char *s)		/* convert s to upper case */
{
	char *p = strdup(s);

	for (s = p; *s != '\0'; s++) {
		if (islower(*s))
			*s = toupper(*s);
	}

	return p;
}

void
ptc(char c)
{
	if (outp > 71)
		contcard();
	outbuf[outp++] = c;
}

void
pts(char *s)
{
	if (strlen(s)+outp > 71)
		contcard();
	while (*s)
		ptc(*s++);
}

void
contcard(void)
{
	int n;
	outbuf[outp] = '\0';
	fprintf(outfil, "%s\n", outbuf);
	n = 6;
	if (printcom) {
		n += INDENT * indent + 1;
		if (n > 35) n = 35;
	}
	for (outp = 0; outp < n; outbuf[outp++] = ' ')
		continue;
	outbuf[contfld-1] = contchar;
	cont++;
	if (cont > 19)
		error("more than 19 continuation cards");
}

void
outtab(void)
{
	int n;

	n = 6;
	if (printcom) {
		n += INDENT * indent;
		if (n > 35)
			n = 35;
	}
	while (outp < n)
		ptc(' ');
}

void
outnum(int n)
{
	int a;

	if ((a = n/10) != 0)
		outnum(a);
	ptc(n%10 + '0');
}

void
outcont(int n)
{
	transfer = 0;
	if (n == 0 && outp == 0)
		return;
	if (n > 0)
		outnum(n);
	outcode("\tcontinue");
	outdon();
}

void
outgoto(int n)
{
	if (transfer != 0)
		return;
	outcode("\tgoto ");
	outnum(n);
	outdon();
}

void
flushcom(void)
{
	int i, j;
	if (printcom == 0)
		comptr = 0;
	else if (cont == 0 && comptr > 0) {
		for (i = j = 0; i < comptr; i++)
			if (comment[i] == '\n') {
				comment[i] = '\0';
				fprintf(outfil, "%s\n", &comment[j]);
				j = i + 1;
			}
		comptr = 0;
	}
}
