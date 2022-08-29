#ifndef BTREE_ADD_OPERATIONS_H
#define BTREE_ADD_OPERATIONS_H

#include "btree_mgr.h"
#include "buffer_mgr.h"
#include "btree_helper.h"

BTNdStruct *addLeaf(BTStruct *treeMgnr, BTNdStruct *leaf, Value *key, BTNdDataStruct *ptr);
BTNdStruct *addSplitLeaf(BTStruct *treeMgnr, BTNdStruct *leaf, Value *key, BTNdDataStruct *ptr);
BTNdStruct *addNode(BTStruct *treeMgnr, BTNdStruct *prnt, int lIndex, Value *key, BTNdStruct *r);
BTNdStruct *addSplitNode(BTStruct *treeMgnr, BTNdStruct *oldNd, int lInd, Value *key, BTNdStruct *rgt);
BTNdStruct *addParent(BTStruct *treeMgnr, BTNdStruct *l, Value *key, BTNdStruct *r);
BTNdStruct *addRoot(BTStruct *treeMgnr, BTNdStruct *l, Value *key, BTNdStruct *r);

#endif