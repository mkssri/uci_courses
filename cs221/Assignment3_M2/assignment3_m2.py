#!/home/mksriram/cs_221/virtual_environments/assignment2_venv/bin/python3

import json, time
from nltk.stem import PorterStemmer

  

# file_path = "/home/mksriram/cs_221/Assignment3_M2/Inverted_Index_File_v1"
# file_path = "/home/mksriram/cs_221/Assignment3_M2/jsonData_0.json"



def main(lst_path):
    
    print("started loading all dicts to memory")
    start_time = time.time()
    
    inverted_idx = loadFileAndGetIdx(lst_path[0])
    url_to_doc_idx = loadFileAndGetIdx(lst_path[1])

    end_time = time.time()
    print("loading all dicts to memory took - {} seconds".format(end_time-start_time))
    
    while(True):
        
        print("Please enter your query - ")
        input_query = input()

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
                ans_lst = inverted_idx[query[0]]
            
            else:
                ans_lst = comparePostings(inverted_idx[query[0]], inverted_idx[query[1]])
                query.pop(0)
                query.pop(0)

                while len(query) > 0:
                    
                    ans_lst = comparePostings(inverted_idx[query[0]], ans_lst)
                    query.pop(0)
                    
        """
        ans_lst = [[a,b],[c,d],[e,f]]
        
        a,b,c are document id
        and b,d,f are term frequencies
        """
        
        """
        Sorting based on frequencies
        """
        # ans_lst => common posting
        ans_lst.sort(key=lambda ans_lst:ans_lst[1], reverse=True)
        
        cnt = 0
        
        set_urls = set()
        
        while((len(set_urls)) < 5):
            
            if cnt < len(ans_lst):
                
                url_key = str(ans_lst[cnt][0])
                
                url = url_to_doc_idx[url_key].split("#")[0]
                
                if url not in set_urls:
                    print(url_to_doc_idx[url_key])
                    set_urls.add(url)
                    
                cnt += 1
                    
            else:
                break
        end_time = time.time()
    
        print("retreiving top 5 urls for input query took - {} seconds".format(end_time-start_time))

def loadFileAndGetIdx(file_path):
    
    with open(file_path) as f:
        idx_obj = f.read()
    idx = (json.loads(idx_obj))["data"]

    return idx
            
               
def comparePostings(p1, p2):
    
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


if __name__ == "__main__":

    # file_path = "/home/mksriram/cs_221/Assignment3_M2/jsonData_final.json"
    # url_file_path = "/home/mksriram/cs_221/Assignment3_M2/jsonDataUrls_0.json"
    
    file_path = "jsonData_final.json"
    url_file_path = "jsonDataUrls_0.json"
    
    lst_path = [file_path, url_file_path]
    main(lst_path)
