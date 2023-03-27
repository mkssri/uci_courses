f = open("out3.txt", "r")

s1,s2,s3,r1,r2,r3 = [],[],[],[],[],[]


def helper(line, keyword):
    
    line = line.split(keyword)[-1].strip()
    line = line.split("seconds")[0]
    return line.strip()


def print_lst(lst, cnt):
    
    print("printing lst - {}".format(cnt))
    for ele in lst:
        print(ele)
    print("\n\n")

for line in f:
    
    
    keyword = "Naive Matix Transpose(Sequential) Exec time:"
    if  keyword in line:
        s1.append(helper(line, keyword))
        continue  
    
    keyword = "Matix Transpose(Sequential) Exec time:"
    if  keyword in line:
        s2.append(helper(line, keyword))
        continue
        
    keyword = "Matix Transpose(Multithreaded) unoptimized out-of-place Exec time:"
    if  keyword in line:
        s3.append(helper(line, keyword))
        continue
        
    keyword = "Updated Matix Transpose(Using cycles) Exec time:"
    if  keyword in line:
        r1.append(helper(line, keyword))
        continue    
    
    keyword = "Matix Transpose in-place(Using Multithreaded) unoptimized Exec time:"
    if  keyword in line:
        r2.append(helper(line, keyword))
        continue 
        
    keyword = "Matix Transpose out-of-place (Multithreaded, cache-friendly) by blocks Exec time:"
    if  keyword in line:
        r3.append(helper(line, keyword))
        continue
        
        
#PRINTING
print_lst(s1, 1)
print_lst(s2, 2)
print_lst(s3, 3)


print_lst(r1, 1)
print_lst(r2, 2)
print_lst(r3, 3)
    
      
f.close()

