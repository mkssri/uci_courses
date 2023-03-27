from operator import le
import re
from urllib.parse import urlparse
import codecs
from bs4 import BeautifulSoup
from urllib.request import Request, urlopen
from nltk.tokenize import RegexpTokenizer
from nltk.corpus import stopwords




def scraper(url, resp):
    links = extract_next_links(url, resp)
    return [link for link in links if is_valid(link)]

def extract_next_links(url, resp):
    # return list()
    # Implementation required.
    # url: the URL that was used to get the page
    # resp.url: the actual url of the page
    # resp.status: the status code returned by the server. 200 is OK, you got the page. Other numbers mean that there was some kind of problem.
    # resp.error: when status is not 200, you can check the error here, if needed.
    # resp.raw_response: this is where the page actually is. More specifically, the raw_response has two parts:
    #         resp.raw_response.url: the url, again
    #         resp.raw_response.content: the content of the page!
    # Return a list with the hyperlinks (as strings) scrapped from resp.raw_response.content

    # html_page = urlopen(url)
    if resp != None and resp.raw_response != None and resp.raw_response.content != None:
        html_page = resp.raw_response.content

        soup = BeautifulSoup(html_page, 'html.parser')

        url_set = set()

        for link in soup.find_all("a"):
            data = link.get('href')
            if data is not None:
                data = data.split('#')[0]
                #data = data.split('?')[0]
            else:
                continue
            if(is_valid(data) and data not in url_set):
                if(".ics.uci.edu/" in data or ".informatics.uci.edu/" \
                in data or ".stat.uci.edu/" in data or "today.uci.edu/department/information_computer_sciences/" \
                in data or ".cs.uci.edu/" in data):
                    
                    url_set.add(data)
        
        return list(url_set)
    else:
        return list()
    
    

def is_valid(url):
    # Decide whether to crawl this url or not. 
    # If you decide to crawl it, return True; otherwise return False.
    # There are already some conditions that return False.
    try:
        parsed = urlparse(url)
        if parsed.scheme not in set(["http", "https"]):
            return False
        return not re.match(
            r".*\.(css|js|bmp|gif|jpe?g|ico"
            + r"|png|tiff?|mid|mp2|mp3|mp4"
            + r"|wav|avi|mov|mpeg|ram|m4v|mkv|ogg|ogv|pdf"
            + r"|ps|eps|tex|ppt|pptx|doc|docx|xls|xlsx|names"
            + r"|data|dat|exe|bz2|tar|msi|bin|7z|psd|dmg|iso"
            + r"|epub|dll|cnf|tgz|sha1"
            + r"|thmx|mso|arff|rtf|jar|csv"
            + r"|rm|smil|wmv|swf|wma|zip|rar|gz)$", parsed.path.lower())

    except TypeError:
        print ("TypeError for ", parsed)
        raise



def parser(resp):

    if resp != None and resp.raw_response != None and resp.raw_response.content != None:
        resp = BeautifulSoup(resp.raw_response.content).get_text()

        tokenizer = RegexpTokenizer("[a-zA-Z0-9@#*&']+")
        stop_words = set(stopwords.words('english'))

        tokenDict = {}; tokenCount = 0
        tokens = tokenizer.tokenize(resp)

        for token in tokens:
            token = token.lower()
            if len(token) >=2 and token not in stop_words:
                tokenCount += 1
                if token in tokenDict: tokenDict[token] += 1
                else: tokenDict[token] = 1
        """
        tokenDict(except stop words) - key:token value:freq 
        tokenCount - all tokens except stop words
        tokens - all tokens count(including stop words)
        """
        return (tokenDict, tokenCount, len(tokens))
    
    else:
        return ({}, 0, 0)

def check_for_query(url):
    if url != None and "?" in url or ".ppsx" in url \
        or ".txt" in url or ".pdf" in url \
        or "/pdf/" in url or "/txt/" in url \
        or "publications/20-secret-sharing" in url:
        return True
    
    return False