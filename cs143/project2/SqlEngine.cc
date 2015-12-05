/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include <string>
#include <climits>

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

/**
 * determine if the index should be used for the given query, as
 * indicated by the input SelCond
 * @param cond[IN] the query as parsed into a vector of SelCond objects
 * @param keyQueries[OUT] the number of queries that are on keys
 * @param startKey[OUT] if GT or GE query on key appears, this will be set
 *        to the first key to examine
 * @param endKey[OUT] if LT or LE query on key appears, this will be set to
 *        the last key to examine
 * @return true if the index should indeed be used
 */
bool SqlEngine::shouldUseIndex(const vector<SelCond>& cond, int& keyQueries, int& startKey, int& endKey) {
    int rangeCount = 0,
        eqCount = 0,
        neqCount = 0;

    keyQueries = 0;

    // Iterate through each of the conditions
    for (int i = 0; i < cond.size(); i++) {
        const SelCond& currentCond = cond[i];
        
        // A key query
        if (currentCond.attr == 1) {
            keyQueries++;
            int currKey = atoi(currentCond.value);

            if (currentCond.comp == SelCond::EQ) {
                eqCount++;
                startKey = endKey = currKey;

            } else if (currentCond.comp != SelCond::NE) {
                switch(currentCond.comp) {
                    case SelCond::GT:
                        if (startKey < currKey) {
                            startKey = currKey + 1;
                        }
                        break;
                    case SelCond::GE:
                        if (startKey < currKey) {
                            startKey = currKey;
                        }
                        break;
                    case SelCond::LT:
                        if (endKey > currKey) {
                            endKey = currKey - 1;
                        }
                        break;
                    case SelCond::LE:
                        if (endKey > currKey) {
                            endKey = currKey;
                        }
                        break;
                }

                rangeCount++;
            } else {
                neqCount++;
            }
        }
    }

    // We'll use the index whenever:
    //  - We have any equality queries on key
    //  - We have a range of key queries
    //  - We have have no ineq on keys or an ineq and a range
    return eqCount || rangeCount;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning
  BTreeIndex bt;   // B+Tree index for efficient querying

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;
  int    keyQueries = 0;
  int    startKey = INT_MIN;
  int    endKey = INT_MAX;
  bool   useIndex = SqlEngine::shouldUseIndex(cond, keyQueries, startKey, endKey) || attr == 4;
  bool   onlyKeys = keyQueries == cond.size();
  IndexCursor cursor;

  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  // if we're not using the index, scan the table from the beginning
  rid.pid = rid.sid = 0;
  count = 0;

  // open the index file if it is intended to be used for the query, and
  // if it even exists
  if (useIndex) {
      rc = bt.open(table + ".idx", 'r', false);
      useIndex = rc == 0;

      // Start the cursor off at the first tuple of interest
      bt.locate(startKey, cursor);

      // Read the element at that index
      rc = bt.readForward(cursor, key, rid);
      if (rc < 0) {
          goto next_tuple;
      }
  }

  next_tuple_start:
    // read the tuple only if you're using the index and you have queries on values
    // OR you're not using the index
    if (!useIndex || !onlyKeys) {
        rc = rf.read(rid, key, value);
    }
    if (rc < 0) {
      fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
      goto exit_select;
    }

    // check the conditions on the tuple
    for (unsigned i = 0; i < cond.size(); i++) {
      // compute the difference between the tuple value and the condition value
      switch (cond[i].attr) 
	  {
		case 1:
			diff = key - atoi(cond[i].value);
			break;
		case 2:
			diff = strcmp(value.c_str(), cond[i].value);
			break;
      }

      // skip the tuple if any condition is not met
      switch (cond[i].comp) 
	  {
      case SelCond::EQ:
			if (diff != 0) goto next_tuple;
			break;
	  case SelCond::NE:
			if (diff == 0) goto next_tuple;
			break;
      case SelCond::GT:
			if (diff <= 0) goto next_tuple;
			break;
      case SelCond::LT:
			if (diff >= 0) goto next_tuple;
			break;
      case SelCond::GE:
			if (diff < 0) goto next_tuple;
			break;
      case SelCond::LE:
			if (diff > 0) goto next_tuple;
			break;
      }
    }

    // the condition is met for the tuple. 
    // increase matching tuple counter
    count++;

    // if we found a match, have not filtered the value,
    // and have yet to read the value from the record file,
    // we'll do that now:
	if (useIndex && onlyKeys && (attr == 2 || attr == 3)) {
        rc = rf.read(rid, key, value);
    }

    // print the tuple 
    switch (attr) {
    case 1:  // SELECT key
      fprintf(stdout, "%d\n", key);
      break;
    case 2:  // SELECT value
      fprintf(stdout, "%s\n", value.c_str());
      break;
    case 3:  // SELECT *
      fprintf(stdout, "%d '%s'\n", key, value.c_str());
      break;
    }

    // move to the next tuple
    next_tuple:
    if (!useIndex && ++rid < rf.endRid()) {
        goto next_tuple_start;
    }
    if (useIndex && (rc = bt.readForward(cursor, key, rid)) == 0) {
        // Only continue if we're not past our endKey
        if (key <= endKey) {
            goto next_tuple_start;
        }
    }

  // print matching tuple count if "select count(*)"
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

  // close the table file and return
  exit_select:
  rf.close();
  return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
    RecordFile rf;
    ifstream loadFile (loadfile.c_str());
    BTreeIndex bt;
    
    if (!loadFile.is_open()) {
	    cerr << "Error: file " << loadfile << " does not exist\n";
	    return RC_FILE_OPEN_FAILED;
	}

    // If the table file doesn't exist, we'll create it here, otherwise,
    // open it with rf
    rf.open(table+".tbl",'w');

    // If an index was specified, then we'll create that index file
    if (index) {
        // Specify the true flag to indicate that this is a new index
        bt.open(table + ".idx", 'w', true);
    }
    
    string line;
    while (getline(loadFile, line)) {
        int key;
        string value;
        RecordId rid;

        // Append the key-value pair when there's no error
        // reading it
        if (parseLoadLine(line, key, value) == 0) {
    	    rf.append(key, value, rid);

            // If we're constructing an index, insert the resultant
            // key and rid into the tree
            if (index) {
                bt.insert(key, rid);
            }
        }
    }
    
    if (index) {
        bt.close();
    }

    loadFile.close();
    return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
