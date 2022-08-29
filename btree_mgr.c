#include "dberror.h"
#include "btree_mgr.h"
#include "storage_mgr.h"
#include "buffer_mgr.h"
#include "tables.h"
#include "btree_helper.h"
#include "btree_delete_operations.h"
#include "btree_add_operations.h"

BTStruct * tMgnr = NULL;

RC tree_checks(Record *record){
	if(record == NULL){
		return RC_TREE_NOT_FOUND;
	}else{
		return RC_OK;
	}
}

RC scans_checks(BT_ScanHandle *handle){
	if(handle == NULL){
		return RC_SCAN_HANDLE_NOT_FOUND;
	}else{
		return RC_OK;
	}
}

RC results_checks(RID *result){
	if(result == NULL){
		return RC_RESULT_NOT_FOUND;
	}else{
		return RC_OK;
	}
}

RC global_btree_checks(BTreeHandle *tree, BT_ScanHandle *handle, RID *result,  int choice){
	if(choice == 1){
		tree_checks(tree);
	}else if(choice == 2){
		scans_checks(handle);
	}else if(choice == 3){
		results_checks(result);
	}else if(choice == 4){
		if(scans_checks(handle) == RC_OK){
			return results_checks(result);
		}else{
			return scans_checks(handle);
		}
	}else if(choice == 5){
		if(tree_checks(tree) == RC_OK){
			return results_checks(result);
		}else{
			return tree_checks(tree);
		}
	}
	return RC_OK;
}

// initIndexManager -> Initialization
// Inputs -> managementData
// Output -> RC_CODE
RC initIndexManager(void *mgmtData) {
	initStorageManager();
	return RC_OK;
}

// shutdownIndexManager -> Shuts down Index Manager
// Inputs -> None
// Output -> RC_CODE
RC shutdownIndexManager() {
	if(tMgnr != NULL){
		tMgnr = NULL;
		return RC_OK;
	}else{
		return RC_OK;
	}
}

// createBTree -> Create new B-Tree
// Inputs -> idxID, KeyType, N
// Output -> RC_CODE
RC createBtree(char *idxId, DataType BTDataType, int n) {
	if(idxId == NULL){
		return RC_ERROR;
	}else{
		int mxNd = PAGE_SIZE / sizeof(BTNdStruct);
		if (mxNd < n) {
			return RC_ORDER_TOO_HIGH_FOR_PAGE;
		}else{
			SM_FileHandle fh;
			RC return_code;
			char dt[PAGE_SIZE];
			tMgnr = (BTStruct *) malloc(sizeof(BTStruct));
			BM_BufferPool * bp = (BM_BufferPool *) malloc(sizeof(BM_BufferPool));

			(*tMgnr).BTRootNd = NULL;
			(*tMgnr).BTNdQueue = NULL;
			(*tMgnr).BTNodeCnt = 0;
			(*tMgnr).BTEntryCnt = 0;
			(*tMgnr).BTSOrd = n + 2;
			(*tMgnr).BTDataType = BTDataType;
			(*tMgnr).BTBuffPool = *bp;

			return_code = createPageFile(idxId);
			if (return_code != RC_OK){
				return return_code;
			}else{
				return_code = openPageFile(idxId, &fh);
				if(return_code != RC_OK){
					return return_code;
				}else{
					return_code = writeBlock(0, &fh, dt);
					if(return_code == RC_OK){
						return_code = closePageFile(&fh);
						if(return_code != RC_OK){
							return return_code;
						}else{
							return RC_OK;
						}
					}else{
						return return_code;
					}
				}
			}
		}
	}
}

// openBtree -> Open B-Tree
// Inputs -> Tree Pointer, idxID
// Output -> RC_CODE
RC openBtree(BTreeHandle **tree, char *idxId) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(*tree, NULL, NULL, 1);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		*tree = (BTreeHandle *) malloc(sizeof(BTreeHandle));
		(*tree)->mgmtData = tMgnr;
		return_code = initBufferPool(&tMgnr->BTBuffPool, idxId, 1000, RS_FIFO, NULL);

		if(return_code != RC_OK){
			return return_code;
		}
	}
	
	return return_code;
}

// closeBtree -> Close/Shut B-Tree
// Inputs -> Tree
// Output -> RC_CODE
RC closeBtree(BTreeHandle *tree) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(tree, NULL, NULL, 1);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		BTStruct * tm = (BTStruct*)(*tree).mgmtData;
		markDirty(&tm->BTBuffPool, &tm->BTPgHndlr);
		shutdownBufferPool(&tm->BTBuffPool);
		free(tm);
		free(tree);
		return_code = RC_OK;
	}

	return return_code;
}

// deleteBtree -> Delete the B-Tree
// Inputs -> idxID
// Outputs -> RC_CODE
RC deleteBtree(char *idxId) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = destroyPageFile(idxId);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		return return_code;
	}
}

// insertKey -> Adding new Key to Tree
// Inputs -> Tree, Key, RID
// Outputs -> RC_CODE
RC insertKey(BTreeHandle *tree, Value *key, RID rid) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(tree, NULL, NULL, 1);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		BTStruct *tm = (BTStruct *) (*tree).mgmtData;
		int odr = (*tm).BTSOrd;
		BTNdDataStruct *nd;
		BTNdStruct *n;

		if(getDeletionRecord((*tm).BTRootNd, key) == NULL){
			// Record Not Found
			nd = generateRecord(&rid);
			if((*tm).BTRootNd != NULL){
				n = getLeafNode((*tm).BTRootNd, key);
				if(((*n).keyCnt + 1) < odr){
					n = addLeaf(tm, n, key, nd);
				}else{
					(*tm).BTRootNd = addSplitLeaf(tm, n, key, nd);
					return_code = RC_OK;
				}
			}else{
				(*tm).BTRootNd = generateTree(tm, key, nd);
				return_code = RC_OK;
				return return_code;
			}
		}else{
			// Record Found
			return_code = RC_IM_KEY_ALREADY_EXISTS;
		}
	}
	return return_code;
}

// findKey -> Search Key
// Inputs -> Tree, Key, Result
// Output -> RC_CODE
extern RC findKey(BTreeHandle *tree, Value *key, RID *result) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(tree, NULL, NULL, 1);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		if(key != NULL && result != NULL){
			// Everything OK
			BTStruct *tm = (BTStruct *)(*tree).mgmtData;
			BTNdDataStruct *nd = getDeletionRecord((*tm).BTRootNd, key);
			if(nd != NULL){
				// Record Found
				// printf("RECORD FOUND");
				return_code = RC_OK;
				*result = (*nd).rid;
			}else{
				return_code = RC_IM_KEY_NOT_FOUND;
			}

		}else{
			// Error
			return_code = RC_ERROR;
		}
	}
	return return_code;
}

// getNumNodes -> Get number of nodes
// Inputs -> Tree, Result
// Output -> RC_CODE
RC getNumNodes(BTreeHandle *tree, int *result) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(tree, NULL, NULL, 1);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		if(result == NULL){
			return_code = RC_ERROR;
		}else{
			BTStruct *tm = (BTStruct *) (*tree).mgmtData;
			*result = (*tm).BTNodeCnt;
			return_code = RC_OK;
		}
	}	
	return return_code;
}

// getNumEntries -> Get Number of Entries in B Tree
// Inputs -> Tree, Result
// Output -> RC_CODE
RC getNumEntries(BTreeHandle *tree, int *result) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(tree, NULL, NULL, 1);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		if(result == NULL){
			return_code = RC_ERROR;
		}else{
			BTStruct *tm = (BTStruct *) (*tree).mgmtData;
			*result = (*tm).BTEntryCnt;
			return_code = RC_OK;
		}
	}
	return return_code;
}

// getKeyType -> Used to get datatype
// Inputs -> Tree, Result
// Outputs -> RC_CODE
RC getKeyType(BTreeHandle *tree, DataType *result) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(tree, NULL, NULL, 1);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		if(result == NULL){
			return_code = RC_ERROR;
		}else{
			BTStruct *tm = (BTStruct *) (*tree).mgmtData;
			*result = (*tMgnr).BTDataType;
			return_code = RC_OK;
		}
	}
	
	return return_code;
}

// deleteKey -> Used to delete Entries
// Inputs -> Tree, Key
// Output -> RC_CODE
RC deleteKey(BTreeHandle *tree, Value *key) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(tree, NULL, NULL, 1);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		if(key == NULL){
			return_code = RC_KEY_NULL;
		}else{
			BTStruct *tm = (BTStruct *) (*tree).mgmtData;
			(*tm).BTRootNd = deleteEntries(tm, key);
			return_code = RC_OK;
		}
	}
	return return_code;
}

// openTreeScan -> Initializes Scan
// Inputs -> Tree, Handle
// Output -> RC_CODE
RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(tree, handle, NULL, 1);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		BTScanner *sMgnr = malloc(sizeof(BTScanner));
		BTStruct *tm = (BTStruct *) (*tree).mgmtData;
		BTNdStruct *nd = (*tm).BTRootNd;
		*handle = malloc(sizeof(BT_ScanHandle));
		
		if((*tm).BTRootNd != NULL){
			// Everything OK
			while(!(*nd).isNodeLeaf){
				nd = (*nd).BTNdPtr[0];
			}
			(*sMgnr).BTSKeyCnt = (*nd).keyCnt;
			(*sMgnr).BTSnode = nd;
			(*sMgnr).BTSIndx = 0;
			(*sMgnr).BTSOrd = (*tm).BTSOrd;
			(*handle)->mgmtData = sMgnr;
			return_code = RC_OK;
		}else{
			return_code = RC_NO_RECORDS_TO_SCAN;
			return return_code;
		}

	}
	return return_code;
}

// nextEntry -> Used to traverse B tree and store details in memory
// Inputs -> Handle, RID
// Outputs -> RC_CODE
RC nextEntry(BT_ScanHandle *handle, RID *result) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(NULL, handle, result, 4);
	if(return_code != RC_OK){
		return return_code;
	}else{
		// Everything OK
		BTScanner *sMgnr = (BTScanner *) (*handle).mgmtData;
		int ki = (*sMgnr).BTSIndx, tk = (*sMgnr).BTSKeyCnt, odr = (*sMgnr).BTSOrd;
		BTNdStruct *nd = (*sMgnr).BTSnode;
		RID rid;
		
		if(nd != NULL){
			// Everything OK
			if (ki < tk) {
				rid = ((BTNdDataStruct *) (*nd).BTNdPtr[ki])->rid;
				(*sMgnr).BTSIndx++;
			} else {
				if ((*nd).BTNdPtr[odr - 1] == NULL) {
					return_code = RC_IM_NO_MORE_ENTRIES;
					return RC_IM_NO_MORE_ENTRIES;
				}else{
					nd = (*nd).BTNdPtr[odr - 1];
					(*sMgnr).BTSnode = nd;
					rid = ((BTNdDataStruct *) (*nd).BTNdPtr[0])->rid;
					(*sMgnr).BTSKeyCnt = (*nd).keyCnt;
					(*sMgnr).BTSIndx = 1;
				}
			}
		}else{
			return_code = RC_IM_NO_MORE_ENTRIES;
			return RC_IM_NO_MORE_ENTRIES;
		}
		*result = rid;
		return_code = RC_OK;
		return return_code;
	}
}

// closeTreeScan -> Closes Tree Scan & frees memory
// Inputs -> Handle
// Outputs -> RC_CODE
extern RC closeTreeScan(BT_ScanHandle *handle) {
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(NULL, handle, NULL, 2);
	if(return_code != RC_OK){
		return return_code;
	}else{
		(*handle).mgmtData = NULL;
		free(handle);
		return_code = RC_OK;
	}
	return return_code;
}

// printTree -> Used to print the tree
// Inputs -> Tree Handler
// Output -> Printed Tree
extern char *printTree(BTreeHandle *tree) {
	printf("TREE PRINTING FUNCTION");
	
	// RC is already defined in dberror.h file using it to store RC_CODES
	RC return_code;

	// Perform global checks
	return_code = global_btree_checks(tree, NULL, NULL, 1);
	if(return_code != RC_OK){
		// Error Return returned
		printf("TREE PRINTING FUNCTION ERROR");
	}else{
		printf("TREE PRINTING BEGINS");
		BTStruct *tm = (BTStruct *) (*tree).mgmtData;
		int i = 0, r = 0, nr = 0;
		BTNdStruct *nd = NULL;

		if((*tm).BTRootNd != NULL){
			// Tree not empty
			(*tm).BTNdQueue = NULL;
			enQueueNode(tm, (*tm).BTRootNd);
			while ((*tm).BTNdQueue != NULL) {
				nd = deQueueNode(tm);
				if((*nd).BTNdprnt == NULL){
					// printf("ND PARENT NULL");
				}else{
					if(nd == (*nd).BTNdprnt->BTNdPtr[0]){
						nr = pathLength((*tm).BTRootNd, nd);
						if(nr == r){
							// printf("NEW RANK EQUALS RANK");
						}else{
							r = nr;
							printf('\n');
						}
					}
				}

				while(i < (*nd).keyCnt){
					if((*tm).BTDataType == DT_STRING){
						printf("%s ", (*nd).BTNdKys[i]->v.stringV);
					}else if((*tm).BTDataType == DT_INT){
						printf("%d ", (*nd).BTNdKys[i]->v.intV);
					}else if((*tm).BTDataType == DT_FLOAT){
						printf("%.02f ", (*nd).BTNdKys[i]->v.floatV);
					}else if((*tm).BTDataType == DT_BOOL){
						printf("%d ", (*nd).BTNdKys[i]->v.boolV);
					}
					printf("(%d - %d) ", ((BTNdDataStruct *) (*nd).BTNdPtr[i])->rid.page, ((BTNdDataStruct *) (*nd).BTNdPtr[i])->rid.slot);
					i++;
				}
				if((*nd).isNodeLeaf){
					// printf("LEAF NODE REACHED");
				}else{
					int z = 0;
					while(z < (*nd).keyCnt+1){
						enQueueNode(tm, (*nd).BTNdPtr[i]);
						z++;
					}
				}
				printf("| ");
			}
			printf('\n');
			return '\0';
		}else{
			// Tree Empty
			printf("TREE IS EMPTY");
			return '\0';
		}
	}
}
