import os, json, time
from bs4 import BeautifulSoup
from nltk.tokenize import RegexpTokenizer
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer



def main():
    
    """
    Directory and Subdirectory processing
    """
    
    # interest_dir = "DEV1"
    interest_dir = "DEV"
    
    curr_dir = os.getcwd()
    
    ele_in_curr_dir = os.listdir(curr_dir)
    
    if interest_dir in ele_in_curr_dir:
        dir_name = os.path.join(curr_dir, interest_dir)
    else:
        raise Exception("DEV not present in current working directory, please fix it!!")
        
    # Get the list of all files in directory tree at given path
    lst_of_files = list()
    
    for (dir_path, dir_names, file_names) in os.walk(dir_name):
        for file in file_names:
            
            if ".DS_Store" in file or "._" in file:
                continue
            else:
                lst_of_files += [os.path.join(dir_path, file)]
       
             
    """
    lst_of_files - have all file paths to process
    Processing each file
    """
    
    
    url_to_idx_dict = {}
    inverted_idx = {}
    """
    key - idx
    val - url
    """
    idx = 0
    for file_path in lst_of_files:
        
        
        if ".json" in file_path and "._" not in file_path:
            
            file_obj = open(file_path)
            file_dict = json.load(file_obj)
            
            # print(file_dict["url"])
            print("working on url[{}] - {}".format(idx,file_dict["url"]))

            webpage_content = file_dict["content"]
             
            """
            TOKENIZER
            """
            resp = BeautifulSoup(webpage_content, features="html.parser").get_text()
            
            if resp == None:
                continue
            
            tokenizer = RegexpTokenizer("[a-zA-Z0-9@#*&']+")
            stop_words = set(stopwords.words('english'))
            words_in_url = []


            tokens = tokenizer.tokenize(resp)

            for w in tokens:
                w = w.lower()
                if len(w) >=2 and w not in stop_words:
                    words_in_url.append(w)
                        
            
            """
            STEMMER
            """
            stemmer = PorterStemmer()
            #TODO
            stem_words_lst = [stemmer.stem(token) for token in words_in_url] 
            
            """
            key - token
            val - [p1, p2]
            p1 - url1, freq1
            p2 - url2, freq1
            """
            
            """
            inverted_idx = {key: token val:[(idx1, freq1), (idx2, freq2)]}
            
            
            """
            
            url_to_idx_dict[idx] = file_dict["url"]
            
            for token in stem_words_lst:
                
                if token in inverted_idx.keys():
                    
                    lst_of_idx_es = []
                    for a_tuple in inverted_idx[token]:
                        lst_of_idx_es.append(a_tuple[0])
                    
                    # lst_of_idx_es = [a_tuple[0] for a_tuple in inverted_idx[token]]

                    if idx in lst_of_idx_es:
                        loc = lst_of_idx_es.index(idx)
                        freq_tmp = inverted_idx[token][loc][1]
                        inverted_idx[token][loc] = (idx,freq_tmp+1)
                    else:
                        # loc = len(inverted_idx[token])
                        # inverted_idx[token].insert(loc, (idx,1))
                        inverted_idx[token].append((idx,1))
                
                else:
                    """
                    token not present
                    """
                    inverted_idx[token] = [(idx,1)]
            
            print("completed working on url[{}] - {}".format(idx,file_dict["url"]))
            idx += 1
            file_obj.close()
            

            
    print("inverted_idx - {}".format(inverted_idx))
    print("url_to_idx_dict - {}".format(url_to_idx_dict))
    print("len(url_to_idx_dict) is {}".format(len(url_to_idx_dict)))

        
            


if __name__ == '__main__':
    start_time = time.time()
    main()
    end_time = time.time()
    print("Execution took - {} seconds".format(end_time-start_time))
