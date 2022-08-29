#include "btree_helper.h"
#include "btree_delete_operations.h"
#include "btree_add_operations.h"
#include "btree_generate_operations.h"
#include "dt.h"
#include "string.h"

// setRootData -> Helper Function to set Root Info
// Input -> BTNdStruct, Key, Pointer, Tree Order
// Output -> Boolean
bool setRootData(BTNdStruct *nd, Value *key, BTNdDataStruct *ptr, int ord){
	if(nd != NULL){
		if(key != NULL){
			(*nd).BTNdKys[0] = key;
		}else{
			return FALSE;
		}
		if(ptr != NULL){
			(*nd).BTNdPtr[0] = ptr;
			(*nd).BTNdPtr[ord] = NULL;
		}else{
			return FALSE;
		}
		(*nd).BTNdprnt = NULL;
		(*nd).keyCnt+=1;
		return TRUE;
	}else{
		return FALSE;
	}
}

// setLeafInfo -> Helper Function to set Leaf BTNdStruct Data
// Input -> Leaf BTNdStruct, Key, Pointer, Index
// Output -> Boolean
bool setLeafInfo(BTNdStruct *nd, Value *key, BTNdDataStruct *ptr, int ind){
	(*nd).BTNdPtr[ind] = ptr;
	(*nd).BTNdKys[ind] = key;
	(*nd).keyCnt+=1;
	return TRUE;
}

// copyKeys -> Helper Function Used to set Leaf Data Pointers to NULL
// Inputs -> BTNdStruct, Order
// Output -> Boolean
bool setLeafData(BTNdStruct *nd, int ord, int choice){
	if(nd != NULL){
		if(choice == 1){
			int x = (*nd).keyCnt;
			while (x < (ord - 1)){
				(*nd).BTNdPtr[x] = NULL;
				x+=1;
			}
			return TRUE;
		}else if(choice == 2){
	
		}
	}else{
		return FALSE;
	}
}

// copyKeys -> Helper Function Used to Copy Pointers to temp Variable
// Inputs -> BTNdStruct Pointers, BTNdStruct, Index
// Output -> Boolean
bool copyPointers(BTNdStruct **pointers, BTNdStruct *oldNode, int lInd){
	if(oldNode != NULL){
		int i = 0, j = 0;
		while ( i < (*oldNode).keyCnt + 1) {
			if (j != lInd + 1){
				//todo
			} else {
				j++;
			}
			pointers[j] = (*oldNode).BTNdPtr[i];
			i+=1;
			j+=1;
		}
		return TRUE;
	}else{
		return FALSE;
	}
}

// copyKeys -> Helper Function Used to Copy Keys to temp Variable
// Inputs -> Value, BTNdStruct, Index
// Output -> Boolean
bool copyKeys(Value **BTNdKys, BTNdStruct *oldNode, int lInd){
	if(oldNode != NULL){
		int i=0, j=0;
		while(i < (*oldNode).keyCnt){
			if(j != lInd){
				//todo
			}else{
				j++;
			}		
			BTNdKys[j] = (*oldNode).BTNdKys[i];
			i+=1;
			j+=1;
		}
		return TRUE;
	}else{
		return FALSE;
	}
}

// getLeafNode -> Used to get the leaf BTSnode Helper Function
// Inputs -> BTNdStruct, Value
// Output -> BTNdStruct
BTNdStruct *getLeafNode(BTNdStruct *BTRootNd, Value *key) {
	BTNdStruct *nd = BTRootNd;
	if (nd != NULL) {
		int i = 0;
		while (!(*nd).isNodeLeaf) {
			i = 0;
			while (i < (*nd).keyCnt) {
				if(areKeysEqual(key, (*nd).BTNdKys[i])){
					// TRUE
					i+=1;
				}else{
					// FALSE
					// printf("KEYS NOT EQUAL ..... NO INCREMENT");
					if(!areKeysGreater(key, (*nd).BTNdKys[i])){
						// FALSE
						// printf("KEYS NOT GREATER... NO INCREMENT");
						break;
					}else{
						i+=1;
					}
				}
			}
			nd = (BTNdStruct *)(*nd).BTNdPtr[i];
		}
		return nd;
	} else {
		return nd;
	}
}

// shiftKeys -> Helper Function used to shift Keys
// Input -> BTNdStruct, Key
// Output -> Boolean
bool shiftKeys(BTNdStruct *nd, Value *key){
	int i = 0;
	while(!areKeysEqual((*nd).BTNdKys[i], key)){
		i++;
	}
	++i;
	while(i<(*nd).keyCnt){
		(*nd).BTNdKys[i - 1] = (*nd).BTNdKys[i];
		i++;
	}
	return TRUE;
}

// setNgbrPtr -> Helper Function
// Input -> Ngbr, BTNdStruct, 2 Integers
// Output -> None
setNgbrPtr( BTNdStruct *ngbr,  BTNdStruct *nd, int i, int j){
	(*ngbr).BTNdKys[i] = (*nd).BTNdKys[j];
	(*ngbr).keyCnt++;
	(*ngbr).BTNdPtr[i] = (*nd).BTNdPtr[j];
}

// combineNodes -> Used to combine/merge Nodes
// Inputs -> Tree Manager, BTNdStruct, Neighbor, Neighbor Index, Prime Index
// Output -> BTNdStruct
BTNdStruct *combineNodes(BTStruct *treeMgnr, BTNdStruct *nd, BTNdStruct *ngbr, int ngbrInd, int kPrime) {
	int x, y, z, ngbriInd, ord = (*treeMgnr).BTSOrd;
	BTNdStruct *newNd;

	if(ngbrInd != -1){
		printf("NGBR Swap Not Needed");
	}else{
		newNd = nd;
		nd = ngbr;
		ngbr = newNd;
	}

	ngbriInd = (*ngbr).keyCnt;

	if ((*nd).isNodeLeaf) {
		x = ngbriInd;
		y = 0; 
		while (y < (*nd).keyCnt) {
			setNgbrPtr(ngbr,nd,x,y);
			x+=1; 
			y+=1;
		}
		(*ngbr).BTNdPtr[ord - 1] = (*nd).BTNdPtr[ord - 1];
		(*treeMgnr).BTRootNd = deleteSingleEntry(treeMgnr, (*nd).BTNdprnt, kPrime, nd);
	}else{
		(*ngbr).BTNdKys[ngbriInd] = kPrime;
		(*ngbr).keyCnt++;

		z = (*nd).keyCnt;

		x = ngbriInd + 1;
		y = 0; 
		while (y < z) {
			setNgbrPtr(ngbr,nd,x,y);
			(*nd).keyCnt--;
			x+=1; 
			y+=1;
		}
		(*ngbr).BTNdPtr[x] = (*nd).BTNdPtr[y];
		x = 0;
		while (x < (*ngbr).keyCnt + 1) {
			newNd = (BTNdStruct *) (*ngbr).BTNdPtr[x];
			(*newNd).BTNdprnt = ngbr;
			x++;
		}
		(*treeMgnr).BTRootNd = deleteSingleEntry(treeMgnr, (*nd).BTNdprnt, kPrime, nd);
	}

	if(freeMemory(nd)){
		return (*treeMgnr).BTRootNd;
	}else{
		printf("MEMORY FREE ERROR");
	}
	return (*treeMgnr).BTRootNd;
}

// findNgbrInd -> Helper Function used to find Index
// Input -> BTNdStruct
// Output -> Integer Index
int findNgbrInd(BTNdStruct *nd) {
	int x=0;
	while(x <= (*nd).BTNdprnt->keyCnt){
		if ((*nd).BTNdprnt->BTNdPtr[x] == nd){
			return x - 1;
		}
		x++;
	}
	exit(RC_ERROR);
}

// findNeighbor -> Used to find neighbor
// Inputs -> Tree Order, Tree Manager, BTNdStruct
// Output -> None
findNeighbor(int ord, BTStruct *treeMgnr, BTNdStruct *nd){
	int kPrimeInd, cap, ngbrInd = findNgbrInd(nd);
	BTNdStruct *ngbr;

	if(ngbrInd == -1){
		kPrimeInd = 0;
		ngbr = (*nd).BTNdprnt->BTNdPtr[1];
	}
	else{
		kPrimeInd = ngbrInd;
		ngbr = (*nd).BTNdprnt->BTNdPtr[ngbrInd];
	} 

	if((*nd).isNodeLeaf){
		cap = ord;
	}
	else{
		cap =  ord - 1;
	}

	int temp = ((*ngbr).keyCnt + (*nd).keyCnt);
	if (temp >= cap){
		return balanceNodes((*treeMgnr).BTRootNd, nd, ngbr, ngbrInd, kPrimeInd, (*nd).BTNdprnt->BTNdKys[kPrimeInd]);
	}
	else{
		return combineNodes(treeMgnr, nd, ngbr, ngbrInd, (*nd).BTNdprnt->BTNdKys[kPrimeInd]);
	}
}

// setPtr -> Supporting function helps set Ptr values
// Inputs -> String, BTNdStruct, Nebr, Index, Value
// Output -> None
setPtr(char str,BTNdStruct * nde,BTNdStruct * ngbr,int kp_indx){
	if(str == "LEAF"){
		(*nde).BTNdPtr[(*nde).keyCnt] = (*ngbr).BTNdPtr[0];
		(*nde).BTNdKys[(*nde).keyCnt] = (*ngbr).BTNdKys[0];
		(*nde).BTNdprnt->BTNdKys[kp_indx] = (*ngbr).BTNdKys[1];
	}
	else{
		(*nde).BTNdPtr[0] = (*ngbr).BTNdPtr[(*ngbr).keyCnt - 1];
		(*ngbr).BTNdPtr[(*ngbr).keyCnt - 1] = NULL;
		(*nde).BTNdKys[0] = (*ngbr).BTNdKys[(*ngbr).keyCnt - 1];
		(*nde).BTNdprnt->BTNdKys[kp_indx] = (*nde).BTNdKys[0];
	}
}

// setKey -> Supporting function helps set key values
// Inputs -> String, BTNdStruct, Nebr, Index, Value
// Output -> None
setKey(char str,BTNdStruct * nde,BTNdStruct * ngbr,int kp_indx,int kp){
	BTNdStruct * temp;
	if(str == "LEAF"){
		temp = (BTNdStruct *) (*nde).BTNdPtr[(*nde).keyCnt + 1];
		(*temp).BTNdprnt = nde;
		(*nde).BTNdprnt->BTNdKys[kp_indx] = (*ngbr).BTNdKys[0];
	}
	else{
		temp = (BTNdStruct *) (*nde).BTNdPtr[0];
		(*temp).BTNdprnt = nde;
		(*ngbr).BTNdPtr[(*ngbr).keyCnt] = NULL;
		(*nde).BTNdKys[0] = kp;
		(*nde).BTNdprnt->BTNdKys[kp_indx] = (*ngbr).BTNdKys[(*ngbr).keyCnt - 1];
	}
}

// balanceNodes -> Used to redistribute entries in nodes when one BTSnode is so small after deletion of BTSnode
// Inputs -> New BTNdStruct, New BTRootNd, Neighbour nodes, Neighbour BTSnode index, Prime index, Prime value
// Output -> BTNdStruct
BTNdStruct *balanceNodes(BTNdStruct *BTRootNd, BTNdStruct *nd, BTNdStruct *nebor, int neborIndex, int kPrimeInd, int kPrimeVal) {
	int i;

	if (neborIndex == -1){
		if (!(*nd).isNodeLeaf) {
			(*nd).BTNdPtr[(*nd).keyCnt + 1] = (*nebor).BTNdPtr[0];
			(*nd).BTNdKys[(*nd).keyCnt] = kPrimeVal;
			setKey("LEAF",nd,nebor,kPrimeInd,kPrimeVal);
		} 
		else {
			setPtr("LEAF",nd,nebor,kPrimeInd);
		}

		i = 0;
		while ( i < (*nebor).keyCnt - 1) {
			(*nebor).BTNdPtr[i] = (*nebor).BTNdPtr[i + 1];
			(*nebor).BTNdKys[i] = (*nebor).BTNdKys[i + 1];
			i++;
		}
			
	} else {
		i = (*nd).keyCnt;
		while (i > 0) {
			(*nd).BTNdPtr[i] = (*nd).BTNdPtr[i - 1];
			(*nd).BTNdKys[i] = (*nd).BTNdKys[i - 1];
			i--;
		}

		if (!(*nd).isNodeLeaf) {
			(*nd).BTNdPtr[0] = (*nebor).BTNdPtr[(*nebor).keyCnt];
			setKey("NON_LEAF",nd,nebor,kPrimeInd,kPrimeVal);
		}

		else {
			setPtr("NON_LEAF",nd,nebor,kPrimeInd);
		}
	} 

	(*nd).keyCnt++;
	(*nebor).keyCnt--;

	return BTRootNd;
}

// enQueueNode -> Used to enQueueNode in the list
// Inputs -> Tree Manager, New BTNdStruct
// Output -> None
void enQueueNode(BTStruct *treeMgnr, BTNdStruct *nwNd) {
	BTNdStruct *n;
	if((*treeMgnr).BTNdQueue != NULL){
		n = (*treeMgnr).BTNdQueue;
		while ((*n).queueNxt != NULL) {
			n = (*n).queueNxt;
		}
		(*n).queueNxt = nwNd;
		(*nwNd).queueNxt = NULL;
	}else{
		(*treeMgnr).BTNdQueue = nwNd;
		(*treeMgnr).BTNdQueue->queueNxt = NULL;
	}
}

// deQueueNode -> Used to deQueueNode from the list
// Inputs -> Tree Manager
// Output -> BTNdStruct
BTNdStruct * deQueueNode(BTStruct *treeMgnr) {
	BTNdStruct *nd = (*treeMgnr).BTNdQueue;
	(*treeMgnr).BTNdQueue = (*treeMgnr).BTNdQueue->queueNxt;
	(*nd).queueNxt = NULL;
	return nd;
}

// pathLength -> Find the Path Length
// Inputs -> Root BTNdStruct & Child BTNdStruct
// Output -> Integer Length
int pathLength(BTNdStruct *BTRootNd, BTNdStruct *child) {
	int len = 0;
	BTNdStruct *n = child;
	while (n != BTRootNd) {
		n = (*n).BTNdprnt;
		len++;
	}
	return len;
}

// areKeysEqual -> Used to compare 2 BTNdKys
// Inputs -> Key 1, Key 2
// Outputs -> Boolean
bool areKeysEqual(Value *k1, Value *k2) {
	
	bool b = false;
	if((*k1).dt == DT_BOOL){
		if ((*k1).v.boolV == (*k2).v.boolV) {
			b = TRUE;
		}
	}
	else if((*k1).dt == DT_INT){
		if ((*k1).v.intV == (*k2).v.intV) {
			b = TRUE;
		}
	}
	else if((*k1).dt == DT_STRING){
		if (strcmp((*k1).v.stringV, (*k2).v.stringV) == 0) {
			b = TRUE;
		}
	}
	else if((*k1).dt == DT_FLOAT){
		if ((*k1).v.floatV == (*k2).v.floatV) {
			b = TRUE;
		}
	}	
	return b;
}

// areKeysLess -> Used to compare 2 BTNdKys
// Inputs -> Key 1, Key 2
// Outputs -> Boolean
bool areKeysLess(Value *k1, Value *k2) {
	bool b = false;	
	if((*k1).dt == DT_BOOL){
		b = FALSE;
	}
	else if((*k1).dt == DT_INT){
		if ((*k1).v.intV < (*k2).v.intV) {
			b = TRUE;
		}
	}
	else if((*k1).dt == DT_STRING){
		if (strcmp((*k1).v.stringV, (*k2).v.stringV) == -1) {
			b = TRUE;
		}
	}
	else if((*k1).dt == DT_FLOAT){
		if ((*k1).v.floatV < (*k2).v.floatV) {
			b = TRUE;
		}
	}	
	return b;
}

// areKeysGreater -> Used to compare 2 BTNdKys
// Inputs -> Key 1, Key 2
// Outputs -> Boolean
bool areKeysGreater(Value *k1, Value *k2) {
	bool b = false;
	if((*k1).dt == DT_BOOL){
		b = FALSE;
	}
	else if((*k1).dt == DT_INT){
		if ((*k1).v.intV > (*k2).v.intV) {
			b = TRUE;
		}
	}
	else if((*k1).dt == DT_STRING){
		if (strcmp((*k1).v.stringV, (*k2).v.stringV) == 1) {
			b = TRUE;
		}
	}
	else if((*k1).dt == DT_FLOAT){
		if ((*k1).v.floatV > (*k2).v.floatV) {
			b = TRUE;
		}
	}
	return b;
}