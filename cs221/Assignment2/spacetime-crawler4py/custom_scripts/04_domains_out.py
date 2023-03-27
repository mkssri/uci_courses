domain = 'ics.uci.edu'
subdomainSet = set()
sub_dict = dict()
# url_path = "/Users/mksriram/Documents/11_Learnings/36-UNI/universities/UCI/UCI-2021/02_CourseWork/36_Coursework/02_Winter_2022/01_IR_CompSci_221/02_assignments/Assignment2/logs/murali-server/"
# file_path = "out_urls.txt"
file_path = "/home/mksriram/cs_221/Assignment2/spacetime-crawler4py/custom_scripts/v4/out_urls.txt"

with open(file_path,"r") as f:
    for i in f:
        if domain in i:
            url = i.strip()
            sub_domain = url.split(domain)[0]
            #to skip https://www.ics.uci.edu
            if(sub_domain == 'https://www.'):
                continue
            if(sub_domain == 'http://www.'):
                continue
            
            new_url = sub_domain.lower() + domain       
            new_url = new_url.strip("http://")
            new_url = new_url.strip("https://")
            subdomainSet.add(new_url)
            if new_url not in sub_dict:
                sub_dict[new_url] = set()
            
            sub_dict[new_url].add(url)       

listSubdomain = list(subdomainSet)
listSubdomain.sort()
output = []
i = 0
for x in listSubdomain:
    output.append([])
    output[i].append(x)
    output[i].append(len(sub_dict[x]))
    i+=1

print(len(listSubdomain))

for x in output:
    print(x)

# print(output)

"""
Execute command -
python3 04_domains_out.py > out_domains.txt
"""