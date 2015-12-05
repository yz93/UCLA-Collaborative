import json
import sys

fileName=sys.argv[1]

with open("excludeList.json") as infile:
	notallowed = json.load(infile)

with open(fileName) as originfile:
	data = json.load(originfile)

for k, entry in enumerate(data):
	for j,ingredient in enumerate(entry["ingredients"]):
		ingredient=ingredient.lower()
		ingredient=ingredient.replace("-","")
		words = ingredient.split(" ")
		for i, word in enumerate(words):
			if word in notallowed:
				words.remove(word)
		new_ingredient = ' '
		new_ingredient = new_ingredient.join(words)
		entry["ingredients"][j] = new_ingredient
	data[k] = entry

with open('prepared_'+fileName, 'w') as outfile:
	json.dump(data, outfile)

