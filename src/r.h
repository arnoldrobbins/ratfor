#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
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
extern	int	dbg;
extern	int	yyval;
extern	int	*yypv;
extern	int	yylval;
extern	int	errorflag;
extern	bool	f77;

extern	char	comment[];	/* save input comments here */
extern	int	comptr;	/* next free slot in comment */
extern	bool	printcom;	/* print comments, etc., if on */
extern	int	indent;	/* level of nesting for indenting */

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

#define EOS 0
#define	HSHSIZ	101
struct	nlist {
	char	*name;
	char	*def;
	int	ydef;
	struct	nlist *next;
};

struct nlist	*lookup();
char	*install();
extern	char	*fcnloc;
extern	char	*FCN1loc;

extern void cant(char *s);
extern void inclstat(void);
extern void defstat(void);
extern int yyparse(void);
extern void swcode(void);
extern void getcase(void);
extern void putcom(char *s);
extern void error(char *s1);
extern int genlab(int n);
extern void outcode(char *xp);
extern void outnum(int n);
extern void balpar(void);
extern void outdon(void);
extern void outgoto(int n);
extern void outcont(int n);
extern int gtok(char *s);
extern void getdefault(void);
extern int gnbtok(char *s);
extern void endsw(int n, int def);
extern void repcode(void);
extern void untils(int p1, int un);
extern void outtab(void);
extern void ifcode(void);
extern void elsecode(int p1);
extern void endif(char *s);
extern void whilecode(void);
extern void whilestat(int p1);
extern void pbstr(char *str);
extern void gokcode(char *p1);
extern int eatup(void);
extern void forcode(void);
extern void forstat(int p1);
extern void retcode(void);
extern bool nonblank(char *s);
extern void docode(void);
extern void dostat(int p1);
extern void breakcode(void);
extern void nextcode(void);
extern void errcode(void);
extern int getchr(void);
