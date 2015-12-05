/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include "Bruinbase.h"
#include "SqlEngine.h"

// [!] REMOVE BEFORE SUBMISSION ---------------------------
#include <iostream>
#include <cassert>
#include "BTreeNode.h"
using namespace std;
const bool TESTING = true;
// --------------------------------------------------------

void testLeaves (PageFile& pf) {
    // Leaf node:
    // [  3,   1,  2,  3,  2, 4, 5, 1, 3, 10, ...   3 ]
    //  size  pid sid key  p1 s1 k1 p2 s2 k2, neighbor's pid
    int leaf1[256] = {3, 1, 2, 3, 2, 4, 5, 1, 3, 10};
    leaf1[255] = 3;
    int leaf2[256] = {1, 1, 3, 20};
	leaf2[255] = -1;
	
	/*create two new nodes for the test of insertAndSplit()*/
	int leaf3[256]; 
	leaf3[0] = 84; // 84 entries, a full node
	for (int i = 1; i < 253; i++) // initialize node to be [84,  4,5,6, 7,8,9, 10,11,12, ... 253,254,255,     NaN,NaN,      -1]
		leaf3[i] = 3+i;			  //						size          ... entries...                 extra 2 bytes  next pid
	leaf3[255] = -1; // node 3 will point to node 4 after split. Now pointing to nowhere
	int leaf4[256] = {0}; // empty node, will be used as sibling node
	leaf4[255] = -1;


    pf.write(0, leaf1);
    pf.write(3, leaf2);
	pf.write(1,leaf3);
	pf.write(4,leaf4);

    // Get some nodes up in this
    BTLeafNode n1,
               n2;
	n1.setPid(0);
	n2.setPid(3);

	BTLeafNode n3, n4;
	n3.setPid(1);
	n4.setPid(4);

    assert(n1.getKeyCount() == -1);
    assert(n2.getKeyCount() == -1);

    // Read values into buffer
    n1.read(0, pf);
    n2.read(3, pf);
	n3.read(1, pf);
	n4.read(4, pf);

    // Verify size of the leaves
    assert(n1.getKeyCount() == 3);
    assert(n2.getKeyCount() == 1);

	// test printLeafNode function
	cerr << "n1 is: ";
	n1.printLeafNode();
	cerr << "n2 is: ";
	n2.printLeafNode();

    // Verify neighbor pid
    assert(n1.getNextNodePtr() == 3);
    assert(n2.getNextNodePtr() == -1);

    // Verify rids and keys
    int k;
    RecordId rid;
    n1.readEntry(0, k, rid);
    assert(k == 3);
    assert(rid.pid == 1 && rid.sid == 2);

    n1.readEntry(1, k, rid);
    assert(k == 5);
    assert(rid.pid == 2 && rid.sid == 4);

	// test insert() function
	// n1 leaf node: [ 3,   1,2,3,  0,0,4,   2,4,5,   1,3,10, ...   3 ]
	RecordId id;
	int key;
	id.pid = 0;
	id.sid = 0;
	key = 4;
	assert(n1.insert(key,id)==0);
	cerr << "Insert-->n1: ";
	n1.printLeafNode();
	key = 11;
	id.pid = 2;
	id.sid = 0;
	assert(n1.insert(key, id) == 0);
	cerr << "Insert-->n1: ";
	n1.printLeafNode();
	assert(n3.insert(key, id) == RC_NODE_FULL);
	
	// test insertAndSplit()
	cerr << "n3 is: ";
	n3.printLeafNode();
	cerr << "n4 is: ";
	n4.printLeafNode();

	id.pid = 1;
	id.sid = 2;
	key = 256;
	int returnKey;
	assert(n3.insertAndSplit(key, id, n4, returnKey) == 0);
	cerr << "Insert and Split-->n3: ";
	n3.printLeafNode();
	cerr << "Insert and Split-->n4: ";
	n4.printLeafNode();

	assert(returnKey == 135); 

	// Check the locate function
	int i;
	n1.locate(1, i);  // first key is larger than 1
	assert(i == 0);
	n1.locate(3, i);  // first key equals to 3
	assert(i == 0);
	n1.locate(4, i);
	assert(i == 1);
	n1.locate(11, i);
	assert(i == 4);

    // Check setting the next node pointers
    n1.setNextNodePtr(4);
    assert(n1.getNextNodePtr() == 4);

    cerr << "[!] ALL LEAF NODE UNIT TESTS PASSED!" << endl;
}

void testNonLeaves (PageFile& pf) {
    // Non-Leaf Node
    // [  3,   1,    3,    2,     5,     2,     10,  1 ]
    //  size  <k0   k0  <k1>=k0  k1   <k2>=k1   k2  >k2
    int node1[256] = {3, 1, 3, 2, 5, 2, 10, 1};
    int node2[256] = {1, 1, 3, 2};
	int node3[256] = {2, 1, 0, 2, 2, 3}; // key is 0 and 2.
	int node4[256] = { 127, 1 }; // full, starting pointer points to 1
	int node5[256] = {0,0}; // empty, has a pointer to pid 0
	for (int i = 0; i < 254; i++) // make node4 full to test insertAndSplit. 0,1,  0,1 2,3 4,5 ... 252,253
		node4[i + 2] = i;
    pf.write(0, node1);
    pf.write(3, node2);
	pf.write(5, node3);
	pf.write(6, node4);
	pf.write(7, node5);

    BTNonLeafNode n1,
                  n2;
	BTNonLeafNode n3, 
				  n4, 
				  n5;
	
    assert(n1.getKeyCount() == -1);
    assert(n2.getKeyCount() == -1);

    // Read values into buffer
    n1.read(0, pf);
    n2.read(3, pf);
	n3.read(5, pf);
	n4.read(6, pf);
	n5.read(7, pf);

    // Verify size of the leaves
    assert(n1.getKeyCount() == 3);
    assert(n2.getKeyCount() == 1);
	assert(n3.getKeyCount() == 2);

    // Check the locateChildPtr function
    int i = -1;
    n1.locateChildPtr(1, i);
    assert(i == 1);
    n1.locateChildPtr(3, i);
    assert(i == 2);
    n1.locateChildPtr(4, i);
    assert(i == 2);
    n1.locateChildPtr(11, i);
    assert(i == 1);

    // initializeRoot()
    BTNonLeafNode root;
    root.initializeRoot(1, 2, 3);
    assert(root.getKeyCount() == 1);
    root.locateChildPtr(1, i);
    assert(i == 1);
    root.locateChildPtr(2, i);
    assert(i == 3);
    root.locateChildPtr(3, i);
    assert(i == 3);

	// insert()
	PageId pid = 6;
	int key = 1;
	n3.insert(key,pid);  //{3, 1, 0, 2, 1, 6, 2, 3}
	n3.printNonLeafNode();
	
	// insertAndSplit()
	int midkey = 0;
	pid = 255;
	key = 254;
	assert(n4.insertAndSplit(key, pid, n5, midkey) == 0);
	cerr << "Insert and Split-->n4: ";
	n4.printNonLeafNode();
	cerr << "Insert and Split-->n5: ";
	n5.printNonLeafNode();
	assert(midkey == 128);
    cerr << "[!] ALL NON-LEAF NODE UNIT TESTS PASSED!" << endl;
}

int main()
{
    if (TESTING) {
        cerr << "[!] UNIT TESTS BEGUN: DO NOT SUBMIT THIS VERSION" << endl;
        PageFile pf1("testLeaves.del", 'w');
        testLeaves(pf1);
        pf1.close();

        PageFile pf2("testNonLeaves.del", 'w');
        testNonLeaves(pf2);
        pf2.close();

        cerr << "[!] ALL UNIT TESTS PASSED!" << endl;
    } else {
        // run the SQL engine taking user commands from standard input (console).
        SqlEngine::run(stdin);
    }
    return 0;
}
