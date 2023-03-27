import shelve
# url_path = "/Users/mksriram/Documents/11_Learnings/36-UNI/universities/UCI/UCI-2021/02_CourseWork/36_Coursework/02_Winter_2022/01_IR_CompSci_221/02_assignments/Assignment2/logs/murali-server/"
# file_path = "out_urls.txt"
     
dir_path = "/home/mksriram/cs_221/Assignment2/spacetime-crawler4py/shelve_files/v4/"
shelve_file = "domains.shelve"

open_file = shelve.open(dir_path+shelve_file)


out_items = list(open_file.items())

# out_items = (open_file.items())

# print(out_items)


for item in out_items:
    print(item)
    

"""
Execute command -
python3 04_domains_out.py > out_domains.txt
"""