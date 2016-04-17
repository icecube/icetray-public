#!/usr/bin/env python

import sys
import string
import optparse
from functools import partial

from icecube import icetray
from icecube import dataio
from icecube import dataclasses

try:
    import urwid
except ImportError:
    print("Cannot run without the urwid python module")
    sys.exit(1)
try:
    from distutils.version import LooseVersion
except ImportError:
    pass
else:
    if LooseVersion(urwid.__version__) < LooseVersion('1.1'):
        print("Your urwid python module is older than 1.1, and is not supported")
        sys.exit(1)

try:
    import IPython
except ImportError:
    print("IPython is not available; vanilla python will be used for user interaction")

term_colors = 16
try:
    import curses
except ImportError:
    print("curses is not available; assuming 16 color terminal")
else:
    curses.setupterm()
    term_colors = curses.tigetnum('colors')

from icecube.dataio import pprint as i3pprint
from icecube.dataio import fileadaptor

class UI:
    '''
    Interactive settings, primarily screen colors and key bindings.
    These could be customized, e.g. by a user-specific .ini file,
    in a future version of the script.
    '''

    keyhelp = [ ('left', 'Go back one frame'),
                ('right', 'Go forward one frame'),
                ('up', 'Select previous frame object'),
                ('down', 'Select next frame object'),
                ('home', 'Go to first frame'),
                ('end', 'Go to last frame'),
                ('select', 'Inspect the selected frame object'),
                ('xml', 'Inspect the selected frame object (XML output)'),
                ('search', 'Search the inspected text'),
                ('search_reverse', 'Search the inspected text in reverse direction'),
                ('search_next', 'Find the next item while searching'),
                ('escape', 'Exit help screen or dialog box, or quit program'),
                ('interact', 'Open an IPython session with the current frame in scope with name "frame"'),
                ('libs', 'Import an additional library'),
                ('goto', 'Go to frame by number'),
                ('gotoevent', 'Go to event by id'),
                ('writeframe', 'Write current frame to file'),
                ('help', 'This help screen'),
              ]

    keys = {'select': ('return','enter','o','O'),
            'xml': ('x','X'),
            #'save_xml': ('s','S'),
            'escape': ('esc','q','Q'),
            'left': ('left',),
            'right': ('right',),
            'up': ('up',),
            'down': ('down',),
            'home': ('home',),
            'end': ('end',),
            'interact': ('i', 'I'),
            'search': ('/',),
            'search_reverse': ('?',),
            'search_next': ('n',),
            'search_next_reverse': ('N',),
            'libs': ('L',),
            'goto':('g','G'),
            'gotoevent':('e','E'),
            'writeframe':('w','W'),
            'help':('h','H'),
           }

    fg = 'light gray'
    bg = 'black'
    fg8 = 'g93'
    bg8 = 'g11'

    tape_colors = {
                   'i3frame': (fg, fg8),
                   'i3frame_G': ('dark cyan', '#6dd'),
                   'i3frame_C': ('dark cyan', '#6dd'),
                   'i3frame_D': ('dark cyan', '#6dd'),
                   'i3frame_I': ('dark magenta', '#f86'),
                   'i3frame_P': ('dark blue', '#66f'),
                   'i3frame_NullSplit': ('dark red', '#d66'),
                   'i3frame_IceTopSplit': ('yellow', '#ff6'),
                   'i3frame_InIceSplit': ('light green', '#6f6'),
                  }

    # urwid palette format is a 6-tuple, with the last 3 items optional:
    # (name, 16col fg, 16col bg, monochrome property, 256col fg, 256col bg)
    palette = [ ('default',     fg,             bg, None       , fg8,    bg8 ),
                ('hilite',      fg+',standout', bg, 'standout' , bg8,    fg8 ),
                ('titletext',   'light blue',   bg, None       , '#aaf', bg8 ),
                ('fobj',        fg,             bg, None       , fg8,    bg8 ),
                ('fobj_other',  'yellow',       bg, None       , '#dd6', bg8 ),
                ('fobj_focus',  fg+',standout', bg, 'standout' , bg8,    fg8 ),
                ('list_header', 'light red',    bg, None       , '#f66', bg8 ),
              ]
    for c in tape_colors:
        c1,c2 = tape_colors[c]
        palette.append((c,c1,bg,None,c2,bg8))
        palette.append((c+'_hilite',c1+',standout',bg,'standout',bg8,c2))


class EllipsesText(urwid.Text):
    def __init__(self,text,**kw):
        if 'wrap' not in kw:
            kw['wrap'] = 'clip'
        super(EllipsesText,self).__init__(text,**kw)
    def render(self, size, focus):
        (maxcol,) = size
        text, attr = self.get_text()
        if len(text) > maxcol:
            text = text[:maxcol-3]+'...'
        trans = self.get_line_translation( maxcol, (text,attr) )
        return urwid.canvas.apply_text_layout(text, attr, trans, maxcol)
        #super(EllipsesText, self).render(size, focus)

class FrameObjectItem(urwid.Columns):
    '''A Columns instance that claims focus for itself instead of
       passing it on to its contents.  Emits a select_object signal
       when a selection key is pressed.'''

    signals = ['select_object', 'xml_object']

    def __init__( self, widget_list, i3frame, i3key, *args, **kw ):
        urwid.Columns.__init__( self, widget_list, *args, **kw )
        self.i3frame = i3frame
        self.i3key = i3key

    def keypress( self, size, key ):
        if key in UI.keys['select']:
            urwid.emit_signal( self, 'select_object', self.i3frame, self.i3key )
        if key in UI.keys['xml']:
            urwid.emit_signal( self, 'xml_object', self.i3frame, self.i3key )
        else:
            return key

    def get_focus( self ):
        return self

    def selectable( self ):
        return True

class Searcher():
    def __init__(self, listbox, line=-1, direction='forward'):
        if direction == 'forward':
            self.direction = 1
        elif direction == 'backward':
            self.direction = -1
        else:
            raise Exception('bad direction')
        self.line_number = line
        self.listbox = listbox
        self.contents = listbox.message
        self.text = ''
        self.writing = True

    def reverse(self):
        if self.direction == -1:
            self.direction = 1
        else:
            self.direction = -1

    def status(self):
        if self.direction == -1:
            return '?'+self.text
        else:
            return '/'+self.text

    def highlight(self,line):
        min_line = 0 if line < 100 else line-100
        max_line = line+100 if line+100 < len(self.contents) else len(self.contents)
        try:
            for i in range(min_line,max_line):
                highlight = []
                if not self.text:
                    highlight.extend([self.contents[i],None])
                else:
                    for p in self.contents[i].split(self.text):
                        highlight.append(p)
                        highlight.append(('hilite',self.text))
                self.listbox.body[i].set_text(highlight[:-1])
        except Exception:
            pass

    def find(self):
        self.writing = False
        line = self.line_number+self.direction
        found = False
        while not found and line < len(self.contents):
            found = self.text in self.contents[line]
            if not found:
                line += self.direction
        if not found:
            raise IndexError()
        # update line
        self.highlight(line)
        if line != self.line_number:
            self.line_number = line
            self.listbox.set_focus(self.line_number)
            self.listbox.set_focus_valign('top')

class ListBoxReporter(urwid.ListBox):
    signals = ['update_status']

    def __init__(self,contents,message=None,usefocusposition=False):
        super(ListBoxReporter,self).__init__(contents)
        self.message = message
        self.search = None
        self.usefocusposition=usefocusposition

    def get_position(self,size):
        if self.usefocusposition:
            return self.focus_position
        else:
            ret = super(ListBoxReporter,self).calculate_visible(size, self.focus is not None)[1]
            return self.focus_position-(len(ret[1]) if ret else 0)

    def keypress( self, size, key ):
        if (not self.search or not self.search.writing) and key in UI.keys['search']:
            self.search = Searcher(self,line=self.get_position(size),direction='forward')
            urwid.emit_signal(self,'update_status','/')
            return
        elif (not self.search or not self.search.writing) and key in UI.keys['search_reverse']:
            self.search = Searcher(self,line=self.get_position(size),direction='backward')
            urwid.emit_signal(self,'update_status','?')
            return
        elif self.search:
            if (((not self.search.writing) and key in UI.keys['search_next'])
                or (self.search.writing and key == 'enter')):
                if not self.search.text:
                    self.search.highlight(self.search.line_number)
                    self.search = None
                    status = ':'
                else:
                    try:
                        self.search.find()
                        status = ':'
                    except Exception:
                        self.search = None
                        status = 'Pattern not found'
            elif (not self.search.writing) and key in UI.keys['search_next_reverse']:
                try:
                    self.search.reverse()
                    self.search.find()
                    status = ':'
                except Exception:
                    self.search = None
                    status = 'Pattern not found'
            elif self.search.writing and key == 'backspace':
                if not self.search.text:
                    self.search = None
                    status = ':'
                else:
                    self.search.text = self.search.text[:-1]
                    status = self.search.status()
            elif self.search.writing and key in string.printable:
                self.search.text += key
                status = self.search.status()
            else:
                self.search = None
                return super(ListBoxReporter,self).keypress(size,key);
            urwid.emit_signal(self,'update_status',status)
            return None
        return super(ListBoxReporter,self).keypress(size,key);

    def calculate_visible(self, size, focus=False):
        ret = super(ListBoxReporter,self).calculate_visible(size, focus)
        if self.search and not self.search.writing:
            line = self.focus_position-(len(ret[1][1]) if ret[1] else 0)
            self.search.highlight(line)
        try:
            middle,top,bottom = ret
            if middle is None:
                self.visible_pos = (0,0,0)
            else:
                row_offset,w,pos,rows,c = middle
                trim_top, above = top
                trim_bottom, below = bottom

                total_len = len(self.body)

                n_top_hidden = pos-len(above)
                n_bottom_hidden = total_len-pos-len(below)-1

                if not self.search:
                    status = 'scroll: {pos:>5}  '
                    format_args = {'pos':pos}
                    if n_top_hidden == 0 and n_bottom_hidden == 0:
                        status = ''
                        format_args = {}
                        #status += 'All'
                    elif n_top_hidden == 0:
                        status += 'Top'
                    elif n_bottom_hidden == 0:
                        status += 'Bot'
                    else:
                        status += '{percent:2d}%'
                        format_args['percent'] = int(pos*100/total_len)
                    urwid.emit_signal(self,'update_status',status.format(**format_args))
        except Exception as e:
            print('%r'%e)
            pass
        finally:
            return ret

class FrameViewer(urwid.Frame):
    ''' Viewer for a single frame in the i3 file.  

    Note that an urwid.Frame has no particular connection to an icetray frame.
    Avoid using generic 'frame' as a variable name for this reason.'''
    
    signals = ['select_key','goto_matching_frame','update_status']

    def __init__(self, i3frame, frameidx, ascii_only=False, key=0):
        self.i3frame = i3frame
        self.key = key

        header = urwid.Pile([ urwid.AttrMap( self.mkcols( 'Key:', 'Type:', 'Contains:', 'Bytes:', plain_columns = True ), 'list_header') ])
        contents = urwid.SimpleListWalker(self.mkcontents())
        urwid.connect_signal(contents, 'modified', self.modified)
        message = []
        for x in contents:
            cols = []
            for y in x.original_widget.contents:
                cols.append(y[0].text)
            message.append(' '.join(cols))
        self.listbox = ListBoxReporter(contents,message=message,usefocusposition=True)
        if self.key >= 0 and self.key < len(contents):
            self.listbox.set_focus(self.key)
        urwid.connect_signal(self.listbox, 'update_status', self.update_status)
        self.linebox_kw = {}
        if ascii_only:
            # make the linebox use ascii chars
            self.linebox_kw = {
                'tlcorner': '-',
                'tline': '-',
                'lline': '|',
                'trcorner': '-',
                'blcorner': '-',
                'rline': '|',
                'bline': '-',
                'brcorner': '-',
            }
        self.inner_frame = urwid.Frame(self.listbox,header=header)
        linebox = urwid.LineBox(self.inner_frame, self.framename(frameidx), **self.linebox_kw)
        urwid.Frame.__init__(self, linebox)
        self.overlay = None

    def framename(self,frameidx):
        return "Frame {0}: {1}".format( frameidx, self.i3frame.Stop )

    def mkcols(self, key, typ, content, bytes, plain_columns = False ):
        cols = [ ( 'weight', 30, EllipsesText(' '+key) ),
                 ( 'weight', 32, EllipsesText(' '+typ) ),
                 ( 'weight', 40, EllipsesText(' '+content)),
                 ( 'weight', 9, urwid.Text(str(bytes)+' ', align='right')) ]
        if plain_columns:
            return urwid.Columns( cols )
        else:
            return FrameObjectItem( cols, self.i3frame, key )
    
    def mkcontents(self):
        #contents = [ urwid.AttrMap( self.mkcols( 'Key:', 'Type:', 'Contains:', 'Bytes:', plain_columns = True ), 'list_header') ]
        contents = []
        for key in sorted(self.i3frame.keys()):
            wid = self.mkcols(key, self.i3frame.type_name(key),
                              i3pprint.format_line(self.i3frame,key),
                              i3pprint.format_size(self.i3frame,key))
            if 'select_object' in wid.signals:
                urwid.connect_signal(wid, 'select_object', self.open_detail)
            if 'xml_object' in wid.signals:
                urwid.connect_signal(wid, 'xml_object', partial(self.open_detail,xml=True))
            contents.append( wid )

        contents_map = []
        for c in contents:
            if hasattr(c,'i3key') and self.i3frame.get_stop(c.i3key) == self.i3frame.Stop:
                attr = 'fobj'
            else:
                attr = 'fobj_other'
            contents_map.append( urwid.AttrMap(c,attr, 'fobj_focus'))
        return contents_map
    
    def set_view(self, i3frame, frameidx, key=None):
        self.i3frame = i3frame
        if key:
            self.key = key

        del self.listbox.body[:]
        contents = self.mkcontents()
        self.listbox.body.extend(contents)
        message = []
        for x in contents:
            cols = []
            for y in x.original_widget.contents:
                cols.append(y[0].text)
            message.append(' '.join(cols))
        self.listbox.message = message
        if self.key >= 0 and self.key < len(self.listbox.body):
            self.listbox.set_focus(self.key)

        if self.overlay:
            self.body = self.overlay.bottom_w
        self.body.set_title(self.framename(frameidx))
        if self.overlay:
            i3key, xml = self.overlay.args
            self.open_detail( i3frame, i3key, xml )

    def modified(self):
        try:
            pos = self.listbox.focus_position
        except IndexError:
            pass
        else:
            urwid.emit_signal(self,'select_key',pos)
    
    def update_status(self,status):
        urwid.emit_signal(self,'update_status',status)        

    def open_detail(self, i3frame, i3key, xml=False):
        if xml:
            message = i3pprint.format_xml( i3frame, i3key )
        else:
            message = i3pprint.format_detail( i3frame, i3key )
        listbox = ListBoxReporter( urwid.SimpleListWalker([urwid.Text(m) for m in message.split('\n')]),
                                   message=message.split('\n') )
        urwid.connect_signal(listbox, 'update_status', self.update_status)
        msgbox = urwid.LineBox( listbox,
                                'Contents of key {0}'.format( i3key ) , **self.linebox_kw)
        self.overlay = urwid.Overlay( msgbox, self.body, 
                                      'center', ('relative', 85), 
                                      'middle', ('relative', 85) )
        self.overlay.args = (i3key, xml)
        self.body = self.overlay

    def keypress( self, size, key ):
        key = urwid.Frame.keypress(self, size, key)
        if self.overlay and key not in UI.keys['help']:
            if key in UI.keys['escape']:
                urwid.disconnect_signal(self.overlay.top_w.original_widget, 'update_status', self.update_status)
                self.body = self.overlay.bottom_w
                self.overlay = None
            if key in UI.keys['left']:
                urwid.emit_signal( self, 'goto_matching_frame', -1 )
            elif key in UI.keys['right']:
                urwid.emit_signal( self, 'goto_matching_frame', 1 )
            return None
        else:
            return key
            
class Footer(urwid.Columns):
    '''Footer at the bottom of the window'''
    
    signals = ['select_frame','drop_focus']
    
    def __init__(self,frames):
        self.width = -1
        self.idx = 0
        self.key = 0
        self.info_left = EventInfo(frames)
        self.cmd = EllipsesText('')
        self.info_right = EventTime(frames)
        self.tape = TapeFooter(frames)
        left_col = urwid.Pile([self.info_left,self.cmd])
        right_col = urwid.Pile([self.info_right,self.tape])
        urwid.Columns.__init__(self,[left_col,right_col])
        urwid.connect_signal( self.tape, 'select_frame', self.select_frame )
        #urwid.connect_signal( self.cmd, 'select_frame', self.select_frame )
        #urwid.connect_signal( self.cmd, 'drop_focus', self.drop_focus )
    
    def drop_focus(self):
        urwid.emit_signal( self, 'drop_focus' )
    
    def select_frame(self, frame):
        urwid.emit_signal( self, 'select_frame', frame )

    def set_view(self, idx, key=0):
        '''Set the view to a selection of frame idx'''
        keychange = False
        framechange = False
        if self.key != key and key >= 0:
            keychange = True
            self.key = key
        if self.idx != idx and idx >= 0:
            framechange = True
            self.idx = idx
        if keychange or framechange:
            self.info_left.set_view(idx,key)
        if framechange:
            self.info_right.set_view(idx)
            self.tape.set_view(idx)
            #self.cmd.set_view(idx)
    
    def update_status(self,txt):
        self.cmd.set_text(txt)

class Cmd(urwid.Edit):
    '''A basic command line at the bottom of the window.
    Currently used to display status information
    '''
    
    signals = ['select_frame','drop_focus']
    
    def __init__(self):
        self.frame = 0
        urwid.Edit.__init__(self,caption=':',wrap='clip')
    
    def set_view(self,idx):
        self.frame = idx
    
    def keypress(self, size, input):
        # only pass 'esc' through
        #print 'hit keypress',input
        if input == 'esc':
            self.set_edit_text('')
            urwid.emit_signal( self, 'drop_focus' )
            return
        elif input in ('return','enter'):
            text = self.edit_text
            # TODO: do something smart with the text
            urwid.emit_signal( self, 'select_frame', self.frame )
            self.set_edit_text('')
            return
        return super(Cmd, self).keypress(size,input)
           

class EventInfo(urwid.Columns):
    '''Event info shown at the bottom of the window'''

    class Col(urwid.Pile):
        def __init__(self):
            self.width = -1
            urwid.Pile.__init__(self,[EllipsesText(''),
                                      EllipsesText(''), 
                                      EllipsesText('')])

    def __init__(self, frames):
        self.frames = frames
        self.width = -1
        urwid.Columns.__init__(self,[('weight', 10, EventInfo.Col()),
                                     ('weight', 20, EventInfo.Col())])
        self.set_view(0)
    
    def set_view(self, idx, key=0):
        
        key_str = ''
        frame = ''
        stop = ''
        run = ''
        subrun = ''
        event = ''
        subevent = ''
        duration = ''
        try:
            f = self.frames[idx]
        except IndexError:
             pass
        else:
            key_str = '{0:d}/{1:d}'.format(key+1,len(f))
            if self.frames.file_length:
                frame = str(idx+1)+'/'+str(self.frames.file_length)
            else:
                frame = str(idx+1)+'/unk'
            stop = str(f.Stop)
            if 'I3EventHeader' in f:
                run = str(f['I3EventHeader'].run_id)
                subrun = str(f['I3EventHeader'].sub_run_id)
                event = str(f['I3EventHeader'].event_id)
                subevent = str(f['I3EventHeader'].sub_event_stream)
                if not subevent:
                    subevent = '(n/a)'
                duration = '{0:.0f} ns'.format(f['I3EventHeader'].end_time-f['I3EventHeader'].start_time)
        
        self.contents[0][0].contents[0][0].set_text([('list_header',' Key: '),('fobj_other',key_str)])
        self.contents[0][0].contents[1][0].set_text([('list_header',' Frame: '),('fobj_other',frame)])
        self.contents[0][0].contents[2][0].set_text([('list_header',' Stop: '),('fobj_other',stop)])
        self.contents[1][0].contents[0][0].set_text([('list_header','Run/Subrun: '),('fobj_other',run+'/'+subrun)])
        self.contents[1][0].contents[1][0].set_text([('list_header','Event/SubEvent: '),('fobj_other',event+'/'+subevent)])
        self.contents[1][0].contents[2][0].set_text([('list_header','Duration: '),('fobj_other',duration)])
        
    
class EventTime(EllipsesText):
    '''Event info shown at the bottom of the window'''

    def __init__(self, frames):
        self.frames = frames
        self.width = -1
        super(EventTime,self).__init__('')
        self.set_view(0)
    
    def set_view(self, idx):

        start_time = ''        
        try:
            f = self.frames[idx]
        except IndexError:
             pass
        else:
            if 'I3EventHeader' in f:
                start_time = str(f['I3EventHeader'].start_time.date_time)+' UTC '
        
        self.set_text([('list_header','Start Time: '),('fobj_other',start_time)])
        

class TapeFooter(urwid.Pile):
    '''Mouse-selectable tape shown at bottom of the window'''

    signals = ['select_frame']

    def __init__(self, framelist):
        self.frames = framelist
        self.idx = 0
        self.size = (0,)
        self.width = -1
        self.stopids = []
        urwid.Pile.__init__(self,[urwid.Text('', wrap='clip'), 
                                  urwid.Text('', wrap='clip'), 
                                  urwid.Text('', wrap='clip')])

    def get_stop_ids(self, start, stop, cur_frame=None):
        '''
        Equivalent to [f.Stop.id for f in frames[start:stop]].
        Results for previously visited frames are cached in self.stopids.
        If an entry in self.stopids is None, that frame has never been
        on the screen.
        '''
        slc = slice(start, stop)
        # pad new indices with Nones
        if stop > len(self.stopids):
            self.stopids.extend([None] * (stop-len(self.stopids)))
        # replace every visible None with a valid id
        for i, s in enumerate(self.stopids[slc],start):
            if self.stopids[i] is None:
                try:
                    f = self.frames[i]
                except IndexError:
                    # stop is larger than len(frames)
                    del self.stopids[i:]
                    break
                else:
                    color = 'i3frame'
                    stop = f.Stop.id
                    if 'i3frame_'+stop in UI.tape_colors:
                        color = 'i3frame_'+stop
                    if 'I3EventHeader' in f:
                        subevent = 'i3frame_'+str(f['I3EventHeader'].sub_event_stream)
                        if subevent in UI.tape_colors:
                            color = subevent
                    self.stopids[i] = (i,color,stop)

        cur_stops = []
        for i,color,stop in self.stopids[slc]:
            if i == cur_frame:
                color += '_hilite'
            cur_stops.append((color,stop))
        return cur_stops

    def set_view(self, idx):
        '''Set the view to a selection of frame idx with a column width of size[0]'''
        if( self.idx == idx and self.width == self.size ):
            # No visible change
            return

        self.idx = idx
        self.width = self.size[0]
        leftwidth = (self.width-1)//2
        rightwidth = leftwidth + (self.width-1)%2
        min_frame = max(0, self.idx-leftwidth)
        max_frame = self.idx + rightwidth

        front_spacing_text = ' ' * (leftwidth - self.idx)

        # Bottom row: frame stop IDs, with the central, selected item hilited
        framestops = self.get_stop_ids(min_frame,max_frame,cur_frame=idx)
        frame_text = front_spacing_text + ''.join([x for c,x in framestops])
        left_frame_text = frame_text[:leftwidth]
        right_frame_text = frame_text[leftwidth + 1:]
        
        self.widget_list[2].set_text([front_spacing_text,framestops])

        # Middle row: alternating tape spacers every ten frames
        spacer = '|' + ' '*9 + '.' + ' '*9
        frontidx = max(0, self.idx-leftwidth) % len(spacer)
        effective_width = 1 + (len(left_frame_text)-len(front_spacing_text)) + len(right_frame_text)
        spacertext = spacer[frontidx:] + spacer * int(effective_width/len(spacer) + 1)
        spacertext = spacertext[:effective_width]
        self.widget_list[1].set_text(front_spacing_text + spacertext)

        # Top row: frame numbers
        tick_width = len(spacer)
        tick_count = int(effective_width / tick_width + 1)
        tick_min = (min_frame // tick_width) * tick_width
        tick_max = tick_min + tick_count * tick_width
        ticks = ['{0:<{width}}'.format(x, width=tick_width) for x in range(tick_min,tick_max, tick_width)]
        tick_text = ''.join(ticks)[min_frame%tick_width:]
        self.widget_list[0].set_text(front_spacing_text + tick_text)

    def mouse_event(self, size, event, button, x, y, focus):
        if 'press' in event:
            min_x = self.idx - (size[0]-1)//2
            self.size = size
            urwid.emit_signal( self, 'select_frame', min_x + x )

    def render(self, size, focus):
        self.size = size
        self.set_view(self.idx)
        return urwid.Pile.render(self, size, focus)


class Popup(urwid.Edit):
    
    signals = ['close_popup','select_frame','select_event','write_frame']
    
    def __init__(self, *args, **kwargs):
        urwid.Edit.__init__(self, *args, **kwargs)
        # change key bindings so we can write all letters
        self._keys = UI.keys.copy()
        UI.keys = {'select': ('return','enter'),
            'escape': ('esc',),
            'left': ('left',),
            'right': ('right',),
            'up': ('up',),
            'down': ('down',),
            'help':('?',),
           }
        for k in self._keys:
            if k not in UI.keys:
                UI.keys[k] = tuple()
        urwid.connect_signal( self, 'close_popup', self.destroy )
    
    def destroy(self):
        UI.keys = self._keys
        urwid.disconnect_signal( self, 'close_popup', self.destroy )
        urwid.emit_signal(self, 'close_popup')
    
    def closing_action(self):
        pass
    
    def keypress(self, size, key):
        if key in UI.keys['help']:
            return key
        key = urwid.Edit.keypress(self,size,key)
        
        if key in UI.keys['escape']:
            urwid.emit_signal(self, 'close_popup')
            return None
        elif key in UI.keys['select']:
            self.closing_action()
            urwid.emit_signal(self, 'close_popup')
            return None
        
        return key

class GotoInput(Popup):

    def __init__(self):
        Popup.__init__(self, 'Go to frame number:')

    def valid_char(self, ch):
        return ch in '0123456789'

    def keypress(self, size, key):
        key = Popup.keypress(self, size, key)
        if key in UI.keys['escape']:
            urwid.emit_signal(self, 'close_popup')
        return None
    
    def closing_action(self):
        try:
            f = int(self.get_edit_text())-1
            if f < 0:
                f = 0
        except ValueError:
            pass
        else:
            urwid.emit_signal(self, 'select_frame', f )
        Popup.closing_action(self)

class GotoEventInput(GotoInput):

    def __init__(self):
        Popup.__init__(self, 'Go to event id:')

    def closing_action(self):
        try:
            f = int(self.get_edit_text())
        except ValueError:
            pass
        else:
            urwid.emit_signal(self, 'select_event', f )
        Popup.closing_action(self)

class WriteFrameInput(Popup):

    def __init__(self):
        Popup.__init__(self, 'Write frame to file:')

    def closing_action(self):
        filename = self.get_edit_text()
        urwid.emit_signal(self, 'write_frame', filename)
        Popup.closing_action(self)

class LibInput(Popup):

    def __init__(self):
        Popup.__init__(self, 'Import a library:')

    def closing_action(self):
        try:
            selection = self.get_edit_text()
            try:
                for lib in selection.split(','):
                    __import__( 'icecube.'+lib )
            except ImportError as e:
                pass
#            else:
#                urwid.emit_signal(self, 'select_frame', None )
        except ValueError:
            pass
        Popup.closing_action(self)
    
class HelpScreen(urwid.Frame):

    def __init__(self, mainviewer, mainloop):
        self.mainviewer = mainviewer
        self.mainloop = mainloop
        helptexts =  ['Current key bindings:',
                      '']
        helptexts += [ '    {0}: {1}'.format(UI.keys[key],msg)
                      for key, msg in UI.keyhelp ]
        helptexts += ['',
                      'Frames and frame objects may also be selected with the mouse.']

        urwid.Frame.__init__(self, urwid.ListBox([urwid.Text(h) for h in helptexts]))

    def keypress(self, size, input):
        input = urwid.Frame.keypress(self, size, input)
        if input in UI.keys['escape']:
            self.mainloop.widget = self.mainviewer
            return None
        else:
            return input

class ViewerMain(urwid.Frame):

    def __init__(self, framelist, header, footer, ascii_only=False):
        self.framelist = framelist
        self.active_view = 0
        self.frame_hierarchy = icetray.I3FrameMixer(True)
        try:
            if isinstance(self.framelist[self.active_view],fileadaptor.FrameWrapper):
                frame_obj = self.framelist[self.active_view].frame
            else:
                frame_obj = self.framelist[self.active_view]
            f = icetray.I3Frame(frame_obj)
            f.purge()
            self.frame_hierarchy.mix(f)
        except Exception:
            raise Exception('No frames in file!')
        self.key = 0
        self.maxkeylen = len(self.framelist[self.active_view])
        urwid.connect_signal( footer, 'select_frame', self.select_frame )
        urwid.connect_signal( footer, 'drop_focus', self.drop_focus )
        self.frame_viewer = FrameViewer(framelist[0], 0, ascii_only=ascii_only)
        urwid.Frame.__init__(self, self.frame_viewer, header=header, footer = footer )
        urwid.connect_signal( self.frame_viewer, 'select_key', self.select_key )
        urwid.connect_signal( self.frame_viewer, 'update_status', footer.update_status )
        urwid.connect_signal( self.frame_viewer, 'goto_matching_frame', self.goto_matching_frame )
        self.popup = None
        self._ipy_namespace = dict()

    def set_mainloop(self, loop):
        self.mainloop = loop
        # Passing self as the first argument to HelpScreen will cause background colors to
        # be incorrect after the help screen is closed.  This is because the help will
        # reset self as the primary widget, rather than the AttrWrap that self exists in.
        # Identifying self as self.mainloop.widget works around this, but a more elegant
        # widget hierarchy is probably needed.
        self.helpscreen = HelpScreen(self.mainloop.widget, self.mainloop)

    def drop_focus(self):
        self.focus_position = 'body'

    def goto_matching_frame(self, delta):
        frame = self.active_view
        current_stop = self.framelist[frame].Stop
        while True:
            frame += 1 if delta > 0 else -1
            try:
                f = self.framelist[frame]
                if f.Stop == current_stop:
                    self.select_frame( frame )
                    return
            except IndexError:
                return

    def select_frame(self, frame):
        '''
        Attempt to access the frame'th I3Frame in the file.  If 'frame' is
        negative, access in reverse order. If 'frame' is invalid, do nothing.
        '''
        if frame is None or frame < 0 or frame == self.active_view: 
            return
        if frame == 'end':
            frame = -1

        # rather than checking against the maximum length of the list,
        # which is slow for sequential files, just try to access the frame
        try:
            self.framelist[frame]
        except IndexError:
            return

        if frame < 0:
            # reverse direction
            frame = len(self.framelist)+frame

        # iterate through the framelist to determine the frame hierarchy
        if frame < self.active_view:
            self.active_view = 0
            self.frame_hierarchy.reset()
            if isinstance(self.framelist[self.active_view],fileadaptor.FrameWrapper):
                frame_obj = self.framelist[self.active_view].frame
            else:
                frame_obj = self.framelist[self.active_view]
            f = icetray.I3Frame(frame_obj)
            f.purge()
            self.frame_hierarchy.mix(f)
        while (self.active_view < frame):
            self.active_view += 1
            if isinstance(self.framelist[self.active_view],fileadaptor.FrameWrapper):
                frame_obj = self.framelist[self.active_view].frame
            else:
                frame_obj = self.framelist[self.active_view]
            f = icetray.I3Frame(frame_obj)
            f.purge()
            self.frame_hierarchy.mix(f)

        old_widget = self.body
        self.maxkeylen = len(self.framelist[self.active_view])
        if self.key >= self.maxkeylen:
            self.key = self.maxkeylen-1
        self.frame_viewer.set_view( self.framelist[self.active_view], self.active_view, key=self.key )
        self.footer.set_view( self.active_view, key=self.key )
        del old_widget
        self.drop_focus()
    
    def select_event(self, event ):
        '''
        Attempt to access an event number in the file.  If 'event' is
        negative or otherwise invalid, do nothing
        '''
        def get_id(frame):
            if 'I3EventHeader' in frame:
                return frame['I3EventHeader'].event_id
            return -1
        if event is None or event < 0 or get_id(self.framelist[self.active_view]) == event:
            return
        
        # start a search through the framelist for the event id
        frame = self.active_view
        new_frame = -1
        while True:
            frame += 1
            if frame == self.active_view:
                return # we looped, and didn't find anything
            try:
                if get_id(self.framelist[frame]) == event:
                    new_frame = frame
                    break
            except IndexError:
                frame = -1

        if new_frame != -1:
            self.select_frame(new_frame)

    def write_frame(self, filename):
        try:
            f = dataio.I3File(filename,'w')
            if isinstance(self.framelist[self.active_view],fileadaptor.FrameWrapper):
                frame_obj = self.framelist[self.active_view].frame
            else:
                frame_obj = self.framelist[self.active_view]
            for fr in self.frame_hierarchy.get_mixed_frames(frame_obj.Stop):
                f.push(fr)
            f.push(frame_obj)
            f.close()
        except Exception:
            raise
    
    def select_key(self, key):
        '''Attempt to select a new key'''
        if key >= 0 and key < self.maxkeylen:
            self.key = key
            self.footer.set_view( self.active_view, key=key )
    
    def open_popup(self, obj):
        self.popup = obj
        urwid.connect_signal( self.popup, 'close_popup', self.close_popup )
        urwid.connect_signal( self.popup, 'select_frame', self.select_frame )
        urwid.connect_signal( self.popup, 'select_event', self.select_event )
        urwid.connect_signal( self.popup, 'write_frame', self.write_frame )
        popupbox = urwid.Filler(urwid.LineBox(obj))
        self.body = urwid.Overlay(popupbox, self.body, ('fixed left',5), 30, ('fixed top',5), 5)

    def close_popup(self):
        if self.popup:
            self.body = self.body.bottom_w
            urwid.disconnect_signal( self.popup, 'close_popup', self.close_popup )
            urwid.disconnect_signal( self.popup, 'select_frame', self.select_frame )
            urwid.disconnect_signal( self.popup, 'select_event', self.select_event )
            urwid.disconnect_signal( self.popup, 'write_frame', self.write_frame )
            self.popup = None

    def keypress(self, size, input):
        input = urwid.Frame.keypress(self,size,input)
        if input is None:
            return
        
        if input in UI.keys['left']:
            self.select_frame( self.active_view-1)
        elif input in UI.keys['right']:
            self.select_frame( self.active_view+1 )
        elif input in UI.keys['home']:
            self.select_frame(0)
        elif input in UI.keys['end']:
            self.select_frame('end')
        elif input in UI.keys['up']:
            if self.key > 0:
                self.key -= 1
                self.footer.set_view( self.active_view, self.key )
                self.body.keypress(size, input)
        elif input in UI.keys['down']:
            if self.key < self.maxkeylen-1:
                self.key += 1
                self.footer.set_view( self.active_view, self.key )
                self.body.keypress(size, input)
        elif input in UI.keys['interact']:
            self.mainloop.screen.stop()
            # put frame and [readable] contents into scope for IPython interaction
            if isinstance(self.framelist[self.active_view],fileadaptor.FrameWrapper):
                frame = self.framelist[self.active_view].frame
            else:
                frame = self.framelist[self.active_view]
            ns = dict()
            for k in frame.keys():
                try:
                    ns[k] = frame[k]
                except RuntimeError:
                    pass
            ns['frame'] = frame
            self._ipy_namespace.update(ns)
            try:
                IPython.embed(user_ns=self._ipy_namespace, display_banner=False)
            except NameError:
                import code
                console = code.InteractiveConsole(self._ipy_namespace)
                console.interact()
            # clean up inserted variables (but leave user-defined ones)
            for k in ns:
                del self._ipy_namespace[k]
            self.mainloop.screen.start()
            self.mainloop.screen.set_mouse_tracking()
        elif input in UI.keys['goto']:
            self.open_popup(GotoInput())
        elif input in UI.keys['gotoevent']:
            self.open_popup(GotoEventInput())
        elif input in UI.keys['writeframe']:
            self.open_popup(WriteFrameInput())
        elif input in UI.keys['libs']:
            self.open_popup(LibInput())
        elif input in UI.keys['help']:
            self.mainloop.widget = self.helpscreen
        else:
            return self.body.keypress(size, input)

def run_viewer(filename, i3file, colors=False, ascii_only=False):

    frames = i3file

    def quit_keys(input):
        if input in UI.keys['escape']:
            raise urwid.ExitMainLoop()
    
    def updown_filter(keys,raw):
        # keys for moving up and down are handled automatically by ListBox.  This filter
        # ensures that our custom definitions in UI.keys are observed by these widgets.
        if len(keys) == 1:
            if keys[0] in UI.keys['up']:
                keys[0] = 'up'
            elif keys[0] in UI.keys['down']:
                keys[0] = 'down'
            elif len(keys[0]) == 4 and keys[0][0] == 'mouse press':
                # mouse press events for buttons 4 and 5 are scroll wheel actions
                # treat these as scroll requests
                if keys[0][1] == 4:
                    keys[0] = 'up'
                elif keys[0][1] == 5:
                    keys[0] = 'down'
        return keys

    header_t1 = urwid.Text( ('titletext', '  Shoveling through file {0}'.format(filename)), wrap='clip' )
    header_t2 = urwid.Text( ('titletext', 'For help press h'), wrap='clip', align='right')
    header = urwid.Columns( [header_t1, header_t2] )
    footer = Footer( frames )
    main = ViewerMain( frames, header, footer, ascii_only=ascii_only )
    amain = urwid.AttrMap(main, 'default')
    loop = urwid.MainLoop( amain, palette = UI.palette, input_filter = updown_filter, unhandled_input = quit_keys )
    main.set_mainloop(loop)
    if colors >= 256:
        try:
            loop.screen.set_terminal_properties(colors=256)
        except:
            print("Trouble setting 256 color mode, falling back on simpler colors")
            loop.screen.reset_default_terminal_palette()
    else:
        try:
            loop.screen.set_terminal_properties(bright_is_bold=True)
        except:
            loop.screen.reset_default_terminal_palette()
    loop.run()

def libs_callback(option, opt, value, parser):
    setattr(parser.values, option.dest, value.split(","))

def main():
    op = optparse.OptionParser()
    op.add_option('-s','--sequential', dest='sequential', action='store_true',
                  help='Force sequential file API (quick initial load, slower random access, supports compressed files)')
    op.add_option('-b','--browsable', dest='browsable', action='store_true',
                  help='Force browsable file API (slower initial load, fast random access, uncompressed files)')
    op.add_option('-c','--cachesize', dest='cachesize', type=int,
                  help='Specify a frame cache size (implies sequential API)')
    op.add_option('-l','--libs', dest='libs', type="string",
                  action='callback', callback=libs_callback,
                  help='Comma-separated list of additional icecube libraries to load')
    op.add_option('--colors', dest='colors', default=term_colors, action='store', 
                  type='int', help='Display in (16,256) colors')
    op.add_option('--ascii', dest='ascii', default=False, action='store_true', 
                  help='Display in ascii only (unicode disabled)')
    (opts, args) = op.parse_args(sys.argv)

    if len(args) != 2:
        op.error('Need exactly one argument: a file to load')

    if opts.browsable and (opts.sequential or opts.cachesize):
        op.error('Contradictory file loading specifiers')

    if opts.libs is not None:
        print('Importing libraries...')
        for lib in opts.libs:
            __import__( 'icecube.'+lib )

    print('Loading file...')
    default_sequential_cache = 256
    i3file = args[1]
    stager = dataio.get_stagers()
    handle = stager.GetReadablePath(i3file)
    i3file = str(handle)
    if opts.sequential or opts.cachesize:
        opts.ensure_value('cachesize', default_sequential_cache)
        i3file = fileadaptor.I3SequentialAdaptor(dataio.I3File(i3file), opts.cachesize)
    elif opts.browsable:
        ibf = dataio.I3BrowsableFile()
        ibf.open_file(i3file)
        i3file = fileadaptor.I3BrowsableAdaptor(ibf)
    else:
        # file type will be inferred
        i3file = fileadaptor.I3FileAdaptor(i3file, sequential_cache_size=default_sequential_cache)
    print('Creating GUI...')
    run_viewer(args[1], i3file, colors=opts.colors, ascii_only=opts.ascii)


if __name__ == '__main__':
    main()
