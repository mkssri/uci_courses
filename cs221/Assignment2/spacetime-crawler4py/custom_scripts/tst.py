from urllib.parse import urlparse
import re

def main(url):
    
    out = is_valid(url)
    
    print("out - {}".format(out))


def is_valid(url):
    # Decide whether to crawl this url or not. 
    # If you decide to crawl it, return True; otherwise return False.
    # There are already some conditions that return False.
    try:
        parsed = urlparse(url)
        if parsed.scheme not in set(["http", "https"]):
            return False
        print("HI")
        print(dir(parsed))
        print(parsed.path.lower())
        print("HI")

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
    

if __name__ == "__main__":
    # url = "http://www.informatics.uci.edu/files/pdf/InformaticsBrochure-March2018"
    url = "http://www.informatics.uci.edu/files/InformaticsBrochure-March2018.pdf"
    # url = "http://sli.ics.uci.edu/Classes/2011W-178?action=download&upname=Mid_soln.pdf"
    main(url)