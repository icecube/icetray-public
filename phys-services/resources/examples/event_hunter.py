#!/usr/bin/env python
"""
Take input files and an event list, and make an output file with only
those events.
"""

import logging
import math
import sys

if sys.version_info[0] < 3:
    from itertools import ifilterfalse as filterfalse
else:
    from itertools import filterfalse

from icecube import icetray,dataio,dataclasses

def unique_everseen(iterable, key=None):
    """
    List unique elements, preserving order. Remember all elements ever seen.
    (from itertools recipes)
    """
    # unique_everseen('AAAABBBCCDAABBB') --> A B C D
    # unique_everseen('ABBCcAD', str.lower) --> A B C D
    seen = set()
    seen_add = seen.add
    if key is None:
        for element in filterfalse(seen.__contains__, iterable):
            seen_add(element)
            yield element
    else:
        for element in iterable:
            k = key(element)
            if k not in seen:
                seen_add(k)
                yield element

def hunt(events,input_files):
    """Return the necessary frames for the list of events."""
    input_files = [{'filename':f,'start_event':None} for f in input_files]
    open_file = {'filename':None,'file':None}
    frame = None
    for event in sorted(events):
        logging.info('now hunting event %r',event)
        if (len(input_files) > 5 and
            (input_files[1]['start_event'] is None or
             event > input_files[1]['start_event'])):
            logging.debug('binary search for correct file')
            index = int(math.floor(len(input_files)/2))
            index_max = len(input_files)
            index_min = 0
            while index:
                logging.debug('%d < %d < %d',index_min,index,index_max)
                # get event at index point
                if input_files[index]['start_event'] is None:
                    for f in dataio.I3File(input_files[index]['filename']):
                        if 'I3EventHeader' in f:
                            input_files[index]['start_event'] = (
                                f['I3EventHeader'].run_id if event[0] != -1 else -1,
                                f['I3EventHeader'].event_id
                            )
                            # check ordering
                            for prev_index in range(index-1, -1, -1):
                                if input_files[prev_index]['start_event']:
                                    assert input_files[index]['start_event'] > input_files[prev_index]['start_event'], "Filenames must be sorted in ascending order"
                                    break
                            for next_index in range(index+1, len(input_files)):
                                if input_files[next_index]['start_event']:
                                    assert input_files[index]['start_event'] < input_files[next_index]['start_event'], "Filenames must be sorted in ascending order"
                                    break
                            break
                if input_files[index]['start_event']:
                    logging.debug('index %d has run %d event %d',index,input_files[index]['start_event'][0],input_files[index]['start_event'][1])
                else:
                    logging.warn('index is empty. should be file %s',input_files[index]['filename'])
                    input_files[index]['start_event'] = False
                    new_index = index + 1
                    if new_index > index_max:
                        new_index = index - 1
                        if new_index < index_min:
                            logging.error('binary search failure')
                            break
                        else:
                            index = new_index
                    else:
                        index = new_index
                    continue
                if event == input_files[index]['start_event']:
                    logging.debug('found event %s' % (event,))
                    break
                elif event < input_files[index]['start_event']:
                    new_index = int(math.floor((index_min+index)/2))
                    if new_index == index:
                        # corner case that we'll just default to linear search
                        logging.debug('< new_index == index (%d)',index)
                        index = index_min
                        break
                    else:
                        logging.debug('go to lower half')
                        index_max = index
                        index = new_index
                else:
                    new_index = int(math.floor((index+index_max)/2))
                    if new_index == index:
                        logging.debug('> new_index == index (%d)',index)
                        break
                    else:
                        logging.debug('go to upper half')
                        index_min = index
                        index = new_index
            if index:
                logging.debug('stripping input_files to [%d:]',index)
                input_files = input_files[index:]
                
        logging.debug('check input_files linearly')
        flag = False
        while not flag and input_files:
            if open_file['filename'] != input_files[0]['filename']:
                try:
                    if open_file['file']:
                        open_file['file'].close()
                except Exception:
                    pass
                open_file['filename'] = input_files[0]['filename']
                logging.info('opening file %s',open_file['filename'])
                try:
                    open_file['file'] = dataio.I3File(open_file['filename'])
                except Exception:
                    raise Exception('cannot open file %s'%open_file['filename'])
            while True:
                if not frame:
                    logging.debug('first pop_frame')
                    frame = open_file['file'].pop_frame()
                if 'I3EventHeader' in frame:
                    if ((event[0] == -1 or frame['I3EventHeader'].run_id == event[0]) and
                        frame['I3EventHeader'].event_id == event[1]):
                        logging.debug('yielding %s frame %s' % (frame.Stop, event,))
                        yield frame
                        flag = True
                    elif flag:
                        logging.debug('break from flag')
                        break
                #logging.debug('second pop_frame')
                if open_file['file'].more(): 
                    frame = open_file['file'].pop_frame()
                    frame.purge()
                else:
                    break
            if flag is False:
                logging.debug('did not find event in %s',input_files[0]['filename'])
                del input_files[0]
            else:
                logging.info('found with event %s' % (event,))
                break
            
    try:
        if open_file['file']:
            open_file['file'].close()
    except Exception:
        pass

def main():
    from optparse import OptionParser, OptionGroup
    parser = OptionParser(usage='usage: %prog [options] <input files>')
    parser.add_option('-o','--output',help='output filename')
    # TODO: actually implement this
    #parser.add_option('--streams',default='IQP',
    #                  help='save streams of type (default: %default)')
    parser.add_option('--debug',default=False,action='store_true',
                      help='turn on debug logging')
    event_group = OptionGroup(parser, 'Event Id Options',
                              'Choose one event selection method')
    event_group.add_option('--event-file',dest='event_file',
                           help='file with lines that have "run_id event_id"')
    event_group.add_option('-e','--event','--events',dest='events',
                           help='comma-separated list of event ids in a single run')
    parser.add_option_group(event_group)
    input_group = OptionGroup(parser, 'Input File Options',
                              'Use either a file of filenames, or a filename per argument')
    input_group.add_option('--input-file',dest='input_file',
                           help='file with one line per input filename')
    parser.add_option_group(input_group)
    (options,args) = parser.parse_args()
    
    if options.event_file and options.events:
        raise Exception('Specify either an event filename or a list of event ids, not both')
    if options.input_file and args:
        raise Exception('Specify either a file of input filenames or file arguments, not both')
    
    logging.basicConfig(level='DEBUG' if options.debug else 'INFO')
    
    events = set()
    input_files = []
    
    if options.event_file:
        for line in open(options.event_file):
            line = line.strip()
            if line and not line.startswith('#'):
                r,e = line.split()
                events.add((int(r),int(e)))
    elif options.events:
        for e in options.events.split(','):
            events.add((-1,int(e)))
    
    if options.input_file:
        for line in open(options.input_file):
            line = line.strip()
            if line and not line.startswith('#'):
                for part in line.split():
                    input_files.append(part)
    elif args:
        input_files += args
    input_files = list(unique_everseen(input_files))
    
    output_file = dataio.I3File(options.output,'w')
    try:
        for frame in hunt(events,input_files):
            output_file.push(frame)
    finally:
        output_file.close()

if __name__ == '__main__':
    main()
