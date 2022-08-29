B+-Tree 

--------------------------------------
Running the Program:

for simplicity we created two scripts which executes the steps below
run_tree_for_test1.sh
run_tree_for_test2.sh


Step 1 - Go to Project root (assign4) using Terminal.
Step 2 - $ make clean (to delete old compiled .o files)
Step 3 - $ make (to compile all project files) 
Step 4 - $ make run_test1 (to run "test_assign4_1.c" file)

To run compile test2 related files:

Step 1 - Repeat Step 1 to 3 from previous steps.
Step 2 - $ make make test2 (to compiletest expression related files "test_assign4_2.c")
Step 3 - $ make run_test2 (to run "test_assign4_2.c" file)


Function Description:

****B+ TREE FUNCTIONS****
--------------------------------------
(a) b_tree_add_operations
--------------------------------------
addLeaf( )
    - This method adds a new pointer and its matching key into a leaf into the record. It returns the leaf node that has been changed.

addSplitLeaf( )
    - This method adds a new key and a pointer to a new record to a leaf node, overriding the tree's order and splitting the leaf in half. It splits the tree and changes it to keep the B+ Tree features.

addNode( )
    - This function generates a new key without breaching the B+ tree constraints.

addSplitNode( )
    - This method creates a new key and pointer to a non-leaf node, causing the node's size to exceed the order and splitting the node in half.

addParent( )
    - This function generates a new node which can be leaf or internal node into the B+ tree and It returns the root of the tree after generateion.

addRoot( )
    - This method creates a new root for two subtrees and populates it with the relevant key.

--------------------------------------
(b) b_tree_delete_operations
--------------------------------------
deleteSingleEntry( )
    - This function removes an entry from the B+ tree and then makes all appropriate changes to preserve the B+ tree properties.

deleteEntries( )
    - This function remomves the the entry/record based on its key.

deleteNodeEntry( )
    - This function removes a record having the key based on its node.

--------------------------------------
(c) b_tree_generate_operations
--------------------------------------
generateRecord( )
    - This function generates a new record element which uses a RID.

generateTree( )
    - This function generates a new tree when the first element/entry is generated in the B+ tree.

generateNewNode( )
    - This function generates a new node, which can be used as a leaf, internal, or root node

--------------------------------------
(d) b_tree_helper
--------------------------------------
getLeafNode( )
    - This function locates the leaf node holding the item with the given key.

getDeletionRecord( )
    - This method looks for an item in our B+ Tree with the supplied key in the argument. If the key exists in the tree, it returns the record; otherwise, it returns null.

enqueueNode()
    - this function is used to add node in queue in FIFO order

dequeueNode()
    - this function is used to remove node in queue in FIFO order

checkRoot( )
    - This function rearranges the root after a record has been deleted and maintains the B+ Tree properties.

combineNodes( )
    - This function merges a node that has become too small. 

balanceNodes( )
    - This function redistributes the entries between two nodes when one entry has less number of nodes.

--------------------------------------
(e) btree_manager
--------------------------------------
initIndexManager( )
    - This function initializes the index manager ans its parameters. 

shutdownIndexManager( )
    - This function disables the index manager and de-allocates all of the index manager's resources. It also frees up all of the Index Manager's resources and memory space.

createBtree( )
    - This function creates a new B+ Tree. We start the buffer manager, construct a buffer pool with Buffer Manager, then use Storage Manager to generate a page with the supplied page name "idxId."

openBtree( )
    - This method opens a previously saved B+ Tree in the file supplied by the "idxId" parameter. We get our TreeManager back and set up the Buffer Pool.

closeBtree( )
    - This function closes the B+ Tree. To write back pages back to disk by the buffer manager, it also marks all pages dirty. It then finally shuts down the buffer pool and frees up memory.

deleteBtree( )
    - This function deletes the page file based on its index in the parameter with the help of storage manager.

getNumNodes( )
    - This function returns the current number of nodes in B+ trees.

getNumEntries( )
    - This function returns the number of entries/records/keys present in our B+ Tree.

getKeyType( ) 
    - The datatype of the keys in our B+ Tree is returned by this function.

findKey( )
    - This method searches the B+ Tree for the key specified in the parameter and If entry found, we store the RID (value) for that key in the one of the parameters.

deleteKey( )
    - This function removes the entry/record based on the key. This finction also rearranges the tree accordingly so as to maintain the B+ Tree properties.

openTreeScan( )
    - This function starts the scan, which is used to scan the B+ Tree entries in sorted key order. This function sets up our BTScanner structure, which holds additional information needed to execute the scan.

nextEntry( )
    - This function traverses the entries in the B+ Tree.

closeTreeScan( )
    - This function ends the scanning process and frees up resources.

printTree( )
    - This function prints the B+ Tree in order.

findNgbrInd( )
    - If a node's nearest neighbor (sibling) to the left exists, this method gives its index. If this is not the case, -1 is returned, indicating that the node is the leftmost child.


