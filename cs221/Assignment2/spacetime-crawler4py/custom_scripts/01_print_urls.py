import shelve

dir_path = "/home/mksriram/cs_221/Assignment2/spacetime-crawler4py/shelve_files/v7/"
shelve_file = "frontier.shelve"

"""
frontier.shelve structure

key - urlhash
value - (url, True/False)
"""

open_file = shelve.open(dir_path+shelve_file)

out_items = open_file.items()


for out_item in out_items:
    print(out_item[1][0])
    
open_file.close()

"""
Execute command -
python3 01_print_urls.py > out_urls.txt
"""