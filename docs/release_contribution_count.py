#!/usr/bin/env python

import argparse

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('start_revision',
                    type=int,
                    help='Starting revision number')
parser.add_argument('stop_revision',
                    type=int,
                    help='Stopping revision number')
args = parser.parse_args()

import os
import subprocess

command = ['svn', 'log',
           os.environ['I3_SRC'],
           '-r%d:%d' % (args.start_revision, args.stop_revision)]

output = subprocess.check_output(command)

contributors = set()
for line_byte in output.splitlines():
    line = line_byte.decode('utf-8')
    if line.count('|') == 3:
        contributor = line.split('|')[1]
        contributors.add(contributor.strip())
print(','.join(contributors))
print('Contributor count = %d' % len(contributors))
