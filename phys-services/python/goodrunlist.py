# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
Provides simple access in Python to the text file based Good run Lists (GRLs) for pass1 and pass2.
The GRL object is just a Python dict (the keys are the run_ids). The values are dicts as well with
some metadata from the GRLs and methods to get the files and other data.

Each run entry looks similar to this: ::

  {'active_doms': 5392,
   'active_inice': 5049,
   'active_strings': 86,
   'comment': None,
   'detector': 'IC86.2017',
   'good_i3': 1,
   'good_it': 1,
   'livetime': 28808.34,
   'outdir': '/data/exp/IceCube/2017/filtered/level2/1022/Run00130150',
   'run_id': 130150}

Usage: ::

  import goodrunlist

  grl = goodrunlist.GoodRunList()

  # Load one specific GRL
  grl.load('/data/exp/IceCube/2011/filtered/level2/IC86_2011_GoodRunInfo.txt')

  # Add another GRL
  grl.load('/data/exp/IceCube/2015/filtered/level2/IC86_2015_GoodRunInfo.txt')

  # Loop over all good runs:
  for run in grl:
    # get L2 files of run
    l2_files = run.get_files()

    # GCD file
    gcd_file = run.get_gcd_file()

    # get all L2 files + metadata
    l2_files_meta = run.get_files_metadata()
    for f in l2_files_meta:
        filepath = f['path']

        # Livetime in seconds
        livetime_of_file = f['livetime']

        # file number aka subrun
        file_number = f['file_number']

        # Event IDs
        first_event = f['first_event']
        last_event = f['last_event']

        # Time of events (I3Time)
        first_event_time = f['first_event_time'] # Do you want a `datetime` object? f['first_event_time'].date_time
        last_event_time = f['last_event_time']

  # Check if run is in GRL
  if 127390 in grl:
    print('Run 127390 is a good run')
  # or
  if grl.has_run(127390):
    print('Run 127390 is a good run')

  # Print run data
  print(grl[127390])

  # Exclude one run
  grl.exclude_runs(127390)

  # Exclude several runs:
  grl.exclude_runs([127390, 127391])
  grl.exclude_runs('/path/to/file.txt') # this file may only contain run ids separated by white spaces (space, newline, tab)

  # Make some cuts
  # Only good InIce runs
  grl_good_inice = GoodRunList({k: v for k, v in grl.items() if v['good_i3']})

  # Burne sample
  grl_burn_samlpe = GoodRunList({k: v for k, v in grl.items() if not k % 10})
  # or
  grl_burn_samlpe = grl.get_burne_sample()

  # IC86.2011 runs only
  grl_ic86_2011 = GoodRunList({k: v for k, v in grl.items() if v['detector'] == 'IC86.2011'})

  # All strings active
  grl_all_strings = GoodRunList({k: v for k, v in grl.items() if v['active_strings'] == 86})

  # etc....

You can also load all GRLs since IC79 (included): ::

  import goodrunlist

  # Pass1 lists/data
  grl_pass1 = goodrunlist.GRL()

  # Pass2 lists/data:
  # IC79 - IC86.2016 will be pass2 data
  grl_pass2 = goodrunlist.GRL(pass2 = True)

Now you can handle those objects the same as shown above.
"""

import os
from glob import glob
from datetime import datetime, timezone
import requests
import json
import urllib
import numpy as np

from typing import no_type_check

class RunInfo(dict):
    def __init__(self, data):
        super().__init__(data)

    def get_gcd_file(self):
        """
        Tries to find the GCD file of this run.
        """

        files = glob(os.path.join(self['outdir'], 'Level2*_IC??*_Run{run_id:0>8}_*GCD*.i3.bz2'.format(run_id = self['run_id'])))
        files.extend(glob(os.path.join(self['outdir'], 'Level2*_IC??*_Run{run_id:0>8}_*GCD*.i3.gz'.format(run_id = self['run_id']))))
        files.extend(glob(os.path.join(self['outdir'], 'Level2*_IC??*_Run{run_id:0>8}_*GCD*.i3.zst'.format(run_id = self['run_id']))))

        if len(files) != 1:
            raise RuntimeError('Did not find exactly one GCD file for run {}'.format(self['run_id']))

        return files[0]

    def get_files(self):
        """
        Lists all L2 files of this run (IT, EHE, SLOP, ROOT, DST, etc are excluded).

        The output is sorted by filename.
        """

        files = glob(os.path.join(self['outdir'], 'Level2*_IC??*_data_Run{run_id:0>8}_*[0-9][0-9][0-9].i3.bz2'.format(run_id = self['run_id'])))
        files.extend(glob(os.path.join(self['outdir'], 'Level2*_IC??*_data_Run{run_id:0>8}_*[0-9][0-9][0-9].i3.gz'.format(run_id = self['run_id']))))
        files.extend(glob(os.path.join(self['outdir'], 'Level2*_IC??*_data_Run{run_id:0>8}_*[0-9][0-9][0-9].i3.zst'.format(run_id = self['run_id']))))

        return sorted(files)

    def get_files_metadata(self):
        """
        Same as `get_files()` but with additional metadata.

        Returns a list of dictionaries, sorted by file number aka sub run.
        """

        from icecube import dataclasses

        files = self.get_files()
        metadata = {int(f['file_number']): f for f in self._get_file_metadata()}

        data = []

        for f in files:
            file_number = self._find_file_number(f)

            if file_number not in metadata:
                raise RuntimeError('No metadata available for run {run_id}, file #{file_number} ({path})'.format(run_id = self['run_id'], file_number = file_number, path = f))

            entry = {
                'path': f,
                'file_number': file_number,
                'livetime': float(metadata[file_number]['livetime']),
                'first_event': int(metadata[file_number]['first_event']),
                'last_event': int(metadata[file_number]['last_event']),
                'first_event_time': dataclasses.I3Time(int(metadata[file_number]['first_event_year']), int(metadata[file_number]['first_event_frac'])),
                'last_event_time': dataclasses.I3Time(int(metadata[file_number]['last_event_year']), int(metadata[file_number]['last_event_frac']))
            }

            data.append(entry)

        return sorted(data, key = lambda f: f['file_number'])

    def get_date(self):
        """
        Tries to get the start date of this run from the path.
        """

        path = self['outdir'].replace('pass2', '').replace('/mnt/lfs2/', '/data/').replace('level2a', 'level2')
        if not path.endswith('/'):
            path += '/'

        try:
            return datetime.strptime(path, '/data/exp/IceCube/%Y/filtered/level2/%m%d/Run{:0>8}/'.format(self['run_id'])).replace(tzinfo=timezone.utc).date()
        except ValueError:
            try:
                return datetime.strptime(path, '/data/exp/IceCube/%Y/filtered/level2/%m%d/').replace(tzinfo=timezone.utc).date()
            except ValueError:
                return datetime.strptime(path.split('_')[:-1], '/data/exp/IceCube/%Y/filtered/level2/%m%d/Run{:0>8}/'.format(self['run_id'])).replace(tzinfo=timezone.utc).date()

    def _get_file_metadata(self):
        r = requests.get(
            'https://grid.icecube.wisc.edu/filtering/CalendarView/gapsfile.php?run_id={run_id}&pass2={pass2}'.format(run_id = self['run_id'], pass2 = int(self['pass'] == 2)),
            auth = ('icecube', 'skua')
        )

        data = r.json()

        if data['error']:
            raise RuntimeError('Error while loading file metadata. Error message: {}'.format(data['error_msg']))
        else:
            return data['data'][str(self['run_id'])]

    def _find_file_number(self, path):
        """
        Tries to find the file number aka sub run.
        """

        name = os.path.basename(path)

        if self['pass'] == 2 or int(self['detector'].split('.')[1]) >= 2017:
            # pass2 run or of season >= 2017
            return int(name.split('Subrun00000000_')[1].split('.')[0])
        elif 'Subrun' in name:
            return  int(name.split('Subrun')[1].split('.')[0])
        elif 'Part' in name:
            return  int(name.split('Part')[1].split('.')[0])
        else:
            raise RuntimeError('I do not know this file name pattern. Cannot determine the file number.')

    def get_run_ids_in_range(self, start_time, end_time):
        """
        Tries to get the run ids from I3Live that are between the ``start_time`` and ``end_time``
        Be warned: this function should be used sparingly to not overload the I3Live server!

        :param start_time: a ``datetime`` instance corresponding to the start of the window to request run ids from

        :param end_time: a ``datetime`` instance corresponding to the end of the window to request run ids from

        :return: list of bundled (run id, start-time of run, end-time of run) as (int, datetime, datetime)
                 that took place between ``start_time`` and ``end_time``

        """

        test_url = "https://virgo.icecube.wisc.edu/run_info/"
        params = {
            "user": "icecube",
            "pass": "skua",
            "start": start_time.strftime("%Y-%m-%d %H:%M:%S"),
            "stop": end_time.strftime("%Y-%m-%d %H:%M:%S"),
        }

        run_dicts = json.loads(urllib.request.urlopen(urllib.request.Request(
            test_url, urllib.parse.urlencode(params).encode("utf-8"))).read())

        # convert to datetime format and bundle together
        run_times = []
        for run in run_dicts:
            if (run["start"] is not None) and (run["stop"] is not None):
                run_id = int(run["run_number"])
                run_start_stamp = datetime.strptime(run["start"], "%Y-%m-%d %H:%M:%S").replace(tzinfo=timezone.utc)
                run_end_stamp = datetime.strptime(run["stop"], "%Y-%m-%d %H:%M:%S").replace(tzinfo=timezone.utc)
                run_times.append([run_id, run_start_stamp, run_end_stamp])

        return run_times

    def get_good_run_times_in_range(self, start_time, end_time):
        """
        Return a list of run ids with their good start/stop times as recorded in the i3live GRL.
        Be warned: this function should be used sparingly to not overload the I3Live server!

        :param start_time: a ``datetime`` instance corresponding to the start of the time window to request

        :param end_time: a ``datetime`` instance corresponding to the end of the time window to request

        :return: list of bundled (run id, good start-time of run, good end-time of run) as (int, datetime, datetime)
                 that took place between ``start_time`` and ``end_time``
        """

        test_url = "https://live.icecube.wisc.edu/snapshot-export/"
        params = {
            "user": "icecube",
            "pass": "skua",
            "start_date": start_time.strftime("%Y-%m-%d %H:%M:%S"),
            "end_date": end_time.strftime("%Y-%m-%d %H:%M:%S"),
        }

        run_dicts = json.loads(urllib.request.urlopen(urllib.request.Request(
            test_url, urllib.parse.urlencode(params).encode("utf-8"))).read())
        run_dicts = run_dicts["runs"]

        run_times = []
        for run in run_dicts:
            if run["good_i3"] is True and run["good_it"] is True:
                run_num = int(run["run"])
                
                good_start = run["good_tstart"]
                good_end = run["good_tstop"]
                
                # truncate to micro-seconds if need be
                if len(good_start) > 26:
                    good_start = good_start[:26]
                if len(good_end) > 26:
                    good_end = good_end[:26]
                    
                # add fractional seconds if need be
                if len(good_start) == 19:
                    good_start += '.0'
                if len(good_end) == 19:
                    good_end += '.0'
                    
                good_start = datetime.strptime(good_start, "%Y-%m-%d %H:%M:%S.%f").replace(tzinfo=timezone.utc)
                good_end = datetime.strptime(good_end, "%Y-%m-%d %H:%M:%S.%f").replace(tzinfo=timezone.utc)
                run_times.append([run_num, good_start, good_end])

        return run_times

    

class GoodRunList(dict):
    def __init__(self, data = {}, columns = ['RunNum', 'Good_i3', 'Good_it', 'LiveTime', 'ActiveStrings', 'ActiveDoms', 'ActiveInIce', 'OutDir', 'Comment(s)'], renamed_columns = ['run_id', 'good_i3', 'good_it', 'livetime', 'active_strings', 'active_doms', 'active_inice', 'outdir', 'comment'], run_id_column = 0, num_decimals = 2):
        super().__init__(data)
        self.paths = []
        self.columns = columns
        self.renamed_columns = renamed_columns
        self._run_id_column = run_id_column
        self.num_decimals = num_decimals

        if len(columns) != len(renamed_columns):
            raise RuntimeError('Column name lists need to be equal in length')

    @no_type_check
    def load(self, path):
        """
        Loads a text GRL file and adds it to the already loeaded ones. This means
        you can load as many files as you want.

        Note: This function can handle pass1 and pass2 GRls as well as IC79 GRLs. But please do not mix pass1 and pass2 ;)
        """

        self.paths.append(path)

        ic79_pass1_mode = 'IC79' in path.upper() and 'pass2' not in path.lower()

        if ic79_pass1_mode:
            tmp_columns = self.columns
            tmp_rcolumns = self.renamed_columns

            self.columns = ['RunNum', 'Good_i3', 'Good_it', 'LiveTime', 'OutDir']
            self.renamed_columns = ['run_id', 'good_i3', 'good_it', 'livetime', 'outdir']

        metadata = os.path.basename(path).split('_')

        pass2 = 'pass2' in path.lower()

        with open(path) as f:
            headers = True

            for line in f:
                # Ignore comments (= lines that start with #)
                if line.strip().startswith('#'):
                    continue

                # Ignore old-style GRL headers but only if no data has been read yet
                if headers and (line.strip().startswith('RunNum') or line.strip() == '(1=good 0=bad)'):
                    continue

                # Ignore empty lines
                if not len(line.strip()):
                    continue

                headers = False

                # Fill data
                columns = [c.strip() for c in line.split(None, len(self.columns) - 1)]

                data = {}
                for column, name in enumerate(self.renamed_columns):
                    data[name] = columns[column].strip() if column < len(columns) else None

                data['detector'] = '{0}.{1}'.format(metadata[0], metadata[1]) if not ic79_pass1_mode else 'IC79.2010'
                data['pass'] = 1 if not pass2 else 2

                self.add_run(data)

        if ic79_pass1_mode:
            self.columns = tmp_columns
            self.renamed_columns = tmp_rcolumns

    def add_run(self, data):
        """
        Adds a run to the GRL. Usually, you don't need this method. It is called by the `load` method.
        """

        run_id = int(data[self.renamed_columns[self._run_id_column]])

        if self.has_run(run_id):
            raise Exception("Run {run_id} has already been added to the GRL.".format(run_id = run_id))

        def convert_to_number(num, num_decimals):
            if isinstance(num, int):
                return num
            elif isinstance(num, float):
                return round(num, num_decimals)
            elif num is None:
                return None

            try:
                return int(num)
            except ValueError:
                try:
                    return round(float(num), num_decimals)
                except ValueError:
                    return str(num)

        self[run_id] = RunInfo({k: convert_to_number(v, self.num_decimals) for k, v in data.items()})

    def get_run(self, run_id):
        """
        Alias for `self[run_id']`.
        """

        return self[run_id]

    def has_run(self, run_id):
        """
        Alias for `run_id in self`.
        """

        return run_id in self

    def get_run_ids(self, good_i3 = None, good_it = None):
        """
        Returns a sorted list of good run ids.

        You can also specify if InIce data needs to be good or not (same for IT data). A few examples:

        Note: This method returns run ids only! ::

          grl = GoodrunList()
          grl.load(...)

          # All good runs (good_i3 or good_it)
          run_ids = grl.get_run_ids()

          # Only good runs with good InIce (excludes good IT only runs)
          run_ids = grl.get_run_ids(good_i3 = True)

          # Only good runs with good IT data (excludes good InIce only runs)
          run_ids = grl.get_run_ids(good_it = True)

          # Only runs that are only good for InIce
          run_ids = grl.get_run_ids(good_i3 = True, good_it = False)

          # Only runs that are only good for IT
          run_ids = grl.get_run_ids(good_i3 = False, good_it = True)

        """

        if good_i3 is None and good_it is None:
            return sorted(self.keys())
        elif good_i3 is None:
            return sorted([run_id for run_id, r in self.items() if bool(r['good_it']) == good_it])
        elif good_it is None:
            return sorted([run_id for run_id, r in self.items() if bool(r['good_i3']) == good_i3])
        else:
            return sorted([run_id for run_id, r in self.items() if bool(r['good_it']) == good_it and bool(r['good_i3']) == good_i3])

    def exclude_runs(self, arg):
        """
        Exclude certain runs from the GRL. You can pass a list of run ids, you can pass exactly one run id,
        or you can pass a path to a file that contains run ids separated by whitespace (space, new line, tab).
        """

        if isinstance(arg, (list, tuple)):
            for r in arg:
                if r in self:
                    del self[r]
        elif isinstance(arg, str):
            with open(arg) as f:
                runs = [int(r) for r in f.read().split()]

            self.exclude_runs(runs)
        else:
            try:
                run_id = int(arg)
                del self[run_id]
            except ValueError:
                raise ValueError('Invalid argument')

    def get_burne_sample(self):
        """
        Returns a GRL that contains burn sample runs only.
        """

        return GoodRunList({k: v for k, v in self.items() if not k % 10})


def GRL(pass2: bool = False, pass2a: bool = False, only_IC86: bool = False) -> GoodRunList:
    """Returns a complete list of all good runs since IC79 (included). If you
    enable `pass2` or `pass2a`, the seasons IC79 - IC86.2016 will be replaced by
    the pass2 GRL. After IC86.2016, the standard GRLs are added (there is no
    pass2 after IC86.2016!).


    Args:
        pass2 (bool, optional): Weather or not to use pass2 instead of the old
            level2 for seasons before 2017. Defaults to False.
        pass2a (bool, optional): Weather or not to use pass2a instead of the old
            level2 for seasons before 2017. If True, this parameter overrides
            the `pass2` parameter. Defaults to False.
        only_IC86 (bool, optional): If True, only IC86 seasons will be loaded.
            Defaults to False.

    Returns:
        GoodRunList: The `GoodRunList` object with all seasons loaded as
            specified.
    """

    # Find all GRLs >= IC79 (or IC86 if only_IC86 is true) for given pass
    ic_grid = 'IC86' if only_IC86 else 'IC??'
    pass2 = 'pass2a' if pass2a else 'pass2'  # type: ignore[assignment]

    if pass2 or pass2a:
        lists = glob(f'/data/exp/IceCube/201[0-6]/filtered/level2{pass2}/{ic_grid}_201[0-6]_GoodRunInfo.txt')
        # level2pass2a became standard in 2017, and is then and thereafter only called level2
        lists.extend(glob(f'/data/exp/IceCube/201[7-9]/filtered/level2/{ic_grid}_201[7-9]_GoodRunInfo.txt'))
        lists.extend(glob(f'/data/exp/IceCube/20[2-9][0-9]/filtered/level2/{ic_grid}_20[2-9][0-9]_GoodRunInfo.txt'))
    else: # old level 2
        lists = glob(f'/data/exp/IceCube/20[1-9][0-9]/filtered/level2/{ic_grid}_20[1-9][0-9]_GoodRunInfo.txt')
        # If it is pass1, the IC79 list sits somewhere else
        if not only_IC86:
            lists.append('/data/exp/IceCube/2010/filtered/level2/IC79_GRLists/IC79_GRL_NewFormat.txt')

    # Load files
    grl = GoodRunList()

    for l in lists:
        grl.load(l)

    return grl


def GRL_pass2():
    return GRL( pass2 = True )


def GRL_pass2a():
    return GRL( pass2a = True )
