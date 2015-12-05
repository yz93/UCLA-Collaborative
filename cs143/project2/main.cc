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
#include <fstream>
#include "BTreeNode.h"
#include "BTreeIndex.h"
using namespace std;

// --------------------------------------------------------

// --------------------------------------------------------------------------------------------------------
// PROJECT 2B ---------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

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
		leaf3[i] = 3+i;			  //						size          ... entries...                 extra 8 bytes  next pid
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
	//cerr << "n1 is: ";
	//n1.printLeafNode();
	//cerr << "n2 is: ";
	//n2.printLeafNode();

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
	//cerr << "Insert-->n1: ";
	//n1.printLeafNode();
	key = 11;
	id.pid = 2;
	id.sid = 0;
	assert(n1.insert(key, id) == 0);
	//cerr << "Insert-->n1: ";
	//n1.printLeafNode();
	assert(n3.insert(key, id) == RC_NODE_FULL);
	
	// test insertAndSplit()
	//cerr << "n3 is: ";
	//n3.printLeafNode();
	//cerr << "n4 is: ";
	//n4.printLeafNode();

	id.pid = 1;
	id.sid = 2;
	key = 256;
	int returnKey;
	assert(n3.insertAndSplit(key, id, n4, returnKey) == 0);
	//cerr << "Insert and Split-->n3: ";
	//n3.printLeafNode();
	//cerr << "Insert and Split-->n4: ";
	//n4.printLeafNode();

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
	//n3.printNonLeafNode();

	int midkey = 0;
	pid = 255;
	key = 254;
	assert(n4.insertAndSplit(key, pid, n5, midkey) == 0);
	//cerr << "Insert and Split-->n4: ";
	//n4.printNonLeafNode();
	//cerr << "Insert and Split-->n5: ";
	//n5.printNonLeafNode();
	assert(midkey == 126);
    cerr << "[!] ALL NON-LEAF NODE UNIT TESTS PASSED!" << endl;
}


// --------------------------------------------------------------------------------------------------------
// PROJECT 2C ---------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

void testProject2C () {
	/*TEST LOCATE*/
    // STEP 1: Configure mock PageFile
    PageFile pf("locateTest.idx", 'w');

    // We'll have a simple B+ tree with 1 non-leaf root and 2 leaves
    int buffer[255];
    buffer[0] = 1; // rootpid
    buffer[1] = 2; // treeHeight
    buffer[2] = 5; // keyCount
    buffer[3] = 2; // leafCount
    buffer[4] = 1; // nonLeafCount
    pf.write(0, buffer);

    // Root will have pid = 1, key = 5, and have children pids
    // 2 for < 5 and 3 for >= 5
    BTNonLeafNode root;
    root.setPid(1);
    root.setIsLeaf(false);
    root.initializeRoot(2, 5, 3);
	//root.printNonLeafNode();
    root.write(1, pf);

    // Leaf1 will have 3 keys: 1, 3, 4
    BTLeafNode leaf1;
    leaf1.setPid(2);
    leaf1.setIsLeaf(true);
    RecordId r1;
    r1.pid = 1;
    r1.sid = 2;
    leaf1.insert(1, r1);
    RecordId r2;
    r2.pid = 3;
    r2.sid = 4;
    leaf1.insert(3, r2);
    RecordId r3;
    r3.pid = 5;
    r3.sid = 6;
    leaf1.insert(4, r3);
    leaf1.setNextNodePtr(3);
	//leaf1.printLeafNode();
    leaf1.write(2, pf);

    // Leaf2 will have 2 keys: 5, 7
    BTLeafNode leaf2;
    leaf2.setPid(3);
    leaf2.setIsLeaf(true);
    RecordId r4;
    r4.pid = 7;
    r4.sid = 8;
    leaf2.insert(5, r4);
    RecordId r5;
    r5.pid = 9;
    r5.sid = 10;
    leaf2.insert(7, r5);
	//leaf2.printLeafNode();
    leaf2.write(3, pf);

    pf.close();


    // STEP 2: Configure the index object using that PageFile
    BTreeIndex index;
    index.open("locateTest.idx", 'w', false); // false passed for existing pagefile


    // STEP 3: Perform locate tests
    int key = -5;
    RecordId ridCheck;
    IndexCursor curseYou;
    curseYou.pid = -5;
    curseYou.eid = -5;

    index.locate(1, curseYou);
    assert(curseYou.pid == 2);
    assert(curseYou.eid == 0);

    index.readForward(curseYou, key, ridCheck);
    assert(key == 1);
    assert(ridCheck.pid == 1 && ridCheck.sid == 2);
    assert(curseYou.pid == 2 && curseYou.eid == 1);

    index.locate(2, curseYou);
    assert(curseYou.pid == 2);
    assert(curseYou.eid == 1);

    index.readForward(curseYou, key, ridCheck);
    assert(key == 3);
    assert(ridCheck.pid == 3 && ridCheck.sid == 4);
    assert(curseYou.pid == 2 && curseYou.eid == 2);

    index.locate(3, curseYou);
    assert(curseYou.pid == 2);
    assert(curseYou.eid == 1);
    
    index.locate(4, curseYou);
    assert(curseYou.pid == 2);
    assert(curseYou.eid == 2);

    index.readForward(curseYou, key, ridCheck);
    assert(key == 4);
    assert(ridCheck.pid == 5 && ridCheck.sid == 6);
    assert(curseYou.pid == 2 && curseYou.eid == 3);

    index.readForward(curseYou, key, ridCheck);
    assert(key == 5);
    assert(ridCheck.pid == 7 && ridCheck.sid == 8);
    assert(curseYou.pid == 3 && curseYou.eid == 1);

    index.locate(5, curseYou);
    assert(curseYou.pid == 3);
    assert(curseYou.eid == 0);

    index.readForward(curseYou, key, ridCheck);
    assert(key == 5);
    assert(ridCheck.pid == 7 && ridCheck.sid == 8);
    assert(curseYou.pid == 3 && curseYou.eid == 1);

    index.locate(7, curseYou);
    assert(curseYou.pid == 3);
    assert(curseYou.eid == 1);

    index.readForward(curseYou, key, ridCheck);
    assert(key == 7);
    assert(ridCheck.pid == 9 && ridCheck.sid == 10);
    assert(curseYou.pid == 3 && curseYou.eid == 2);

    assert(index.readForward(curseYou, key, ridCheck) == RC_END_OF_TREE);

    index.locate(8, curseYou);
    assert(curseYou.pid == 3);
    assert(curseYou.eid == -1);

    assert(index.readForward(curseYou, key, ridCheck) == RC_INVALID_CURSOR);
    
    index.close();
    cerr << "[!] ALL LOCATE UNIT TESTS PASSED!" << endl;

	/*****************************************************************************/
	/**********************TEST INSERT********************************************/
	/*****************************************************************************/
	index.open("locateTest.idx", 'w', false); // false passed for existing pagefile
	// insert key = 8, pid = 11, sid = 12.
	RecordId r6;
	r6.pid = 11;
	r6.sid = 12;
	assert(index.insert(8, r6) == 0);
	index.locate(8, curseYou);
	assert(curseYou.pid == 3);
	assert(curseYou.eid == 2);
	//pf.open("locateTest.idx", 'w');
	leaf2.read(3, *index.getPf());  // re-load leaf2 to test if insert successfully write to disk.

	/*cerr <<endl<< "After inserting \"11 12 8\", leaf2 looks like: " << endl;
	leaf2.printLeafNode();
	cerr << endl;*/
	
	RecordId rid;
	for (int i = 9; i <= 5465; i++)  // make leaf2 a full node has key : 5,7,8,9,10,11,12...89 --- a total of 84 keys
	{
		rid.pid = 11 + i - 8;
		rid.sid = 12 + i - 8;
		index.insert(i,rid);
	}
	/*
	BTNonLeafNode newRoot;
	newRoot.read(131, *index.getPf());
	cerr << "new root looks like: " << endl;
	newRoot.printNonLeafNode();
	cerr << endl;

	BTNonLeafNode level2LeftNode;
	level2LeftNode.read(1, *index.getPf());
	cerr << "the left node on level 2 looks like: " << endl;
	level2LeftNode.printNonLeafNode();
	cerr << endl;
	
	BTNonLeafNode level2RightNode;
	level2RightNode.read(130, *index.getPf());
	cerr << "the right node on level 2 looks like: " << endl;
	level2RightNode.printNonLeafNode();
	cerr << endl;
	
	BTLeafNode newLeafNode;
	newLeafNode.read(129, *index.getPf());
	cerr << "the new leaf node looks like: " << endl;
	newLeafNode.printLeafNode();
	cerr << endl;*/

	/*
	root.read(1, *index.getPf());
	cerr << "root looks like: " << endl;
	root.printNonLeafNode();
	cerr << endl;
	
	leaf1.read(2, *index.getPf());
	cerr << "leaf1 looks like: " << endl;
	leaf1.printLeafNode();
	cerr << endl;*/
	
	/*leaf2.read(3, *index.getPf());
	cerr << "leaf2 looks like: " << endl;
	leaf2.printLeafNode();  
	cerr << endl;

	BTLeafNode leaf126;
	leaf126.read(127, *index.getPf());
	cerr << "leaf127 looks like: " << endl;
	leaf126.printLeafNode();
	cerr << endl;

	
	BTLeafNode leaf127;
	leaf127.read(128, *index.getPf());
	cerr << "leaf127 looks like: " << endl;
	leaf127.printLeafNode();
	cerr << endl;*/
	//leaf2.write(3, *index.getPf());

	//RecordId r7;
	//r7.pid = 93;
	//r7.sid = 94;
	//assert(index.insert(90, r7) == 0);  // now root should be 2 2 5 3 49 4 
	//root.read(1, *index.getPf());
	//leaf2.read(3, *index.getPf());
	//cerr << "After inserting 93, 94, 90, the root looks like this:" << endl;
	//root.printNonLeafNode();
	//cerr << endl;
	//cerr << "After inserting 93, 94, 90, leaf2 looks like this:" << endl;
	//leaf2.printLeafNode();

	//BTLeafNode node4;
	//node4.read(4, *index.getPf());
	//cerr << endl;
	//cerr << "After inserting 93, 94, 90, we have a new node: " << endl;
	//node4.printLeafNode();

	/*TEST insert on empty index tree*/
	BTreeIndex index2;
	index2.open("insertTest2.idx", 'w', true);  // true->creates a new index
	RecordId r;
	BTNonLeafNode shouldBeRoot;
	BTLeafNode node1, node2;

	for (int i = 1; i <= 85; i++)
	{
		r.pid = i;
		r.sid = i;
		index2.insert(i, r);
	}

	/*shouldBeRoot.read(3, *index2.getPf());
	cerr << "root is:" << endl;
	shouldBeRoot.printNonLeafNode();
	cerr << endl;

	node1.read(1, *index2.getPf());
	cerr << "node 1 is:" << endl;
	node1.printLeafNode();
	cerr << endl;

	node2.read(2, *index2.getPf());
	cerr << "node 2 is:" << endl;
	node2.printLeafNode();
	cerr << endl;*/

	//int key = -5; // output for readforward
	RecordId ri;  // output for readforward

	IndexCursor cursor;
	cursor.pid = -5;
	cursor.eid = -5;
	/*TEST LOCATE*/
	//cerr << "root pid is " << index2.getRootPid();
	index2.locate(1, cursor);
	assert(cursor.pid == 1);
	assert(cursor.eid == 0);

	index2.locate(2, cursor);
	assert(cursor.pid == 1);
	assert(cursor.eid == 1);

	index2.locate(44, cursor);
	assert(cursor.pid == 2);
	assert(cursor.eid == 0);

	index2.locate(56, cursor);
	assert(cursor.pid == 2);
	assert(cursor.eid == 12);

	index2.locate(85, cursor);
	assert(cursor.pid == 2);
	assert(cursor.eid == 41);
	/*TEST READFORWARD*/
	/*read through the entire node: 1,1,1 ... 43,43,43 */
	cursor.pid = 1;
	cursor.eid = 0;

	for (int k = 1; k <= 42; k++)
	{
		index2.readForward(cursor, key, ri);
		assert(key == k);
		assert(ri.pid == k && ri.sid == k);
		assert(cursor.pid == 1 && cursor.eid == k);
	}

	cerr << "[!] ALL INSERTION UNIT TESTS PASSED!" << endl;
}


// --------------------------------------------------------------------------------------------------------
// PROJECT 2D ---------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------


void testProject2D () {
    const char TEST_FILE[]    = "commandTests.txt";
    const char OUT_FILE[]     = "commandOutputs.txt";
    const char CORRECT_FILE[] = "commandCorrect.txt";
    string currentTest;

    // First remove all of the movieTest related files
    remove(TEST_FILE);
    remove(OUT_FILE);
    remove(CORRECT_FILE);
    remove("movietest.tbl");
    remove("movietest.idx");

    // Create an output file to compare our tests with
    ofstream testOutput, testCorrect;
    testOutput.open(OUT_FILE);
    testCorrect.open(CORRECT_FILE);

    // Begin by creating a test file that will simulate the user's command
    // line statements
    ofstream testWrite;
    testWrite.open(TEST_FILE);

    // --------------------------------------------------
    // BEGIN UNIT TESTS ---------------------------------
    // --------------------------------------------------

    // Test: Load statement
    testWrite << "LOAD movieTest FROM 'movie.del' WITH INDEX\n";
    testCorrect << "Bruinbase> ";

    // Test: Simple EQ query
    testWrite << "select * from movieTest where key = 1000\n";
    testCorrect << "Bruinbase> 1000 'Deadly Rhapsody'\n";

    // Test: Simple LT query
    testWrite << "select * from movieTest where key < 3\n";
    testCorrect << "Bruinbase> 2 'Til There Was You'\n";

    // Test: Multi-return LT query
    testWrite << "select * from movieTest where key < 4\n";
    testCorrect << "Bruinbase> 2 'Til There Was You'\n3 '...First Do No Harm'\n";

    // Test: Multi-return LE query
    testWrite << "select * from movieTest where key <= 3\n";
    testCorrect << "Bruinbase> 2 'Til There Was You'\n3 '...First Do No Harm'\n";

    // Test: Count query
    testWrite << "select count(*) from movieTest\n";
    testCorrect << "Bruinbase> 3616\n";

    // Test: Count query with where clause
    testWrite << "select count(*) from movieTest where key <= 3\n";
    testCorrect << "Bruinbase> 2\n";

    // Test: ne clause
    testWrite << "select count(*) from movieTest where key <> 3\n";
    testCorrect << "Bruinbase> 3615\n";

    testWrite << "select count(*) from movieTest where key > 3 and key < 10\n";
    testCorrect << "Bruinbase> 5\n";

    testWrite << "select count(*) from movieTest where key >= 3 and key < 10\n";
    testCorrect << "Bruinbase> 6\n";

    testWrite << "select count(*) from movieTest where key > 3 and key >= 5 and key < 10\n";
    testCorrect << "Bruinbase> 4\n";

    testWrite << "select count(*) from movieTest where key > 3 and key < 10 and key <> 5\n";
    testCorrect << "Bruinbase> 4\n";

    testWrite << "select * from movieTest where key > 3 and key < 10 and key = 5\n";
    testCorrect << "Bruinbase> 5 '100 Girls'\n";

    testWrite << "select value from movieTest where key > 3 and key < 10 and key = 5\n";
    testCorrect << "Bruinbase> 100 Girls\n";

    testWrite << "select count(*) from movieTest where key = 5 and key = 10\n";
    testCorrect << "Bruinbase> 0\n";

    // TESTS ON VALUES

    testWrite << "select count(*) from movieTest where value = 'Die Another Day'\n";
    testCorrect << "Bruinbase> 1\n";

    testWrite << "select * from movieTest where value = 'Die Another Day'\n";
    testCorrect << "Bruinbase> 1074 'Die Another Day'\n";

    testWrite << "select count(*) from movieTest where value = 'Die Another Day' and key > 2000\n";
    testCorrect << "Bruinbase> 0\n";

    // The final new line entered by the user from hitting enter
    testCorrect << "Bruinbase> ";

    // --------------------------------------------------
    // END UNIT TESTS -----------------------------------
    // --------------------------------------------------

    testWrite.close();
    testOutput.close();
    testCorrect.close();

    // Now that the tests are written, we'll read them as though they're
    // the users' input, and write the results of stdout to our output file
    freopen(TEST_FILE, "r", stdin);
    freopen(OUT_FILE, "w", stdout);
    SqlEngine::run(stdin);

    // Re-redirect stdout back to its original setting
    fclose(stdout);
    stdout = fdopen(1, "w");

    // Now that our tests have run, we can compare the output file with the
    // expected correct file
    ifstream answers;
    ifstream expected;
    answers.open(OUT_FILE);
    expected.open(CORRECT_FILE);

    bool passed = true;

    // --------------------------------------------------
    // REVIEW TESTS RESULTS -----------------------------
    // --------------------------------------------------
    string s1;
    string s2;
    int line = 0;

    while (getline(answers, s1)) {
        getline(expected, s2);

        if (s1 != s2) {
            cerr << "[X] OUTPUT MISMATCH AT LINE " << line << ": " << endl
                 << "  EXPECTED: " << s2 << endl
                 << "  GOT: " << s1 << endl;
            passed = false;
            break;
        }
        
        line++;
    }

    // Make sure they end at the same time, and furthermore, that
    // the end comparison is not a record-keeping on the query time
    if (s1 != s2) {
        passed = false;
    }
    

    answers.close();
    expected.close();
    
    if (passed) {
        cerr << "[!] ALL LOAD TESTS PASSED!" << endl;
    } else {
        cerr << "[X] UNIT TESTS FAILED!" << endl;
    }
}

int main(int argc, char *argv[])
{
    // [!] If Bruinbase is run with no arguments, we'll run the tests
    // Otherwise, it will initiate the standard shell
    bool testing = argc == 1;
    if (testing) {
        cerr << "[!] UNIT TESTS BEGUN: DO NOT SUBMIT THIS VERSION" << endl;
        
        // Omitting all previous unit tests
        /*
        PageFile pf1("testLeaves.del", 'w');
        testLeaves(pf1);
        pf1.close();

        PageFile pf2("testNonLeaves.del", 'w');
        testNonLeaves(pf2);
        pf2.close();

		testProject2C();
        */
        
        testProject2D();

        cerr << "[!] ALL UNIT TESTS PASSED!" << endl;
    } else {
        // run the SQL engine taking user commands from standard input (console).
        SqlEngine::run(stdin);
    }
    return 0;
}
