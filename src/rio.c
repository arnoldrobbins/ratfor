#include "r.h"
#define	BUFSIZE	5120
char	ibuf[BUFSIZE];
char	*ip = ibuf;

static	void	getfname(void);
static	int	peek(char *p, char c1);

int
gtok(char *s)	/* get token into s */
{
	int c;
	char *p;
	struct nlist *q;

	for (;;) {
		p = s;
		*p++ = c = getchr();
		if (isalpha(c) || isdigit(c)) {
			while (isalpha(*p = getchr()) || isdigit(*p))
				p++;
			putbak(*p);
			*p = '\0';
			if ((q = lookup(s))->name != NULL && q->ydef == 0) {	/* found but not keyword */
				if (q->def != fcnloc && q->def != FCN1loc) {	/* not "function" */
					pbstr(q->def);
					continue;
				}
				getfname();	/* recursive gtok */
			}
			for (p = s; *p != '\0'; p++)
				if (isupper(*p))
					*p = tolower(*p);
			for (p = s; *p != '\0'; p++)
				if (!isdigit(*p))
					return LET;
			return DIG;
		}
		switch(c) {
		case 0:
			if (infptr > 0) {
				fclose(infile[infptr]);
				infptr--;
				continue;
			}
			if (svargc > 1) {
				svargc--;
				svargv++;
				if (infile[0] != stdin)
					fclose(infile[0]);
				if (!strcmp(*svargv, "-"))
					infile[0] = stdin;
				else if( (infile[0] = fopen(*svargv,"r")) == NULL )
					cant(*svargv);
				linect[0] = 0;
				curfile[0] = *svargv;
				continue;
			}
			return EOF;	/* real eof */
		case ' ':
		case '\t':
			while ((c = getchr()) == ' ' || c == '\t')
				;	/* skip others */
			if (c == COMMENT || c == '_') {
				putbak(c);
				continue;
			}
			if (c != '\n') {
				putbak(c);
				*p = '\0';
				return ' ';
			} else {
				*s = '\n';
				*(s + 1) = '\0';
				return *s;
			}
		case '_':
			while ((c = getchr()) == ' ' || c == '\t')
				;
			if (c == COMMENT) {
				putbak(c);
				gtok(s);	/* recursive */
			}
			else if (c != '\n')
				putbak(c);
			continue;
		case '[':
			*p = '\0';
			return '{';
		case ']':
			*p = '\0';
			return '}';
		case '$':
		case '\\':
			if ((*p = getchr()) == '(' || *p == ')') {
				putbak(*p == '(' ? '{' : '}');
				continue;
			}
			if (*p == '"' || *p == '\'')
				p++;
			else
				putbak(*p);
			*p = '\0';
			return '$';
		case COMMENT:
			comment[comptr++] = 'c';
			while ((comment[comptr++] = getchr()) != '\n')
				;
			flushcom();
			*s = '\n';
			*(s + 1) = '\0';
			return *s;
		case '"':
		case '\'':
			for (; (*p = getchr()) != c; p++) {
				if (*p == '\\')
					*++p = getchr();
				if (*p == '\n') {
					error("missing quote");
					putbak('\n');
					break;
				}
			}
			*p++ = c;
			*p = '\0';
			return QUOTE;
		case '%':
			while ((*p = getchr()) != '\n')
				p++;
			putbak(*p);
			*p = '\0';
			return '%';
		case '>': case '<': case '=': case '!': case '^':
			return peek(p, '=');
		case '&':
			return peek(p, '&');
		case '|':
			return peek(p, '|');
		default:
			if (!isprint(c))
				continue;
		case '\n':
			*p = '\0';
			return *s;
		}
	}
}

int
gnbtok(char *s)
{
	int c;

	while ((c = gtok(s)) == ' ' || c == '\t')
		;
	return c;
}

static void
getfname(void)
{
	while (gtok(fcname) == ' ')
		;
	pbstr(fcname);
	putbak(' ');
}

static int
peek(char *p, char c1)
{
	int c;

	c = *(p - 1);
	if ((*p = getchr()) == c1)
		p++;
	else
		putbak(*p);
	*p = '\0';
	return c;
}

void
pbstr(char *str)
{
	char *p;

	p = str;
	while (*p++)
		continue;
	--p;
	if (ip >= &ibuf[BUFSIZE]) {
		error("pushback overflow");
		exit(EXIT_FAILURE);
	}
	while (p > str)
		putbak(*--p);
}

int
getchr(void)
{
	int c;

	if (ip > ibuf)
		return *--ip;
	c = getc(infile[infptr]);
	if (c == '\n')
		linect[infptr]++;
	if (c == EOF)
		return 0;
	return c;
}
