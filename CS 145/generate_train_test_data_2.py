# generate_train_test_data_1.py
# Version 3.0
# Author: Nero Geng, Zhao Yang

# This script generates train and test data files from the original json files downloaded from Kaggle
# It requires one integer parameter that specifies the desired number of features in the output files
# It assumes the json files are located in the 'json' folder in the current directory
# It outputs one train data file and one test data file in csv format in the current directory. The naming convention
# is 'train_' or 'test_' followed by the number of features, such as 'train_200.csv' and 'test_200.csv'
# Sample usage: python27 PATH/generate_train_test_data_1.py 200

import sys
import getopt
import json
import csv


def main(argv):
    train_file = './json/train.json'
    test_file = './json/test.json'
    # number of interested frequent ingredients
    frequent = int(sys.argv[1])
    bag_of_word_train_file = './train_'+str(frequent)+'.csv'
    bag_of_word_test_file = './test_'+str(frequent)+'.csv'

    fin = open(test_file)
    test_data = json.load(fin)
    fin.close()

    fin2 = open(train_file)
    train_data = json.load(fin2)
    fin2.close()

    # create a list of all ingredients, dups allowed
    ingredients = list()
    for i in range(0, len(test_data)):
        for j in range(0, len(test_data[i]["ingredients"])):
            ingredients.append(test_data[i]["ingredients"][j].encode("utf-8"))

    counter = {}
    for ingredient in ingredients:
        if ingredient in counter:
            counter[ingredient] += 1
        else:
            counter[ingredient] = 1

    # ingredients in frequency descending order
    frequent_ingredients = sorted(counter, key=counter.get, reverse=True)
    #print len(frequent_ingredients)
    frequent_ingredients = frequent_ingredients[1:frequent]

    # pprint(frequentIngredients)

    # generate the bag of words for test data
    fout = open(bag_of_word_test_file, "wb+")
    writer = csv.writer(fout)
    # write header
    writer.writerow(['id', 'cuisine'] + frequent_ingredients)
    # write entries
    for i in range(0, len(test_data)):
        row = list()
        row.append(test_data[i]["id"])
        cuisine = test_data[i].get('cuisine', '')
        row.append(cuisine.encode('utf-8'))

        items = [x.encode('utf-8') for x in test_data[i]["ingredients"]]

        for j in range(0, len(frequent_ingredients)):
            if frequent_ingredients[j] in items:
                row.append(1)
            else:
                row.append(0)
        writer.writerow(row)
    fout.close()

    # Use frequent_ingredients which is a dictionary built upon test data to generate the bag of words for train data
    fout = open(bag_of_word_train_file, "wb+")
    writer = csv.writer(fout)
    # write header
    writer.writerow(['id', 'cuisine'] + frequent_ingredients)
    # write entries
    for i in range(0, len(train_data)):
        row = list()
        row.append(train_data[i]["id"])
        cuisine = train_data[i].get('cuisine', '')
        row.append(cuisine.encode('utf-8'))

        items = [x.encode('utf-8') for x in train_data[i]["ingredients"]]

        for j in range(0, len(frequent_ingredients)):
            if frequent_ingredients[j] in items:
                row.append(1)
            else:
                row.append(0)
        writer.writerow(row)
    fout.close()

if __name__ == "__main__":
   main(sys.argv[1:])