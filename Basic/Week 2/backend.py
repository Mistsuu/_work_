import requests
import argparse
import sys
import math
import datetime
import json
from   bs4      import BeautifulSoup

# Global vars ----------------------------------------------------------------------------------------
userAgent              = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:88.0) Gecko/20100101 Firefox/88.0"
cookies                = {}
initDone               = False
favoriteIDs            = []
FAVORITE_IDS_FILENAME  = "favorite_ids.txt"

"""
    /function/ getDescriptionByID(id):
        "" Purpose:
            Get description of an exploit by its ID.
"""
def getDescriptionByID(id):
    exploit_description_url = "https://www.exploit-db.com/exploits/" + id

    # Get miliseconds from 1970-1-1, very important? -----------------------------------------------------
    milisecsFrom1970_1_1utc = math.floor(((datetime.datetime.now() - datetime.datetime(1970,1,1)).total_seconds() - 7 * 3600) * 1000)

    # Create headers and queries -------------------------------------------------------------------------
    descriptionHeaders = {
        "Host": "www.exploit-db.com",
        "User-Agent": "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:88.0) Gecko/20100101 Firefox/88.0",
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
        "Accept-Language": "en-US,en;q=0.5",
        "Accept-Encoding": "gzip, deflate, br",
        "Referer": "https://www.exploit-db.com/",
        "Connection": "keep-alive",
        "Cookie": "CookieConsent={{stamp:%27-1%27%2Cnecessary:true%2Cpreferences:true%2Cstatistics:true%2Cmarketing:true%2Cver:1%2Cutc:{}%2Cregion:%27VN%27}}; XSRF-TOKEN={}; exploit_database_session={}".format(milisecsFrom1970_1_1utc, cookies['XSRF-TOKEN'], cookies['exploit_database_session']),
        "Upgrade-Insecure-Requests": "1",
        "Sec-GPC": "1",
        "Cache-Control": "max-age=0"
    }
    
    soup = BeautifulSoup(requests.get(exploit_description_url, headers=descriptionHeaders).content.decode(), features="lxml")
    return soup.find('code').get_text() 

"""
    /function/ getArgs():
        "" Purpose:
            Get arguments from the command line.
"""
def getArgs():
    parser = argparse.ArgumentParser(description='Display the exploitdb database.')
    parser.add_argument('--exploit_id', help='Returns the thorough description of an exploit.')
    parser.add_argument('--page_num',   help='Returns the list of exploits in a page.')
    parser.add_argument('--favorite',   help='Returns the favorite list of exploits of the user of page <FAVORITE>.')
    return parser.parse_args()


"""
    /function/ initBackend():
        "" Purpose:
            Crawling the front page of exploit-db.com.
            Getting the needed cookies and stuffs. 
"""
def initBackend():
    # Create a fake browser header to get data from exploit-db -------------------------------------------
    beginHeaders = {
        "User-Agent": userAgent,
        'Accept-Encoding': 'gzip, deflate', 
        'Accept': '*/*', 
        'Connection': 'keep-alive'
    }
    response = requests.get("https://exploit-db.com", headers=beginHeaders)

    # Get cookie from response header --------------------------------------------------------------------
    global cookies
    for cookieStatement in response.headers['Set-Cookie'].split(';'):
        if 'XSRF-TOKEN' in cookieStatement or 'exploit_database_session' in cookieStatement:
            cookies[cookieStatement.split('=')[0].replace('Secure, ', '')] = cookieStatement.split('=')[1]

    # Loads the IDs in favorite_ids.txt ------------------------------------------------------------------
    global favoriteIDs
    try:
        favoriteIDFile = open(FAVORITE_IDS_FILENAME, "r")
        for ID in favoriteIDFile:
            ID = ID.rstrip().lstrip()
            if ID == '':
                continue
            favoriteIDs.append(ID)
    except:
        pass

    # Returns true to unlock the state in case of SIGINT-cacher thread calls cleanBackend() first --------
    global initDone
    initDone = True

"""
    /function/ cleanBackend():
        "" Purpose:
            Write the changed data to 'favorite.json'
            (possibly) close the connection
"""
def cleanBackend():
    global initDone
    if not initDone:
        return

    # Set again to prevent user hits multiple CTRL-C... --------------------------------------------------
    initDone = False

    # Rewrite the favorite id array to file --------------------------------------------------------------
    favoriteIDFile = open(FAVORITE_IDS_FILENAME, "w")
    for id in favoriteIDs:
        favoriteIDFile.write(id + '\n')

    # Exits the program ----------------------------------------------------------------------------------
    exit(0)

"""
    /function/ addExploitFromFavoriteByID(id):
        "" Function: Add an exploit from the favorite list by id.
"""
def addExploitToFavoriteByID(id):
    global favoriteIDs
    if id in favoriteIDs:
        print('[!] You have marked this exploit as favorite!\n')
        return

    # Fetch data from exploit-db.com to check if there is such exploit with that ID. ---------------------
    data = loadOneExploitJSONByID(id)
    if len(data["data"]) == 0:
        print("[!] The exploit you request does not exist!\n")
        return

    # Add to list ----------------------------------------------------------------------------------------
    favoriteIDs.append(id)

    print("[i] Add exploit successfully!\n")
    

"""
    /function/ removeExploitFromFavoriteByID(id):
        "" Function: Remove an exploit from the favorite list by id.
"""
def removeExploitFromFavoriteByID(id):
    global favoriteIDs
    if id not in favoriteIDs:
        print('[!] You don\'t have this exploit marked as favorite!\n')
        return
    
    # Remove from list -----------------------------------------------------------------------------------
    favoriteIDs.remove(id)

    print("[i] Remove an exploit successfully!\n")


"""
    /function/ loadExploitJSON(page, itemsPerPage):
        "" Purpose:
            Get JSON data from exploit-db server.
"""
def loadExploitJSON(page=1, itemsPerPage=15):
    # Get miliseconds from 1970-1-1, very important? -----------------------------------------------------
    milisecsFrom1970_1_1utc = math.floor(((datetime.datetime.now() - datetime.datetime(1970,1,1)).total_seconds() - 7 * 3600) * 1000)

    # Get JSON file describing the data ------------------------------------------------------------------
    JSON_headers = {
        "Host": "www.exploit-db.com",
        "User-Agent": "{}".format(userAgent),
        "Accept": "application/json, text/javascript, */*; q=0.01",
        "Accept-Language": "en-US,en;q=0.5",
        "Accept-Encoding": "gzip, deflate, br",
        "Referer": "https://www.exploit-db.com/",
        "X-Requested-With": "XMLHttpRequest",
        "Connection": "keep-alive",
        "Cookie": "CookieConsent={{stamp:%27-1%27%2Cnecessary:true%2Cpreferences:true%2Cstatistics:true%2Cmarketing:true%2Cver:1%2Cutc:{}%2Cregion:%27VN%27}}; XSRF-TOKEN={}; exploit_database_session={}".format(milisecsFrom1970_1_1utc, cookies['XSRF-TOKEN'], cookies['exploit_database_session']),
        "Sec-GPC": "1",
        "TE": "Trailers"
    }

    JSON_queries = {
        "draw":"1",
        "columns%5B0%5D%5Bdata%5D":"date_published",
        "columns%5B0%5D%5Bname%5D":"date_published",
        "columns%5B0%5D%5Bsearchable%5D":"true",
        "columns%5B0%5D%5Borderable%5D":"true",
        "columns%5B0%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B0%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B1%5D%5Bdata%5D":"download",
        "columns%5B1%5D%5Bname%5D":"download",
        "columns%5B1%5D%5Bsearchable%5D":"false",
        "columns%5B1%5D%5Borderable%5D":"false",
        "columns%5B1%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B1%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B2%5D%5Bdata%5D":"application_md5",
        "columns%5B2%5D%5Bname%5D":"application_md5",
        "columns%5B2%5D%5Bsearchable%5D":"true",
        "columns%5B2%5D%5Borderable%5D":"false",
        "columns%5B2%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B2%5D%5Bsearch%5D%5Bregex%5D":"true",
        "columns%5B3%5D%5Bdata%5D":"verified",
        "columns%5B3%5D%5Bname%5D":"verified",
        "columns%5B3%5D%5Bsearchable%5D":"true",
        "columns%5B3%5D%5Borderable%5D":"false",
        "columns%5B3%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B3%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B4%5D%5Bdata%5D":"description",
        "columns%5B4%5D%5Bname%5D":"description",
        "columns%5B4%5D%5Bsearchable%5D":"true",
        "columns%5B4%5D%5Borderable%5D":"false",
        "columns%5B4%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B4%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B5%5D%5Bdata%5D":"type_id",
        "columns%5B5%5D%5Bname%5D":"type_id",
        "columns%5B5%5D%5Bsearchable%5D":"true",
        "columns%5B5%5D%5Borderable%5D":"false",
        "columns%5B5%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B5%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B6%5D%5Bdata%5D":"platform_id",
        "columns%5B6%5D%5Bname%5D":"platform_id",
        "columns%5B6%5D%5Bsearchable%5D":"true",
        "columns%5B6%5D%5Borderable%5D":"false",
        "columns%5B6%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B6%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B7%5D%5Bdata%5D":"author_id",
        "columns%5B7%5D%5Bname%5D":"author_id",
        "columns%5B7%5D%5Bsearchable%5D":"false",
        "columns%5B7%5D%5Borderable%5D":"false",
        "columns%5B7%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B7%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B8%5D%5Bdata%5D":"code",
        "columns%5B8%5D%5Bname%5D":"code.code",
        "columns%5B8%5D%5Bsearchable%5D":"true",
        "columns%5B8%5D%5Borderable%5D":"true",
        "columns%5B8%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B8%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B9%5D%5Bdata%5D":"id",
        "columns%5B9%5D%5Bname%5D":"id",
        "columns%5B9%5D%5Bsearchable%5D":"false",
        "columns%5B9%5D%5Borderable%5D":"true",
        "columns%5B9%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B9%5D%5Bsearch%5D%5Bregex%5D":"false",
        "order%5B0%5D%5Bcolumn%5D":"9",
        "order%5B0%5D%5Bdir%5D":"desc",
        "start":"{}".format((page-1)*itemsPerPage),
        "length":"{}".format(itemsPerPage),
        "search%5Bvalue%5D":"",
        "search%5Bregex%5D":"false",
        "author":"",
        "port":"",
        "type":"",
        "tag":"",
        "platform":"",
        "_":"1619710715421" 
    }

    JSON_response = requests.get("https://exploit-db.com", params=JSON_queries, headers=JSON_headers)
    return json.loads(JSON_response.text)

"""
    /function/ loadOneExploitJSONByID(id):
        "" Purpose:
            Get JSON data of an exploit by its ID from exploit-db server.
"""
def loadOneExploitJSONByID(id):
    # Get miliseconds from 1970-1-1, very important? -----------------------------------------------------
    milisecsFrom1970_1_1utc = math.floor(((datetime.datetime.now() - datetime.datetime(1970,1,1)).total_seconds() - 7 * 3600) * 1000)

    # Get JSON file describing the data ------------------------------------------------------------------
    JSON_headers = {
        "Host": "www.exploit-db.com",
        "User-Agent": "{}".format(userAgent),
        "Accept": "application/json, text/javascript, */*; q=0.01",
        "Accept-Language": "en-US,en;q=0.5",
        "Accept-Encoding": "gzip, deflate, br",
        "Referer": "https://www.exploit-db.com/",
        "X-Requested-With": "XMLHttpRequest",
        "Connection": "keep-alive",
        "Cookie": "CookieConsent={{stamp:%27-1%27%2Cnecessary:true%2Cpreferences:true%2Cstatistics:true%2Cmarketing:true%2Cver:1%2Cutc:{}%2Cregion:%27VN%27}}; XSRF-TOKEN={}; exploit_database_session={}".format(milisecsFrom1970_1_1utc, cookies['XSRF-TOKEN'], cookies['exploit_database_session']),
        "Sec-GPC": "1",
        "TE": "Trailers"
    }

    JSON_queries = {
        "id":"{}".format(id),
        "draw":"1",
        "columns%5B0%5D%5Bdata%5D":"date_published",
        "columns%5B0%5D%5Bname%5D":"date_published",
        "columns%5B0%5D%5Bsearchable%5D":"true",
        "columns%5B0%5D%5Borderable%5D":"true",
        "columns%5B0%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B0%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B1%5D%5Bdata%5D":"download",
        "columns%5B1%5D%5Bname%5D":"download",
        "columns%5B1%5D%5Bsearchable%5D":"false",
        "columns%5B1%5D%5Borderable%5D":"false",
        "columns%5B1%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B1%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B2%5D%5Bdata%5D":"application_md5",
        "columns%5B2%5D%5Bname%5D":"application_md5",
        "columns%5B2%5D%5Bsearchable%5D":"true",
        "columns%5B2%5D%5Borderable%5D":"false",
        "columns%5B2%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B2%5D%5Bsearch%5D%5Bregex%5D":"true",
        "columns%5B3%5D%5Bdata%5D":"verified",
        "columns%5B3%5D%5Bname%5D":"verified",
        "columns%5B3%5D%5Bsearchable%5D":"true",
        "columns%5B3%5D%5Borderable%5D":"false",
        "columns%5B3%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B3%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B4%5D%5Bdata%5D":"description",
        "columns%5B4%5D%5Bname%5D":"description",
        "columns%5B4%5D%5Bsearchable%5D":"true",
        "columns%5B4%5D%5Borderable%5D":"false",
        "columns%5B4%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B4%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B5%5D%5Bdata%5D":"type_id",
        "columns%5B5%5D%5Bname%5D":"type_id",
        "columns%5B5%5D%5Bsearchable%5D":"true",
        "columns%5B5%5D%5Borderable%5D":"false",
        "columns%5B5%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B5%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B6%5D%5Bdata%5D":"platform_id",
        "columns%5B6%5D%5Bname%5D":"platform_id",
        "columns%5B6%5D%5Bsearchable%5D":"true",
        "columns%5B6%5D%5Borderable%5D":"false",
        "columns%5B6%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B6%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B7%5D%5Bdata%5D":"author_id",
        "columns%5B7%5D%5Bname%5D":"author_id",
        "columns%5B7%5D%5Bsearchable%5D":"false",
        "columns%5B7%5D%5Borderable%5D":"false",
        "columns%5B7%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B7%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B8%5D%5Bdata%5D":"code",
        "columns%5B8%5D%5Bname%5D":"code.code",
        "columns%5B8%5D%5Bsearchable%5D":"true",
        "columns%5B8%5D%5Borderable%5D":"true",
        "columns%5B8%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B8%5D%5Bsearch%5D%5Bregex%5D":"false",
        "columns%5B9%5D%5Bdata%5D":"id",
        "columns%5B9%5D%5Bname%5D":"id",
        "columns%5B9%5D%5Bsearchable%5D":"false",
        "columns%5B9%5D%5Borderable%5D":"true",
        "columns%5B9%5D%5Bsearch%5D%5Bvalue%5D":"",
        "columns%5B9%5D%5Bsearch%5D%5Bregex%5D":"false",
        "order%5B0%5D%5Bcolumn%5D":"9",
        "order%5B0%5D%5Bdir%5D":"desc",
        "start":"0",
        "length":"1",
        "search%5Bvalue%5D":"",
        "search%5Bregex%5D":"false",
        "author":"",
        "port":"",
        "type":"",
        "tag":"",
        "platform":"",
        "_":"1619894401581" 
    }

    JSON_response = requests.get("https://exploit-db.com", params=JSON_queries, headers=JSON_headers)
    return json.loads(JSON_response.text)

def getFavoriteIDs(l, r):
    return favoriteIDs[l:r], len(favoriteIDs)