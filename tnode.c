#include <stdio.h>
#include <stdlib.h>
#include "tnode.h"

struct TNODE {
	void *value;
	TNODE *left;
	TNODE *right;
	TNODE *parent;
	// int count;
	int debug;
	void (*display)(void *,FILE *);
	void (*freedom)(void *);
};

TNODE *newTNODE(void *v,TNODE *l,TNODE *r,TNODE *p){
	TNODE *new = malloc(sizeof(TNODE));
	new->value = v;
	new->left = l;
	new->right = r;
	new->parent = p;
	// new->count = 1;
	new->debug = 0;
	new->display = NULL;
	new->freedom = NULL;
	return new;
}

void   setTNODEdisplay(TNODE *n,void (*d)(void *value,FILE *fp)){
		n->display = d;
}

void   setTNODEfree(TNODE *n,void (*f)(void *value)){
		n->freedom = f;
}

void  *getTNODEvalue(TNODE *n){
	if(n != NULL){
		return n->value;
	}
	else{
		return NULL;
	}
}

void   setTNODEvalue(TNODE *n,void *replacement){
		n->value = replacement;
}

TNODE *getTNODEleft(TNODE *n){
	if(n != NULL){
		return n->left;
	}
	else{
		return NULL;
	}
}

void   setTNODEleft(TNODE *n,TNODE *replacement){
		n->left = replacement;
}

TNODE *getTNODEright(TNODE *n){
	if(n != NULL){
		return n->right;
	}
	else{
		return NULL;
	}
}

void   setTNODEright(TNODE *n,TNODE *replacement){
		n->right = replacement;
}

TNODE *getTNODEparent(TNODE *n){
	if(n != NULL){
		return n->parent;
	}
	else{
		return NULL;
	}
}

void   setTNODEparent(TNODE *n,TNODE *replacement){
		n->parent = replacement;
}

void displayTNODE(TNODE *n,FILE *fp){
	n->display(n->value,fp);
	if(n->debug > 0){
		fprintf(fp,"&");
		fprintf(fp,"%p",n);
	}
}

int    debugTNODE(TNODE *n,int level){
	int old = n->debug;
	n->debug = level;
	return old;
}

void freeTNODE(TNODE *n){
	// printf("Calling freeTNODE\n");
	if(n->freedom != NULL){
		// printf("n->freedom wasn't NULL\n");
		n->freedom(n->value);
	}
		free(n);
}
