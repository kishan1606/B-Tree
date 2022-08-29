#include "btree_generate_operations.h"
#include "btree_helper.h"
#include "dt.h"
#include "string.h"

// generateRecord -> Used to generate new record
// Input -> RID
// Output -> BTNdDataStruct
BTNdDataStruct *generateRecord(RID * rid) {
	BTNdDataStruct *rcd = (BTNdDataStruct *) malloc(sizeof(BTNdDataStruct));
	if (rcd != NULL) {
		(*rcd).rid.slot = (*rid).slot;
		(*rcd).rid.page = (*rid).page;
	} else {
		exit(RC_INS_ERROR);
	}
	return rcd;
}

// generateNewNode -> Used to generate/create New BTNdStruct
// Inputs -> Tree Manager
// Output -> BTNdStruct
BTNdStruct * generateNewNode(BTStruct *treeMgnr) {
	(*treeMgnr).BTNodeCnt++;
	int ord = (*treeMgnr).BTSOrd;

	BTNdStruct * newNode = malloc(sizeof(BTNdStruct));
	
	if (newNode != NULL) {
		(*newNode).BTNdKys = malloc(sizeof(Value *) * (ord - 1));
		if ((*newNode).BTNdKys != NULL) {
			(*newNode).BTNdPtr = malloc(sizeof(void *) * ord);
			if ((*newNode).BTNdPtr != NULL) {
				(*newNode).BTNdprnt = NULL;
				(*newNode).isNodeLeaf = false;
				(*newNode).queueNxt = NULL;
				(*newNode).keyCnt = 0;
				return newNode;
			} else {
				exit(RC_INS_ERROR);
			}
		} else {
			exit(RC_INS_ERROR);
		}
	} else {
		exit(RC_INS_ERROR);
	}
}

// generateTree -> Generate a new Tree
// Inputs -> Tree Manager, Key, Pointer
// Output -> BTNdStruct
BTNdStruct *generateTree(BTStruct *treeMgnr, Value *key, BTNdDataStruct *ptr) {
	BTNdStruct *nd = generateNewNode(treeMgnr);
	if(nd != NULL){
		(*nd).isNodeLeaf = true;
		if(setRootData(nd, key, ptr, ((*treeMgnr).BTSOrd) - 1)){
			(*treeMgnr).BTEntryCnt+=1;
			return nd;
		}else{
			exit(RC_ERROR);
		}
	}else{
		exit(RC_ERROR);
	}
}