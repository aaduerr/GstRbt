#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bst.h"
#include "tnode.h"
#include "queue.h"

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

struct BST {
	int size;
	int debug;
	TNODE *root;
	int (*comparator)(void *,void *);
	void (*display)(void *,FILE *);
	void (*swapper)(TNODE *,TNODE *);
	void (*freedom)(void *);
};

BST *newBST(int (*c)(void *,void *)){
	BST *new = malloc(sizeof(BST));
	new->size = 0;
	new->debug = 0;
	new->root = NULL;
	new->comparator = c;
	new->display = NULL;
	new->swapper = swapperBSTdefault;
	new->freedom = NULL;
	return new;
}

void setBSTdisplay(BST *t,void (*d)(void *,FILE *)){
	t->display = d;
}

void setBSTswapper(BST *t,void (*s)(TNODE *,TNODE *)){
	t->swapper = s;
}

void setBSTfree(BST *t,void (*f)(void *)){
	t->freedom = f;
}

TNODE *getBSTroot(BST *t){
	return t->root;
}

void setBSTroot(BST *t,TNODE *replacement){
	t->root = replacement;
}

void   setBSTsize(BST *t,int s){
	t->size = s;
}

TNODE *insertBST(BST *t,void *value){
	// printf("insertBST: Starting size: %d\n",sizeBST(t));
	TNODE *temp = getBSTroot(t);
	TNODE *temp2 = NULL;
	if(temp == NULL){
		setBSTroot(t,newTNODE(value,NULL,NULL,NULL));
		setTNODEdisplay(getBSTroot(t),t->display);
		// setTNODEfree(getBSTroot(t),t->freedom);
		t->size += 1;
		// printf("insertBST: ending size: %d\n",sizeBST(t));
		return getBSTroot(t);
	}
	else{
		while(temp != NULL){
			if(t->comparator(getTNODEvalue(temp),value) > 0){
				if(getTNODEleft(temp) == NULL){
					setTNODEleft(temp,newTNODE(value,NULL,NULL,temp));
					setTNODEdisplay(getTNODEleft(temp),t->display);
					// setTNODEfree(getTNODEleft(temp),t->freedom);
					temp2 = getTNODEleft(temp);
					temp = NULL;
				}
				else{
					temp = getTNODEleft(temp);
				}
			}
			else{
				if(getTNODEright(temp) == NULL){
					setTNODEright(temp,newTNODE(value,NULL,NULL,temp));
					setTNODEdisplay(getTNODEright(temp),t->display);
					// setTNODEfree(getTNODEright(temp),t->freedom);
					temp2 = getTNODEright(temp);
					temp = NULL;
				}
				else{
					temp = getTNODEright(temp);
				}
			}
		}
		t->size += 1;
		// printf("insertBST: ending size: %d\n",sizeBST(t));
		return temp2;
	}
}

void *findBST(BST *t,void *key){
	// printf("Calling findBST...\n");
	TNODE *temp = t->root;
	while(temp != NULL && t->comparator(getTNODEvalue(temp),key) != 0){
		if(t->comparator(getTNODEvalue(temp),key) > 0){
			temp = getTNODEleft(temp);
		}
		else{
			temp = getTNODEright(temp);
		}
	}

	if(temp == NULL){
		return NULL;
	}
	else{
		return getTNODEvalue(temp);
	}
}

TNODE *locateBST(BST *t,void *key){
	// printf("Calling locateBST...\n");
	TNODE *temp = t->root;
	while(temp != NULL && t->comparator(getTNODEvalue(temp),key) != 0){
		if(t->comparator(getTNODEvalue(temp),key) > 0){
			temp = getTNODEleft(temp);
		}
		else{
			temp = getTNODEright(temp);
		}
	}
	return temp;
}

int deleteBST(BST *t,void *key){
	// printf("Calling deleteBST...\n");
	// printf("Starting size: %d\n",sizeBST(t));
	TNODE *temp = NULL;
	if(t->size == 1){
		// printf("deleteBST: Size is 1.\n");
		temp = t->root;
		t->root = NULL;
		t->size = 0;
		setTNODEfree(temp,t->freedom);
		freeTNODE(temp);
		// free(temp);
	}
	else{
		// printf("deleteBST: Size greater than 1\n");
		temp = swapToLeafBST(t,locateBST(t,key));
		pruneLeafBST(t,temp);
		setTNODEfree(temp,t->freedom);
		freeTNODE(temp);
		t->size -= 1;
	}
	// printf("Ending size: %d\n",sizeBST(t));
	if(temp != NULL){
		return 0;
	}
	else{
		return -1;
	}
}

TNODE *swapToLeafBST(BST *t,TNODE *node){
	// printf("Calling swapToLeafBST...\n");
	TNODE *temp = NULL;
	// void *value = getTNODEvalue(node);
	if(getTNODEleft(node) == NULL && getTNODEright(node) == NULL){
		return node;
	}
	else if(getTNODEleft(node) != NULL){
		temp = maximum(getTNODEleft(node));
	}
	else{
		temp = minimum(getTNODEright(node));
	}
	// setTNODEvalue(node,getTNODEvalue(temp));
	// setTNODEvalue(temp,value);
	t->swapper(node,temp);
	return swapToLeafBST(t,temp);
}

void pruneLeafBST(BST *t,TNODE *leaf){
	// printf("Calling pruneLeafBST...\n");
	assert(sizeBST(t) > 0);
	// if(sizeBST(t) > 0){
		if(getBSTroot(t) == leaf){
			setBSTroot(t,NULL);
		}
		else if(getTNODEleft(getTNODEparent(leaf)) == leaf){
			setTNODEleft(getTNODEparent(leaf),NULL);
		}
		else{
			assert(getTNODEright(getTNODEparent(leaf)) == leaf);
			setTNODEright(getTNODEparent(leaf),NULL);
		}
	// }
	// free(leaf);
}

int sizeBST(BST *t){
	return t->size;
}

void statisticsBST(BST *t,FILE *fp){
	fprintf(fp,"Nodes: %d\n",t->size);
	fprintf(fp,"Minimum depth: %d\n",minDepth(t->root));
	fprintf(fp,"Maximum depth: %d\n",maxDepth(t->root));
}

void displayBST(BST *t,FILE *fp){
	if(t->debug == 1){
		printf("[");
		if(getBSTroot(t) != NULL){
			inorder(getBSTroot(t));
		}
		printf("]");
	}
	else if(t->debug == 2){
		printf("[");
		if(getBSTroot(t) != NULL){
			preorder(getBSTroot(t));
		}
		printf("]");
	}
	else if(t->debug == 3){
		printf("[");
		if(getBSTroot(t) != NULL){
			postorder(getBSTroot(t));
		}
		printf("]");
	}
	else{
		TNODE *temp = NULL;
		QUEUE *print = newQUEUE();
		int level = 1;
		enqueue(print,getBSTroot(t));
		enqueue(print,NULL);
		temp = peekQUEUE(print);
		fprintf(fp,"0: ");
		while(sizeQUEUE(print) != 0){
			temp = peekQUEUE(print);
			if(temp != NULL){
				if(getTNODEleft(temp) == NULL && getTNODEright(temp) == NULL){
					fprintf(fp,"=");
				}
				displayTNODE((TNODE *)dequeue(print),fp);
				TNODE *parent = getTNODEparent(temp);
				if(parent == NULL){
					fprintf(fp,"(");
					displayTNODE(temp,fp);
					fprintf(fp,")X");
				}
				else{
					fprintf(fp,"(");
					displayTNODE(parent,fp);
					if(temp == getTNODEleft(parent)){
						fprintf(fp,")L");
					}
					else{
						fprintf(fp,")R");
					}
				}
				if(peekQUEUE(print) != NULL){
					fprintf(fp," ");
				}
				if(getTNODEleft(temp) != NULL){
					enqueue(print,getTNODEleft(temp));
				}
				if(getTNODEright(temp) != NULL){
					enqueue(print,getTNODEright(temp));
				}
			}
			else{
				dequeue(print);
				if(sizeBST(t) > 0 && sizeQUEUE(print) > 0){
					fprintf(fp,"\n");
					fprintf(fp,"%d: ",level);
					level += 1;
					enqueue(print,NULL);
				}
			}
		}
		fprintf(fp,"\n");
		freeQUEUE(print);
	}

}

int debugBST(BST *t,int level){
	int old = t->debug;
	t->debug = level;
	return old;
}

void freeBST(BST *t){
	QUEUE *freeing = newQUEUE();
	TNODE *temp = NULL;
	enqueue(freeing,getBSTroot(t));
	while(sizeQUEUE(freeing) != 0){
		if(peekQUEUE(freeing) != NULL){
			enqueue(freeing,getTNODEleft(peekQUEUE(freeing)));
			enqueue(freeing,getTNODEright(peekQUEUE(freeing)));
		}
		temp = dequeue(freeing);
		if(temp != NULL){
			setTNODEfree(temp,t->freedom);
			freeTNODE(temp);
		}
	}
	freeQUEUE(freeing);
	free(t);
}

//==============  additional functions =================

void swapperBSTdefault(TNODE *a, TNODE *b){
	void *x = getTNODEvalue(a);
	void *y = getTNODEvalue(b);
	setTNODEvalue(a,y);
	setTNODEvalue(b,x);
}

TNODE *minimum(TNODE *x){
	if(getTNODEleft(x) != NULL) {
		return minimum(getTNODEleft(x));
	}
	else{
		return x;
	}
}

TNODE *maximum(TNODE *x){
	if(getTNODEright(x) != NULL) {
		return maximum(getTNODEright(x));
	}
	else{
		return x;
	}
}

int minDepth(TNODE *x){
	if (x == NULL){
		return -1;
	}
	else if(getTNODEleft(x) != NULL && getTNODEright(x) != NULL){
		return min(minDepth(getTNODEleft(x)),minDepth(getTNODEright(x))) + 1;
	}
	else{
		return 0;
	}
}

int maxDepth(TNODE *x){
	if (x == NULL){
		return -1;
	}
	if (getTNODEleft(x) == NULL && getTNODEright(x) == NULL){
		return 0;
	}
	if (!getTNODEleft(x)){
		return maxDepth(getTNODEright(x)) + 1;
	}
	if (!getTNODEright(x)){
		return maxDepth(getTNODEleft(x)) + 1;
	}
	return max(maxDepth(getTNODEleft(x)), maxDepth(getTNODEright(x))) + 1;
}

void inorder(TNODE *x){
	if(getTNODEleft(x) != NULL){
		printf("[");
		inorder(getTNODEleft(x));
		printf("] ");
	}
	displayTNODE(x,stdout);
	if(getTNODEright(x) != NULL){
		printf(" [");
		inorder(getTNODEright(x));
		printf("]");
	}
}

void preorder(TNODE *x){
	displayTNODE(x,stdout);
	if(getTNODEleft(x) != NULL){
		printf(" [");
		preorder(getTNODEleft(x));
		printf("]");
	}
	if(getTNODEright(x) != NULL){
		printf(" [");
		preorder(getTNODEright(x));
		printf("]");
	}
}

void postorder(TNODE *x){
	if(getTNODEleft(x) != NULL){
		printf("[");
		postorder(getTNODEleft(x));
		printf("] ");
	}
	if(getTNODEright(x) != NULL){
		printf("[");
		postorder(getTNODEright(x));
		printf("] ");
	}
	displayTNODE(x,stdout);
}
