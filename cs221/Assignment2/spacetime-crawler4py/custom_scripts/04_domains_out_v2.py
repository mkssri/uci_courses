domain = '.ics.uci.edu'
subdomainSet = set()
sub_dict = dict()
count = 0
with open("out_urls.txt","r") as f:
    for i in f:
        count+=1
        if domain in i:
            if "?uri=http://www.ics.uci.edu" in i:
                continue
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
            # new_url = new_url.strip("www.")
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
