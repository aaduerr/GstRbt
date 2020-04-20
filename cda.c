/*Written by Andrew Duerr
 *For Project0
 *In class CS201
 *Taught by Dr. Lusth	*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cda.h"

struct CDA{
	int startIndex;
	int endIndex;
	int size;
	int capacity;
	int debug;
	void **data;
	void (*display)(void *,FILE *);
	void (*freedom)(void *);
};

CDA *newCDA(void){
	CDA *new = malloc(sizeof(CDA));
	new->size = 0;
	new->capacity = CDA_INITIAL_CAPACITY;
	new->debug = 0;
	new->startIndex = 0;
	new->endIndex = 0;
	new->data = malloc(sizeof(void *) * new->capacity);
	new->display = NULL;
	new->freedom = NULL;
	return new;
}

void setCDAdisplay(CDA *items, void (*d)(void *,FILE *)){
	items->display = d;
}

void setCDAfree(CDA *items, void (*f)(void *)){
	items->freedom = f;
}

void insertCDA(CDA *items,int index,void *value){
	assert(index <= items->size && index >= 0);
	if(items->size < items->capacity){
		if(index == items->size){
			if(items->size == 0){ //nothing in array, reset
				items->data[0] = value;
				items->startIndex = 0;
				items->endIndex = 0;
				items->size += 1;
			}
			else{ //insert at back
				items->endIndex = (items->endIndex+1)%items->capacity;
				items->size += 1;
				items->data[items->endIndex] = value;
			}
		}
		else if(index == 0){ //insert at front
			items->startIndex = (items->startIndex + items->capacity - 1)%items->capacity;
			items->data[items->startIndex] = value;
			items->size += 1;
		}
		else{ //insert somewhere inbetween
			if(index < items->size/2){
				for(int i = -1; i < index; i++){
					setCDA(items,i,getCDA(items,i+1));
				}
			}
			else{
				for(int i = items->size; i > index; i--){
					setCDA(items,i,getCDA(items,i-1));
				}
			}
			setCDA(items,index,value);
		}
	}
	else{ //grow
		// printf("Growing...\n");
		void **temp = malloc(sizeof(void *) * (items->capacity * 2));
		for(int i = 0; i < items->size; i++){
			temp[i] = getCDA(items,i);
		}
		free(items->data);
		items->capacity *= 2;
		items->data = temp;
		items->startIndex = 0;
		items->endIndex = items->size-1;
		insertCDA(items,index,value);
	}
}

void *removeCDA(CDA *items,int index){
	assert(index < items->size && index >= 0);
	assert(items->size > 0);
	void *old = getCDA(items,index);
	if(index == 0){
		items->startIndex = (items->startIndex + 1) % items->capacity;
	}
	else if(index == items->size-1){
		items->endIndex = (items->endIndex + items->capacity - 1) % items->capacity;
	}
	else{
		if(index <= items->size / 2){ //removal on left half
			for(int i = index; i > 0; i--){
				setCDA(items,i,getCDA(items,i-1));
			}
			items->startIndex = (items->startIndex + 1) % items->capacity;
		}
		else{ //removal on right half
			for(int j = index; j < items->size-1; j++){
				setCDA(items,j,getCDA(items,j+1));
			}
			items->endIndex = (items->endIndex - 1 + items->capacity) % items->capacity;
		}
	}
	items->size -= 1;
	if((double)((double)items->size/(double)items->capacity) < 0.25){ //shrink
		// printf("Shrinking...\n"); //debug
		int newcap = items->capacity / 2;
		if(newcap <= 0){newcap = 1;}
		if(items->size == 0){
		 	newcap = 1;
		}
		if(items->startIndex == 0){
			// printf("Index at 0.\n");
			items->data = realloc(items->data,sizeof(void *) * (newcap));
			items->capacity = newcap;
		}
		else{
			// printf("Index not at 0.\n");
			void **temp = malloc(sizeof(void *) * (newcap));
			for(int i = 0; i < items->size; i++){
					temp[i] = getCDA(items,i);
			}
			items->startIndex = 0;
			items->endIndex = items->size-1;
			items->capacity = newcap;
			free(items->data);
			items->data = temp;
		}
		// printf("startIndex[%d]\tendIndex[%d]\n",items->startIndex,items->endIndex);
	}
	return old;
}

void unionCDA(CDA *recipient,CDA *donor){
	for(int i = 0; i < donor->size; i++){
		insertCDAback(recipient,getCDA(donor,i));
	}
	free(donor->data);
	donor->data = malloc(sizeof(void *));
	donor->size = 0;
	donor->capacity = 1;
}

void *getCDA(CDA *items,int index){
	assert(index >= 0 && index <= items->size);
	return items->data[(index + items->startIndex)%items->capacity];
}

void *setCDA(CDA *items,int index,void *value){
	assert(index >= -1 && index <= items->size);
	void *temp = NULL;
	if(index == -1){
		insertCDAfront(items,value);
	}
	else if(index == items->size){
		insertCDAback(items,value);
	}
	else{
		temp = getCDA(items,index);
		items->data[(index+items->startIndex)%items->capacity] = value;
	}
	return temp;
}

int sizeCDA(CDA *items){
	return items->size;
}

int capCDA(CDA *items){
	return items->capacity;
}

void displayCDA(CDA *items,FILE *fp){
	fprintf(fp,"(");
	if(items->size > 0){
		for(int i = 0; i < items->size-1; i++){
			if(items->display == NULL){
				fprintf(fp,"%p",getCDA(items,i));
			}
			else{
				items->display(getCDA(items,i),fp);
			}
			fprintf(fp,",");
			// fprintf(fp,"\n"); //debug
		}
		if(items->display == NULL){
			fprintf(fp,"%p",getCDA(items,items->size-1));
		}
		else{
			items->display(getCDA(items,items->size-1),fp);
		}
		if(items->debug != 0) fprintf(fp,",");
	}
	if(items->debug != 0){
		fprintf(fp,"(%d)",items->capacity - items->size);
	}
	fprintf(fp,")");
}

int debugCDA(CDA *items,int level){
	int old = items->debug;
	items->debug = level;
	return old;
}

void freeCDA(CDA *items){
	if(items->freedom != NULL){
		for(int i = 0; i < items->size; i++){
			items->freedom(getCDA(items,i));
		}
	}
	free(items->data);
	free(items);
}
