#include "btree_delete_operations.h"
#include "dt.h"
#include "string.h"

// freeMemory -> Simple Helper Function to free memory without errors
// Input -> BTNdStruct
// Output -> Boolean
bool freeMemory(BTNdStruct *nd){
	bool flag = TRUE;
	if((*nd).BTNdKys != NULL){
		free((*nd).BTNdKys);
	}else if ((*nd).BTNdPtr != NULL){
		free((*nd).BTNdPtr);
	}else if(nd != NULL){
		free(nd);
	}else{
		flag = FALSE;
	}
	return flag;
}

// getDeletionRecord -> Helper to get get the record to delete
// Input -> BTNdStruct, Value
// Output -> BTNdDataStruct
BTNdDataStruct *getDeletionRecord(BTNdStruct *BTRootNd, Value *key) {
	BTNdStruct *nd = getLeafNode(BTRootNd, key);
	if (nd == NULL)
		return NULL;
	else{
		int x=0;
		while (x < (*nd).keyCnt) {
			bool temp = areKeysEqual((*nd).BTNdKys[x], key);
			if (!temp){
				x++;
			}else{
				break;
			}
		}
		if(x != (*nd).keyCnt){
			return (BTNdDataStruct *) (*nd).BTNdPtr[x];
		}
		else{
			return NULL;
		}
	}
}

BTNdStruct *checkRoot(BTNdStruct *existingRoot) {
	BTNdStruct *newRoot;

	if ((*existingRoot).keyCnt > 0){
		return existingRoot;
	}else{
		if ((*existingRoot).isNodeLeaf) {
			newRoot = NULL;
		} else {
			newRoot = (*existingRoot).BTNdPtr[0];
			(*newRoot).BTNdprnt = NULL;
		}

		if(freeMemory(existingRoot)){
			return newRoot;
		}else{
			printf("MEMORY FREE ERROR");
		}
		return newRoot;
	}
}

// shiftPointers -> Helper Function used to shift Pointers
// Input -> BTNdStruct, Pointer, Number of Pointers
// Output -> Boolean
bool shiftPointers(BTNdStruct *nd, BTNdStruct *ptr,int numOfPtrs){
	int i = 0;
	while((*nd).BTNdPtr[i] != ptr)
		i++;
	
	++i; 
	while(i < numOfPtrs){
		(*nd).BTNdPtr[i - 1] = (*nd).BTNdPtr[i];
		i++;
	}
	return TRUE;
}

// Remove a record having the specified key from the the specified BTSnode.
// deleteNodeEntry -> Delete record with the key
// Inputs -> Tree Manager, BTNdStruct, Key, Pointer
// Output -> BTNdStruct
BTNdStruct *deleteNodeEntry(BTStruct *treeMgnr, BTNdStruct *nd, Value *key, BTNdStruct *ptr) {
	int numOfPtrs;
	int ord = (*treeMgnr).BTSOrd;
	
	if(shiftKeys(nd, key)){
		if(!(*nd).isNodeLeaf){
			numOfPtrs = (*nd).keyCnt +1;
		}else{
			numOfPtrs = (*nd).keyCnt;
		}

		if(shiftPointers(nd, ptr, numOfPtrs)){
			// Everything OK
			
			(*nd).keyCnt--;
			(*treeMgnr).BTEntryCnt--;

			if (!(*nd).isNodeLeaf){
				int x = (*nd).keyCnt + 1;
				while(x < ord){
					(*nd).BTNdPtr[x] = NULL;
					x++;
				}
			}else{
				int x = (*nd).keyCnt;
				while(x < ord-1){
					(*nd).BTNdPtr[x] = NULL;
					x++;
				}
			}
			return nd;
		}else{
			printf("SIFTING POINTERS ERROR");
		}
	}else{
		printf("KEY SHIFTING ERROR");
	}
}

// deleteSingleEntry -> Used to delete an entry from the B+ tree with given key and pointer
// Inputs -> New BTNdStruct, Btree manager, key value, pointer to remove 
// Output -> BTNdStruct
BTNdStruct *deleteSingleEntry(BTStruct *treeMgnr, BTNdStruct *nd, Value *key, void *ptr) {

	int minKeys, ord = (*treeMgnr).BTSOrd;

	nd = deleteNodeEntry(treeMgnr, nd, key, ptr);

	if (nd == (*treeMgnr).BTRootNd){
		return checkRoot((*treeMgnr).BTRootNd);
	}else{
		if (!(*nd).isNodeLeaf) {
			if ((ord) % 2 != 0){
				minKeys = (ord/2) + 1;
			}else{
				minKeys = ord/2;
			}
			minKeys--;
		}else{
			if ((ord - 1) % 2 != 0){
				minKeys = ((ord - 1)/2)+1;
			}else{
				minKeys = ((ord - 1)/2) + 0;
			}
		}

		if ((*nd).keyCnt >= minKeys){
			return (*treeMgnr).BTRootNd;
		}else{
			findNeighbor(ord,treeMgnr,nd);
		}
	}
}

// deleteEntries -> Used to delete entry
// Inputs -> Tree Manager, Key
// Output -> BTNdStruct
BTNdStruct *deleteEntries(BTStruct *treeMgnr, Value *key) {
	BTNdStruct *rcd = getDeletionRecord((*treeMgnr).BTRootNd, key);
	BTNdDataStruct *leafKey = getLeafNode((*treeMgnr).BTRootNd, key);
	if(rcd == NULL){
		// Error Occured
		printf("Error Occured. Record Can't be NULL");
	}else if(leafKey == NULL){
		// Error Occured
		printf("Error Occured. Key Can't be NULL");
	}else{
		if (rcd != NULL) {
			if( leafKey != NULL){
				(*treeMgnr).BTRootNd = deleteSingleEntry(treeMgnr, leafKey, key, rcd);
				free(rcd);
			}
		}
		return (*treeMgnr).BTRootNd;
	}
}