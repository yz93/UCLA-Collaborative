#include "BTreeNode.h"
#include <iostream>

using namespace std;

// --------------------------------------------------------------------------------------------------------
// NODES --------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

/*
 * Constructor that initializes any data members upon new node
 * construction.
 */
BTNode::BTNode () {
    // Starting size of -1 means we haven't read anything into the
    // buffer yet
    m_size = -1;
    m_pid = -1;
    m_isLeaf = false; // Just a default setting

    int* i = (int*) buffer;
    *i = 0;
    i[254] = 0;
    i[255] = -1;
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNode::read(PageId pid, const PageFile& pf)
{
    //if (pid < 0) return RC_INVALID_PID;

    // Attempt to read the page with the given pid in the
    // given PageFile, and store into the member buffer
    RC code = pf.read(pid, buffer);

    // Assuming the read was OK, we'll initialize the data members
    if (!code) {
        m_pid = pid;
        // First element of the buffer will maintain the size
        int* i = (int*) buffer;
        m_size = *(i);
        m_isLeaf = *((int*)((char*)buffer + PageFile::PAGE_SIZE - 2*sizeof(int)));
    }

    return code;
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNode::write(PageId pid, PageFile& pf)
{
    if (pid < 0) return RC_INVALID_PID;

    // Attempt to write to the page in buffer with the given pid in the
    // given PageFile
    RC code = pf.write(pid, buffer);

    return code;
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNode::getKeyCount()
{
    return m_size;
}

void BTNode::setIsLeaf(bool isLeaf) {
    m_isLeaf = isLeaf;
    int* i = (int*) ((char*)buffer + PageFile::PAGE_SIZE - 2*sizeof(int));
    *i = isLeaf;
}

bool BTNode::isLeafNode() {
    return m_isLeaf;
}

/*
 * Sets the int ptr to the index of the key at index eid, but
 * is generalized to jump by jumpAmt ints in the buffer
 */
RC BTNode::ptrToKeyHelp (int eid, int*& ptr, int jumpAmt) {
    if (eid < 0 || eid >= m_size) return RC_INVALID_CURSOR;

    int* i = (int*) buffer;
    ptr = (i + jumpAmt + (jumpAmt * eid));
    return 0;
}

RC BTNode::ptrToKeyHelp2(int* buf, int eid, int*& ptr, int jumpAmt) {
	if (eid < 0 || eid >= m_size) return RC_INVALID_CURSOR;

	int* i = buf;
	ptr = (i + jumpAmt + (jumpAmt * eid));
	return 0;
}

// --------------------------------------------------------------------------------------------------------
// LEAF NODES ---------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

/*
* Insert a (key, rid) pair to the node.
* @param key[IN] the key to insert
* @param rid[IN] the RecordId to insert
* @return 0 if successful. Return an error code if the node is full.
*/
RC BTLeafNode::insert(int key, const RecordId& rid)
{
	/*parameters for memmove()*/
	char* src = NULL;
	char* dest = NULL;
	size_t n = 0;


	if (m_size >= BTNode::MAX_LEAF_KEYS)  // 84 = (1024-4-4)/12, is the maximum number of keys in a leaf node
		return RC_NODE_FULL;

	int* iter = NULL;
	int eid = 0;
	// if node is empty
	if (m_size == -1)
	{
		iter = (int*)buffer;
		*iter = 1;
		*(iter + 1) = rid.pid;
		*(iter + 2) = rid.sid;
		*(iter + 3) = key;
		m_size = 1;
		return 0;
	}
	// if node not empty
	for (; eid < m_size; ++eid)
	{
		ptrToKey(eid, iter);
		if (*iter >= key)
		{
			/*shift the right part by 12 bytes to make room for the new entry*/
			src = (char*)(iter - 2); // move iter to pid of the key
			dest = (char*)(iter + 1);
			n = (m_size - eid) * 12; // three int variables, pid, sid and key: 12 bytes
			memmove(dest, src, n);
			/*copy the new entry*/
			iter -= 2; // move iter to pid
			*iter = rid.pid; // insert pid

			++iter;
			*iter = rid.sid; //insert sid

			++iter;
			*iter = key;  //insert key
			m_size++;  // increment size
			++*((int*)buffer);
			break;
		}
	}
	/*If the new entry'e key is the largest*/
	if (eid == m_size)
	{
		// iter has been left at the last key if we got here. So increment it.
		++iter; // now iter points to the new location for the new entry
		*iter = rid.pid;
		++iter;
		*iter = rid.sid;
		++iter;
		*iter = key;
		m_size++;
		++*((int*)buffer);
	}

	return 0;
}


/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ 
	char* temp[PageFile::PAGE_SIZE + 12]; // allocate space for one more entry
	memcpy((char*)temp, (char*)buffer, PageFile::PAGE_SIZE);
	
	/*parameters for memmove(), used to shift data*/
	char* src = NULL;
	char* dest = NULL;
	size_t n = 0;


	int* iter = NULL;
	int eid = 0;
	int* buf = (int*)temp;
	for (; eid < m_size; ++eid)
	{
		ptrToKey(buf, eid, iter);
		if (*iter >= key)
		{
			/*shift the right part by 12 bytes to make room for the new entry*/
			src = (char*)(iter - 2); // move iter to pid of the key
			dest = (char*)(iter + 1);
			n = (m_size - eid) * 12; // three int variables, pid, sid and key: 12 bytes
			memmove(dest, src, n);
			/*copy the new entry*/
			iter -= 2; // move iter to pid
			*iter = rid.pid; // insert pid

			++iter;
			*iter = rid.sid; //insert sid

			++iter;
			*iter = key;  //insert key
			break;
		}
	}
	/*If the new entry'e key is the largest*/
	if (eid == m_size)
	{
		//cerr << "success!";
		// iter has been left at the last key if we got here. So increment it.
		++iter; // now iter points to the new location for the new entry
		*iter = rid.pid;
		++iter;
		*iter = rid.sid;
		++iter;
		*iter = key;
	}

	/*by this time the new entry is inserted into temp buffer
	 *now copy half of temp buffer into sibling and half back to current node*/
	/*after insertion, temp now has 85 entries. 43 goes to current node, 42 goes to sibling node*/
	
	/*insert 42 entries to sibling*/
	*(int*)sibling.buffer = 42; // set size to 42
	sibling.m_size = 42;
	memcpy((char*)sibling.buffer + sizeof(int), (char*)temp + sizeof(int) + 43 * 12, 42 * 12); // 42*12 =  504 bytes. temp+4+43*12
	//cerr << "my pid is " << this->m_pid;
	sibling.setNextNodePtr(this->getNextNodePtr()); // set next pid to current node's next pid

	/*insert 43 nodes to current node*/
	memset(buffer, 0, PageFile::PAGE_SIZE); // clear current node
	m_size = 43;
	memcpy((char*)buffer, (char*)temp, 43 * 12 + sizeof(int)); // 43*12 =  516 bytes  + sizeof(int)  + sizeof(int)
	*(int*)buffer = 43;
	*((int*)((char*)buffer + PageFile::PAGE_SIZE - 2 * sizeof(int))) = true; // restore isLeaf flag
	setNextNodePtr(sibling.getPid()); // set next node's pid

	int* p = (int*)sibling.buffer + 3;
	siblingKey = *p;

	return 0;

}

/*print the content of the node to help debug*/
void BTLeafNode::printLeafNode()
{

	int *pointer = (int*)buffer;
	cerr << *pointer << " ";
	for (int eid = 0; eid < m_size; ++eid)
	{
		ptrToKey(eid, pointer);
		cerr <<*(pointer-2)<<" "<<*(pointer-1)<<" "<< *pointer << " ";
	}

	pointer = (int*)buffer + 255;
	cerr << *pointer<<endl;
}


/*
 * Private helper function that returns a pointer to the eid-th key
 * in the buffer
 * @param eid[IN] the index of the key in the buff
 * @param ptr[OUT] the pointer to set
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::ptrToKey (int eid, int*& ptr) {
    return ptrToKeyHelp(eid, ptr, 3);
}

/*
* Private helper function that returns a pointer to the eid-th key
* in buf
* @param eid[IN] the index of the key in the buff
* @param ptr[OUT] the pointer to set
* @return 0 if successful. Return an error code if there is an error.
*/
RC BTLeafNode::ptrToKey(int* buf, int eid, int*& ptr) {
	return ptrToKeyHelp2(buf, eid, ptr, 3);
}

/*
 * Find the entry whose key value is larger than or equal to searchKey
 * and output the eid (entry number) whose key value >= searchKey.
 * Remember that all keys inside a B+tree node should be kept sorted.
 * @param searchKey[IN] the key to search for
 * @param eid[OUT] the entry number that contains a key larger than or equal to searchKey
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{
    if (m_size <= 0) {
        eid = -1;
        return 0;
    }

    int* i = (int*) buffer;
    ptrToKey(0, i);

    // Inspect each key in sequence and determine if the current key is
    // greater than or equal to the query, and if not, look to the next one
    for (int index = 0; index < m_size; index++) {
        // Inspect current key
        ptrToKey(index, i);

        if (*i >= searchKey) {
            eid = index;
            return 0;
        }
    }

    // If we make it here, then there was no key greater than or equal to
    // the search key
    eid = -1;
    return 0;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{
    if (eid >= m_size) return RC_NO_SUCH_RECORD;
    if (eid < 0)       return RC_INVALID_CURSOR;

    int* i = (int*) buffer;

    // Each eid corresponds to the eid-th key, meaning the rid
    // came 2 ints before it (also have to skip over the size
    // at the front)
    ptrToKey(eid, i);
    rid = *((RecordId*) (i - 2));
    key = *((int*) (i));

    return 0;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{
    if (m_pid == -1) return RC_INVALID_PID;

    int* i = (int*) buffer;
    return *(i + (PageFile::PAGE_SIZE / sizeof(int) - 1));
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{
    if (pid < 0) return RC_INVALID_PID;
    int* i = (int*) buffer;
    i[(PageFile::PAGE_SIZE / sizeof(int) - 1)] = pid;

    return 0;
}


// --------------------------------------------------------------------------------------------------------
// NON-LEAF NODES -----------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

/*
* Insert a (key, pid) pair to the node.
* @param key[IN] the key to insert
* @param pid[IN] the PageId to insert
* @return 0 if successful. Return an error code if the node is full.
*/
RC BTNonLeafNode::insert(int key, PageId pid)
{
	if (m_size >= BTNode::MAX_NON_LEAF_KEYS)  // 126 = (1024-4)/8 - 1, is the maximum number of keys in a node
		return RC_NODE_FULL;

	/*parameters for memmove()*/
	char* src = NULL;
	char* dest = NULL;
	size_t n = 0;

	int* iter = NULL;
	int eid = 0;
	// if node is empty
	if (m_size == -1)
	{
		iter = (int*)buffer;
		*iter = 1;
		*(iter + 2) = key;
		*(iter + 3) = pid;
		m_size = 1;
		return 0;
	}

	for (; eid < m_size; ++eid)
	{
		ptrToKey(eid, iter);
		if (*iter >= key)
		{
			/*shift the right part by 12 bytes to make room for the new entry*/
			src = (char*)iter; // move iter to pid of the key
			dest = (char*)(iter + 2);
			n = (m_size - eid) * (sizeof(int)+sizeof(PageId)); // # of entries times size of two int variables, pid and key: 8 bytes
			memmove(dest, src, n);
			/*copy the new entry*/
			*iter = key;
			++iter;
			*iter = pid; // insert pid
			m_size++;  // increment size
			++*((int*)buffer);
			break;
		}
	}
	/*If the new entry's key is the largest*/
	if (eid == m_size)
	{
		// iter has been left at the last key if we got here. So increment it.
		iter += 2; // now iter points to the new location for the new entry
		*iter = key;
		++iter;
		*iter = pid;
		m_size++;
		++*((int*)buffer);
	}

	return 0;
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * [NOTE:] mid key is not in either node because it is "moved" into midkey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{
	char* temp[PageFile::PAGE_SIZE + sizeof(int) + sizeof(PageId)]; // allocate space for one more entry
	memcpy((char*)temp, (char*)buffer, PageFile::PAGE_SIZE);
	/*parameters for memmove(), used to shift data*/
	char* src = NULL;
	char* dest = NULL;
	size_t n = 0;
	int* iter = NULL;
	int eid = 0;
	int* buf = (int*)temp;
	for (; eid < m_size; ++eid)
	{	
		ptrToKey(buf, eid, iter);
		if (*iter >= key)
		{
			/*shift the right part by 8 bytes to make room for the new entry*/
			src = (char*)iter; // move iter to pid of the key
			dest = (char*)(iter + 2);
			n = (m_size - eid) * (sizeof(int) + sizeof(PageId));  // calcualte how many bytes need to be moved
			memmove(dest, src, n);
			/*copy the new entry*/
			*iter = key;
			++iter;
			*iter = pid; // insert pid
			break;
		}
	}
	/*If the new entry'e key is the largest*/
	if (eid == m_size)
	{
		// iter has been left at the last key if we got here. So increment it.
		iter += 2; // now iter points to the new location for the new entry
		*iter = key;
		++iter;
		*iter = pid;
	}

	midKey = *((int*)temp + 2 + 63 * 2); // move past size and starting pid, then move past first 63 entries. The 64th one is the mid-entry.

	/*by this time the new entry is inserted into temp buffer
	 *now copy half of temp buffer into sibling and half back to current node
	 *after insertion, temp now has 127 entries. 
	 *63 go to current node; 63 go to sibling node, 1 is passed into param*/

	/*insert 63 entries to sibling*/
	*(int*)sibling.buffer = 63; // 127 total keys, 63 for each node, after moves sibling's first key into 'midkey', so it only has 63
	sibling.m_size = 63;
	// the starting pid in sibling node will be the pid before the first key, checked by hand, should be correct.
	memcpy((char*)sibling.buffer + sizeof(int),
		   (char*)temp + sizeof(int) + 8 * 64, 
		   63 * 8 + sizeof(PageId));

	/*insert 63 entries to current node*/
	memset(buffer, 0, PageFile::PAGE_SIZE); // clear current node
	m_size = 63;
	memcpy((char*)buffer, (char*)temp, 63 * 8 + sizeof(int) + sizeof(PageId)); // 63*8 =  504 bytes  + sizeof(int)  + sizeof(int)
	*((int*)((char*)buffer + PageFile::PAGE_SIZE - 2 * sizeof(int))) = false; // restore isLeaf flag
	*(int*)buffer = 63;

	return 0;
}

/*print the content of the non-leaf node to help debug*/
void BTNonLeafNode::printNonLeafNode()
{

	int *pointer = (int*)buffer;
	cerr << *pointer << " ";
	for (int eid = 0; eid < m_size; ++eid)
	{
		ptrToKey(eid, pointer);
		cerr << *(pointer - 1) << " " << *pointer << " ";
	}

	cerr << *(pointer+1) << endl;
}


/*
 * Private helper function that returns a pointer to the eid-th key
 * in the buffer
 * @param eid[IN] the index of the key in the buff
 * @param ptr[IN] the pointer to set
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::ptrToKey (int eid, int*& ptr) {
    return ptrToKeyHelp(eid, ptr, 2);
}

/*
* Private helper function that returns a pointer to the eid-th key
* in buf
* @param eid[IN] the index of the key in the buff
* @param ptr[OUT] the pointer to set
* @return 0 if successful. Return an error code if there is an error.
*/
RC BTNonLeafNode::ptrToKey(int* buf, int eid, int*& ptr) {
	return ptrToKeyHelp2(buf, eid, ptr, 2);
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{
    if (m_size <= 0) {
        pid = -1;
        return 0;
    }

    int* i = (int*) buffer;
    ptrToKey(0, i);

    // Inspect each key in sequence and determine if the current key is
    // less than the search key; if it is, return the left pid, otherwise,
    // keep looking
    for (int index = 0; index < m_size; index++) {
        // Inspect current key
        ptrToKey(index, i);

        if (searchKey < *i) {
            pid = *(i - 1);
            return 0;
        }
    }

    // If we get here, then the search key was greater than all keys in the node
    pid = *(i + 1);
    return 0;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{
    if (pid1 < 0 || pid2 < 0) return RC_INVALID_PID;

    int* i = (int*) buffer;

    // Set size to 1
    *i = 1;
    // Set pid1, then key, then pid2
    *(i + 1) = pid1;
    *(i + 2) = key;
    *(i + 3) = pid2;

    m_size = 1;

    return 0;
}
