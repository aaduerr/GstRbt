#include <stdio.h>
#include <stdlib.h>
#include "rbt.h"
#include "gst.h"
#include "tnode.h"

typedef struct RBTValue RBTValue;

RBTValue *newRBTValue(void *value,RBT *t);
void 	insertionFixup(RBT *t,TNODE *x);
void 	deletionFixup(RBT *t,TNODE *x);
void 	rotateRight(TNODE *x);
void 	rotateLeft(TNODE *x);
int 	rbtComparator(void *a, void *b);
void 	rbtDisplayer(void *x,FILE *fp);
void 	rbtSwapper(TNODE *a,TNODE *b);
void 	rbtSwapperWithColor(TNODE *a,TNODE *b);
int  	isRed(TNODE *a);
void 	colorBlack(TNODE *a);
void 	colorRed(TNODE *a);
int 	isLeftChild(TNODE *x);
TNODE *uncle(TNODE *x);
TNODE *grandparent(TNODE *x);
int 	isLinear(TNODE *x);
TNODE *sibling(TNODE *x);
TNODE *niece(TNODE *x);
TNODE *nephew(TNODE *x);
GST   *getTree(RBT *t);
void  rbtFreedom(void *a);

struct RBT {
	GST *tree;
	int (*comparator)(void *,void *);
	void (*display)(void *,FILE *);
	void (*swapper)(TNODE *,TNODE *);
	void (*freedom)(void *);
};

struct RBTValue {
	void *value;
	int color; //0 for black, 1 for red
	RBT *tree;
};

RBT *newRBT(int (*c)(void *,void *)){
	RBT *new = malloc(sizeof(RBT));
	new->tree = newGST(rbtComparator);
	setGSTdisplay(new->tree,rbtDisplayer);
	setGSTswapper(new->tree,rbtSwapper);
	setGSTfree(new->tree,rbtFreedom);
	new->comparator = c;
	new->display = NULL;
	new->swapper = rbtSwapper;
	new->freedom = NULL;
	// printf("size of rbt value is: %d\n",(int)sizeof(RBTValue));
	return new;
}

RBTValue *newRBTValue(void *value,RBT *t){
	RBTValue *new = malloc(sizeof(RBTValue));
	new->value = value;
	new->color = 1;
	new->tree = t;
	// printf("allocated RBTValue %p\n",new);
	return new;
}

void setRBTdisplay(RBT *t,void (*d)(void *,FILE *)){
	t->display = d;
}

void setRBTswapper(RBT *t,void (*s)(TNODE *,TNODE *)){
	t->swapper = s;
}

void setRBTfree(RBT *t,void (*f)(void *)){
	t->freedom = f;
}

TNODE *getRBTroot(RBT *t){
	return getGSTroot(t->tree);
}

void setRBTroot(RBT *t,TNODE *replacement){
	setGSTroot(t->tree,replacement);
}

void setRBTsize(RBT *t,int s){
	setGSTsize(t->tree,s);
}

TNODE *insertRBT(RBT *t,void *value){
	TNODE *temp = NULL;
	temp = insertGST(t->tree,newRBTValue(value,t));
	if(temp != NULL){
		insertionFixup(t,temp);
	}
	return temp;
}

void *findRBT(RBT *t,void *key){
	TNODE *temp = locateRBT(t,key);
	if(temp != NULL){
		RBTValue *x = unwrapGST(temp);
		return x->value;
	}
	else{
		return NULL;
	}
}

TNODE *locateRBT(RBT *t,void *key){
	RBTValue *value = newRBTValue(key,t);
	TNODE *pass = locateGST(t->tree,value);
	// printf("locateRBT: Freeing %p\n",value);
	free(value);
	return pass;
}

int deleteRBT(RBT *t,void *key){
	TNODE *temp = locateRBT(t,key);
	if(temp == NULL){
		return -1;
	}
	// RBTValue *value = newRBTValue(key,t);
	RBTValue *y = unwrapGST(temp);
	if(freqGST(t->tree,y) > 1){
		return deleteGST(t->tree,y);
	}
	else{
		TNODE *n = swapToLeafRBT(t,temp);
		deletionFixup(t,n);
		pruneLeafGST(t->tree,n);
		setRBTsize(t,sizeRBT(t)-1);
		// printf("The node being deleted is: ");
		// displayTNODE(n,stdout);
		// printf("\n");
		freeTNODE(n);
		// printf("deleteRBT: Freeing %p\n",y);
		free(y);
		// free(value);
		return 0;
	}
}

TNODE *swapToLeafRBT(RBT *t,TNODE *node){
	return swapToLeafGST(t->tree,node);
}

void pruneLeafRBT(RBT *t,TNODE *leaf){
	pruneLeafGST(t->tree,leaf);
}

int sizeRBT(RBT *t){
	return sizeGST(t->tree);
}

void statisticsRBT(RBT *t,FILE *fp){
	statisticsGST(t->tree,fp);
}

void displayRBT(RBT *t,FILE *fp){
	displayGST(t->tree,fp);
}

int debugRBT(RBT *t,int level){
	return debugGST(t->tree,level);
}

void freeRBT(RBT *t){
	freeGST(t->tree);
	free(t);
}

void *unwrapRBT(TNODE *n){
	if(n != NULL){
		RBTValue *x = unwrapGST(n);
		return x->value;
	}
	else{
		return NULL;
	}
}

int freqRBT(RBT *g,void *key){
	RBTValue *value = newRBTValue(key,g);
	int pass = freqGST(g->tree,value);
	free(value);
	return pass;
}

int duplicatesRBT(RBT *g){
	return duplicatesGST(g->tree);
}

//=================== new stuff =========================

void insertionFixup(RBT *t,TNODE *a){
	// printf("Calling insertionFixup...\n");
	TNODE *x = a;
	while(1){
		if(x == getRBTroot(t)){
			break;
		}
		if(isRed(getTNODEparent(x)) == 0){
			break;
		}
		if(isRed(uncle(x)) == 1){
			// printf("IF: Uncle was Red\n");
			colorBlack(getTNODEparent(x));
			colorBlack(uncle(x));
			colorRed(grandparent(x));
			x = grandparent(x);
		}
		else{
			// printf("IF: Uncle is black.\n");
			if(isLinear(x) == 0){
				// printf("IF: Wasn't linear.\n");
				if(isLeftChild(x) == 1){
					// printf("Left child\n");
					rotateRight(x);
				}
				else{
					// printf("Right Child\n");
					rotateLeft(x);
				}
			}
			colorBlack(getTNODEparent(x));
			colorRed(grandparent(x));
			if(isLeftChild(getTNODEparent(x))){
				rotateRight(getTNODEparent(x));
			}
			else{
				rotateLeft(getTNODEparent(x));
			}
			break;
		}
	}
	colorBlack(getRBTroot(t));
}

void deletionFixup(RBT *t,TNODE *x){
	while(1){
		if(x == getRBTroot(t)){
			break;
		}
		if(isRed(x) == 1){
			break;
		}
		if(isRed(sibling(x)) == 1){
			colorRed(getTNODEparent(x));
			colorBlack(sibling(x));
			if(isLeftChild(x)){
				rotateLeft(sibling(x));
			}
			else{
				rotateRight(sibling(x));
			}
		}
		else if(isRed(nephew(x)) == 1){
			if(isRed(getTNODEparent(x)) == 1){
				colorRed(sibling(x));
			}
			else{
				colorBlack(sibling(x));
			}
			colorBlack(getTNODEparent(x));
			colorBlack(nephew(x));
			if(isLeftChild(x) == 1){
				rotateLeft(sibling(x));
			}
			else{
				rotateRight(sibling(x));
			}
			break;
		}
		else if(isRed(niece(x)) == 1){
			// printf("DF: Had a red niece.\n");
			colorBlack(niece(x));
			colorRed(sibling(x));
			if(isLeftChild(x) == 1){
				rotateRight(niece(x));
			}
			else{
				rotateLeft(niece(x));
			}
		}
		else{
			colorRed(sibling(x));
			x = getTNODEparent(x);
		}
	}
	colorBlack(x);
}

void rotateRight(TNODE *x){
	// printf("Rotating right.\n");
	TNODE *parent = getTNODEparent(x);
	if(x == NULL || parent == NULL) return;
	else{
		if(getTNODEleft(x) != NULL){
			setTNODEparent(getTNODEleft(x),parent);
		}
		setTNODEleft(parent,getTNODEleft(x));
		setTNODEleft(x,getTNODEright(x));
		setTNODEright(x,getTNODEright(parent));
		if(getTNODEright(parent) != NULL){
			setTNODEparent(getTNODEright(parent),x);
		}
		setTNODEright(parent,x);
		rbtSwapperWithColor(x,parent);
	}
	// printf("Finished rotate.\n");
}

void rotateLeft(TNODE *x){
	// printf("Rotating left.\n");
	TNODE *parent = getTNODEparent(x);
	if(x == NULL || parent == NULL) return;
	else{
		if(getTNODEright(x) != NULL){
			setTNODEparent(getTNODEright(x),parent);
		}
		setTNODEright(parent,getTNODEright(x));
		setTNODEright(x,getTNODEleft(x));
		setTNODEleft(x,getTNODEleft(parent));
		if(getTNODEleft(parent) != NULL){
			setTNODEparent(getTNODEleft(parent),x);
		}
		setTNODEleft(parent,x);
		rbtSwapperWithColor(x,parent);
	}
}

int rbtComparator(void *a,void *b){
	RBTValue *x = a;
	RBTValue *y = b;
	return x->tree->comparator(x->value,y->value);
}

void rbtDisplayer(void *a,FILE *fp){
	RBTValue *x = a;
	x->tree->display(x->value,fp);
	if(x->color == 1){
		fprintf(fp,"*");
	}
}

void rbtSwapper(TNODE *a,TNODE *b){
	void *va = getTNODEvalue(a);   //get the GST value
	void *vb = getTNODEvalue(b);   //get the GST value
	// swap the GST values
	setTNODEvalue(a,vb);
	setTNODEvalue(b,va);
	// the above swap swapped the colors,
	// but the colors should stay with the nodes,
	// so swap the colors back to the original nodes
	RBTValue *x = unwrapGST(a);
	RBTValue *y = unwrapGST(b);
	int color = x->color;
	x->color = y->color;
	y->color = color;
}

void rbtSwapperWithColor(TNODE *a,TNODE *b){
	void *va = getTNODEvalue(a);   //get the GST value
	void *vb = getTNODEvalue(b);   //get the GST value
	// swap the GST values
	setTNODEvalue(a,vb);
	setTNODEvalue(b,va);
}

int isRed(TNODE *a){
	if(a == NULL){
		return 0;
	}
	else{
		RBTValue *x = unwrapGST(a);
		return x->color;
	}
}

void colorBlack(TNODE *a){
	if(a != NULL){
		RBTValue *x = unwrapGST(a);
		x->color = 0;
		// printf("Successful coloring of black.\n");
	}
}

void colorRed(TNODE *a){
	if(a != NULL){
		RBTValue *x = unwrapGST(a);
		x->color = 1;
	}
}

int isLeftChild(TNODE *x){
	if(getTNODEleft(getTNODEparent(x)) == x){
		return 1;
	}
	else{
		return 0;
	}
}

TNODE *uncle(TNODE *x){
	if(isLeftChild(getTNODEparent(x)) == 1){
		return getTNODEright(grandparent(x));
	}
	else{
		return getTNODEleft(grandparent(x));
	}
}

TNODE *grandparent(TNODE *x){
	if(getTNODEparent(x) != NULL){
		return getTNODEparent(getTNODEparent(x));
	}
	else{
		return NULL;
	}
}

int isLinear(TNODE *x){
	if(isLeftChild(x) == isLeftChild(getTNODEparent(x))){
		return 1;
	}
	else{
		return 0;
	}
}

TNODE *sibling(TNODE *x){
	if(isLeftChild(x) == 1){
		return getTNODEright(getTNODEparent(x));
	}
	else{
		return getTNODEleft(getTNODEparent(x));
	}
}

TNODE *niece(TNODE *x){
	if(isLeftChild(x) == 1){
		return getTNODEleft(sibling(x));
	}
	else{
		return getTNODEright(sibling(x));
	}
}

TNODE *nephew(TNODE *x){
	if(isLeftChild(x) == 0){
		return getTNODEleft(sibling(x));
	}
	else{
		return getTNODEright(sibling(x));
	}
}

GST *getTree(RBT *t){
	return t->tree;
}

void rbtFreedom(void *a){
	// printf("Calling rbtFreedom\n");
	RBTValue *x = a;
	if(x->tree->freedom != NULL){
		// printf("rbtFreedom wasn't NULL\n");
		x->tree->freedom(x->value);
	}
	// printf("rbtFreedom: Freeing %p\n",x);
	free(x);
}
