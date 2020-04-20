/*Written by Andrew Duerr
 *For Project0
 *In class CS201
 *Taught by Dr. Lusth
 *Last Updated: 9/19/2018		*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cda.h"
#include "queue.h"

struct QUEUE{
	CDA *array;
	int debug;
	void (*display)(void *,FILE *);
	void(*freedom)(void *);
};

QUEUE *newQUEUE(void){
	QUEUE *new = malloc(sizeof(QUEUE));
	assert(new != NULL);
	new->array = newCDA();
	new->debug = 0;
	new->display = NULL;
	new->freedom = NULL;
	return new;
}

void  setQUEUEdisplay(QUEUE *items,void (*d)(void *,FILE *)){
	items->display = d;
	setCDAdisplay(items->array,d);
}

void  setQUEUEfree(QUEUE *items,void (*f)(void *)){
	items->freedom = f;
	setCDAfree(items->array,f);
}

void  enqueue(QUEUE *items,void *value){
	insertCDAback(items->array,value);
}

void *dequeue(QUEUE *items){
	assert(sizeCDA(items->array) > 0);
	return removeCDAfront(items->array);
}

void *peekQUEUE(QUEUE *items){
	assert(sizeCDA(items->array) > 0);
	return getCDA(items->array,0);
}

void  displayQUEUE(QUEUE *items,FILE *fp){
	if(items->debug == 1){
		displayCDA(items->array,fp);
	}
	else if(items->debug == 2){
		int old = debugCDA(items->array,1);
		displayCDA(items->array,fp);
		debugCDA(items->array,old);
	}
	else{
		fprintf(fp,"<");
		if(sizeCDA(items->array) > 0){
			for(int i = 0; i < sizeCDA(items->array)-1; i++){
				items->display(getCDA(items->array,i),fp);
				fprintf(fp,",");
			}
			items->display(getCDA(items->array,sizeCDA(items->array)-1),fp);
		}
		fprintf(fp,">");
	}
}

int   debugQUEUE(QUEUE *items,int level){
	int old = items->debug;
	items->debug = level;
	return old;
}

void  freeQUEUE(QUEUE *items){
	freeCDA(items->array);
	free(items);
}

int   sizeQUEUE(QUEUE *items){
	return sizeCDA(items->array);
}
