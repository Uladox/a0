#!/usr/bin/env python3

# from http://aakashjapi.com/fuckin-search-engines-how-do-they-work/

def porcess_files(filenames):
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
