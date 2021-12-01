#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "y.tab.h"

#define	putbak(c)	*ip++ = c
/*	#define	getchr()	(ip>ibuf?*--ip: getc(infile[infptr]))	*/

#define	LET	1
#define	DIG	2
#define	COMMENT	'#'
#define	QUOTE	'"'

extern int	transfer;

#define	INDENT	3	/* indent delta */
#define	CONTFLD	6	/* default position of continuation character */
extern	int	contfld;	/* column for continuation char */
extern	int	contchar;
extern	YYSTYPE	yyval;
/* extern	int	yylval; */
extern	int	errorflag;
extern	bool	f77;
extern	bool	uppercase;
extern	bool	hollerith;

extern	char	comment[];	/* save input comments here */
extern	int	comptr;		/* next free slot in comment */
extern	bool	printcom;	/* print comments, etc., if on */
extern	int	indent;		/* level of nesting for indenting */

extern	char	ibuf[];
extern	char	*ip;

extern	FILE	*outfil;	/* output file id */
extern	FILE	*infile[];
extern	char	*curfile[];
extern	int	infptr;
extern	int	linect[];

extern	char	fcname[];

extern	int	svargc;
extern	char	**svargv;

struct	nlist {
	char	*name;
	char	*def;
	int	ydef;
	struct	nlist *next;
};

extern	struct nlist	*lookup(char *str);
extern	char	*install(char *nam, char *val, int tran);
extern	char	*fcnloc;
extern	char	*FCN1loc;

extern int yylex(void);
extern void yyerror(const char *p);

extern bool nonblank(char *s);
extern char *foldup(char *s);
extern int eatup(void);
extern int genlab(int n);
extern int getchr(void);
extern int gnbtok(char *s);
extern int gtok(char *s);
extern int peek(char *p, char c1);
extern int yyparse(void);
extern void balpar(void);
extern void breakcode(void);
extern void cant(char *s);
extern void contcard(void);
extern void defstat(void);
extern void docode(void);
extern void dostat(int p1);
extern void elsecode(int p1);
extern void endif(void);
extern void endsw(int n, int def);
extern void errcode(void);
extern void error(char *s1);
extern void flushcom(void);
extern void forcode(void);
extern void forstat(int p1);
extern void getcase(void);
extern void getdefault(void);
extern void getfname(void);
extern void gokcode(char *p1);
extern void ifcode(void);
extern void inclstat(void);
extern void nextcode(void);
extern void outcode(char *xp);
extern void outcont(int n);
extern void outdon(void);
extern void outgoto(int n);
extern void outnum(int n);
extern void outtab(void);
extern void pbstr(char *str);
extern void ptc(char c);
extern void pts(char *s);
extern void putcom(char *s);
extern void repcode(void);
extern void retcode(void);
extern void swcode(void);
extern void untils(int p1, int un);
extern void whilecode(void);
extern void whilestat(int p1);
