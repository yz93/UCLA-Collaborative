import json
import matplotlib.pyplot as plt

with open("train.json") as infile:
	data = json.load(infile)

cuisineDict = {}

for entry in data:
	cuisine=entry["cuisine"]
	if cuisine in cuisineDict:
		cuisineDict[cuisine] += 1
	else:
		cuisineDict[cuisine] =1

print cuisineDict

plt.bar(range(len(cuisineDict)), cuisineDict.values(), align='center')
plt.xticks(range(len(cuisineDict)), cuisineDict.keys())

plt.show()