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
    milisecsFrom1970_1_1utc = math.ceil(((datetime.datetime.now().timestamp()) * 1000))

    # Create headers and queries -------------------------------------------------------------------------
    descriptionHeaders = {
        "Host": "www.exploit-db.com",
        "User-Agent": "{}".format(userAgent),
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
        "Accept-Language": "en-US,en;q=0.5",
        "Accept-Encoding": "gzip, deflate, br",
        "Referer": "https://www.exploit-db.com/",
        "Connection": "keep-alive",
        "Cookie": "CookieConsent={{stamp:'-1',necessary:true,preferences:true,statistics:true,marketing:true,ver:1,utc:{},region:'VN'}}; XSRF-TOKEN={}; exploit_database_session={}".format(milisecsFrom1970_1_1utc, cookies['XSRF-TOKEN'], cookies['exploit_database_session']),
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
    milisecsFrom1970_1_1utc = math.ceil(((datetime.datetime.now().timestamp()) * 1000))

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
        "Cookie": "CookieConsent={{stamp:'-1',necessary:true,preferences:true,statistics:true,marketing:true,ver:1,utc:{},region:'VN'}}; XSRF-TOKEN={}; exploit_database_session={}".format(milisecsFrom1970_1_1utc, cookies['XSRF-TOKEN'], cookies['exploit_database_session']),
        "Sec-GPC": "1",
        "TE": "Trailers"
    }

    JSON_queries = {
        "draw":"1",
        "columns[0][data]":"date_published",
        "columns[0][name]":"date_published",
        "columns[0][searchable]":"true",
        "columns[0][orderable]":"true",
        "columns[0][search][value]":"",
        "columns[0][search][regex]":"false",
        "columns[1][data]":"download",
        "columns[1][name]":"download",
        "columns[1][searchable]":"false",
        "columns[1][orderable]":"false",
        "columns[1][search][value]":"",
        "columns[1][search][regex]":"false",
        "columns[2][data]":"application_md5",
        "columns[2][name]":"application_md5",
        "columns[2][searchable]":"true",
        "columns[2][orderable]":"false",
        "columns[2][search][value]":"",
        "columns[2][search][regex]":"true",
        "columns[3][data]":"verified",
        "columns[3][name]":"verified",
        "columns[3][searchable]":"true",
        "columns[3][orderable]":"false",
        "columns[3][search][value]":"",
        "columns[3][search][regex]":"false",
        "columns[4][data]":"description",
        "columns[4][name]":"description",
        "columns[4][searchable]":"true",
        "columns[4][orderable]":"false",
        "columns[4][search][value]":"",
        "columns[4][search][regex]":"false",
        "columns[5][data]":"type_id",
        "columns[5][name]":"type_id",
        "columns[5][searchable]":"true",
        "columns[5][orderable]":"false",
        "columns[5][search][value]":"",
        "columns[5][search][regex]":"false",
        "columns[6][data]":"platform_id",
        "columns[6][name]":"platform_id",
        "columns[6][searchable]":"true",
        "columns[6][orderable]":"false",
        "columns[6][search][value]":"",
        "columns[6][search][regex]":"false",
        "columns[7][data]":"author_id",
        "columns[7][name]":"author_id",
        "columns[7][searchable]":"false",
        "columns[7][orderable]":"false",
        "columns[7][search][value]":"",
        "columns[7][search][regex]":"false",
        "columns[8][data]":"code",
        "columns[8][name]":"code.code",
        "columns[8][searchable]":"true",
        "columns[8][orderable]":"true",
        "columns[8][search][value]":"",
        "columns[8][search][regex]":"false",
        "columns[9][data]":"id",
        "columns[9][name]":"id",
        "columns[9][searchable]":"false",
        "columns[9][orderable]":"true",
        "columns[9][search][value]":"",
        "columns[9][search][regex]":"false",
        "order[0][column]":"9",
        "order[0][dir]":"desc",
        "start":"{}".format((page-1)*itemsPerPage),
        "length":"{}".format(itemsPerPage),
        "search[value]":"",
        "search[regex]":"false",
        "author":"",
        "port":"",
        "type":"",
        "tag":"",
        "platform":"",
        "_":"{}".format(milisecsFrom1970_1_1utc) 
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
        "Cookie": "CookieConsent={{stamp:'-1',necessary:true,preferences:true,statistics:true,marketing:true,ver:1,utc:{},region:'VN'}}; XSRF-TOKEN={}; exploit_database_session={}".format(milisecsFrom1970_1_1utc, cookies['XSRF-TOKEN'], cookies['exploit_database_session']),
        "Sec-GPC": "1",
        "TE": "Trailers"
    }

    JSON_queries = {
        "id":"{}".format(id),
        "draw":"1",
        "columns[0][data]":"date_published",
        "columns[0][name]":"date_published",
        "columns[0][searchable]":"true",
        "columns[0][orderable]":"true",
        "columns[0][search][value]":"",
        "columns[0][search][regex]":"false",
        "columns[1][data]":"download",
        "columns[1][name]":"download",
        "columns[1][searchable]":"false",
        "columns[1][orderable]":"false",
        "columns[1][search][value]":"",
        "columns[1][search][regex]":"false",
        "columns[2][data]":"application_md5",
        "columns[2][name]":"application_md5",
        "columns[2][searchable]":"true",
        "columns[2][orderable]":"false",
        "columns[2][search][value]":"",
        "columns[2][search][regex]":"true",
        "columns[3][data]":"verified",
        "columns[3][name]":"verified",
        "columns[3][searchable]":"true",
        "columns[3][orderable]":"false",
        "columns[3][search][value]":"",
        "columns[3][search][regex]":"false",
        "columns[4][data]":"description",
        "columns[4][name]":"description",
        "columns[4][searchable]":"true",
        "columns[4][orderable]":"false",
        "columns[4][search][value]":"",
        "columns[4][search][regex]":"false",
        "columns[5][data]":"type_id",
        "columns[5][name]":"type_id",
        "columns[5][searchable]":"true",
        "columns[5][orderable]":"false",
        "columns[5][search][value]":"",
        "columns[5][search][regex]":"false",
        "columns[6][data]":"platform_id",
        "columns[6][name]":"platform_id",
        "columns[6][searchable]":"true",
        "columns[6][orderable]":"false",
        "columns[6][search][value]":"",
        "columns[6][search][regex]":"false",
        "columns[7][data]":"author_id",
        "columns[7][name]":"author_id",
        "columns[7][searchable]":"false",
        "columns[7][orderable]":"false",
        "columns[7][search][value]":"",
        "columns[7][search][regex]":"false",
        "columns[8][data]":"code",
        "columns[8][name]":"code.code",
        "columns[8][searchable]":"true",
        "columns[8][orderable]":"true",
        "columns[8][search][value]":"",
        "columns[8][search][regex]":"false",
        "columns[9][data]":"id",
        "columns[9][name]":"id",
        "columns[9][searchable]":"false",
        "columns[9][orderable]":"true",
        "columns[9][search][value]":"",
        "columns[9][search][regex]":"false",
        "order[0][column]":"9",
        "order[0][dir]":"desc",
        "start":"0",
        "length":"1",
        "search[value]":"",
        "search[regex]":"false",
        "author":"",
        "port":"",
        "type":"",
        "tag":"",
        "platform":"",
        "_":"{}".format(milisecsFrom1970_1_1utc)  
    }

    JSON_response = requests.get("https://exploit-db.com", params=JSON_queries, headers=JSON_headers)
    return json.loads(JSON_response.text)

def getFavoriteIDs(l, r):
    return favoriteIDs[l:r], len(favoriteIDs)
