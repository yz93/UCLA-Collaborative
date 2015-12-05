import json
import sys
import operator
import re
import nltk
from nltk.corpus import wordnet as wn

with open("adjectives.json") as infile:
	adjs = json.load(infile)

with open("train.json") as infile:
	data = json.load(infile)

with open("test.json") as infile:
	test = json.load(infile)

with open("ed-words.json") as infile:
	edwords = json.load(infile)

adjDict = {}
dupcheck = set()
for adj in adjs:
	adj = adj.lower().replace("-","")
	adjDict[adj] = 0
	dupcheck.add(adj)

for word in edwords:
	word=word.lower().replace("-","")
	if word not in dupcheck:
		adjDict[word]=0
		dupcheck.add(word)

for entry in data:
	for ingredient in entry["ingredients"]:
		words = ingredient.lower().replace("-","").split(" ")
		for i, word in enumerate(words):
			if word in adjDict and i<len(words)-1:
				adjDict[word] += 1

for entry in test:
	for ingredient in entry["ingredients"]:
		words = ingredient.lower().replace("-","").split(" ")
		for i, word in enumerate(words):
			if word in adjDict and i<len(words)-1:
				adjDict[word] += 1

wordList = []
for key, value in adjDict.iteritems():
	if value>5:
		wordList.append(key)
			



nouns = {x.name().split('.', 1)[0] for x in wn.all_synsets('n')}
# adjs = {x.name().split('.', 1)[0] for x in wn.all_synsets(wn.ADJ)}

finalList = []
seen = set()
for word in wordList:
	if word in nouns:
		wordList.remove(word)
	else:
		finalList.append(word)
		seen.add(word)

print finalList
with open('excludeList.json', 'w') as outfile:
	json.dump(finalList, outfile)
