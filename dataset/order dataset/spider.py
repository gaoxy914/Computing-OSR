# -*- coding: utf-8 -*-
"""
Created on Sat Oct 12 14:46:53 2019

@author: xiang
"""

from bs4 import BeautifulSoup
import requests
import csv
import bs4
import tabula
import os

def check_link(url):
    try:
        r = requests.get(url)
        r.raise_for_status()
        r.encoding = r.apparent_encoding
        return r.text
    except:
        print('error in connecting!')
        
def get_contents(ulist, rurl):
    soup = BeautifulSoup(rurl, 'lxml')
    trs = soup.find_all('tr')
    for tr in trs:
        ui = []
        for td in tr:
            ui.append(td.string)
        ulist.append(ui)
        
def save_contents(path, urlist):
    with open(path, 'w') as f:
        writer = csv.writer(f)
        for i in range(1, len(urlist)):
            writer.writerow([urlist[i][0], urlist[i][1]])

def convert_pdf():
    tabula.convert_into("Store_List.pdf", "data.csv", pages="all")

def main():
    #urli = []
    #url1 = "http://phaster.com/zip_code.html"
    #path1 = "state_city_zip.csv"
    #url2 = "https://bennetyee.org/ucsd-pages/area.html"
    #path2 = "ac_state.csv"
    #rs = check_link(url2)
    #get_contents(urli, rs)
    #save_contents(path2, urli)
    #convert_pdf()

main()