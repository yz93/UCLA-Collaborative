/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#ifndef BTREEINDEX_H
#define BTREEINDEX_H

#include "Bruinbase.h"
#include "PageFile.h"
#include "RecordFile.h"
#include "BTreeNode.h"
             
/**
 * The data structure to point to a particular entry at a b+tree leaf node.
 * An IndexCursor consists of pid (PageId of the leaf node) and 
 * eid (the location of the index entry inside the node).
 * IndexCursor is used for index lookup and traversal.
 */
typedef struct {
  // PageId of the index entry
  PageId  pid;  
  // The entry number inside the node
  int     eid;  
} IndexCursor;

/**
 * Implements a B-Tree index for bruinbase.
 * 
 */
class BTreeIndex {
 public:
  BTreeIndex();
  /**
   * Open the index file in read or write mode.
   * Under 'w' mode, the index file should be created if it does not exist.
   * @param indexname[IN] the name of the index file
   * @param mode[IN] 'r' for read, 'w' for write
   * @param newIndex[IN] true for creating new index bookkeeping, false if
   *                     this already exists in the given pagefile
   * @return error code. 0 if no error
   */
  RC open(const std::string& indexname, char mode, bool newIndex = true);

  /**
   * Close the index file.
   * @return error code. 0 if no error
   */
  RC close();
    
  /**
   * Insert (key, RecordId) pair to the index.
   * @param key[IN] the key for the value inserted into the index
   * @param rid[IN] the RecordId for the record being inserted into the index
   * @return error code. 0 if no error
   */
  RC insert(int key, const RecordId& rid);

  /**
   * Find the leaf-node index entry whose key value is larger than or
   * equal to searchKey and output its location (i.e., the page id of the node
   * and the entry number in the node) as "IndexCursor."
   * IndexCursor consists of pid (page id of the node that contains the 
   * searchKey) and eid (the entry number inside the node)
   * to indicate the location of a particular index entry in the B+tree.
   * Note that, for range queries, we need to scan the B+tree leaf nodes.
   * For example, if the query is "key > 1000", we should scan the leaf
   * nodes starting with the key value 1000. For this reason,
   * this function returns the location of the leaf node entry
   * for a given searchKey, instead of returning the RecordId
   * associated with the searchKey.
   * Using the returned "IndexCursor", you will have to call readForward()
   * to retrieve the actual (key, rid) pair from the index.
   * @param key[IN] the key to find
   * @param cursor[OUT] the cursor pointing to the first index entry
   * with the key value
   * @return error code. 0 if no error.
   */
  RC locate(int searchKey, IndexCursor& cursor);

  /**
   * Read the (key, rid) pair at the location specified by the index cursor,
   * and move foward the cursor to the next entry.
   * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
   * @param key[OUT] the key stored at the index cursor location
   * @param rid[OUT] the RecordId stored at the index cursor location
   * @return error code. 0 if no error
   */
  RC readForward(IndexCursor& cursor, int& key, RecordId& rid);

  /*return a pointer to index's pf. Used for testing now, may be useful in part d*/
  PageFile* getPf() { return &pf; }
  
  PageId getRootPid() { return rootPid; }
 private:
  /**
   * Private helper function that will update the data members assuming that pid
   * 0 holds the index's bookkeeping values. Sets rootPid, treeHeight, and keys
   * according to the values found therein.
   * @return error code. 0 if no error.
   */
  RC getIndexBookkeeping();

  /**
   * Private helper function that will write to the pagefile whatever is in the
   * current bookkeeping values (rootPid, treeHeight, keyCount).
   * @return error code. 0 if no error.
   */
  RC writeIndexBookkeeping();

  /**
   * Private helper function that computes the keyCount and treeHeight after
   * an insertion, automatically updating them and writing them to the bookkeeping
   * pagefile.
   * @param isLeaf[IN] whether or not the inserted node was a leaf
   * @return error code. 0 if no error.
   */
  RC postInsertBookkeep(bool isNewNode, bool isLeaf, bool addNewLevel);

  /**
   * Private helper function that recursively searches for a given searchKey by
   * starting the search at the root.
   * @param key[IN] the key to find
   * @param currentPid[IN] the pid of the current node to examine
   * @param cursor[OUT] the cursor pointing to the first index entry
   * with the key value
   * @return error code. 0 if no error.
   */
  RC locateHelper(int searchKey, PageId currentPid, IndexCursor& cursor);

  /**
  * Private helper function that recursively insert an entry and split nodes
  * @param key[IN] the key for the value inserted into the index
  * @param rid[IN] the RecordId for the record being inserted into the index
  * @returns true/false. If there has been a split on the current node, return true;
  * if there has been no split, return false
  */
  bool insertHelper(int key, const RecordId& rid, PageId currentPid, int& outKey, PageId& newNodePid);

  /*Private helper function to wirte a node to disk and do updates on meta-info*/
  RC writeNodeToDisk(bool isNewNode, bool isNewRoot, BTNode& node);

  static const PageId BOOKKEEPING_PID = 0; /// pid of pagefile's bookkeeping records

  PageFile pf;           /// the PageFile used to store the actual b+tree in disk

  PageId   rootPid;      /// the PageId of the root node
  int      treeHeight;   /// the height of the tree
  int      keyCount;     /// the number of keys stored in the tree
  int      leafCount;    /// the number of leaves in the tree
  int      nonLeafCount; /// the number of non-leaves in the tree
  /// Note that the content of the above two variables will be gone when
  /// this class is destructed. Make sure to store the values of the two 
  /// variables in disk, so that they can be reconstructed when the index
  /// is opened again later.
};

#endif /* BTREEINDEX_H */
