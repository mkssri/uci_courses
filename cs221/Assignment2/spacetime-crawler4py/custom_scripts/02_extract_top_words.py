import shelve

dir_path = "/home/mksriram/cs_221/Assignment2/spacetime-crawler4py/shelve_files/v7/"
shelve_file = "tokens.shelve"

"""
tokens.shelve structure

key - token
value - freq
"""

sh = shelve.open(dir_path+shelve_file)

sort_orders = sorted(sh.items(), key=lambda x: x[1], reverse=True)

#print(sort_orders)

for i in range(50):
    print(sort_orders[i][0], sort_orders[i][1])

#print(type(sort_orders))

# for n in range(50):
#     sort_orders[i]

# for i in sort_orders:
#     print(i[0], i[1])


#print(list(sh.items()))

    

sh.close() 