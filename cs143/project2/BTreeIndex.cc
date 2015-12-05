/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"

using namespace std;

/*
 * BTreeIndex default constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid      = -1;
    treeHeight   =  0;
    keyCount     =  0;
    leafCount    =  0;
    nonLeafCount =  0;
}

/*parametarized constructor*/
//BTreeIndex::BTreeIndex(PageId   p_rootPid, int p_treeHeight, int p_keyCount, int p_leafCount, int p_nonLeafCount)
//	: rootPid(p_rootPid), treeHeight(p_treeHeight), keyCount(p_keyCount), leafCount(p_leafCount), nonLeafCount(p_nonLeafCount)
//{}

/**
 * Private helper function that will update the data members assuming that pid
 * 0 holds the index's bookkeeping values. Sets rootPid, treeHeight, and keys
 * according to the values found therein.
 * @return error code. 0 if no error.
 */
RC BTreeIndex::getIndexBookkeeping() {
    // Read the bookkeeping values into the buffer
    int buffer[PageFile::PAGE_SIZE / sizeof(int)];
    RC code = pf.read(BTreeIndex::BOOKKEEPING_PID, buffer);

    if (code) {
        return code;
    }

    // If we're here, then everything went OK on the read, and we now
    // know that the bookkeeping values will be stored in the following
    // order:
    //   1. Root pid
    //   2. Tree height
    //   3. Key count of keys stored in tree
    //   4. Number of leaf nodes
    //   5. Number of non-leaf nodes
    rootPid      = buffer[0];
    treeHeight   = buffer[1];
    keyCount     = buffer[2];
    leafCount    = buffer[3];
    nonLeafCount = buffer[4];
    
    return 0;
}

/**
 * Private helper function that will write to the pagefile whatever is in the
 * current bookkeeping values (rootPid, treeHeight, keyCount).
 * @return error code. 0 if no error.
 */
RC BTreeIndex::writeIndexBookkeeping() {
    int buffer[PageFile::PAGE_SIZE / sizeof(int)];
    
    buffer[0] = rootPid;
    buffer[1] = treeHeight;
    buffer[2] = keyCount;
    buffer[3] = leafCount;
    buffer[4] = nonLeafCount;

    RC code = pf.write(BTreeIndex::BOOKKEEPING_PID, buffer);

    return code;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @param newIndex[IN] true for creating new index bookkeeping, false if
 *                     this already exists in the given pagefile
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode, bool newIndex)
{
    // First, open the pagefile in our index's pf member
    RC code = pf.open(indexname, mode);

    // Return an error if there was one opening it
    if (code) {
        return code;
    }

    // If the index is newly created, we want to set the first page (the
    // one at index BOOKKEEPING_PID) to hold default values
    if (newIndex && (mode == 'W' || mode == 'w')) {
        code = writeIndexBookkeeping();

    // Otherwise, assume that the index file has already been configured
    } else {
        code = getIndexBookkeeping();
    }

    return code;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
    RC code = pf.close();

    return code;
}

/**
 * Private helper function that computes the keyCount and treeHeight after
 * an insertion, automatically updating them and writing them to the bookkeeping
 * pagefile.
 * @param isLeaf[IN] whether or not the inserted node was a leaf
 * @return error code. 0 if no error.
 */
RC BTreeIndex::postInsertBookkeep(bool isNewNode, bool isLeaf, bool addNewLevel) {
    keyCount++;
	if (isNewNode && isLeaf) {
        leafCount++;
	}
	else if (isNewNode && !isLeaf)
	{
        nonLeafCount++;
    }
    
	if (addNewLevel)
		treeHeight++;

    return writeIndexBookkeeping();
}

/* insert helper function
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @returns true/false. If there has been a split on the current node, return true;
 * if there has been no split, return false
 */
bool BTreeIndex::insertHelper(int key, const RecordId& rid, PageId currentPid, int& outKey, PageId& newNodePid)
{
	BTNode node;
	node.read(currentPid, pf);

	// base case: leaf node
	if (node.isLeafNode())
	{
		// Copy the base class BTNode into a local BTLeafNode
		BTLeafNode leafNode(node);
		// Perform the insert
		if (leafNode.insert(key, rid) == 0)
		{
			writeNodeToDisk(false, false, leafNode);  // requires m_pid and m_isleaf already set.
			return false; // To indicate we are done. No split. Assuming no key is -1
		}

		BTLeafNode siblingNode;
		PageId newPid = pf.endPid();
		siblingNode.setPid(newPid);
		siblingNode.setIsLeaf(true);

		int siblingKey; // output value for insertAndSplit

		if (leafNode.insertAndSplit(key, rid, siblingNode, siblingKey) == 0)
			{
				//cerr << "testing" << endl;
				outKey = siblingKey;
				newNodePid = newPid; // pass out back the pid of the new node
				writeNodeToDisk(false, false, leafNode);  // write node to disk
				writeNodeToDisk(true, false, siblingNode);  // write node to disk
				return true;  // splitted the node, needs a promote to parent
			} 
	}

	// recursive case: non-leaf node
	else
	{
		// Copy the base class BTNode into a local BTNonLeafNode
		BTNonLeafNode nonLeafNode(node);
		PageId nextPid;
		int midKey;
		PageId new_node_pid; // for insertHelper to pass back the new node waiting to be incorporated
		if (nonLeafNode.locateChildPtr(key, nextPid) == 0)  // && nextPid != -1)
		{	
			//cerr <<"next pid is "<< nextPid << endl;
			if (!insertHelper(key, rid, nextPid, midKey, new_node_pid)) 
			{
				return false;   // no split on child node, we don't need to insert anything, done!
			}
			
			//PageId newPid = pf.endPid();
			if (nonLeafNode.insert(midKey, new_node_pid) == 0)
			{
				//cerr << "midkey is " << midKey;
				writeNodeToDisk(false, false, nonLeafNode);  // write node to disk
				return false; // no split on current node after insertion, done.
			}

			// needs to split current non-leaf node
			BTNonLeafNode siblingNode;
			PageId newPid = pf.endPid();
			siblingNode.setPid(newPid);
			siblingNode.setIsLeaf(false);

			int siblingKey; // output value for insertAndSplit
			if (nonLeafNode.insertAndSplit(midKey, new_node_pid, siblingNode, siblingKey) == 0);
				{
					outKey = siblingKey;
					newNodePid = newPid;
					writeNodeToDisk(false, false, nonLeafNode);  // write node to disk
					writeNodeToDisk(true, false, siblingNode);  // write node to disk
					return true;
				}
		}
	}
}
/*the node passed in must have already set pid*/
RC BTreeIndex::writeNodeToDisk(bool isNewNode, bool isNewRoot, BTNode& node)
{
	PageId pid = node.getPid();
	RC code = node.write(pid, pf);
	
	if (isNewRoot)
		rootPid = pid;

	postInsertBookkeep(isNewNode, node.isLeafNode(), isNewRoot);
	
	return code;
}

/*
// if insertAndSplit() is called, a new leaf node will be returned in the parameter.
// assign a new pid to it and insert that pid and the new node's key value into
// the original node's parent node.
// if insert() returns success, then done. if the parent (non-leaf node) is full,
// call insertAndSplit() on the parent. Then assign the new non-leaf node a new pid,
// move the midkey to the grandparent. if insert() fails, call insertAndSplit(). Repeat the steps.
// if I splitted the old root, then I have created a new root. Create a new non-leaf node and/
// insert the mid-ley and the newly assigned pid into that non-leaf node.
}
*/
/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
    // ---------------------------------
    // CASE 1: Empty tree
    // ---------------------------------
    if (rootPid == -1) 
	{
        // Get the next available pid in the pagefile
        PageId newRootPid = pf.endPid();

        // Create a new "root" that will simply be a leaf
        BTLeafNode root;
        root.setPid(newRootPid);
        root.setIsLeaf(true);
        root.insert(key, rid);
        
		return writeNodeToDisk(true, true, root);

        //RC code = root.write(newRootPid, pf);
        //// If everything went well, we have a new root!
        //if (! code) {
        //    rootPid = newRootPid;
        //    postInsertBookkeep(true);
        //}
        //return code;
    }

    // ---------------------------------
    // CASE 2: Non-empty tree
    // ---------------------------------
	int outKey;
	PageId outPid;
	//cerr << "freaking test" << endl;
	if (!insertHelper(key, rid, rootPid, outKey, outPid))
	{
		return 0;  // we are done inserting, no need to create a new root.
	}
	// we need to create a new root
	BTNonLeafNode newRoot;
	// Get the next available pid in the pagefile
	PageId newRootPid = pf.endPid();
	newRoot.setPid(newRootPid);
	newRoot.setIsLeaf(false);
	newRoot.initializeRoot(rootPid, outKey,outPid);

    return writeNodeToDisk(true, true, newRoot);
}

/**
 * Private helper function that recursively searches for a given searchKey by
 * starting the search at the root.
 * @param key[IN] the key to find
 * @param currentPid[IN] the pid of the current node to examine
 * @param cursor[OUT] the cursor pointing to the first index entry
 * with the key value
 * @return error code. 0 if no error.
 */
RC BTreeIndex::locateHelper(int searchKey, PageId currentPid, IndexCursor& cursor) {
    // First, create node object for the current page
    BTNode node;
    node.read(currentPid, pf);
    RC code = 0;

    // Case where our node is a leaf
    if (node.isLeafNode()) {
        // Copy the base class BTNode into a local BTLeafNode
        BTLeafNode leafNode(node);
        // Perform the lookup
        code = leafNode.locate(searchKey, cursor.eid);
        cursor.pid = currentPid;
    
    // Otherwise, we have a non-leaf, so we must recurse
    } else {
        // Copy the base class BTNode into a local BTNonLeafNode
        BTNonLeafNode nonLeafNode(node);
        // Perform the lookup
        PageId nextPid = -1;
        code = nonLeafNode.locateChildPtr(searchKey, nextPid);

        if (nextPid != -1) {
            code = locateHelper(searchKey, nextPid, cursor);
        }
        
    }

    return code;
}

/*
 * Find the leaf-node index entry whose key value is larger than or 
 * equal to searchKey, and output the location of the entry in IndexCursor.
 * IndexCursor is a "pointer" to a B+tree leaf-node entry consisting of
 * the PageId of the node and the SlotID of the index entry.
 * Note that, for range queries, we need to scan the B+tree leaf nodes.
 * For example, if the query is "key > 1000", we should scan the leaf
 * nodes starting with the key value 1000. For this reason,
 * it is better to return the location of the leaf node entry 
 * for a given searchKey, instead of returning the RecordId
 * associated with the searchKey directly.
 * Once the location of the index entry is identified and returned 
 * from this function, you should call readForward() to retrieve the
 * actual (key, rid) pair from the index.
 * @param key[IN] the key to find.
 * @param cursor[OUT] the cursor pointing to the first index entry
 *                    with the key value.
 * @return error code. 0 if no error.
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    return locateHelper(searchKey, rootPid, cursor);
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
    // First, create node object for the current page
    BTLeafNode leafNode;
    RC code = leafNode.read(cursor.pid, pf);
    
    if (code) { return code; }

    int keyHold = -1;
    RecordId riHold;
    code = leafNode.readEntry(cursor.eid, keyHold, riHold);

    // Case where we try to access a negative bound
    if (code == RC_INVALID_CURSOR) {
        return code;
    }

    // Case where we've gone out of the bounds of our node
    if (code == RC_NO_SUCH_RECORD) {
        PageId next = leafNode.getNextNodePtr();
        // Check if we can advance to the neighbor
        if (next != -1) {
            cursor.pid = next;
            cursor.eid = 0;
            return readForward(cursor, key, rid);

        // Otherwise, we're at the end of the line
        } else {
            return RC_END_OF_TREE;
        }
    }

    // If we get here, then we have a valid cursor entry read, so read
    // it, and then increment the eid
    key = keyHold;
    rid = riHold;
    cursor.eid++;

    return code;
}
