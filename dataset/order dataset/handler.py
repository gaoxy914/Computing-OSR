# -*- coding: utf-8 -*-
"""
Created on Mon Nov  4 17:12:53 2019

@author: xiang
"""

import csv
import string
import random
import pandas as pd
import os
import sys
'''
with open('uszips.csv', 'r') as f:
    reader = csv.reader(f)
    datas = []
    for row in reader:
        s = ''.join(random.sample(string.digits, 3)) + '-' + ''.join(random.sample(string.digits, 4))
        pn = '(' + row[4] + ') ' + s
        row.append(pn)
        datas.append(row)

with open('result.csv', 'w', newline = '') as f:
    writer = csv.writer(f)
    for row in datas:
        writer.writerow(row)
'''

'''
with open('US.csv', 'w+', newline='') as f:
    writer = csv.writer(f, dialect='excel')
    with open('US.txt', 'r') as f2:
        for line in f2:
            line_list = line.strip('\n').split('\t')
            writer.writerow(line_list)
'''

#st = ['CT', 'MA', 'ME', 'NH', 'NJ', 'NY', 'PA', 'RI', 'VT']
#
#
#
#
'''
with open('US_northeast.csv', 'w', newline = '' ) as f:
    writer = csv.writer(f, dialect='excel')
    with open('US.csv', 'r') as f2:
        reader = csv.reader(f2)
        for row in reader:
            if row[3] in st:
                writer.writerow(row)
'''

'''
data1 = pd.read_csv('US_northeast.csv')
data2 = pd.read_csv('northeast/city_of_hamden.csv')
data2 = data2.drop_duplicates(subset = ['POSTCODE', 'STREET'], keep = 'first')
data1['POSTCODE'] = pd.to_numeric(data1['POSTCODE'], errors = 'coerce')
data2['POSTCODE'] = pd.to_numeric(data2['POSTCODE'], errors = 'coerce')
data = pd.merge(data1, data2, on = ['POSTCODE'])
data = data[['POSTCODE', 'CITY_x', 'STATE', 'STATE_ID', 'COUNTRY', 'AREA_CODE', 'STREET']]
data.to_csv('test3.csv', header = None, index = None)
'''


'''
file_path = 'northeast/'
data1 = pd.read_csv('US_northeast.csv')
for file in os.listdir(file_path):
    data2 = pd.read_csv(os.path.join(file_path, file))
    data2 = data2.drop_duplicates(subset = ['POSTCODE', 'STREET'], keep = 'first')
    data1['POSTCODE'] = pd.to_numeric(data1['POSTCODE'], errors = 'coerce')
    data2['POSTCODE'] = pd.to_numeric(data2['POSTCODE'], errors = 'coerce')
    data = pd.merge(data1, data2, on = ['POSTCODE'])
    data = data[['POSTCODE', 'CITY_x', 'STATE', 'STATE_ID', 'COUNTRY', 'AREA_CODE', 'STREET']]
    data.to_csv('test.csv', mode = 'a', header = None, index = None)

'''
'''
def get_filelist(dir, list):
    newDir = dir
    if os.path.isfile(dir):
        if dir.endswith('.csv'):
            list.append(dir)
    elif os.path.isdir(dir):
        for s in os.listdir(dir):
            newDir = os.path.join(dir, s)
            get_filelist(newDir, list)
    return list

list = get_filelist('us', [])

data1 = pd.read_csv('US.csv', dtype = object)
data1['POSTCODE'] = pd.to_numeric(data1['POSTCODE'], errors = 'coerce')
for file in list:
    data2 = pd.read_csv(file, dtype = object)
    data2 = data2.drop_duplicates(subset = ['POSTCODE', 'STREET'], keep = 'first') 
    data2['POSTCODE'] = pd.to_numeric(data2['POSTCODE'], errors = 'coerce')
    
    data = pd.merge(data1, data2, on = ['POSTCODE'], how = 'right')
    data = data[['POSTCODE', 'CITY_x', 'STATE', 'STATE_ID', 'COUNTRY', 'AREA_CODE', 'STREET']]
    data.to_csv('US_whole.csv', mode = 'a', header = None, index = None)



pn = []
for i in range(1000):
    for j in range(10000):
        pn.append(str(i).zfill(3) + '-' + str(j).zfill(4))
random.shuffle(pn)
index = 0

count = 0
with open('US_data.csv', 'w', newline = '') as f:
    writer = csv.writer(f, dialect='excel')
    with open('US_whole.csv', 'r') as f2:
        reader = csv.reader(f2)
        for row in reader:
            row[6] = row[6].replace('\n', '')
            row.append('(' + row[5] + ') ' + pn[index])
            index += 1
            count += 1
            if (index > len(pn) - 1):
                index = 0
            writer.writerow(row)
print(count)

'''
'''
count1 = 0
count2 = 0
with open('US_data.csv', 'r') as f:
    reader = csv.reader(f)
    for row in reader:
        count1 += 1
        if '' not in row:
            count2 += 1
            #print(row)
print(count1)
print(count2)

'''
'''
with open('US_data2.csv', 'w', newline = '') as f:
    writer = csv.writer(f, dialect='excel')
    with open('US_data.csv', 'r') as f2:
        reader = csv.reader(f2)
        for row in reader:
            if '' not in row:
                writer.writerow(row)
'''
'''
with open('US_data2.csv', 'r') as f:
    reader = csv.reader(f)
    for row in reader:
        print(row)
'''

count = 0
with open('US_data3.csv', 'w', newline = '') as f:
    writer = csv.writer(f, dialect='excel')
    with open('US_data2.csv', 'r') as f2:
        reader = csv.reader(f2)
        for row in reader:
            row[6] = row[6].replace(',', '')
            count += 1
            writer.writerow(row)
print(count)