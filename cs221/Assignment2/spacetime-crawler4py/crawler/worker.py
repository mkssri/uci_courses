from threading import Thread

from inspect import getsource
from utils.download import download
from utils import get_logger
import scraper
import time
import shelve
import os

class Worker(Thread):
    def __init__(self, worker_id, config, frontier, database, dataStruct):
        self.logger = get_logger(f"Worker-{worker_id}", "Worker")
        self.config = config
        self.frontier = frontier
        self.database = database
        self.dataStruct = dataStruct
        # basic check for requests in scraper
        assert {getsource(scraper).find(req) for req in {"from requests import", "import requests"}} == {-1}, "Do not use requests from scraper.py"
        super().__init__(daemon=True)
        
    def run(self):
        
        flag = self.frontier.restart
        
        while True:
            tbd_url = self.frontier.get_tbd_url()
            
            if not tbd_url:
                self.logger.info("Frontier is empty. Stopping Crawler.")
                break
            
            """
            Avoding Traps 
            """
            if scraper.check_for_query(tbd_url):
                self.dataStruct.addURLtoDomain(tbd_url)
                self.frontier.mark_url_complete(tbd_url)
                continue
                   

            resp = download(tbd_url, self.config, self.logger)
            
            self.logger.info(
                f"Downloaded {tbd_url}, status <{resp.status}>, "
                f"using cache {self.config.cache_server}.")
            print(
                f"Downloaded {tbd_url}, status <{resp.status}>, "
                f"using cache {self.config.cache_server}.")


            """
            1xx: Informational – Communicates transfer protocol-level information.
            2xx: Success – Indicates that the client’s request was accepted successfully.
            3xx: Redirection – Indicates that the client must take some additional action in order to complete their request.
            4xx: Client Error – This category of error status codes points the finger at clients.
            5xx: Server Error – The server takes responsibility for these error status codes.
            """
            
            #TODO t
            # response codes that can be accepted
            if resp.status >= 200 and resp.status < 400:
            
                scraped_urls = scraper.scraper(tbd_url, resp)
                """
                tokens(except stop words) - key:token value:freq 
                tokens_len - all tokens count(except stop words)
                word_count - all tokens count with stop words
                """
                (tokens, tokens_len, word_count) = scraper.parser(resp)

                self.dataStruct.addTokens(tokens)
                
                #SIMHASH
                simhash = self.dataStruct.computeSimHash(tokens)

                # https://seocopywriting.com/whats-the-best-word-count-for-seo-copywriting/
                """
                Taking care of low information website and similar website
                """
                if self.dataStruct.compareSimhash(simhash) < 90 and int(word_count) < 200000:
                    self.database.addEntry(tbd_url, resp)
                    self.dataStruct.saveSimHash(simhash)
                else:
                    print("tbd_url - {} is not being added to hdd".format(tbd_url))

                self.dataStruct.updateLargestPage(tbd_url, tokens_len)
                self.dataStruct.addURLtoDomain(tbd_url)

                for scraped_url in scraped_urls:
                    self.frontier.add_url(scraped_url)
                
                
            self.frontier.mark_url_complete(tbd_url)
            
            time.sleep(self.config.time_delay)
        
        self.database.closeConnection()

        print("50 most command words are: \n")
        commonTokens = self.dataStruct.getTopKFrequentTokens(50)
        for token in commonTokens:
            print(token[0], token[1])

        print("Largest webpage is {}, which has {} words".format(\
                                self.dataStruct.results['largestPage'][0],\
                                self.dataStruct.results['largestPage'][1]))
        
        # TODO
        # for subdomain in sorted(self.dataStruct.domains):
        #     # print(subdomain, self.dataStruct.domains['domains'][subdomain])
        
        
        # No. of unique urls
        print("Number of unique urls are - {}".format(len(self.frontier.save)))
        
        
        
        
