#include <stdio.h>
#include <stdlib.h>
#include "gst.h"
#include "bst.h"
#include "tnode.h"

typedef struct GSTValue GSTValue;

GSTValue *newGSTValue(void *value,GST *tree);
void  	  incFreq(GSTValue *x);
void 		 	decFreq(GSTValue *x);
int 			getGSTfreq(TNODE *x);
int 			getFreq(GSTValue *x);
void 	   *getGSTValue(GSTValue *x);
int 			gstComparator(void *a,void *b);
void  		gstDisplayer(void *a,FILE *fp);
void 			gstFreedom(void *a);
int  			getInsert(GST *t);

struct GST {
	BST *tree;
	int insertions;
	int (*comparator)(void *,void *);
	void (*display)(void *,FILE *);
	// void (*swapper)(TNODE *,TNODE *);
	void (*freedom)(void *);
};

struct GSTValue {
	void *value;
	int frequency;
	GST *tree;
};

GST *newGST(int (*c)(void *,void *)){
	GST *new = malloc(sizeof(GST));
	new->tree = newBST(gstComparator);
	setBSTdisplay(new->tree,gstDisplayer);
	setBSTfree(new->tree,gstFreedom);
	new->insertions = 0;
	new->comparator = c;
	new->display = NULL;
	// new->swapper = NULL;
	new->freedom = NULL;
	return new;
}

GSTValue *newGSTValue(void *value,GST *tree){
	GSTValue *new = malloc(sizeof(GSTValue));
	new->value = value;
	new->tree = tree;
	new->frequency = 1;
	return new;
}

void setGSTdisplay(GST *t,void (*d)(void *,FILE *)){
	t->display = d;
	// setBSTdisplay(t->tree,d);
}

void setGSTswapper(GST *t,void (*s)(TNODE *,TNODE *)){
	// t->swapper = s;
	setBSTswapper(t->tree,s);
}

void setGSTfree(GST *t,void (*f)(void *)){
	t->freedom = f;
}

TNODE *getGSTroot(GST *t){
	return getBSTroot(t->tree);
}

void setGSTroot(GST *t,TNODE *replacement){
	setBSTroot(t->tree,replacement);
}

void setGSTsize(GST *t,int s){
	setBSTsize(t->tree,s);
}

TNODE *insertGST(GST *t,void *value){
	TNODE *temp = locateGST(t,value);
	if(temp != NULL){
		// printf("insertGST: Got a duplicate.\n");
		incFreq(getTNODEvalue(temp));
		if(t->freedom != NULL){
			// printf("insertGST: deleting created value.\n");
			t->freedom(value);
		}
		t->insertions += 1;
		return NULL;
	}
	else{
		temp = insertBST(t->tree,newGSTValue(value,t));
		return temp;
	}
}

void *findGST(GST *t,void *key){
	TNODE *temp = locateGST(t,key);
	if(temp != NULL){
		GSTValue *x = getTNODEvalue(temp);
		// printf("in findGST: x is %p\n",x->value);
		return x->value;
	}
	else{
		return NULL;
	}
}

TNODE *locateGST(GST *t,void *key){
	GSTValue *temp = newGSTValue(key,t);
	TNODE *pass = locateBST(t->tree,temp);
	free(temp);
	return pass;
}

int deleteGST(GST *t,void *key){
	TNODE *temp = locateGST(t,key);
	if(temp == NULL){
		return -1;
	}
	// GSTValue *y = getTNODEvalue(temp);
	if(getGSTfreq(temp) > 1){
		decFreq(getTNODEvalue(temp));
		t->insertions -= 1;
		// t->freedom(key);
		return getGSTfreq(temp);
	}
	else{
		GSTValue *x = newGSTValue(key,t);
		deleteBST(t->tree,x);
		// free(y);
		// freeTNODE(temp);
		free(x);
		return 0;
	}
}

TNODE *swapToLeafGST(GST *t,TNODE *node){
	return swapToLeafBST(t->tree,node);
}

void pruneLeafGST(GST *t,TNODE *leaf){
	GSTValue *temp = getTNODEvalue(leaf);
	pruneLeafBST(t->tree,leaf);
	free(temp);
}

int sizeGST(GST *t){
	return sizeBST(t->tree);
}

void statisticsGST(GST *t,FILE *fp){
	fprintf(fp,"Duplicates: %d\n",duplicatesGST(t));
	statisticsBST(t->tree,fp);
}

void displayGST(GST *t,FILE *fp){
	displayBST(t->tree,fp);
}

int debugGST(GST *t,int level){
	return debugBST(t->tree,level);
}

void freeGST(GST *t){
	freeBST(t->tree);
	free(t);
}

void *unwrapGST(TNODE *n){
	GSTValue *x = getTNODEvalue(n);
	return x->value;
}

int freqGST(GST *g,void *key){
	TNODE *temp = locateGST(g,key);
	if(temp == NULL){
		return 0;
	}
	else{
		return getGSTfreq(temp);
	}
}

int duplicatesGST(GST *g){
	return g->insertions;
}

int gstComparator(void *a,void *b){
	GSTValue *x = a;
	GSTValue *y = b;
	return x->tree->comparator(x->value,y->value);
}

void gstDisplayer(void *a,FILE *fp){
	GSTValue *x = a;
	x->tree->display(x->value,fp);
	if(x->frequency > 1){
		fprintf(fp,"<%d>",x->frequency);
	}
}

void gstFreedom(void *a){
	// printf("Calling gstFreedom\n");
	GSTValue *x = a;
	if(x->tree->freedom != NULL){
		// printf("gstFreedom wasn't NULL\n");
		x->tree->freedom(x->value);
	}
	free(x);
}

void incFreq(GSTValue *x){
	x->frequency += 1;
}

void decFreq(GSTValue *x){
	x->frequency -= 1;
}

int getGSTfreq(TNODE *x){
	GSTValue *temp = getTNODEvalue(x);
	return temp->frequency;
}

int getFreq(GSTValue *x){
	return x->frequency;
}

void *getGSTValue(GSTValue *x){
	return x->value;
}

int getInsert(GST *t){
	return t->insertions;
}
