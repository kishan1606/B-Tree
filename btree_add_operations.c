#include "btree_add_operations.h"
#include "btree_helper.h"
#include "dt.h"
#include "string.h"

// addLeaf -> Used to add leaf to a record
// Inputs -> Tree Manager, Leaf, Key, Pointer
// Output -> BTNdStruct
BTNdStruct *addLeaf(BTStruct *treeMgnr, BTNdStruct *leaf, Value *key, BTNdDataStruct *ptr) {
	if(treeMgnr != NULL){
		int i, y = (*leaf).keyCnt - 1, z;
		(*treeMgnr).BTEntryCnt++;

		z = 0;
		while (z < (*leaf).keyCnt && areKeysLess((*leaf).BTNdKys[z], key))
			z+=1;

		i = (*leaf).keyCnt;
		while (z < i) {
			(*leaf).BTNdKys[i] = (*leaf).BTNdKys[y];
			(*leaf).BTNdPtr[i] = (*leaf).BTNdPtr[y];
			i-=1;
			y-=1;
		}
		if(setLeafInfo(leaf, key, ptr, z)){
			return leaf;
		}else{
			exit(RC_TREE_NOT_FOUND);
		}
	}else{
		exit(RC_TREE_NOT_FOUND);
	}
}

// addSplitLeaf -> Addition of Split Leaf in case Splitting Happens
// Inputs -> Tree Manager, Leaf BTNdStruct, Key, Data Pointer
// Output -> BTNdStruct
BTNdStruct *addSplitLeaf(BTStruct *treeMgnr, BTNdStruct *leaf, Value *key, BTNdDataStruct *ptr) {
	BTNdStruct *nwLf;
	Value ** tmpK;
	void ** tmpP;
	int ord = (*treeMgnr).BTSOrd;

	tmpK = malloc(ord * sizeof(Value));
	tmpP = malloc(ord * sizeof(void *));

	if (tmpK != NULL && tmpP != NULL) {
		nwLf = generateNewNode(treeMgnr);
		(*nwLf).isNodeLeaf = true;

		int ind = 0, x = 0, y = 0, split = 0;
		while (ind < ord - 1 && areKeysLess(leaf->BTNdKys[ind], key))
			ind++;

		while (x < (((*leaf).keyCnt))){
			if (y != ind){
				//todo
			}else{
				y+=1;
			}
			tmpK[y] = (*leaf).BTNdKys[x];
			tmpP[y] = (*leaf).BTNdPtr[x];
			x+=1;
			y+=1;
		}

		tmpK[ind] = key;
		tmpP[ind] = ptr;
		(*leaf).keyCnt = 0;

		x = 0, y = 0;
		int z = (ord - 1);
		if (z%2 != 0){
			split = 1 + (z/2);
		}else{
			split = (z/2);
		}
		
		while (split > x) {
			(*leaf).keyCnt+=1;
			(*leaf).BTNdPtr[x] = tmpP[x];
			(*leaf).BTNdKys[x] = tmpK[x];
			x+=1;
		}

		x = split;
		while(ord > x) {
			(*nwLf).BTNdPtr[y] = tmpP[x];
			(*nwLf).BTNdKys[y] = tmpK[x];
			(*nwLf).keyCnt = 1 + (*nwLf).keyCnt;
			x+=1;
			y+=1;
		}

		free(tmpP);
		free(tmpK);

		(*nwLf).BTNdPtr[ord - 1] = (*leaf).BTNdPtr[ord - 1];
		(*leaf).BTNdPtr[ord - 1] = nwLf;

		if(setLeafData(leaf, ord, 1)){
			if(setLeafData(nwLf, ord, 1)){
				(*nwLf).BTNdprnt = (*leaf).BTNdprnt;
				int nwKey = (*nwLf).BTNdKys[0];
				(*treeMgnr).BTEntryCnt = (*treeMgnr).BTEntryCnt + 1;
				return addParent(treeMgnr, leaf, nwKey, nwLf);
			}else{
				exit(RC_CONDITION_NOT_MET);
			}
		}else{
			exit(RC_CONDITION_NOT_MET);
		}
	}else if(tmpP == NULL){
		exit(RC_INS_ERROR);
	}else{
		exit(RC_INS_ERROR);
	}
}

// addSplitNode -> Addition of Split BTNdStruct in case Splitting Happens
// Inputs -> Tree Manager, Old BTNdStruct, Left Index, Value, Right BTNdStruct
// Output -> BTNdStruct
BTNdStruct *addSplitNode(BTStruct *treeMgnr, BTNdStruct *oldNd, int lInd, Value *key, BTNdStruct *rgt) {
	BTNdStruct * newNd, *nd, **tmpP;
	Value **tmpK;
	int ord = (*treeMgnr).BTSOrd;

	tmpP = malloc(sizeof(BTNdStruct *) * (ord + 1));
	tmpK = malloc(sizeof(Value *) * ord);
	
	if (tmpK != NULL && tmpP != NULL) {
		if(copyPointers(*tmpP, oldNd, lInd)){
			// Everything OK
			if(copyKeys(*tmpK, oldNd, lInd)){
				// Everything OK
				int x = 0, y = 0, split = 0, prime = 0;

				tmpP[lInd + 1] = rgt;
				tmpK[lInd] = key;

				int z = (ord - 1);
				if (z % 2 != 0){
					split = 1 + (z/2);
				}else{
					split = (z/2);
				}

				newNd = generateNewNode(treeMgnr);
				(*oldNd).keyCnt = 0;
				while (x <= split) {
					(*oldNd).BTNdPtr[x] = tmpP[x];
					(*oldNd).BTNdKys[x] = tmpK[x];
					(*oldNd).keyCnt+=1;
					x+=1;
				}
				(*oldNd).BTNdPtr[x] = tmpP[x];
				prime = tmpK[split - 1];
				
				++x;
				while (x <= (ord - 1)){
					(*newNd).BTNdPtr[y] = tmpP[x];
					(*newNd).BTNdKys[y] = tmpK[x];
					(*newNd).keyCnt = (*newNd).keyCnt + 1;
					x+=1;
					y+=1;
				}
				(*newNd).BTNdPtr[y] = tmpP[x];
				(*newNd).BTNdprnt = (*oldNd).BTNdprnt;
				
				free(tmpP);
				free(tmpK);
				
				x = 0;
				while (x < ((*newNd).keyCnt + 1)) {
					nd = (*newNd).BTNdPtr[x];
					(*nd).BTNdprnt = newNd;
					x+=1;
				}
				(*treeMgnr).BTEntryCnt =  1 + (*treeMgnr).BTEntryCnt;
				return addParent(treeMgnr, oldNd, prime, newNd);
			}else{
				exit(RC_CONDITION_NOT_MET);
			}
		}else{
			exit(RC_CONDITION_NOT_MET);
		}
	}else if(tmpP == NULL){
		exit(RC_INS_ERROR);
	}else{
		exit(RC_INS_ERROR);
	}
}

// addParent -> Used to add new Parent
// Inputs -> Tree Manager, Left BTNdStruct, Right BTNdStruct, Key
// Outputs -> BTNdStruct
BTNdStruct *addParent(BTStruct *treeMgnr, BTNdStruct *l, Value *key, BTNdStruct *r) {
	if(treeMgnr != NULL){
		if(l != NULL && r != NULL){
			BTNdStruct *nd = (*l).BTNdprnt;
			int ord = (*treeMgnr).BTSOrd - 1;

			if (nd != NULL){
				int lIndex = 0;
				while ((*nd).BTNdPtr[lIndex] != l && lIndex <= (*nd).keyCnt){
					lIndex+=1;
				}
				// printf("LEFT INDEX: %d", lIndex);
				if ((*nd).keyCnt >= ord) {
					return addSplitNode(treeMgnr, nd, lIndex, key, r);
				} else {
					return addNode(treeMgnr, nd, lIndex, key, r);
				}
			} else {
				return addRoot(treeMgnr, l, key, r);
			}		
		}else{
			exit(RC_INS_ERROR);
		}
	}else{
		exit(RC_INS_ERROR);
	}
}

// Inserts a new key and pointer to a BTSnode into a BTSnode into which these can fit without violating the B+ tree properties.
// addNode -> Used to add BTSnode if possible
// Inputs -> Tree Manager, Parent BTNdStruct, Left Index, Key, Right BTNdStruct
// Output -> Root BTNdStruct
BTNdStruct *addNode(BTStruct *treeMgnr, BTNdStruct *prnt, int lIndex, Value *key, BTNdStruct *r) {
	if(treeMgnr != NULL){
		if(prnt != NULL){
			if(r != NULL){
				int x = (*prnt).keyCnt;
				while (x > lIndex) {
					(*prnt).BTNdKys[x] = (*prnt).BTNdKys[x - 1];
					(*prnt).BTNdPtr[x + 1] = (*prnt).BTNdPtr[x];
					x-=1;
				}
				(*prnt).keyCnt++;
				(*prnt).BTNdPtr[lIndex + 1] = r;
				(*prnt).BTNdKys[lIndex] = key;
				return (*treeMgnr).BTRootNd;
			}else{
				exit(RC_INS_ERROR);
			}
		}else{
			exit(RC_INS_ERROR);	
		}
	}else{
		exit(RC_INS_ERROR);
	}
}

// addRoot -> Used to add BTRootNd with all the values
// Inputs -> Tree Manager, Left BTNdStruct, Right BTNdStruct, Key
// Output -> BTNdStruct
BTNdStruct *addRoot(BTStruct *treeMgnr, BTNdStruct *l, Value *key, BTNdStruct *r) {
	BTNdStruct *nd = generateNewNode(treeMgnr);
	(*nd).BTNdKys[0] = key;
	(*nd).keyCnt++;
	(*nd).BTNdprnt = NULL;
	(*nd).BTNdPtr[0] = l;
	(*l).BTNdprnt = nd;
	(*nd).BTNdPtr[1] = r;
	(*r).BTNdprnt = nd;
	return nd;
}