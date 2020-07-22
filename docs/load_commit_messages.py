#!/usr/bin/env python3
'''
Extracts log messages from svn.
Translates into a dictionary using proper types.
Loads into a Mongo DB collection.
'''

import argparse

parser = argparse.ArgumentParser(description='Generate release notes.')
parser.add_argument('path', help='Path.')
parser.add_argument('--start_revision',
                    type=int,
                    help='Starting date.')
parser.add_argument('--stop_revision',
                    type=int,
                    help='Stopping date.')
args = parser.parse_args()

import os
import subprocess
import datetime

from pymongo import MongoClient

# Extract
command = ['svn', 'log',
           args.path]
if args.start_revision and args.stop_revision:
    command.append('-r%d:%d' %
                   (args.start_revision, args.stop_revision))
output = subprocess.check_output(command)

# Translate
documents = list()
document = dict()

database_url = 'mongodb://localhost:27017/'
database_name = 'svn_logs'
collection_name = 'combo'
client = MongoClient(database_url)
db = client[database_name]
collection = db[collection_name]

for line_byte in output.splitlines():
    line = line_byte.decode('utf-8')

    if not line:
        continue

    if line.count('-') > 60:
        if document:
            if not collection.find_one({'revision': document['revision']}):
                documents.append(document)
        document = dict()
    elif line.count('|') == 3:
        header = line.split('|')
        revision = int(header[0].strip('r'))
        committer = header[1].strip()
        date = datetime.date.fromisoformat(header[2].split()[0])
        n_lines = int(header[3].split()[0])

        document['revision'] = revision
        document['committer'] = committer
        document['date'] = datetime.datetime(date.year, date.month, date.day)
        document['n_lines'] = n_lines
        document['message'] = ''
        
    else:
        # message body
        document['message'] += line    

# Load
if documents:
    collection.insert_many(documents)
print(collection.count_documents({}))

