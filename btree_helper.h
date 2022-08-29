#ifndef BTREE_HELPER_H
#define BTREE_HELPER_H

#include "btree_mgr.h"
#include "buffer_mgr.h"

typedef struct BTNdStruct {
	void **BTNdPtr;
	Value **BTNdKys;
	struct BTNdStruct *BTNdprnt;
	bool isNodeLeaf;
	int keyCnt;
	struct BTNdStruct *queueNxt;
} BTNdStruct;

typedef struct BTScanner {
	int BTSIndx;
	int BTSKeyCnt;
	int BTSOrd;
	BTNdStruct * BTSnode;
} BTScanner;

typedef struct BTStruct {
	int BTSOrd;
	int BTEntryCnt;
	int BTNodeCnt;
	BTNdStruct * BTNdQueue;
	BM_BufferPool BTBuffPool;
	BTNdStruct * BTRootNd;
	BM_PageHandle BTPgHndlr;
	DataType BTDataType;
} BTStruct;

typedef struct BTNdDataStruct {
	RID rid;
} BTNdDataStruct;

BTNdStruct *getLeafNode(BTNdStruct *BTRootNd, Value *key);
BTNdDataStruct *getDeletionRecord(BTNdStruct *BTRootNd, Value *key);

void enQueueNode(BTStruct *treeMgnr, BTNdStruct *nwNd);
BTNdStruct * deQueueNode(BTStruct *treeMgnr);

int pathLength(BTNdStruct *BTRootNd, BTNdStruct *child);
int findNgbrInd(BTNdStruct *nd);

BTNdStruct *checkRoot(BTNdStruct *existingRoot);
BTNdStruct *combineNodes(BTStruct *treeMgnr, BTNdStruct *nd, BTNdStruct *ngbr, int ngbrInd, int kPrime);
BTNdStruct *balanceNodes(BTNdStruct *BTRootNd, BTNdStruct *nd, BTNdStruct *nebor, int neborIndex, int kPrimeInd, int kPrimeVal);

bool areKeysLess(Value *k1, Value *k2);
bool areKeysGreater(Value *k1, Value *k2);
bool areKeysEqual(Value *k1, Value *k2);

#endif
