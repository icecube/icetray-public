#!/usr/bin/env python
#
#  $Id: icetray-inspect.py 2188 2015-07-30 04:09:28Z nega $
#
#  Copyright (C) 2011
#  Jakob van Santen <vansanten@wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#

def check_regex(option, opt, value):
    try:
        return re.compile(value)
    except:
        raise OptionValueError("'%s' is not a regular expression" % value)

class sphinx_writer:
    def __init__(self,file):
        self.file = file

    def file_header(self):
        self.file.write(opts.title+'\n')
        self.file.write(len(opts.title)*'='+'\n\n')

    def file_footer(self):
        pass

    def project_header(self,project,loadstr):
        self.file.write('\n')
        self.file.write('Project %s\n' % project)
        self.file.write('-'*(len(project)+8)+'\n\n')
        self.file.write("Invoke with: ``%s``\n\n"%loadstr)

    def project_footer(self):
        pass

    def module_header(self,modname,category,docs):

        if category in ['C++ I3Module','C++ ServiceFactory']:
            usage = '"%s"'%modname
            altname = modname.replace("<","_").replace(">","")
        else:
            usage = modname
            altname = modname.split('.')[-1]

        if opts.sphinx_functions:
            self.file.write(".. js:data:: %s\n\n    ``%s`` *(%s)*\n\n"
                                            %(altname,usage,category))
        elif opts.sphinx_references:
            self.file.write("* :js:data:`%s` *(%s)* - "%(altname,category))
        else:
            self.file.write("**%s** (%s)\n\n"
                                            %(modname,category))

        if docs:
            for d in docs.strip().splitlines():
                self.file.write('    %s' % d)
            if not opts.sphinx_references:
                self.file.write('\n\n')

    def module_footer(self):
        self.file.write('\n')

    def subsection(self,subsection):
        self.file.write('%s\n'%subsection)
        self.file.write('^'*len(subsection)+'\n\n')

    def parameters(self,config):
        if config:
            desc = config.descriptions
            for k in config.keys():
                try:
                    default = repr(config[k])
                except RuntimeError:
                    default = "<Unprintable>"

                self.file.write('    :param %s: *Default* = ``%s``, %s\n'
                                                %(k,default,desc[k].replace('\n', '')))

    def segment_header(self):
        pass

    def segment_footer(self):
        self.file.write('\n')
        pass


    def segelement(self,modtype,component_name,name,args):
        line = '    Add%s(%s, \'%s\'' % (modtype, component_name, name)
        for i in args.keys():
            line += ', %s=%s' % (i, args[i].__repr__())
        line += ')\n'
        self.file.write(line)



class xml_writer:
    def __init__(self,file):
        self.file = file

    def file_header(self):
        self.file.write('<?xml version=\'1.0\'?>\n')
        self.file.write('<icetray-inspect>\n')

    def file_footer(self):
        self.file.write('</icetray-inspect>\n')

    def project_header(self,project,loadstr):
        self.file.write('<project name="%s">\n' % project)

    def project_footer(self):
        self.file.write('</project>\n')

    def module_header(self,modname,category,docs):
        self.file.write('<module>\n')
        self.file.write('<type>%s</type>\n' % html.escape(modname))
        self.file.write('<kind>%s</kind>\n' % html.escape(category))
        self.file.write('<description>%s</description>\n' % html.escape(docs))

    def module_footer(self):
        self.file.write('</module>\n')

    def subsection(self,subsection):
        pass

    def segment_header(self):
        self.file.write('<segment>\n')

    def segment_footer(self):
        self.file.write('</segment>\n')

    def parameters(self,config):
        if config:
            desc = config.descriptions
            for k in config.keys():
                self.file.write('<parameter>\n')
                self.file.write('\t<name>%s</name>\n' % html.escape(k))
                self.file.write('\t<description>%s</description>\n'
                                        % html.escape(desc[k]))
                try:
                    default = repr(config[k])
                except RuntimeError:
                    default = "[Unprintable]"
                self.file.write('\t<default_value>%s</default_value>\n'
                                        % html.escape(default))
                self.file.write('</parameter>\n')

    def segelement(self,modtype,component_name,name,args):
        self.file.write('\t<segelement type="%s" name="%s" instance="%s">\n'
                                        % (modtype, html.escape(component_name), html.escape(name)))
        for i in args.keys():
            self.file.write('\t<argument name="%s">%s</argument>\n'
                      % (html.escape(i), html.escape(args[i].__repr__())))
        self.file.write('\t</segelement>\n')

class human_writer:
    def __init__(self,file):
        self.file = file

    def file_header(self):
        self.file.write('**** IceTray Inspect ****\n\n')

    def file_footer(self):
        #self.file.write(
        #       '\n%d module, %d service, and %d tray segment configurations reported.\n'
        #       % (modcount, servicecount, segmentcount))
        pass

    def project_header(self,project,loadstr):
        self.file.write('*** %s ***\n' % project)
        self.file.write('-'*79+'\n')
        self.file.write('Invoke with: "%s"\n'%loadstr)

    def project_footer(self):
        self.file.write('\n')

    def module_header(self,modname,category,docs):
        self.file.write('  %s (%s)\n' % (modname, category))
        if not opts.names_only and len(docs) > 0:
            self.file.write('\n')
            self.file.write('    ' + docs.replace('\n', '\n    '))
            self.file.write('\n\n')
    def module_footer(self):
        pass

    def subsection(self,subsection):
        self.file.write('%s\n'%subsection)

    def segment_header(self):
        self.file.write('  Equivalent to:\n')

    def segment_footer(self):
        self.file.write('\n')

    def parameters(self,config):
        if config:
            desc = config.descriptions
            if len(config.keys()) > 0:
                self.file.write('  Parameters:\n')
                sortedkeys = list(config.keys())
                sortedkeys.sort()
                for k in sortedkeys:
                    self.file.write('    %s\n' % k)
                    if  desc[k]:
                        self.file.write('      Description : %s\n' % desc[k])
                    try:
                        default = repr(config[k])
                    except RuntimeError:
                        default = "[Unpritable]"
                    self.file.write('      Default     : %s\n' % default)

                    self.file.write('\n')
            else:
                self.file.write('    (No parameters)\n')
        self.file.write('  ' + '-'*77 + '\n')


    def segelement(self,modtype,component_name,name,args):
        line = '    Add%s(%s, \'%s\'' % (modtype, component_name, name)
        for i in args.keys():
            line += ', %s=%s' % (i, args[i].__repr__())
            line += ')'
        line+='\n'
        self.file.write(line)


class PotemkinTray:
    def PrintArgs(self, modtype, component, name, args):
        # Try lots of ways to get the name of modules that
        # work even if they are Python modules. Mostly
        # valid.
        try:
            component_name = component.__repr__()
        except TypeError:
            try:
                component_name = component.__module__ + component.__name__
            except AttributeError:
                component_name = repr(component)

        # Provide a simulacram of what the call looked like
        output.segelement(modtype,component_name,name,args)

    def AddModule(self, module, name, **kwargs):
        self.PrintArgs('Module', module, name, kwargs)
    def AddService(self, module, name, **kwargs):
        self.PrintArgs('Service', module, name, kwargs)
    def AddSegment(self, module, name, **kwargs):
        self.PrintArgs('Segment', module, name, kwargs)

def print_segment(segment):
    potemkin = PotemkinTray()
    output.segment_header()
    try:
        segment(potemkin, 'example')
    except:
        raise
        sys.stderr.write('Error instantiating segment \'%s\' with default arguments\n'%
                                         segment)
    output.segment_footer()


def get_doxygen_docstring(project,modulename):
    xmlfile = os.path.join(os.environ["I3_BUILD"],"docs","doxygen",
                                               project,"xml","class"+modulename+".xml")
    try:
        tree = ET.parse(xmlfile)
    except IOError:
        return ""

    root = tree.getroot()
    comp = root.find("compounddef")
    brief = comp.find('briefdescription')

    if sys.version_info[0] < 3:
        doc = ET.tostring(brief, encoding='utf-8', method='text')
    else:
        doc = ET.tostring(brief, encoding='unicode', method='text')

    if opts.verbose_docs:

        detail = comp.find('detaileddescription')

        #remove metadata tags
        for c in detail:
            for s in c.findall('simplesect'):
                #element tree is weird and this is more complicated than it needs to be
                for t in list(s):
                    s.remove(t)

        if sys.version_info[0] < 3:
            doc+='\n\n'+ET.tostring(detail, encoding='utf-8', method='text')
        else:
            doc+='\n\n'+ET.tostring(detail, encoding='unicode', method='text')

    return doc.strip()


def display_config(mod, category, modname,config,docs):
    if opts.regex and not opts.regex.match(modname):
        return False

    if docs is None:
        docs = ''
    elif not opts.verbose_docs:
        docs = docs.strip().split('\n\n')[0]

    output.module_header(modname,category,docs)

    if i3inspect.is_traysegment(mod) and opts.expand_segments:
        print_segment(mod)

    if not opts.names_only:
        output.parameters(config)

    output.module_footer()
    return True

def get_converters(project):
    converters = []
    for obj,convs in tableio.I3ConverterRegistry.registry.items():
        for conv in convs:
            if conv.__module__.split('.')[1]==project:
                converters.append(conv)
    return converters


def display_project(project):

    if project in python_projects:
        pyproject = project
    else:
        pyproject = None

    dashproject = project.replace('_','-')
    if project in compiled_projects:
        cppproject = project
    elif dashproject in compiled_projects:
        cppproject = dashproject
    else:
        cppproject = ""

    if pyproject:
        import_name = 'icecube.%s' %pyproject
        try:
            pymodule = __import__(import_name,
                                  globals(), locals(), [pyproject])
        except Exception as e:
            sys.stderr.write("ERROR: can't load '%s': %s\n" % (import_name,str(e)))
            return

        if hasattr(pymodule,"__path__"):
            for subpackage in pkgutil.walk_packages(pymodule.__path__,prefix=import_name+'.'):
                try:
                    __import__(subpackage[1],globals(), locals(), [pyproject])
                except Exception as e:
                    sys.stderr.write("WARNING: can't load subpackage '%s': %s\n" % (subpackage[1],str(e)))

        loadstr = "import icecube.%s"%pyproject
    elif cppproject:
        try:
            icetray.load(cppproject, False)
        except Exception as e:
            sys.stderr.write("Error: can't load '%s': %s\n" % (project,str(e)))
            return

        pymodule=None
        loadstr = "icetray.load('%s',False)"%cppproject
    else:
        sys.stderr.write("Error: can't load '%s'\n" % (project))
        return

    if cppproject:
        proj_name = cppproject
    else:
        proj_name = pyproject

    icetray.modules(cppproject)
    modules = []

    if not opts.no_modules:
        for mod in icetray.modules(cppproject):
            try:
                config =  i3inspect.module_default_config(mod)
            except:
                sys.stderr.write("WARNING Ignoring '%s': %s\n" % (mod, sys.exc_info()[1]))
                continue
            docs = get_doxygen_docstring(cppproject,mod)
            modules.append((mod, 'C++ I3Module',mod,config,docs))

        python_modules = i3inspect.harvest_objects(pymodule, i3inspect.is_I3Module)
        for mod,py_mod in python_modules.items():

            #reject modules imported by python files which should show up elsewhere
            #without this we get a bunch of `I3ConditionalModules` everywhere
            if not mod.__module__.startswith(import_name):
                continue

            try:
                config = mod(icetray.I3Context()).configuration
            except:
                sys.stderr.write("WARNING Ignoring '%s': %s\n" % (mod, sys.exc_info()[1]))
                continue
            docs = inspect.getdoc(mod)
            modules.append((mod, 'Python I3Module', py_mod,config,docs))

    if not opts.no_services:
        for mod in icetray.services(cppproject):
            try:
                config =  i3inspect.module_default_config(mod)
            except:
                sys.stderr.write("WARNING Ignoring '%s': %s\n" % (mod, sys.exc_info()[1]))
                continue
            docs = get_doxygen_docstring(cppproject,mod)
            modules.append((mod, 'C++ ServiceFactory',mod,config,docs))

    if not opts.no_segments:
        segments = i3inspect.harvest_objects(pymodule, i3inspect.is_traysegment)
        for segment,pyseg in segments.items():
            config = i3inspect.I3HoboConfiguration.from_traysegment(segment)
            docs = inspect.getdoc(config)
            modules.append((segment, 'I3Tray segment', pyseg,config,docs))

    if not opts.no_converters:
        for converter in get_converters(pyproject):
            doc = converter.__doc__
            convname =  pymodule.__name__ + "." + converter.__name__
            modules.append((converter, 'TableIO converter',convname, None,doc))


    if modules:
        output.project_header(proj_name,loadstr)
        subsection = None

        for module in modules:
            if opts.subsection_headers and module[1]!=subsection:
                subsection = module[1]
                output.subsection(subsection+'s')
            display_config(*module)
        output.project_footer()

from optparse import Option, OptionParser
import re,os.path

Option.TYPES = Option.TYPES + ("regex",)
Option.TYPE_CHECKER["regex"] = check_regex

parser = OptionParser("usage: %prog [options] project1 project2 ...")
parser.add_option('-a', '--all', action='store_true',
        help='Examine all projects/libraries in DIRECTORY', metavar='DIRECTORY',
        default=False)
parser.add_option('-R', '--regex', dest='regex', type='regex',
        help='Print only modules/services/segments whose names match this regular expression',
        default=None)
parser.add_option('-s', '--sphinx', dest='sphinx',
    help='Output in rst',
        action='store_true', default=False)
parser.add_option('--sphinx-functions', dest='sphinx_functions',
    help='output functions ',
        action='store_true', default=False)
parser.add_option('--sphinx-references', dest='sphinx_references',
    help='output functions ',
        action='store_true', default=False)
parser.add_option('-x', '--xml', dest='xml', help='Output in XML',
        action='store_true', default=False)
parser.add_option('--subsection-headers', dest='subsection_headers',
    help='headers between different types of output',
        action='store_true', default=False)
parser.add_option('-o', '--output-file',dest='output',default=None,
        help='filename to write output')
parser.add_option('--verbose-docs', dest='verbose_docs', action='store_true',
        help='Print long form of docs strings',
        default=False)
parser.add_option('--no-params', dest='names_only', action='store_true',
        help='Print the names of modules only, not their parameters',
        default=False)
parser.add_option('--no-modules', dest='no_modules', action='store_true',
        help='Skip I3Modules',
        default=False)
parser.add_option('--no-services', dest='no_services', action='store_true',
        help='Skip ServiceFactories',
        default=False)
parser.add_option('--no-segments', dest='no_segments', action='store_true',
        help='Skip I3Tray segments',
        default=False)
parser.add_option('--no-converters', dest='no_converters', action='store_true',
        help='Skip tableio segments',
        default=False)
parser.add_option('--expand-segments', dest='expand_segments', action='store_true',
        help='Expand I3Tray segments, printing the sequence of modules they add to the tray',
        default=False)
parser.add_option('--title', dest='title',
        default='IceTray Inspect',
        help='title to place at top of output')

opts, args = parser.parse_args()

if len(args) == 0 and not opts.all:
    parser.print_help()
    parser.exit(1)

import inspect, sys, html,signal,pkgutil
import xml.etree.ElementTree as ET
from glob import glob

from icecube import icetray, dataclasses,tableio
from icecube.icetray import i3inspect
from icecube.icetray import traysegment


def sig_handler(signum, frame):
    raise Exception("Segfault")
signal.signal(signal.SIGSEGV, sig_handler)

icetray.I3Logger.global_logger = icetray.I3NullLogger()

compiled_projects,python_projects = i3inspect.get_inspectable_projects()

if opts.all:
    args = compiled_projects+python_projects

if opts.output:
    outfile = open(opts.output,'wt')
else:
    outfile = sys.stdout

if opts.sphinx:
    output = sphinx_writer(outfile)
elif opts.xml:
    output = xml_writer(outfile)
else:
    output = human_writer(outfile)

output.file_header()

args = sorted(set([a.replace('-','_') for a in args]),
              key=lambda s:s.lower())

#level3_filter_cascade calls the segfaulting IceHive
noinspect = i3inspect.get_uninspectable_projects()+["level3_filter_cascade"]

for p in args:
    if p in noinspect:
        sys.stderr.write("WARNING: Skipping uninspectable project: {}".format(p))
        continue
    display_project(p)

output.file_footer()
