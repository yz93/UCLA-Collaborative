#!/usr/bin/env python
from sklearn.naive_bayes import BernoulliNB
import sys, getopt
import json
import csv
import re


def main(argv):
    trainfile = str(sys.argv[1])
    testfile = str(sys.argv[2])
    reader=csv.reader(open(trainfile,'rb'),delimiter=",")
    train_rows =[]
    label_array=[]
	
    next(reader)
    for row in reader:
        for i in range(2,len(row)):
            row[i] = int(row[i])
        train_rows.append(row[2:])
        label_array.append(row[1])
   
    
    

    
    
    clf = BernoulliNB()
    
    clf.fit(train_rows, label_array)
    
    
    reader2=csv.reader(open(testfile,'rb'),delimiter=",")
    test_rows =[]
    id_rows=[]
    next(reader2)
    for row in reader2:
        for i in range(2,len(row)):
            row[i] = int(row[i])
	test_rows.append(row[2:])
	id_rows.append(row[0])
		
    predict_result=clf.predict(test_rows)
    result = [['id','cuisine']]
    for i in range(0,len(predict_result)):
	    j=[]
	    j.append(id_rows[i])
	    j.append(predict_result[i])
	    result.append(j)
	    
	
    with open('bay_result.csv', 'wb') as fp:
	    writer = csv.writer(fp)
	    writer.writerows(result)
        
    
if __name__ == "__main__":
    main(sys.argv[1:])
