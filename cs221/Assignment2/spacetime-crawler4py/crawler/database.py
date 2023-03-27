import os
import sqlite3
from utils import get_urlhash, normalize


class Database:
    def __init__(self, config, restart):
        
        createTable = False
        if not os.path.exists(config.DB) or restart: createTable = True
        
        if os.path.exists(config.DB) and restart:
            os.remove(config.DB)
            
        self.DBconnect = sqlite3.connect(config.DB, check_same_thread=False)
        self.DBcursor = self.DBconnect.cursor()
        
        # Create table
        if createTable: self.DBcursor.execute('''CREATE TABLE pages (urlHash integer, url text, page blob)''')
        
        
    def addEntry(self, url, response):
        urlNorm = normalize(url)
        urlhash = get_urlhash(urlNorm)
        try:
            self.DBcursor.execute("insert into pages values (?,?,?)", (urlhash, url, response.raw_response.content))
            self.DBconnect.commit()
        except:
            pass
        
    def closeConnection(self):
        self.DBconnect.close()
