#!/usr/bin/env python
'''
This script scans the svn logs for a specified date range,
pulls commit messages with the appropriate tags and generates
release notes, both for the meta-project and individual projects.
'''

#import argparse
#parser = argparse.ArgumentParser(description='Generate release notes.')
#parser.add_argument('start_date',
#                    type=int,
#                    help='Starting date.')
#parser.add_argument('stop_date',
#                    type=int,
#                    help='Stopping date.')
#args = parser.parse_args()

from pymongo import MongoClient
import nltk.sentiment.vader

database_url = 'mongodb://localhost:27017/'
database_name = 'svn_logs'
collection_name = 'combo'
client = MongoClient(database_url)
db = client[database_name]
collection = db[collection_name]

sid = nltk.sentiment.vader.SentimentIntensityAnalyzer()
for document in collection.find():
    message = document['message']
    print(message)
    ss = sid.polarity_scores(message)
    for k in sorted(ss):
        print('{0}: {1}, '.format(k, ss[k]), end='')




