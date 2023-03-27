#!/usr/bin/python3

"""
Assignment 1: Text Processing

# PART-B

Name - Murali Krishna Sai Sriram
Student ID - 29405372
Email ID - mksriram@uci.edu
"""
import sys
from typing import List
from PartA import tokenize


"""
Following method does following -
part(1) Call's tokenize method on two input files and gets list of tokens
part(2) Removes duplicate tokens from the both list of tokens
part(3) Finally it finds the common list of tokens between both list of tokens and returns them

Time complexity for above part(1) is O(P*Q) where P and Q are (total document data)/(buffer_size_data),
total number of lumps and total number of elements in the buffer(for each lump) respectively

Time complexity for above part(2) is O(max(s,t)), where s and t are list of tokens in input - file1 and file2
respectively.

Time complexity for above part(3) is O(min(s1,t1)), where s1 and t1 are list of unique tokens in input - file1 and file2
respectively(assuming average time complexity for finding token from a list is O(1))

So time complexity of below function is O(P*Q) + O(max(s,t)) + O(min(s1,t1)) ~ O(P*Q)
So output varies quadratically with input.
"""

def findCommonTokens(file_path_1, file_path_2):

    lst_of_common_tokens = []
    tokens_set_1 = set(tokenize(file_path_1))
    tokens_set_2 = set(tokenize(file_path_2))

    # lst_of_common_tokens = (tokens_set_1) & (tokens_set_2)

    for token_i in tokens_set_1:
        if (token_i in tokens_set_2) and (len(token_i) >= 2):
            lst_of_common_tokens.append(str(token_i))

    return lst_of_common_tokens

"""
Read the file paths from the command line and print the common tokens and 
total number of common tokens
"""

if __name__ == "__main__":

    file_path_1 = sys.argv[1]
    file_path_2 = sys.argv[2]

    lst_of_common_tokens = findCommonTokens(file_path_1, file_path_2)

    print("Total Number of common tokens are - {}".format(len(lst_of_common_tokens)))
    print("Common tokens are - {}".format(','.join(lst_of_common_tokens)))