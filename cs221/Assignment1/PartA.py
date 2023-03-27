#!/usr/bin/python3

"""
Assignment 1: Text Processing

# PART-A

Name - Murali Krishna Sai Sriram
Student ID - 29405372
Email ID - mksriram@uci.edu
"""

from typing import Dict, List
import sys, re

"""
Assumption 1: 
The frequency of any token cannot be more than sys.maxsize

Assumption 2: 
Word size maximum is considered to be 1 MB (buffer size)

Assumption 3: 
Total unique tokens for any given input file would fit into the memory. Further
dictionary of tokens is updated for every lump.
            
Assumption 4: 
There is enough memory capacity to load text(1MB) every time for processing
"""

"""
Following function tokenizes the input text file and returns list of tokens. it process the input text file
in various lumps, it will load each lump in to buffer and does further processing.

Time complexity for iterating over text file is O(P) where P is (total document data)/(buffer_size_data),
total number of lumps.
Time complexity for doing (splitting data based seperators), filtering out empty strings and tokens of length < 2, 
removing white spaces, removing new line characters etc is O(Q) where Q is total number of elements 
in the buffer(for each lump).
So, Time complexity of below method is O(P*Q) -> So output varies quadratically with respect to input
"""

def tokenize(file_path: str) -> List[str]:

    total_file_obj  = open(file_path, mode='r', encoding="UTF-8")
    buffer_size = 1024*1024
    tokens_lst = []

    while True:

        accept_tokens_lst = []

        buffer_data = total_file_obj.read(buffer_size)

        if not buffer_data:
            # print(tokens_lst)
            return tokens_lst

        # Converting all buffer_data characters to lower case
        buffer_data = buffer_data.lower()
        # Replacing tab with space in buffer_data
        buffer_data = buffer_data.replace('\t', ' ')
        # Replacing new line characters with space in buffer_data         
        buffer_data = buffer_data.replace('\n', ' ')       

        # Regex for seperator characters
        seperator_regex = re.compile('[^a-zA-Z0-9@#*&\']')  

        ## Building accept_tokens_lst
        # splitting buffer_data based on seperator_regex
        accept_tokens_lst = seperator_regex.split(buffer_data)
        
        # Removing empty strings from the accept_tokens_lst
        # accept_tokens_lst = list(filter(None, accept_tokens_lst))

        # Filtering out tokens whose length < 2 or who are empty strings
        # and adding appropriate tokens to final tokens_lst
        for token in accept_tokens_lst:
            if len(token) < 2 or token == "":
                continue
            else:
                tokens_lst.append(token)


"""
Here we are counting frequency of tokens and updating dictionary token_freq_dict

Time Complexity for following code is O(m) - it varies linearly with respect to the input,
where m is the number of tokens.
Assuming here searching a token in dictionary have average time complexity of O(1).
"""

def computeWordFrequencies(tokens_lst: List[str]) -> Dict:
    
    token_freq_dict = {}

    for token in tokens_lst:

        if token not in token_freq_dict.keys():
            token_freq_dict[token] = 1
        else:
            token_freq_dict[token] += 1
    
    return token_freq_dict


"""
Following code sorts the token_freq_dict in descending order(based on freq of tokens)
and prints each token with it's frequency (based on descending order of frequency)

Time Complexity for following code is O(nlogn) - 
it varies loglinear with respect to the input

Explanation - .
sort() function has loglinear time complexity O(nlogn) and for loop has time complexity of
O(n) out of both loglinear has bigger time complexity, hence timecomplexity of below method is O(nlogn)
"""

def printResult(token_freq_dict):

    # Converting dict to list of items
    lst_items = list(token_freq_dict.items())

    # Sorting based on values of dictionary descending order
    lst_items.sort(key=lambda lst_items: lst_items[1], reverse=True)

    for (token,freq) in lst_items:
        print(token, "=>",freq)

"""
Following code reads the input_text_file_name from the command line and 
prints its tokens along with frequencies (sorted in descending order)
"""
if __name__ == "__main__":

    input_path = sys.argv[1]
    tokens_lst = tokenize(input_path)

    if len(tokens_lst) == 0:
        print("There are no tokens")
    else:
        token_freq_dict = computeWordFrequencies(tokens_lst)
        printResult(token_freq_dict)