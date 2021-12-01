%{
#include "r.h"
%}

%union {
	int token;
	char *strval;
}

%term	IF ELSE FOR WHILE BREAK NEXT 
%term	DO
%term	DEFINE INCLUDE
%term	REPEAT UNTIL
%term	RETURN
%term	SWITCH CASE DEFAULT
%term	GOK DIGITS 

%type <token>	IF ELSE FOR WHILE BREAK NEXT 
%type <token>	DO
%type <token>	DEFINE INCLUDE
%type <token>	REPEAT UNTIL
%type <token>	RETURN
%type <token>	SWITCH CASE DEFAULT
%type <strval>	GOK DIGITS 

%type <token>	if ifelse switch while for repeat do fullcase
%%

statl	: statl  stat
	|
	;
stat	: if stat	{ indent--; if (f77) endif(); else outcont($1); }
	| ifelse stat	{ indent--; if (f77) endif(); else outcont($1+1); }
	| switch fullcase '}'	{ endsw($1, $2); }
	| while stat	{ whilestat($1); }
	| for stat	{ forstat($1); }
	| repeat stat UNTIL	{ untils($1,1); }
	| repeat stat		{ untils($1,0); }
	| BREAK	{ breakcode(); }
	| NEXT		{ nextcode(); }
	| do stat	{ dostat($1); }
	| GOK		{ gokcode($1); }
	| RETURN	{ retcode(); }
	| ';'
	| '{' statl '}'
	| label stat
	| error		{ errcode(); yyclearin; }
	;
switch	: sw '{'
	;
sw	: SWITCH	{ swcode(); }
	;
fullcase: caselist	{ $$ = 0; }
	| caselist defpart	{ $$ = 1; }
	;
caselist: casepart
	| caselist casepart
	;
defpart	: default statl
	;
default	: DEFAULT	{ getdefault(); }
	;
casepart: case statl
	;
case	: CASE	{ getcase(); }
	;
label	: DIGITS	{ transfer = 0; outcode($1); }
	;
if	: IF		{ ifcode(); }
	;
ifelse	: if stat ELSE	{ elsecode($1); }
	;
while	: WHILE	{ whilecode(); }
	;
for	: FOR		{ forcode(); }
	;
repeat	: REPEAT	{ repcode(); }
	;
do	: DO		{ docode(); }
	;
%%
