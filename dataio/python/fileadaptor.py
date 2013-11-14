import collections

from icecube import icetray,dataio

class FrameWrapper(object):
    '''Wrap an I3Frame to override the .size() method'''
    def __init__(self, frame, prev_frame=None):
        # pre-load key sizes
        self.frame = frame
        self._prev_frame = prev_frame
        self._sizes = {}
        for k in self.keys():
            self.size(k)
    
    def size(self,key=None):
        if not key:
            return self.frame.size()
        if key not in self._sizes:
            if self.get_stop(key) != self.Stop and self._prev_frame:
                self._sizes[key] = self._prev_frame.size(key)
            else:
                self._sizes[key] = self.frame.size(key)
        return self._sizes[key]
    
    def __len__(self):
        return self.frame.__len__()
    
    def __contains__(self,key):
        return self.frame.__contains__(key)
   
    def __sizeof__(self):
        return self.frame.__sizeof__()
        
    def __repr__(self):
        return self.frame.__repr__()
        
    def __str__(self):
        return self.frame.__str__()
    
    def __getitem__(self,key):
        return self.frame.__getitem__(key)
    
    def __getattr__(self,attr):
        return getattr(self.frame,attr)


class I3BrowsableAdaptor:
    '''
    Sequence-like adaptor for I3BrowsableFile, providing len() and
    read-only bracket notation access (both integers and slices).
    '''

    def __init__(self, i3file):
        self.i3file = i3file
        self.file_length = self.i3file.size()

    def __len__(self):
        return self.file_length

    def __getitem__(self, key):
        if isinstance(key, slice):
            indices = key.indices(self.file_length)
            return [self.i3file.get_frame(k) for k in range(*indices)]
        elif isinstance(key, int):
            fsize = self.i3file.size()
            if abs(key) > fsize or key == fsize:
                raise IndexError("Index too large")
            if key < 0:
                return self.i3file.get_frame(fsize + key)
            else:
                return self.i3file.get_frame(key)
        else:
            raise IndexError("Cannot index with " + str(type(key)))


class I3SequentialAdaptor:
    '''
    Sequence-like adaptor for I3File, providing len() and
    read-only bracket notation access (integers and slices).

    Since I3File is a sequential API, providing this random
    access abstraction may result in very slow lookups, especially
    on compressed files.  This class performs some basic caching
    to limit the suffering this causes.  Users who require consistent file
    access performance should decompress their files and use
    I3BrowsableAdaptor instead.
    '''
    
    def __init__(self, i3file, cachesize=None):
        self.i3file = i3file
        self.cache = collections.deque([])
        if cachesize:
            self.cachesize = cachesize
        else:
            self.cachesize = 32
        # Index of the frame most recently read
        self.idx = -1
        # Index of the highest frame seen so far
        self.maxidx = -1
        # When the file end is reached, this will be set
        # to the maximum frame number.  While it is None,
        # we know we have not seen the end yet.
        self.file_length = None
        # actual frame object
        self.obj = None
        self._get_next_frame()

    def _put_cache(self, num, frame):
        self.cache.appendleft((num, frame))
        if len(self.cache) > self.cachesize:
            self.cache.pop()

    def _get_cache(self, num):
        for (n, f) in self.cache:
            if n == num:
                return f
        return None

    def _get_next_frame(self):
        '''Advance one frame; assumes self.i3file.more() is true'''
        self.obj = FrameWrapper(self.i3file.pop_frame(),self.obj)
        self.idx += 1
        self._put_cache(self.idx, self.obj)
        if not self.file_length:
            self.max_idx = max(self.idx, self.maxidx)

    def _get_frame(self, num):
        '''Get the specified frame, from cache or from disk'''
        if(num < 0):
            raise IndexError(num)
        cached = self._get_cache(num)
        if cached:
            return cached

        if num < self.idx:
            self.i3file.rewind()
            self.idx = -1
            self._get_next_frame()

        while num > self.idx:
            if self.i3file.more():
                self._get_next_frame()
            else:
                self.file_length = self.idx + 1
                raise IndexError("Too high: " + str(num))

        if self.obj is None:
            raise RuntimeError(num)
        return self.obj

    def __len__(self):
        '''
        Scan to the end of the file to figure out how many frames it has.

        For large files that have not yet been fully scanned, this function
        will be slow the first time it is called.
        '''
        if self.file_length:
            return self.file_length
        while self.i3file.more():
            self._get_next_frame()

        self.file_length = self.idx + 1
        return self.file_length

    def __getitem__(self, key):
        if isinstance(key, slice):
            # Slice indexing of SequentialFiles...
            # Call len() only if we already know the file's length, or if this
            # slice requests indexing from the end of the file
            if self.file_length or key.start < 0 or key.stop < 0:
                max_slen = len(self)
            else:
                # Make a conservative estimate, without scanning to end of file.
                # What comes back from key.indices() may include invalid indices,
                # so we must catch potential IndexErrors below
                max_slen = max(key.start, key.stop, self.maxidx)
            indices = list(range(*key.indices(max_slen)))
            ret = [None for x in indices]
            # build the return list in order by ascending indices
            # this ensures we will rewind the file at most one time
            for x in sorted(indices):
                # Determine where this frame should go in the return list
                idx = indices.index(x)
                try:
                    ret[idx] = self._get_frame(x)
                except IndexError:
                    pass
            # Any Nones remaining in the return list were invalid indices
            ret = [x for x in ret if x is not None]
            return ret
        elif isinstance(key, int):
            if key < 0:
                return self._get_frame(len(self) + key)
            else:
                return self._get_frame(key)
        else:
            raise IndexError("Cannot index with " + str(type(key)))


def I3FileAdaptor(i3file, **kw):
    '''
    Return a sequence-like adaptor to an I3File.  The returned
    object will support the len() operation, the iterator protocol,
    and bracket access.

    The argument may be an I3File or I3BrowsableFile object, or a
    path to a readable file on disk.  File paths that appear to be
    uncompressed i3 files will be opened as I3BrowsableFile.

    Clients should expect compressed files to be slow if accessed
    in anything other than a sequential manner.

    Keyword args:
        sequential_cache_size: Cache size to pass to I3SequentialAdaptor,
                               if that adaptor is used
    '''
    f = i3file
    if isinstance(i3file, str):
        if i3file.endswith('.i3'):
            f = dataio.I3BrowsableFile()
            f.open_file(i3file)
        else:
            f = dataio.I3File(i3file)

    if isinstance(f, dataio.I3File):
        return I3SequentialAdaptor(f, kw.get('sequential_cache_size', None))
    else:
        return I3BrowsableAdaptor(f)
