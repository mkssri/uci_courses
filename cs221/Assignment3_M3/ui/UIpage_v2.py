# -*- coding: utf-8 -*-
"""
Created on Mon Mar  7 14:23:36 2022

@author: Marzhan
"""

import json, time
from nltk.stem import PorterStemmer
import streamlit as st
  

# file_path = "/home/mksriram/cs_221/Assignment3_M2/Inverted_Index_File_v1"
# file_path = "/home/mksriram/cs_221/Assignment3_M2/jsonData_0.json"



class QueryProcessing():
    
    def __init__(self, lst_path):
        self.lst_path = lst_path
    
    @st.cache(persist=True, allow_output_mutation=True)
    def loadBuffer(self):        
        return self.loadFileAndGetIdx(self.lst_path[0]), self.loadFileAndGetIdx(self.lst_path[1])
    
    def loadFileAndGetIdx(self, file_path):
        with open(file_path) as f:
            idx_obj = f.read()
        idx = (json.loads(idx_obj))["data"]

        return idx
    
    def comparePostings(self, p1, p2):
        
        ans_lst = []

        p1_len, p2_len  = 0, 0

        while p1_len < len(p1) and p2_len < len(p2):
            
            if p1[p1_len][0] == p2[p2_len][0]:
                
                # ans_lst.append([p1[p1_len][0], min(p1[p1_len][1], p2[p2_len][1])])
                ans_lst.append( [p1[p1_len][0], (p1[p1_len][1]+p2[p2_len][1]) ])


                p1_len += 1
                p2_len += 1
            
            elif p1[p1_len][0] < p2[p2_len][0]:
                
                p1_len += 1
            
            else:
                p2_len += 1
        
        return ans_lst


    def queryProcessing(self, inverted_idx, url_to_doc_idx):
        self.inverted_idx = inverted_idx
        self.url_to_doc_idx = url_to_doc_idx
        
        print("in queryProcessing...")
        
        st.title("Top 5 URL results")
        st.subheader("Enter your query")
        
        # with st.form("form1"):
        #     input_query = st.text_input("Query input")
        #     button = st.form_submit_button("Submit")
            
        input_query = st.text_input("Query input")
        button = st.button("Submit")
                
        if button:
            with st.expander("Results", expanded=True):    
            
                start_time = time.time()
            
            
                if input_query == "":
                    raise Exception("invalid query")
            
                else:
                    
                    input_query = input_query.split(" ")
                    query = []
                    stemmer = PorterStemmer()
                    query_posting_map = {}
            
                    for x in range(len(input_query)):
                        #query.append(stemmer.stem(input_query[x]))
                        
                        #sorting query elements based on it's posting size
                        query_posting_map[(stemmer.stem(input_query[x]))] = len(inverted_idx[(stemmer.stem(input_query[x]))])
                        
                    
                    #sort_orders = sorted(orders.items(), key=lambda x: x[1], reverse=True)
                    query_posting_map = sorted(query_posting_map.items(), key=lambda x: x[1], reverse=False)
                    
                    for i in query_posting_map:
                        query.append(i[0])
                        
                    
                    if len(query) == 1:
                        ans_lst = self.inverted_idx[query[0]]
                    
                    else:
                        ans_lst = self.comparePostings(inverted_idx[query[0]], inverted_idx[query[1]])
                        query.pop(0)
                        query.pop(0)
            
                        while len(query) > 0:
                            
                            ans_lst = self.comparePostings(inverted_idx[query[0]], ans_lst)
                            query.pop(0)
                        
                
                # ans_lst => common posting
                ans_lst.sort(key=lambda ans_lst:ans_lst[1], reverse=True)
                
                cnt = 0
                
                set_urls = set()
                
                while((len(set_urls)) < 5):
                    
                    if cnt < len(ans_lst):
                        
                        url_key = str(ans_lst[cnt][0])
                        
                        url = url_to_doc_idx[url_key].split("#")[0]
                        
                        if url not in set_urls:
                            st.caption(url_to_doc_idx[url_key])
                            print(url_to_doc_idx[url_key])
                            set_urls.add(url)
                            
                        cnt += 1
                            
                    else:
                        break
                exec_time = round((time.time()-start_time)*1000,3)            
                # print("retreiving top 5 urls for input query took - {} seconds".format(end_time-start_time))
                print("Retreiving top 5 urls for input query took {} milliseconds".format(exec_time))
                st.caption("Retreving results took "+str(exec_time)+" milliseconds")


if __name__ == "__main__":

    # file_path = "/home/mksriram/cs_221/Assignment3_M2/jsonData_final.json"
    # url_file_path = "/home/mksriram/cs_221/Assignment3_M2/jsonDataUrls_0.json"
    
    file_path = "jsonData_final.json"
    url_file_path = "jsonDataUrls_0.json"
    
    lst_path = [file_path, url_file_path]
    
    obj = QueryProcessing(lst_path)
    
    print("Started loading all dicts to memory")
    start_time = time.time()    
    print("start loadBuffer...")
    inverted_idx, url_to_doc_idx = obj.loadBuffer()
    exec_time = round((time.time()-start_time)*1000,3)            
    print("Loading all dicts to memory took {} milliseconds".format(exec_time))

    obj.queryProcessing(inverted_idx, url_to_doc_idx)