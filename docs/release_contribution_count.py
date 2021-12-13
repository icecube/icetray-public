#!/usr/bin/env python3

import argparse
import logging
import os
import subprocess

parser = argparse.ArgumentParser(description='Count contributors between git revisions.')
parser.add_argument('start_revision', help='Starting revision number')                    
parser.add_argument('stop_revision', help='Stopping revision number')                    
args = parser.parse_args()

logging.basicConfig(level=logging.INFO)

def _extract_author(git_log_line):
    # Given a line like:
    #     Author: kathrawlins <78505717+kathrawlins@users.noreply.github.com>
    #         ...or...
    #     Author: Jakob van Santen <jvansanten@gmail.com>
    # return the contributor name
    # Essentially this function strips the leading "Author: " and trailing email.
    if 'Author: ' in git_log_line:
        result = git_log_line[len('Author: '):]
        idx = result.find('<')
        return result[:idx].strip()

def parse_contributors(git_log):
    # from the git log returns a set of contributor
    result = set()
    for line_byte in output.splitlines():
        line = line_byte.decode('utf-8')        
        contributor = _extract_author(line)
        if contributor:
            result.add(contributor)
    return result
    
if 'I3_SRC' not in os.environ:
    logging.fatal('I3_SRC not set.')
I3_SRC = os.environ['I3_SRC']

command = \
    ['git', 'log',
    '--mailmap',
     '%s..%s' % (args.start_revision, args.stop_revision),
     I3_SRC
    ]
output = subprocess.check_output(command)

contributors = parse_contributors(output)

for contributor in contributors:
    logging.info('    %s' % contributor)
logging.info('Contributor count = %d' % len(contributors))
