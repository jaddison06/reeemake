#!/usr/bin/env python3

# if you want to build this outside linux then yeah you have to do it yourself x

import os
import os.path as path
import itertools

def getAllFiles(dirName="."):
    allItems = os.listdir(dirName)
    allFiles = []

    for item in allItems:
        fullPath = os.path.join(dirName, item)
        if path.isdir(fullPath):
            allFiles.append(getAllFiles(fullPath))
        else:
            allFiles.append(fullPath)

    return allFiles


# itertools.chain.from_iterable() unfortunately counts strings as iterables
#
# we take all strings that aren't in a nested list and put them in one
filesDimensioned = getAllFiles()
rootFiles = []

# get all root strings and put them in a new list
for item in filesDimensioned:
    if type(item) != list:
        rootFiles.append(item)

# now remove them all from the original list
for item in rootFiles:
    filesDimensioned.remove(item)

# now add them back within the nested list
filesDimensioned.append(rootFiles)

# NOW we can do from_iterable
files = itertools.chain.from_iterable(filesDimensioned)

fnames = []

for f in files:
    if ".cpp" in f:
        fnames.append(f[:-4])

objFiles = f"build/objects/{'.o build/objects/'.join([ f[f.rfind('/')+1:] for f in fnames ])}.o"

makefile = ""
makefile += f"main: {objFiles}\n		g++-8 {objFiles} -o ./reeemake\n\n"
makefile += "clean:\n		rm -r ./build/objects\n		mkdir ./build/objects\n		rm ./reeemake\n\n"
makefile += "makefile:\n		python3 ./build/generate_makefile.py\n\n"


for f in fnames:
    rootF = f[f.rfind('/')+1:]
    makefile += f"build/objects/{rootF}.o: {f}.cpp"

    # main.h doesn't exist
    if not f=="./main":
        makefile += f" {f}.h"

    makefile += f"\n		g++-8 -c {f}.cpp -I . -Wall -Wno-sign-compare -std=c++17 -o ./build/objects/{rootF}.o\n\n"


print(f"Generated makefile:\n\n{makefile}")
with open("./Makefile", "wt") as fh:
    fh.write(makefile)
