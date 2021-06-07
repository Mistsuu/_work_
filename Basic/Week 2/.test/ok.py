import requests
import argparse
import sys
import math
import datetime
import json
from   bs4      import BeautifulSoup

userAgent = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:88.0) Gecko/20100101 Firefox/88.0"

cookies                             = {}
cookies['XSRF-TOKEN']               = ''
cookies['exploit_database_session'] = ''

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

JSON_response = loadExploitJSON(1, 15)
# print(JSON_response)