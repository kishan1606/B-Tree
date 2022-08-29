#ifndef BTREE_DELETE_OPERATIONS_H
#define BTREE_DELETE_OPERATIONS_H

#include "btree_mgr.h"
#include "buffer_mgr.h"
#include "btree_helper.h"

BTNdStruct *deleteSingleEntry(BTStruct *treeMgnr, BTNdStruct *nd, Value *key, void *ptr);
BTNdStruct *deleteEntries(BTStruct *treeMgnr, Value *key);
BTNdStruct *deleteNodeEntry(BTStruct *treeMgnr, BTNdStruct *nd, Value *key, BTNdStruct *ptr);

#endif