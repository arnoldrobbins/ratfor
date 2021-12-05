#include "r.h"

char *keyword [] = {
	"do",
	"if",
	"else",
	"for",
	"repeat",
	"until",
	"while",
	"break",
	"next",
	"define",
	"include",
	"return",
	"switch",
	"case",
	"default",
	0};

int keytran[] = {
	DO,
	IF,
	ELSE,
	FOR,
	REPEAT,
	UNTIL,
	WHILE,
	BREAK,
	NEXT,
	DEFINE,
	INCLUDE,
	RETURN,
	SWITCH,
	CASE,
	DEFAULT,
	0};

char	*fcnloc;	/* spot for "function" */
char	*FCN1loc;	/* spot for "FUNCTION"; kludge */

int	svargc;
char	**svargv;
char	*curfile[10]	= { "" };
int	infptr	= 0;
FILE	*outfil	= NULL;
FILE	*infile[10];
int	linect[10];

int	contfld		= CONTFLD;	/* place to put continuation char */
bool	printcom	= false;	/* print comments if on */
bool	hollerith	= false;	/* convert "..." to 27H... if on */
bool	uppercase	= false;	/* produce output in upper case (except for "...") */
bool	f77		= false;	/* output in fortran 77 (if-then-else-endif) */

static	void	inclstat(void);
static	void	defstat(void);

static void
usage(const char *name)
{
	fprintf(stderr, "usage: %s [-c N[c]] [-Chu] [-f 66|77] [file ...]\n", name);
	exit(EXIT_FAILURE);
}

static void
set_continuation(const char *str)
{
	const char *cp;

	if (! isdigit(*str))
		return;		// leave contfld and contchar at their defaults

	for (contfld = 0, cp = str; *cp != '\0'; cp++) {
		if (! isdigit(*cp))
			break;
		contfld = (contfld * 10) + (*cp - '0');
	}

	if (*cp != '\0')
		contchar = *cp;
}

int
main(int argc, char **argv)
{
	int i, c;

	outfil = stdout;
	infile[0] = stdin;

	while ((c = getopt(argc, argv, "+c:Cf:hu")) != -1) {
		switch (c) {
		case 'c':
			set_continuation(optarg);
			break;
		case 'C':
			printcom = true;
			break;
		case 'f':
			f77 = (strcmp(optarg, "77") == 0);
			break;
		case 'h':
			hollerith = true;
			break;
		case 'u':
			uppercase = true;
			break;
		default:
			usage(argv[0]);
			break;
		}
	}

	svargc = argc - (optind - 1);
	svargv = argv + (optind - 1);

	if (svargc > 1)
		putbak('\0');
	for (i = 0; keyword[i] != NULL; i++)
		install(keyword[i], "", keytran[i]);
	fcnloc = install("function", "", 0);
	FCN1loc = install("FUNCTION", "", 0);
	yyparse();

	return errorflag;
}

void
cant(char *s)
{
	linect[infptr] = 0;
	curfile[infptr] = s;
	error("can't open");
	exit(1);
}

static void
inclstat(void)
{
	int c;
	char *ps;
	char fname[100];

	while ((c = getchr()) == ' ' || c == '\t')
		continue;
	if (c == '(') {
		for (ps = fname; (*ps=getchr()) != ')'; ps++)
			continue;
		*ps = '\0';
	} else if (c == '"' || c == '\'') {
		for (ps = fname; (*ps = getchr()) != c; ps++)
			continue;
		*ps = '\0';
	} else {
		putbak(c);
		for (ps = fname; (*ps = getchr()) != ' ' && *ps != '\t' && *ps != '\n' && *ps != ';'; ps++)
			continue;
		*ps = '\0';
	}
	if ((infile[++infptr] = fopen(fname, "r")) == NULL) {
		cant(fname);
		/*NOTREACHED*/
	}
	linect[infptr] = 0;
	curfile[infptr] = fname;
}

char	str[5000];
int	nstr;

int
yylex(void)
{
	int c, t;

	for (;;) {
		while ((c = gtok(str)) == ' ' || c == '\n' || c == '\t')
			continue;
		yylval.token = c;
		if (c == ';' || c == '{' || c == '}')
			return c;
		if (c == EOF)
			return 0;
		yylval.strval = str;
		if (c == DIG)
			return DIGITS;
		t = lookup(str)->ydef;
		if (t == DEFINE)
			defstat();
		else if (t == INCLUDE)
			inclstat();
		else if (t > 0)
			return t;
		else
			return GOK;
	}
}

void
yyerror(const char *p)
{
	// ADR: The original code has this as an empty function.
	// I'm not sure why.  Let's try printing the error and
	// see what breaks.
	fprintf(stderr, "%s\n", p);
	fflush(stderr);
}


static void
defstat(void)
{
	int c, i, val, t, nlp;
	extern int nstr;
	extern char str[];

	while ((c = getchr()) == ' ' || c == '\t')
		continue;
	if (c == '(') {
		t = '(';
		while ((c = getchr()) == ' ' || c == '\t')
			continue;
		putbak(c);
	} else {
		t = ' ';
		putbak(c);
	}
	for (nstr = 0; (c = getchr()) != 0; nstr++) {
		if (!isalpha(c) && !isdigit(c))
			break;
		str[nstr] = c;
	}
	putbak(c);
	str[nstr] = '\0';
	if (c != ' ' && c != '\t' && c != '\n' && c != ',') {
		error("illegal define statement");
		return;
	}
	val = nstr + 1;
	if (t == ' ') {
		while ((c = getchr()) == ' ' || c == '\t')
			continue;
		putbak(c);
		for (i = val; (c = getchr()) != '\n' && c != '#' && c != '\0'; i++)
			str[i] = c;
		putbak(c);
	} else {
		while ((c = getchr()) == ' ' || c == '\t' || c == ',' || c == '\n')
			continue;
		putbak(c);
		nlp = 0;
		for (i = val; nlp >= 0 && (c = str[i] = getchr()) != 0; i++) {
			if (c == '(')
				nlp++;
			else if (c == ')')
				nlp--;
		}
		i--;
	}
	for (; i > 0; i--)
		if (str[i - 1] != ' ' && str[i - 1] != '\t')
			break;
	str[i] = '\0';
	install(str, &str[val], 0);
}
