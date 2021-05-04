import requests
import datetime
import math
import json

# Global vars ----------------------------------------------------------------------------------------
userAgent = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:88.0) Gecko/20100101 Firefox/88.0"

# Create a fake browser header to get data from exploit-db -------------------------------------------
beginHeaders = {
    "User-Agent": userAgent,
    'Accept-Encoding': 'gzip, deflate', 
    'Accept': '*/*', 
    'Connection': 'keep-alive'
}
response = requests.get("https://exploit-db.com", headers=beginHeaders)

# Get cookie from response header --------------------------------------------------------------------
cookies = {}
for cookieStatement in response.headers['Set-Cookie'].split(';'):
    if 'XSRF-TOKEN' in cookieStatement or 'exploit_database_session' in cookieStatement:
        cookies[cookieStatement.split('=')[0].replace('Secure, ', '')] = cookieStatement.split('=')[1]

# Get miliseconds from 1970-1-1, very important? -----------------------------------------------------
milisecsFrom1970_1_1utc = math.floor(((datetime.datetime.now() - datetime.datetime(1970,1,1)).total_seconds() - 7 * 3600) * 1000)

# Get JSON file describing the data ------------------------------------------------------------------
page = 1
itemsPerPage = 15

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
    "start":"{}".format(page-1),
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
print(json.loads(JSON_response.text))