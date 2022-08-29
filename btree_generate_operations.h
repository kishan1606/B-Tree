#ifndef BTREE_GENERATE_OPERATIONS_H
#define BTREE_GENERATE_OPERATIONS_H

#include "btree_mgr.h"
#include "buffer_mgr.h"
#include "btree_helper.h"

BTNdDataStruct *generateRecord(RID *rid);
BTNdStruct *generateTree(BTStruct *treeMgnr, Value *key, BTNdDataStruct *ptr);
BTNdStruct *generateNewNode(BTStruct *treeMgnr);

#endif