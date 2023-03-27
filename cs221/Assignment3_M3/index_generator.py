import os, json, time
from bs4 import BeautifulSoup
from nltk.tokenize import RegexpTokenizer
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer
from sortedcontainers import SortedDict
from collections import defaultdict
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



def main(make_dicts):
    
    """
    Directory and Subdirectory processing
    """
    # interest_dir = "DEV1"
    interest_dir = "DEV"
    # interest_dir = "DEV2"


    # curr_dir = os.getcwd()
    # curr_dir = "/home/mksriram/cs_221/Assignment3_M3/marzhan/"
    # curr_dir = "/home/mksriram/cs_221/Assignment3_M3/yagya/"
    curr_dir = "/home/mksriram/cs_221/Assignment3_M1"

    
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
    negation = ""
    
    while file_count:
        
        inverted_idx = SortedDict()
        imp_words_inverted_idx = SortedDict()
        anchor_words_inverted_idx = SortedDict()
        two_gram_inverted_idx = SortedDict()
        three_gram_inverted_idx = SortedDict()

        
        print("Itearation: " + str(pickle_id))

        for i in range(min(batch_size, file_count)):

            file_obj = open(list_of_files[file_index])
            file_dict = json.load(file_obj)
            
            resp = BeautifulSoup(file_dict["content"], features="html.parser").get_text()
            if resp == None: continue
            
            
            
            
            """Two gram processing and creating index
            """
            
            if make_dicts["get_two_gram_inverted_idx"]:
                
                tokens = word_tokenize(resp)
                if len(tokens) != 0:
                    two_gram_inverted_idx = tokenizeAndReturnIdxDict(tokens, file_index, \
                                                two_gram_inverted_idx, "two")
                    
            """Three gram processing and creating index
            """
            
            if make_dicts["get_three_gram_inverted_idx"]:
                
                tokens = word_tokenize(resp)
                if len(tokens) != 0:
                    three_gram_inverted_idx = tokenizeAndReturnIdxDict(tokens, file_index, \
                                                three_gram_inverted_idx, "three")
 
            
            """All Words processing and creating index
            """          
            if make_dicts["get_inverted_idx"]:
                
                tokens = word_tokenize(resp)
                if len(tokens) != 0:
                    inverted_idx = tokenizeAndReturnIdxDict(tokens, file_index, inverted_idx)
                    
            """Doc to token processing and creating index(All Words)
            """
            
            if make_dicts["get_doc_to_token_dict"]:
                
                tokens = word_tokenize(resp)
                if len(tokens) != 0:
                    doc_to_token_dict = tokenizeAndReturnIdxDict(tokens, file_index, \
                                                doc_to_token_dict, None, True)

            

            bs_html_obj = BeautifulSoup(file_dict["content"], features="html.parser")
            
            """Anchor Words processing and creating index
            """          
            if make_dicts["get_anchor_words_inverted_idx"]:
                
                anchor_words_data = ""         
                for tags in bs_html_obj.find_all("a"):
                    anchor_words_data += tags.text + " "
                    
                tokens = word_tokenize(anchor_words_data.strip())
                if len(tokens) != 0:
                    anchor_words_inverted_idx = tokenizeAndReturnIdxDict(tokens, file_index, anchor_words_inverted_idx)
                    
            """Important Words processing and creating index
            """
            if make_dicts["get_imp_words_inverted_idx"]:
            
                imp_words_data = ""              
                for txt_in_tag in bs_html_obj.find_all(["b","h1", "h2", "h3", "title"]):
                    imp_words_data += txt_in_tag.text.strip() + " "
                    
                tokens = word_tokenize(imp_words_data)
                if len(tokens) != 0:
                    imp_words_inverted_idx = tokenizeAndReturnIdxDict(tokens, file_index, imp_words_inverted_idx)

            """Dictionary to map doc_idx to url
            """
            if make_dicts["get_map_doc_idx_to_url"]:
                url_to_idx_dict[file_index] = file_dict["url"]


            file_obj.close()
            print("Done with " + file_dict["url"] + " : " + str(i))
            file_index += 1




        
        if make_dicts["get_inverted_idx"]:
            jsonMap = {}
            jsonMap["data_inverted_idx"] = inverted_idx
            jsonMap["len_inverted_idx"] = len(inverted_idx)
            
            file_name = "jsonDataInvIdx_" + str(pickle_id) + ".json"
            writeToDisk(file_name, jsonMap)
            print("Number of unique tokens are - {}".format(jsonMap["len_inverted_idx"]))

            
        if make_dicts["get_imp_words_inverted_idx"]:
            jsonMap = {}
            jsonMap["data_imp_words_inverted_idx"] = imp_words_inverted_idx
            jsonMap["len_imp_words_inverted_idx"] = len(imp_words_inverted_idx)
            
            file_name = "jsonDataImpWordsIdx_" + str(pickle_id) + ".json"
            writeToDisk(file_name, jsonMap)
            print("Number of imp unique tokens are - {}".format(jsonMap["len_imp_words_inverted_idx"]))

            
        if make_dicts["get_anchor_words_inverted_idx"]:
            jsonMap = {}
            jsonMap["data_anchor_words_inverted_idx"] = anchor_words_inverted_idx
            jsonMap["len_anchor_words_inverted_idx"] = len(anchor_words_inverted_idx)
            
            file_name = "jsonDataAnchorWordsIdx_" + str(pickle_id) + ".json"
            writeToDisk(file_name, jsonMap)
            print("Number of anchor unique tokens are - {}".format(jsonMap["len_anchor_words_inverted_idx"]))

        if make_dicts["get_doc_to_token_dict"]:
            jsonMap = {}
            jsonMap["data_doc_to_token_dict"] = doc_to_token_dict
            jsonMap["len_doc_to_token_dict"] = len(doc_to_token_dict)
            
            file_name = "jsonDataDocToTokenIdx_" + str(pickle_id) + ".json"
            writeToDisk(file_name, jsonMap)
            print("Number of anchor unique tokens are - {}".format(jsonMap["len_anchor_words_inverted_idx"]))



        """doc_idx to url mapping dict dump to HDD
        """
        if make_dicts["get_map_doc_idx_to_url"]:
            jsonMap = {}
            jsonMap["data_map_doc_idx_to_url"] = url_to_idx_dict
            file_name = "jsonDataUrl_" + str(pickle_id) + ".json" 
            writeToDisk(file_name, jsonMap)


        """Write two gram inverted idexes to disk
        """
        if make_dicts["get_two_gram_inverted_idx"]:
            jsonMap = {}
            jsonMap["data_two_gram_inverted_idx"] = two_gram_inverted_idx
            jsonMap["len_two_gram_inverted_idx"] = len(two_gram_inverted_idx)
            
            file_name = "jsonDataTwoGram_" + str(pickle_id) + ".json" 
            writeToDisk(file_name, jsonMap)
            print("Number of two gram unique tokens are - {}".format(jsonMap["len_two_gram_inverted_idx"]))


        """Write three gram inverted idexes to disk
        """
        if make_dicts["get_three_gram_inverted_idx"]:
            jsonMap = {}
            jsonMap["data_three_gram_inverted_idx"] = three_gram_inverted_idx            
            jsonMap["len_three_gram_inverted_idx"] = len(three_gram_inverted_idx)

            file_name = "jsonDataThreeGram_" + str(pickle_id) + ".json" 
            writeToDisk(file_name, jsonMap)
            print("Number of three gram unique tokens are - {}".format(jsonMap["len_three_gram_inverted_idx"]))



        pickles_files.append(file_name)
        pickle_id += 1

        file_count -= min(batch_size, file_count)
    
    mergeIntoOne(pickles_files, 0)
    print("The number of indexed documents - {}".format(len(list_of_files)))
    
    
def writeToDisk(file_name, jsonMap):
    with open(file_name, "w") as outfile:
        json.dump(jsonMap, outfile)


def tokenizeAndReturnIdxDict(tokens, file_index, idx_dict, gram_processing=None, doc_to_token=False):
    
    token_hashmap = {}
    idx_token_hashmap = {}
    stemmer = PorterStemmer()


    if gram_processing ==  None:

        for (idx,token) in enumerate(tokens):
            stemmed_token = stemmer.stem(token)
            if (stemmed_token in token_hashmap) and (stemmed_token in idx_token_hashmap):
                token_hashmap[stemmed_token] += 1
                if doc_to_token == False:
                    idx_token_hashmap[stemmed_token].append(idx)
            else: 
                token_hashmap[stemmed_token] = 1
                if doc_to_token == False:
                    idx_token_hashmap[stemmed_token] = [idx]
        
        if doc_to_token:
            idx_dict[file_index] = token_hashmap
            return idx_dict
        

        for token in token_hashmap:
            
            # if token in idx_dict.keys():
            if token in idx_dict:

                idx_dict[token].append((file_index, token_hashmap[token], idx_token_hashmap[token]))
            else:
                idx_dict[token] = [(file_index, token_hashmap[token], idx_token_hashmap[token])]
                
    else:
        
        if gram_processing == "two":
            iter_loop = len(tokens) - 1
        elif gram_processing == "three":
            iter_loop = len(tokens) - 2

        
        for i in range(iter_loop):
            
            if gram_processing == "two":
                token = tokens[i] + " " + tokens[i+1]
            elif gram_processing == "three":
                token = tokens[i] + " " + tokens[i+1] + " " +tokens[i+2]  

            stemmed_token = stemmer.stem(token)
            if (stemmed_token in token_hashmap):
                token_hashmap[stemmed_token] += 1
            else:
                token_hashmap[stemmed_token] = 1
                
        for token in token_hashmap:
            if token in idx_dict:
                idx_dict[token].append((file_index, token_hashmap[token]))
            else:
                idx_dict[token] = [(file_index, token_hashmap[token])]
        
    return idx_dict
    


if __name__ == '__main__':
    
    start_time = time.time()
    
    make_dicts = {}
    make_dicts = defaultdict(lambda: False)
    
    make_dicts["get_inverted_idx"] = True
    make_dicts["get_imp_words_inverted_idx"] = True
    make_dicts["get_anchor_words_inverted_idx"] = True
    make_dicts["get_map_doc_idx_to_url"] = True
    make_dicts["get_two_gram_inverted_idx"] = True
    make_dicts["get_three_gram_inverted_idx"] = True
    make_dicts["get_doc_to_token_dict"] = True


 
    main(make_dicts)
    
    end_time = time.time()
    
    print("Execution took - {} seconds".format(end_time-start_time))