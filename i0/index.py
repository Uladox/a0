#!/usr/bin/env python3

# from http://aakashjapi.com/fuckin-search-engines-how-do-they-work/

import sys
import re

def process_files(filenames):
    file_to_terms = {}
    for file in filenames:
        pattern = re.compile('[\W_]+')
        file_to_terms[file] = open(file, 'r').read().lower()
        file_to_terms[file] = pattern.sub(' ', file_to_terms[file])
        re.sub(r'[\W_]+', '', file_to_terms[file])
        file_to_terms[file] = file_to_terms[file].split()
    return file_to_terms

#input = [word1, word2, ...]
#output = {word1: [pos1, pos2], word2: [pos2, pos434], ...}
def index_one_file(termlist):
    fileIndex = {}
    for index, word in enumerate(termlist):
        if word in fileIndex.keys():
            fileIndex[word].append(index)
        else:
            fileIndex[word] = [index]
    return fileIndex

#input = {filename: [word1, word2, ...], ...}
#res = {filename: {word: [pos1, pos2, ...]}, ...}
def make_indices(termlists):
    total = {}
    for filename in termlists.keys():
        total[filename] = index_one_file(termlists[filename])
    return total

#input = {filename: {word: [pos1, pos2, ...], ... }}
#res = {word: {filename: [pos1, pos2, ...]}, ...}
def fullIndex(regdex):
    total_index = {}
    for filename in regdex.keys():
        for word in regdex[filename].keys():
            if word in total_index.keys():
                total_index[word][filename] = regdex[filename][word]
            else:
                total_index[word] = {filename: regdex[filename][word]}
    return total_index

def one_word_query(word, invertedIndex):
    pattern = re.compile('[\W_]+')
    word = pattern.sub(' ', word)
    if word in invertedIndex.keys():
        return [filename for filename in invertedIndex[word].keys()]
    else:
        return []

def free_text_query(string, invertedIndex):
    pattern = re.compile('[\W_]+')
    string = pattern.sub(' ', string)
    result = []
    for word in string.split():
        result += one_word_query(word, invertedIndex)
    return list(set(result))

def phrase_query(string, invertedIndex):
    pattern = re.compile('[\W_]+')
    string = pattern.sub(' ', string)
    listOfLists, result = [], []
    for word in string.split():
        listOfLists.append(one_word_query(word, invertedIndex))
    setted = set(listOfLists[0]).intersection(*listOfLists)
    for filename in setted:
        tmp = []
        for word in string.split():
            for word in string.split():
                tmp.append(invertedIndex[word][filename][:])
            for i in range(len(tmp)):
                for ind in range(len(tmp[i])):
                    tmp[i][ind] -= i
            if set(tmp[0]).intersection(*tmp):
                result.append(filename)
    return result

index = fullIndex(make_indices(process_files(sys.argv[1].split())))
print(phrase_query(sys.argv[2], index))

