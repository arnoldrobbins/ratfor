#include "r.h"

#define	wasbreak	brkused[brkptr] == 1 || brkused[brkptr] == 3
#define	wasnext	brkused[brkptr] == 2 || brkused[brkptr] == 3

int	transfer	= 0;	/* 1 if just finished retrun, break, next */

char	fcname[200];
char	scrat[500];

int	brkptr	= -1;
int	brkstk[50];	/* break label */
int	typestk[50];	/* type of loop construct */
int	brkused[50];	/* loop contains BREAK or NEXT */

int	forptr	= 0;
char	*forstk[50];

static	int	eatup(void);
static	bool	nonblank(char *s);

void
repcode(void)
{
	transfer = 0;
	outcont(0);
	putcom("repeat");
	yyval.token = genlab(3);
	indent++;
	outcont(yyval.token);
	brkstk[++brkptr] = yyval.token + 1;
	typestk[brkptr] = REPEAT;
	brkused[brkptr] = 0;
}

void
untils(int p1, int un)
{
	outnum(p1 + 1);
	outtab();
	if (un > 0) {
		outcode("if(.not.");
		balpar();
		outcode(")");
	}
	transfer = 0;
	outgoto(p1);
	indent--;
	if (wasbreak)
		outcont(p1 + 2);
	brkptr--;
}

void
ifcode(void)
{
	transfer = 0;
	outtab();
	if (f77) {
		outtab();
		outcode("if");
		balpar();
		outcode("then");
		outdon();
	} else {
		outcode("if(.not.");
		balpar();
		outcode(")");
		outgoto(yyval.token = genlab(2));
	}
	indent++;
}

void
elsecode(int p1)
{
	if (f77) {
		outtab();
		outcode("else");
		outdon();
	} else {
		outgoto(p1 + 1);
		indent--;
		putcom("else");
		indent++;
		outcont(p1);
	}
}

void
endif(void)
{
	outtab();
	outcode("endif");
	outdon();
}

void
whilecode(void)
{
	transfer = 0;
	outcont(0);
	putcom("while");
	brkstk[++brkptr] = yyval.token = genlab(2);
	typestk[brkptr] = WHILE;
	brkused[brkptr] = 0;
	outnum(yyval.token);
	outtab();
	outcode("if(.not.");
	balpar();
	outcode(")");
	outgoto(yyval.token + 1);
	indent++;
}

void
whilestat(int p1)
{
	outgoto(p1);
	indent--;
	putcom("endwhile");
	outcont(p1 + 1);
	brkptr--;
}

void
balpar(void)
{
	int c, lpar;

	while ((c = gtok(scrat)) == ' ' || c == '\t')
		;
	if (c != '(') {
		error("missing left paren");
		return;
	}
	outcode(scrat);
	lpar = 1;
	do {
		c = gtok(scrat);
		if (c == ';' || c == '{' || c == '}' || c == EOF) {
			pbstr(scrat);
			break;
		}
		if (c == '(')
			lpar++;
		else if (c == ')')
			lpar--;
		else if (c == '\n') {
			while ((c = gtok(scrat)) == ' ' || c == '\t' || c == '\n')
				;
			pbstr(scrat);
			continue;
		}
		else if (c == '=' && scrat[1] == '\0')
			error("assigment inside conditional");
		outcode(scrat);
	} while (lpar > 0);
	if (lpar != 0)
		error("missing parenthesis");
}

int	labval	= 23000;

int
genlab(int n)
{
	labval += n;
	return labval - n;
}

void
gokcode(char *p1)
{
	transfer = 0;
	outtab();
	outcode(p1);
	eatup();
	outdon();
}

static int
eatup(void)
{
	int t, lpar;
	char temp[100];

	lpar = 0;
	do {
		if ((t = gtok(scrat)) == ';' || t == '\n')
			break;
		if (t == '{' || t == '}' || t == EOF) {
			pbstr(scrat);
			break;
		}
		if (t == ',' || t == '+' || t == '-' || t == '*' || t == '('
		  || t == '&' || t == '|' || t == '=') {
			while (gtok(temp) == '\n')
				;
			pbstr(temp);
		}
		if (t == '(')
			lpar++;
		else if (t == ')') {
			lpar--;
			if (lpar < 0) {
				error("missing left paren");
				return 1;
			}
		}
		outcode(scrat);
	} while (lpar >= 0);
	if (lpar > 0) {
		error("missing right paren");
		return 1;
	}
	return 0;
}

void
forcode(void)
{
	int lpar, t;
	char *ps, *qs;

	transfer = 0;
	outcont(0);
	putcom("for");
	yyval.token = genlab(3);
	brkstk[++brkptr] = yyval.token + 1;
	typestk[brkptr] = FOR;
	brkused[brkptr] = 0;
	forstk[forptr++] = malloc(1);
	if ((t = gnbtok(scrat)) != '(') {
		error("missing left paren in FOR");
		pbstr(scrat);
		return;
	}
	if (gnbtok(scrat) != ';') {	/* real init clause */
		pbstr(scrat);
		outtab();
		if (eatup() > 0) {
			error("illegal FOR clause");
			return;
		}
		outdon();
	}
	if (gnbtok(scrat) == ';')	/* empty condition */
		outcont(yyval.token);
	else {	/* non-empty condition */
		pbstr(scrat);
		outnum(yyval.token);
		outtab();
		outcode("if(.not.(");
		for (lpar = 0; lpar >= 0;) {
			if ((t = gnbtok(scrat)) == ';')
				break;
			if (t == '(')
				lpar++;
			else if (t == ')') {
				lpar--;
				if (lpar < 0) {
					error("missing left paren in FOR clause");
					return;
				}
			}
			if (t != '\n')
				outcode(scrat);
		}
		outcode("))");
		outgoto(yyval.token + 2);
		if (lpar < 0)
			error("invalid FOR clause");
	}
	ps = scrat;
	for (lpar = 0; lpar >= 0;) {
		if ((t = gtok(ps)) == '(')
			lpar++;
		else if (t == ')')
			lpar--;
		if (lpar >= 0 && t != '\n')
			while (*ps != '\0')
				ps++;
		if (t == EOF) {
			error("EOF encountered in FOR clause");
			exit(1);
		}
	}
	*ps = '\0';
	qs = forstk[forptr - 1] = malloc((unsigned)(ps - scrat + 1));
	ps = scrat;
	while (*qs++ = *ps++)
		;
	indent++;
}

void
forstat(int p1)
{
	char *bp, *q;
	bp = forstk[--forptr];
	if (wasnext)
		outnum(p1 + 1);
	if (nonblank(bp)){
		outtab();
		outcode(bp);
		outdon();
	}
	transfer = 0;
	outgoto(p1);
	indent--;
	putcom("endfor");
	outcont(p1 + 2);
	for (q = bp; *q++;)
		continue;
	free(bp);
	brkptr--;
}

void
retcode(void)
{
	int c;

	if ((c = gnbtok(scrat)) != '\n' && c != ';' && c != '}') {
		pbstr(scrat);
		outtab();
		outcode(fcname);
		outcode(" = ");
		eatup();
		outdon();
	}
	else if (c == '}')
		pbstr(scrat);
	outtab();
	outcode("return");
	outdon();
	transfer = 1;
}

void
docode(void)
{
	transfer = 0;
	outtab();
	outcode("do ");
	yyval.token = genlab(2);
	brkstk[++brkptr] = yyval.token;
	typestk[brkptr] = DO;
	brkused[brkptr] = 0;
	outnum(yyval.token);
	eatup();
	outdon();
	indent++;
}

void
dostat(int p1)
{
	outcont(p1);
	indent--;
	if (wasbreak)
		outcont(p1 + 1);
	brkptr--;
}

void
breakcode(void)
{
	int level, t;

	level = 0;
	if ((t = gnbtok(scrat)) == DIG)
		level = atoi(scrat) - 1;
	else if (t != ';')
		pbstr(scrat);
	if (brkptr-level < 0)
		error("illegal BREAK");
	else {
		outgoto(brkstk[brkptr-level] + 1);
		brkused[brkptr-level] |= 1;
	}
	transfer = 1;
}

void
nextcode(void)
{
	int level, t;

	level = 0;
	if ((t = gnbtok(scrat)) == DIG)
		level = atoi(scrat) - 1;
	else if (t != ';')
		pbstr(scrat);
	if (brkptr-level < 0)
		error("illegal NEXT");
	else {
		outgoto(brkstk[brkptr-level]);
		brkused[brkptr-level] |= 2;
	}
	transfer = 1;
}

static bool
nonblank(char *s)
{
	int c;
	while (c = *s++)
		if (c != ' ' && c != '\t' && c != '\n')
			return true;
	return false;
}

int	errorflag	= 0;

void
error(char *s1)
{
	if (errorflag == 0)
		fprintf(stderr, "ratfor:");
	fprintf(stderr, "error at line %d, file %s: ", linect[infptr], curfile[infptr]);
	fprintf(stderr, "%s", s1);
	fprintf(stderr, "\n");
	errorflag = 1;
}

void
errcode(void)
{
	int c;

	if (errorflag == 0)
		fprintf(stderr, "******\n");
	fprintf(stderr, "*****F ratfor:");
	fprintf(stderr, "syntax error, line %d, file %s\n", linect[infptr], curfile[infptr]);
	while ((c = getchr()) != ';' && c != '}' && c != '\n' && c != EOF && c != '\0')
		;
	if (c == EOF || c == '\0')
		putbak(c);
	errorflag = 1;
}
