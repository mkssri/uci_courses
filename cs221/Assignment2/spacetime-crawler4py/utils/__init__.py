import os
import logging
from hashlib import sha256
from urllib.parse import urlparse

def get_logger(name, filename=None):
    logger = logging.getLogger(name)
    logger.setLevel(logging.INFO)
    if not os.path.exists("Logs"):
        os.makedirs("Logs")
    fh = logging.FileHandler(f"Logs/{filename if filename else name}.log")
    fh.setLevel(logging.DEBUG)
    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO)
    formatter = logging.Formatter(
       "%(asctime)s - %(name)s - %(levelname)s - %(message)s")
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)
    # add the handlers to the logger
    logger.addHandler(fh)
    logger.addHandler(ch)
    return logger


def get_urlhash(url):
    parsed = urlparse(url)
    # everything other than scheme.
    return sha256(
        f"{parsed.netloc}/{parsed.path}/{parsed.params}/"
        f"{parsed.query}/{parsed.fragment}".encode("utf-8")).hexdigest()

# 32 bit token hash generator
def get_tokenhash(token, length):
    length = length // 8
    return int.from_bytes(sha256(b"H").digest()[:length], 'little')


def getSimhashSimilarity(hash1, hash2, length):
    result = 0
    for index in range(length):
        if (hash1 & (1 << index)) == (hash2 & (1 << index)):
            result += 1
    return ((result * 100) // length)


def normalize(url):
    if url.endswith("/"):
        return url.rstrip("/")
    return url
