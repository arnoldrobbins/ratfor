#include <stdlib.h>
#include <string.h>

#define	HSHSIZ	101
struct	nlist {
	char	*name;
	char	*def;
	int	ydef;
	struct	nlist *next;
};

struct	nlist	*hshtab[HSHSIZ];
struct nlist	*lookup();
int	hshval;

struct nlist *
lookup(char *str)
{
	char *s1;
	struct nlist *np;
	static struct nlist nodef;

	s1 = str;
	for (hshval = 0; *s1; )
		hshval += *s1++;
	hshval %= HSHSIZ;
	for (np = hshtab[hshval]; np != NULL; np = np->next) {
		if (strcmp(str, np->name) == 0)
			return np;
	}
	return &nodef;
}

char *
install(char *nam, char *val, int tran)
{
	struct nlist *np;

	if ((np = lookup(nam))->name == NULL) {
		np = (struct nlist *) malloc(sizeof(*np));
		np->name = strdup(nam);
		np->def = strdup(val);
		np->ydef = tran;
		np->next = hshtab[hshval];
		hshtab[hshval] = np;
		return np->def;
	}
	free(np->def);
	np->def = strdup(val);
	return np->def;
}
