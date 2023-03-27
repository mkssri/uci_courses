#!/home/mksriram/cs_221/virtual_environments/assignment2_venv/bin/python3

import os, json, time
from bs4 import BeautifulSoup
from nltk.tokenize import RegexpTokenizer
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer
from sortedcontainers import SortedDict
from nltk import word_tokenize
import pickle
import json


def mergeFiles(filename1, filename2, resultFilename):
    file1 = open(filename1); file2 = open(filename2)
    data1 = json.load(file1); data2 = json.load(file2)
    tokens1 = data1["tokens"]; tokens2 = data2["tokens"]
    resultJSON = {}
    resultJSON["tokens"] = []
    resultJSON["data"] = {}

    size1 = len(tokens1); size2 = len(tokens2)
    ptr1 = 0; ptr2 = 0

    while ptr1 < size1 and ptr2 < size2:
        if tokens1[ptr1] == tokens2[ptr2]:
            resultJSON["data"][tokens1[ptr1]] = data1["data"][tokens1[ptr1]] + data2["data"][tokens2[ptr2]]
            ptr1 += 1; ptr2 += 1
        elif tokens1[ptr1] < tokens2[ptr2]:
            resultJSON["data"][tokens1[ptr1]] = data1["data"][tokens1[ptr1]]
            ptr1 += 1
        else:
            resultJSON["data"][tokens2[ptr2]] = data2["data"][tokens2[ptr2]]
            ptr2 += 1

    while ptr1 < size1:
        resultJSON["data"][tokens1[ptr1]] = data1["data"][tokens1[ptr1]]
        ptr1 += 1

    while ptr2 < size2:
        resultJSON["data"][tokens2[ptr2]] = data2["data"][tokens2[ptr2]]
        ptr2 += 1

    with open(resultFilename, "w") as outfile:
        json.dump(resultJSON, outfile)


def mergeIntoOne(pickles_files, index):

    if len(pickles_files) == 1: return
    iters = len(pickles_files) // 2

    newFiles = []
    for i in range(iters):
        resultName = "result_" + str(index) + "_" + str(i) + ".json"
        newFiles.append(resultName)
        mergeFiles(pickles_files[2 * i], pickles_files[2 * i + 1], resultName)
        os.system("rm -rf " + pickles_files[2 * i])
        os.system("rm -rf " + pickles_files[2 * i + 1])

    if len(pickles_files) % 2: newFiles.append(pickles_files[-1])
    mergeIntoOne(newFiles, index + 1)



def main():
    """
    Directory and Subdirectory processing
    """
    interest_dir = "DEV"
    # interest_dir = "DEV1"

    curr_dir = os.getcwd()
    
    ele_in_curr_dir = os.listdir(curr_dir)
    
    if interest_dir in ele_in_curr_dir:
        dir_name = os.path.join(curr_dir, interest_dir)
    else:
        raise Exception("DEV not present in current working directory, please fix it!!")
        
    # Get the list of all files in directory tree at given path
    list_of_files = list()
    file_count = 0

    for (dir_path, dir_names, file_names) in os.walk(dir_name):
        for file in file_names:
            if ".DS_Store" in file or "._" in file:
                continue
            else:
                list_of_files.append(os.path.join(dir_path, file))
                file_count += 1
                    
    """
    list_of_files - have all file paths to process
    Processing each file
    """
    batch_size = 60000
    url_to_idx_dict = {}
    doc_to_token_dict = {}

    file_index = 0
    pickle_id = 0
    pickles_files = []

    stemmer = PorterStemmer()
    
    while file_count:
        inverted_idx = SortedDict()
        print("Itearation: " + str(pickle_id))

        for i in range(min(batch_size, file_count)):

            file_obj = open(list_of_files[file_index])
            file_dict = json.load(file_obj)
            resp = BeautifulSoup(file_dict["content"], features="html.parser").get_text()
            if resp == None: continue

            tokens = word_tokenize(resp)
            token_hashmap = {}

            for token in tokens:
                stemmed_token = stemmer.stem(token)
                if stemmed_token in token_hashmap:
                    token_hashmap[stemmed_token] += 1
                else: 
                    token_hashmap[stemmed_token] = 1

            url_to_idx_dict[file_index] = file_dict["url"]
            doc_to_token_dict[file_index] = token_hashmap

            for token in token_hashmap:
                if token in inverted_idx.keys():
                    inverted_idx[token].append((file_index, token_hashmap[token]))
                else:
                    inverted_idx[token] = [(file_index, token_hashmap[token])]

            file_obj.close()
            print("Done with " + file_dict["url"] + " : " + str(i))
            file_index += 1

        file_name = "v1jsonData_" + str(pickle_id) + ".json"
        jsonMap = {"tokens": list(inverted_idx), "data": inverted_idx}

        print("Number of unique tokens are - {}".format(len(jsonMap["tokens"])))


        with open(file_name, "w") as outfile:
            json.dump(jsonMap, outfile)
            
        file_name = "v1jsonDataDocToTokens_" + str(pickle_id) + ".json"
        jsonMap = {"data": doc_to_token_dict}
        
        with open(file_name, "w") as outfile:
            json.dump(jsonMap, outfile)

        pickles_files.append(file_name)
        pickle_id += 1

        file_count -= min(batch_size, file_count)
    
    mergeIntoOne(pickles_files, 0)
    print("The number of indexed documents - {}".format(len(list_of_files)))




if __name__ == '__main__':
    start_time = time.time()
    main()
    end_time = time.time()
    print("Execution took - {} seconds".format(end_time-start_time))