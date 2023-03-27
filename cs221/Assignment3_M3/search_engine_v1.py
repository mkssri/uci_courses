"""
Streamlit UI
Search Engine Backend
"""

import json, time, math
from nltk.stem import PorterStemmer
import streamlit as st
from collections import defaultdict
from numpy import dot
from numpy.linalg import norm


class QueryProcessing():
    
    def __init__(self, load_dicts):
        self.load_dicts = load_dicts
        

    """Loading data to Memory
    """
    # DATA        
    @st.cache(persist=True, allow_output_mutation=True)
    def loadBufferData(self):

        for (key, value) in self.load_dicts.items():
            
            if key == "data_inverted_idx" and value[0] == True:
                data = self.loadFileAndGetIdx(value[1], key)
                
            if key == "data_map_doc_idx_to_url" and value[0] == True:
                url_map_data = self.loadFileAndGetIdx(value[1], key)
               
        return data, url_map_data
    
    # Anchor        
    @st.cache(persist=True, allow_output_mutation=True)
    def loadBufferAnchor(self):
        
        for (key, value) in self.load_dicts.items():

            if key == "data_anchor_words_inverted_idx" and value[0] == True:
                data = self.loadFileAndGetIdx(value[1], key)
                
            if key == "data_map_doc_idx_to_url" and value[0] == True:
                url_map_data = self.loadFileAndGetIdx(value[1], key)
                
        return data, url_map_data
    
    # IMP        
    @st.cache(persist=True, allow_output_mutation=True)
    def loadBufferImp(self):
        
        for (key, value) in self.load_dicts.items():

            if key == "data_imp_words_inverted_idx" and value[0] == True:
                data = self.loadFileAndGetIdx(value[1], key)
                
            if key == "data_map_doc_idx_to_url" and value[0] == True:
                url_map_data = self.loadFileAndGetIdx(value[1], key)
                
        return data, url_map_data

    # two_gram
    @st.cache(persist=True, allow_output_mutation=True)
    def loadBufferDataTwoGram(self):
        
        for (key, value) in self.load_dicts.items():

            if key == "data_two_gram_inverted_idx" and value[0] == True:
                data = self.loadFileAndGetIdx(value[1], key)
                
            if key == "data_map_doc_idx_to_url" and value[0] == True:
                url_map_data = self.loadFileAndGetIdx(value[1], key)
               
        return data, url_map_data

    # three_gram
    @st.cache(persist=True, allow_output_mutation=True)
    def loadBufferThreeGram(self):

        for (key, value) in self.load_dicts.items():

            if key == "data_three_gram_inverted_idx" and value[0] == True:
                data = self.loadFileAndGetIdx(value[1], key)
                
            if key == "data_map_doc_idx_to_url" and value[0] == True:
                url_map_data = self.loadFileAndGetIdx(value[1], key)
               
        return data, url_map_data

    # Doc To Token Data        
    @st.cache(persist=True, allow_output_mutation=True)
    def loadBufferDocToToken(self):

        for (key, value) in self.load_dicts.items():
            
            if key == "data_doc_to_token_dict" and value[0] == True:
                data = self.loadFileAndGetIdx(value[1], key)
                total_docs = len(data)
               
        return data, total_docs


    
    def loadFileAndGetIdx(self, file_path, tag):
        with open(file_path) as f:
            idx_obj = f.read()
        idx = (json.loads(idx_obj))[tag]

        return idx
    
    def comparePostings(self, p1, p2):
        
        common_posting_lst = []

        p1_ptr, p2_ptr  = 0, 0

        while p1_ptr < len(p1) and p2_ptr < len(p2):
            
            if p1[p1_ptr][0] == p2[p2_ptr][0]:
                
                # common_posting_lst.append([p1[p1_ptr][0], min(p1[p1_ptr][1], p2[p2_ptr][1])])
                common_posting_lst.append( [p1[p1_ptr][0], (p1[p1_ptr][1]+p2[p2_ptr][1]) ])


                p1_ptr += 1
                p2_ptr += 1
            
            elif p1[p1_ptr][0] < p2[p2_ptr][0]:
                
                p1_ptr += 1
            
            else:
                p2_ptr += 1
        
        return common_posting_lst


    def queryProcessing(self, data_dicts):
        
        self.tf_idf_processing = False
        
        self.data_dicts = defaultdict(lambda: None)
        
        for (key, value) in data_dicts.items():
            self.data_dicts[key] = value
            
            # TODO loading gram processing
            if value != None and key != "data_map_doc_idx_to_url":
                self.gram_processing = key
                
            if value != None and key == "data_doc_to_token_dict":
                self.tf_idf_processing = True
                
        print("in queryProcessing...")
        
        st.title("Top 5 URL results")
        st.subheader("Enter your query")
        
            
        input_query = st.text_input("Query input")
        button = st.button("Submit")
                
        if button:
            with st.expander("Results", expanded=True):    
            
                self.start_time = time.time()
                
                input_query = input_query.split(" ")

                if "data_inverted_idx" in self.gram_processing or "imp_words" in self.gram_processing\
                    or "anchor_words" in self.gram_processing:
                    
                    if input_query == ['']:
                        raise Exception("invalid query")
                    else:
                        iter_loop = len(input_query)
                        
                    if "data_inverted_idx" in self.gram_processing:
                        
                        if self.tf_idf_processing:
                            common_posting_lst = self.refineProcessingTfIdf(input_query, \
                                self.data_dicts["data_inverted_idx"], self.data_dicts["data_doc_to_token_dict"])
                        
                        # Boolean Processing (Comparing Postings)
                        else:       
                            common_posting_lst = self.refineProcessing(input_query, iter_loop, \
                                self.data_dicts["data_inverted_idx"])
                        
                    elif "imp_words" in self.gram_processing:
                        
                        common_posting_lst = self.refineProcessing(input_query, iter_loop, \
                            self.data_dicts["data_imp_words_inverted_idx"])
                        
                    elif "anchor_words" in self.gram_processing:
                        
                        common_posting_lst = self.refineProcessing(input_query, iter_loop, \
                            self.data_dicts["data_anchor_words_inverted_idx"])

                elif "two_gram" in self.gram_processing:
                    
                    if len(input_query) < 2:
                        raise Exception("invalid query")
                    else:
                        iter_loop = len(input_query)-1
                        common_posting_lst = self.refineProcessing(input_query, iter_loop, \
                            self.data_dicts["data_two_gram_inverted_idx"])
                    
                elif "three_gram" in self.gram_processing:
                    
                    if len(input_query) < 3:
                        raise Exception("invalid query")
                    else:
                        iter_loop = len(input_query)-2
                        common_posting_lst = self.refineProcessing(input_query, iter_loop, \
                            self.data_dicts["data_three_gram_inverted_idx"])

  
                cnt = 0
                
                set_urls = set()
                
                while((len(set_urls)) < 5):
                    
                    if cnt < len(common_posting_lst):
                        
                        url_key = str(common_posting_lst[cnt][0])
                        
                        url = self.data_dicts["data_map_doc_idx_to_url"][url_key].split("#")[0]
                        
                        if url not in set_urls:
                            st.caption(self.data_dicts["data_map_doc_idx_to_url"][url_key])
                            print(self.data_dicts["data_map_doc_idx_to_url"][url_key])
                            set_urls.add(url)
                            
                        cnt += 1
                            
                    else:
                        break
                exec_time = round((time.time()-self.start_time)*1000,3)            
                # print("retreiving top 5 urls for input query took - {} seconds".format(end_time-start_time))
                print("Retreiving top 5 urls for input query took {} milliseconds".format(exec_time))
                st.caption("Retreving results took "+str(exec_time)+" milliseconds")
                
                
    def refineProcessing(self, input_query, iter_loop, inverted_idx):
        
        query = []
        stemmer = PorterStemmer()
        query_posting_map = {}

        for x in range(iter_loop):
            
            if "data_inverted_idx" in self.gram_processing or "imp_words" in self.gram_processing\
                or "anchor_words" in self.gram_processing:
                    
                query_posting_map[(stemmer.stem(input_query[x]))] = len(inverted_idx[(stemmer.stem(input_query[x]))])
                
            elif "two_gram" in self.gram_processing:
                query_posting_map[(stemmer.stem(input_query[x] + " " + input_query[x+1]))] = \
                    len(inverted_idx[(stemmer.stem(input_query[x] + " " + input_query[x+1]))])

            elif "three_gram" in self.gram_processing:
                query_posting_map[(stemmer.stem(input_query[x] + " " + input_query[x+1] + " " + input_query[x+2]))] = \
                    len(inverted_idx[(stemmer.stem(input_query[x] + " " + input_query[x+1] + " " + input_query[x+2]))])

        query_posting_map = sorted(query_posting_map.items(), key=lambda x: x[1], reverse=False)
        
        for x in query_posting_map:
            query.append(x[0])
            
        
        if len(query) == 1:
            common_posting_lst = inverted_idx[query[0]]
        
        else:
            common_posting_lst = self.comparePostings(inverted_idx[query[0]], inverted_idx[query[1]])
            query.pop(0)
            query.pop(0)

            while len(query) > 0:
                
                common_posting_lst = self.comparePostings(inverted_idx[query[0]], common_posting_lst)
                query.pop(0)
            
        common_posting_lst.sort(key=lambda common_posting_lst:common_posting_lst[1], reverse=True)
        return common_posting_lst



    def computeWeight(self, type, token, input_dict):
        
        
        if type == "query":
            tf_raw = int(input_dict[token]) if (token in input_dict.keys()) else 0
        
        # input_dict = freq here
        else:
            tf_raw = input_dict

        tf_wt = (1+math.log10(tf_raw)) if (tf_raw > 0) else 0

        
        if type == "query":
            df = len(self.inverted_idx[token])
            total_docs = int(self.data_dicts["total_docs"])
            idf = math.log10(total_docs/df)
            
        # type = "document"
        else:
            #TODO
            # df = len(self.inverted_idx[token])
            idf = 1
        
        wt = tf_wt*idf        
        return wt


    def refineProcessingTfIdf(self, input_query, inverted_idx, docs_to_tokens_idx):
        
        self.inverted_idx = inverted_idx
        
        query_set = set()
        query_freq_dict = {}
        scores = {}
        doc_length = {}
        
        
        stemmer = PorterStemmer()
        
        
        """Making query_freq_dict - {query_token: frequency}
        """

        for x in range(len(input_query)):
            stemmed_query = stemmer.stem(input_query[x])
            if stemmed_query not in query_set:
                query_set.add(stemmed_query)
            
            if stemmed_query not in query_freq_dict.keys():
                query_freq_dict[stemmed_query] = 1
            else:
                query_freq_dict[stemmed_query] += 1
                
                    
        """Compute scores dict
        """                                    
        for token in query_set:
            
            wt_t_q = self.computeWeight("query", token, query_freq_dict)

            posting_lst = self.inverted_idx[token]
            
            for doc_element in posting_lst:
                
                # TODO Break
                doc_id = doc_element[0]
                # doc_id = str(doc_element[0])

                freq = doc_element[1]
                
                wt_t_doc = self.computeWeight("document", token, freq)
                
                if doc_id not in scores.keys():
                    # doc_id - str
                    scores[doc_id] = wt_t_q * wt_t_doc
                else:
                    scores[doc_id] += wt_t_q * wt_t_doc
                    
        """Compute doc_length dict
        """
        for doc_id in scores.keys():
            
            #TODO
            tokens_cnt = 0
            
            # if doc_id in self.docs_to_tokens_idx.keys():
            if doc_id in docs_to_tokens_idx.keys():

                tokens_dict = docs_to_tokens_idx[doc_id]
            
                for token in tokens_dict.keys():
                    tokens_cnt +=  tokens_dict[token]
                
                doc_length[doc_id] = tokens_cnt
            else:
                # print("unable to find doc_id - {}".format(doc_id))
                doc_length[doc_id] = 0
                continue
            
            
        """Compute doc_length v1
        """   
        # for token in self.inverted_idx.keys():
            
        #     posting_lst = self.inverted_idx[token]
            
        #     for doc_element in posting_lst:
                
        #         doc_id = doc_element[0]
        #         freq = doc_element[1]
                
        #         if doc_id in scores.keys():
                    
        #             if doc_id in doc_length.keys():
        #                 doc_length[doc_id] += freq
        #             else:
        #                 doc_length[doc_id] = freq
        #         else:
        #             continue
            
            
        for doc_id in scores.keys():
            
            if doc_id in doc_length.keys():
            
                if doc_length[doc_id] != 0:
                    scores[doc_id] = scores[doc_id]/doc_length[doc_id]
                else:
                    continue
            else:
                print("unable to find doc_id = {} in dict doc_length".format(doc_id))
                

        """Sorting based on scores
        """
        # ans_lst => common posting
        scores_items = sorted(scores.items(), key=lambda x: x[1], reverse=True)

        return scores_items





if __name__ == "__main__":
    
    
    load_dicts = {}
    load_dicts = defaultdict(lambda: [False, "path"])
    
    # Always True
    load_dicts["data_map_doc_idx_to_url"] = [True, "01_final/jsonDataUrl_0.json"]

    # SELECT
    load_dicts["data_inverted_idx"] = [True, "01_final/jsonDataInvIdx_0.json"]
    load_dicts["data_imp_words_inverted_idx"] = [False, "01_final/jsonDataImpWordsIdx_0.json"]
    load_dicts["data_anchor_words_inverted_idx"] = [False, "01_final/jsonDataAnchorWordsIdx_0.json"]
    load_dicts["data_two_gram_inverted_idx"] = [False, "01_final/jsonDataTwoGram_0.json"]
    load_dicts["data_three_gram_inverted_idx"] = [False, "01_final/jsonDataThreeGram_0.json"]
    
    # Doc to tokens tf-idf scoring
    load_dicts["data_doc_to_token_dict"] = [True, "01_final/jsonDataDocToTokenIdx_0.json"]
    
    
    obj = QueryProcessing(load_dicts)
    
    print("Started loading all dicts to memory")
    
    data_dicts = defaultdict(lambda: None)

    start_time = time.time()    
    print("start loadBuffer...")
    
    if load_dicts["data_doc_to_token_dict"][0]:
        data, total_docs = obj.loadBufferDocToToken()
        data_dicts["data_doc_to_token_dict"] = data
        data_dicts["total_docs"] = total_docs
    
    if load_dicts["data_inverted_idx"][0]:
        data, url_map_data = obj.loadBufferData()
        data_dicts["data_inverted_idx"] = data
        data_dicts["data_map_doc_idx_to_url"] = url_map_data

        
    if load_dicts["data_imp_words_inverted_idx"][0]:
        data, url_map_data = obj.loadBufferImp()
        data_dicts["data_imp_words_inverted_idx"] = data
        data_dicts["data_map_doc_idx_to_url"] = url_map_data
        
    if load_dicts["data_anchor_words_inverted_idx"][0]:
        data, url_map_data = obj.loadBufferAnchor()
        data_dicts["data_anchor_words_inverted_idx"] = data
        data_dicts["data_map_doc_idx_to_url"] = url_map_data



    if load_dicts["data_two_gram_inverted_idx"][0]:
        data, url_map_data = obj.loadBufferDataTwoGram()
        data_dicts["data_two_gram_inverted_idx"] = data
        data_dicts["data_map_doc_idx_to_url"] = url_map_data

    
    if load_dicts["data_three_gram_inverted_idx"][0]:   
        data, url_map_data = obj.loadBufferThreeGram()
        data_dicts["data_three_gram_inverted_idx"] = data
        data_dicts["data_map_doc_idx_to_url"] = url_map_data



    # data_dicts = obj.loadBuffer()
    exec_time = round((time.time()-start_time)*1000,3)            
    print("Loading all dicts to memory took {} milliseconds".format(exec_time))

    obj.queryProcessing(data_dicts)
    # obj.queryProcessing(data, url_map_data)