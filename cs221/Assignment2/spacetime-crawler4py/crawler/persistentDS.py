import os
import shelve
import random

from utils import get_tokenhash, getSimhashSimilarity

class TokenNode:
    def __init__(self, token, count):
        self.word = token
        self.count = count

class TokenNodeList:
    def __init__(self, tokens):
        self.array = []
        #TODO
        # for token in tokens: self.list.append(TokenNode(token, tokens[token]))
        for token in tokens: self.array.append(TokenNode(token, tokens[token]))
        self.tokenCount = len(self.array)



    def getPivotIndex(self, left, right, pivot):
        currentCount = self.array[pivot].count; maxLeft = left
        self.array[pivot], self.array[right] = self.array[right], self.array[pivot]
        for i in range(left, right):
            if self.array[i].count < currentCount:
                self.array[maxLeft], self.array[i] = self.array[i], self.array[maxLeft]
                maxLeft += 1
        self.array[right], self.array[maxLeft] = self.array[maxLeft], self.array[right]  
        return maxLeft


    def quickSelect(self, left, right, k):
        if left == right: return
        pivot = random.randint(left, right)
        #TODO     
        # pivot = self.getPivotIndex(self.array, left, right, pivot)
        pivot = self.getPivotIndex(left, right, pivot)

        if k == pivot: return
        #TODO
        # elif k > pivot: self.quickSelect(self.array, pivot + 1, right, k)
        elif k > pivot: self.quickSelect(pivot + 1, right, k)
        #TODO
        # else: self.quickSelect(self.array, left, pivot - 1, k)
        else: self.quickSelect(left, pivot - 1, k)

    
    def getTopElements(self, k):
        self.quickSelect(0, self.tokenCount - 1, self.tokenCount - k)
        result = []
        # TODO
        # for token in self.array[self.count - k: ]:
        for token in self.array[self.tokenCount - k: ]:
            result.append((token.word, token.count))
        return result


class PersistentDataStructure:
    def __init__(self, config, restart):
        
        if os.path.exists(config.tokenDict) and restart:
            os.remove(config.tokenDict)

        if os.path.exists(config.simhash) and restart:
            os.remove(config.simhash)

        if os.path.exists(config.results) and restart:
            os.remove(config.results)

        if os.path.exists(config.domains) and restart:
            os.remove(config.domains)

        # Load existing save file, or create one if it does not exist.
        self.tokenDict = shelve.open(config.tokenDict)
        self.simhashDict = shelve.open(config.simhash)
        self.results = shelve.open(config.results)
        self.simhashLength = config.simhashLength
        self.domains = shelve.open(config.domains)
  
        
    def addTokens(self, tokens):
        for token in tokens:
            if token in self.tokenDict:
                self.tokenDict[token] += tokens[token]
            else:
                self.tokenDict[token] = tokens[token]
        self.tokenDict.sync()


    def computeSimHash(self, tokens):
        bitBucket = [0] * self.simhashLength
        for token in tokens:
            tokenhash = get_tokenhash(token, self.simhashLength)
            for index in range(self.simhashLength):
                if tokenhash & (1 << index):
                    bitBucket[index] += tokens[token]
                else:
                    bitBucket[index] -= tokens[token]
        simhash = 0
        for index in range(self.simhashLength):
            if bitBucket[index] > 0:
                simhash += (1 << index)
        return simhash


    def compareSimhash(self, simhash):
        similarity = 0
        for hash in self.simhashDict:
            similarity = max(similarity, getSimhashSimilarity(simhash, hash, self.simhashLength))
        return similarity


    def saveSimHash(self, simhash, url):
        self.simhashDict[simhash] = url
        self.simhashDict.sync()


    def getTopKFrequentTokens(self, k):
        #TODO
        tokenNodeList = TokenNodeList(self.tokenDict)
        # tokenNodeList = TokenNodeList(self.tokenDict, k)
        return tokenNodeList.getTopElements(k)

    #TODO
    # def updateLargestPage(self, length, url):
    def updateLargestPage(self, url, length):
        if 'largestPage' not in self.results: self.results['largestPage'] = (url, length)
        else: 
            if self.results['largestPage'][1] < length: self.results['largestPage'] = (url, length)
        self.results.sync()

    
    def addURLtoDomain(self, url):
        # print("came here!")
        icsDomain = 'ics.uci.edu'
        # if 'domains' not in self.results: self.domains = dict({})
        sub_domain = url.split(icsDomain)[0]
        if sub_domain == 'https://www.': return
        if sub_domain[:12] == 'https://www.':
            if sub_domain[12:] + icsDomain in self.domains: 
                self.domains[sub_domain[12:] + icsDomain] += 1
            else:
                self.domains[sub_domain[12:] + icsDomain] = 1
        elif sub_domain[:11] == 'http://www.':
            if sub_domain[11:] + icsDomain in self.domains: 
                self.domains[sub_domain[11:] + icsDomain] += 1
            else:
                self.domains[sub_domain[11:] + icsDomain] = 1
        elif sub_domain[:8] == 'https://':
            if sub_domain[8:] + icsDomain in self.domains: 
                self.domains[sub_domain[8:] + icsDomain] += 1
            else:
                self.domains[sub_domain[8:] + icsDomain] = 1
        elif sub_domain[:7] == 'http://.':
            if sub_domain[7:] + icsDomain in self.domains: 
                self.domains[sub_domain[7:] + icsDomain] += 1
            else:
                self.domains[sub_domain[7:] + icsDomain] = 1
        else:
            return
        self.domains.sync()
        