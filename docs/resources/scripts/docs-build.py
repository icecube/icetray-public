#!/usr/bin/env python3

import os.path,sys,pkgutil,time,subprocess
from collections import deque
from glob import glob
from icecube.icetray import i3inspect

import logging
log = logging.getLogger(__file__)
log.setLevel(logging.DEBUG)
try:
    from rich.logging import RichHandler
    log_handler = RichHandler()
    log_handler.setFormatter(logging.Formatter("%(message)s", datefmt="[%X]"))
except ImportError:
    log_handler = logging.StreamHandler()
    log_handler.setFormatter(logging.Formatter("%(levelname)s:%(name)s: %(message)s", datefmt="[%X]"))

log.addHandler(log_handler)

class command_queue:
    def __init__(self, max_processes=1):
        self.queue = deque([])
        self.running = []
        self.max_processes = max_processes

    def check(self):
        newrunning = []
        for args, popen, endtime in self.running:
            st = popen.poll()
            if st is None:
                if endtime is not None and time.monotonic() > endtime:
                    log.warning("Timed out, Killing %s", args[0])
                    popen.kill()
                newrunning.append((args, popen, endtime))
            elif st:
                log.warning("Exit status %s: %s", args[0], st)

        self.running = newrunning
        while len(self.queue) and len(self.running) < self.max_processes:
            args, timeout = self.queue.popleft()
            log.debug("Running %s", args)

            if timeout:
                endtime = time.monotonic() + timeout
            else:
                endtime = None
            self.running.append((args, subprocess.Popen(*args), endtime))

    def call(self, *args, timeout=None):
        self.queue.append((args, timeout))
        self.check()

    def wait(self, polltime=0.01):
        while len(self.running):
            self.check()
            time.sleep(polltime)

def mkdir_p(dir):
    if not os.path.isdir(dir):
        # log.debug("making directory %s", dir)
        os.mkdir(dir)

def symlink(src,dst):
    if os.path.islink(dst):
        os.unlink(dst)
    # log.debug("Linking %s -> %s", src, dst)
    os.symlink(src,dst)

def symlinkdir(srcdir,destdir):
    for f in os.listdir(srcdir):
        if f.endswith('.in') or f.endswith('.pyc') or f.endswith('~') or f.startswith('#'):
            continue
        if f in ["source","Makefile","CMakeLists.txt"]:
            continue
        src = os.path.join(srcdir,f)
        symlink(src, os.path.join(destdir,f))

def copy_replace(infile,outfile,replace):
    with open(infile,'rt') as f:
        txt = f.read()
    for rin,rout in replace.items():
        txt = txt.replace(rin,rout)
    with open(outfile,'wt') as f:
        f.write(txt)

def use_this_project(proj):
    return (args.projects is None or
            proj in args.projects or
            proj.replace("_","-") in args.projects or
            proj.replace("-","_") in args.projects
            )

def call(*args,**kwargs):
    log.debug("Calling %s with arguments %s", str(args), str(kwargs))
    status = subprocess.call(args,**kwargs)
    if status:
        log.warning("Exit status %s: %s", args[0], status)
    return status

cppautodoctxt = """
.. _{PROJECT_NAME}-cpp:

{PROJECT_NAME} C++ API Reference
================================================================================

.. doxygenindex::
    :path: {DOXYGEN_PROJECT_PATH}
"""

def main():

    import argparse

    parser = argparse.ArgumentParser(description='Generate Documentation for IceTray')
    parser.add_argument('--verbose', '-v', action='count')
    parser.add_argument('--redirect-sphinx-output', action='store_true')
    parser.add_argument('--clean',action='store_true',
                        help='clean out the old files in documentaiton build '
                        'directory before generating any new files')
    parser.add_argument('--projects', nargs='+',metavar='proj',
                        help='only generate documentation for these projects')
    parser.add_argument('--skip-doxygen', nargs='+',metavar='proj',
                        help='do not generate doxygen for these projects',
                        default=['steamshovel'])
    parser.add_argument('--skip-inspect', nargs='+',metavar='proj',
                        help='do not generate inspect docs for these projects',
                        default=['HiveSplitter','IceHive','photonics_service'])
    parser.add_argument('--build-type', default='html',
                        help="type of output to build [default=html] (see "
                        "http://www.sphinx-doc.org/en/stable/invocation.html"
                        "#invocation-of-sphinx-build for complete list)")
    parser.add_argument('--no-general-docs',action='store_true',
                        help="don't generate the general interest docs")
    parser.add_argument('--no-project-docs', action='store_true',
                        help="don't generate docs from individual projects `docs` dir")
    parser.add_argument('--no-python', action='store_true',
                        help="don't generate python API reference docs")
    parser.add_argument('--no-doxygen', action='store_true',
                        help="don't generate doxygen XML files")
    parser.add_argument('--no-cpp', action='store_true',
                        help="don't generate C++ API reference docs from doxygen XML")
    parser.add_argument('--no-inspect', action='store_true',
                        help="don't generate IceTray reference docs using icetray-inspect")
    parser.add_argument('--no-sphinx',action='store_true',
                        help='don''t run sphinx')
    parser.add_argument('--open',action='store_true',
                        help='convenience option to open a browser with the output of the html')
    parser.add_argument('-j',default=1,type=int,
                        help='number of parallel processes to run')

    global args
    args = parser.parse_args()

    log.info("Setting up directories")
    I3_BUILD = os.environ["I3_BUILD"]
    I3_SRC = os.environ["I3_SRC"]

    docsdir = os.path.join(I3_SRC,'docs')
    sphinxdir = os.path.join(I3_BUILD,'docs')
    builddir=os.path.join(sphinxdir,'build')
    sourcedir=os.path.join(sphinxdir,'source')
    confdir=os.path.join(sphinxdir,'conf')
    doxygendir=os.path.join(sphinxdir,"doxygen")

    queue = command_queue(args.j)

    if args.clean:
        log.info("Cleaning out old files")
        call("rm", "-rf", builddir)
        call("rm", "-rf", sourcedir)
        if not args.no_doxygen:
            call("rm", "-rf", doxygendir)

    mkdir_p(builddir)
    mkdir_p(sourcedir)
    mkdir_p(sourcedir+'/python')
    mkdir_p(sourcedir+'/projects')
    mkdir_p(sourcedir+'/inspect')
    mkdir_p(sourcedir + '/doxygen/')

    symlink(os.path.join(confdir,"conf.py"),
            os.path.join(sourcedir,"conf.py"))

    #symlink base directory of the source tree
    symlinkdir(os.path.join(docsdir,"basedocs"),sourcedir)
    symlink(os.path.join(confdir,"metaproject_metainfo.rst"),
            os.path.join(sourcedir,"metaproject_metainfo.rst"))

    if not args.no_general_docs:
        symlink(os.path.join(docsdir,"info"),
                os.path.join(sourcedir,"info"))
        symlink(os.path.join(docsdir,"general"),
                os.path.join(sourcedir,"general"))

    if not args.no_project_docs:
        log.info("Symlink projects' docs dir")
        for projdocdir in glob(os.path.join(I3_BUILD,"*","resources","docs")):
            proj = projdocdir.split(os.sep)[-3]
            if not use_this_project(proj):
                continue
            if os.path.isdir(projdocdir):
                symlink(projdocdir,os.path.join(sourcedir,"projects",proj))

    if not args.no_python:
        log.info("Generating Python references")
        python_src_dir=os.path.join(sourcedir,"python")
        #call program which generates rsts for all python moudles in libdir
        call("sphinx-apidoc",
             "-q", "-l", "-M", "-e",
             "-H", "Python API Reference",
             "-o", python_src_dir,
             os.path.join(I3_BUILD, "lib"))

        #delete the ones we dont need
        os.unlink(os.path.join(python_src_dir,'modules.rst'))
        for rstfile in glob(python_src_dir+"/lib*.rst"):
            os.unlink(rstfile)

        if args.projects:
            for rstfile in glob(python_src_dir+"/*"):
                s= os.path.basename(rstfile).split('.')
                if s[0]=='icecube':
                    if s[1]!='rst' and not use_this_project(s[1]):
                        log.debug("Removing %s", rstfile)
                        os.unlink(rstfile)
                else:
                    if use_this_project(s[0]):
                        log.debug("Removing %s", rstfile)
                        os.unlink(rstfile)

    if not args.no_doxygen:
        log.info("Generating Doxygen Documentation")
        mkdir_p(doxygendir)

        for projectdir in glob(os.path.join(I3_SRC,"*")):
            project = os.path.basename(projectdir)

            if project in args.skip_doxygen:
                continue

            if os.path.isdir(projectdir) and use_this_project(project):
                doxyfile = os.path.join(doxygendir,project+'.doxyfile')
                copy_replace(os.path.join(docsdir,"conf","doxyfile.in"),
                             doxyfile,
                             {"@PROJECT_NAME@":project,
                              "@DOXYGEN_OUTPUT_PATH@":os.path.join(doxygendir,project),
                              "@CMAKE_CURRENT_SOURCE_DIR@":projectdir,
                             })
                queue.call(["doxygen",doxyfile])

        queue.wait()
    else:
        log.info("Skipping Doxygen Documentation")

    if not args.no_cpp:
        log.info("Generating C++ references from Doxygen XML")
        for doxygen_dir in glob(os.path.join(doxygendir,"*","xml")):
            project_name = os.path.basename(os.path.dirname(doxygen_dir))
            if not use_this_project(project_name):
                continue
            if project_name in args.skip_doxygen:
                continue
            mkdir_p(os.path.join(sourcedir, "doxygen", project_name))
            outfilename = os.path.join(sourcedir, "doxygen", project_name, "index.rst")
            log.debug("Writing %s, with project name: %s, doxygen dir: %s", outfilename, project_name, doxygen_dir)
            with open(outfilename,'wt') as f:
                f.write(cppautodoctxt.format(PROJECT_NAME=project_name,
                                             DOXYGEN_PROJECT_PATH=doxygen_dir))
    else:
        log.info("Skipping C++ references from Doxygen XML")

    if not args.no_inspect:
        log.info("Generating icetray quick reference with icetray-inspect")
        quick_rst = os.path.join(sourcedir,"icetray_quick_reference.rst")
        cmd = ["icetray-inspect",
               "--sphinx","--sphinx-references","--no-params",
               "--title=IceTray Inspect Quick Reference",
               "--exclude="+','.join(args.skip_inspect),
               "-o",quick_rst]
        if args.projects:
            cmd += args.projects
        else:
            cmd += ["--all"]
        log.info("Writing %s", quick_rst)
        queue.call(cmd, timeout=60)

        inspectlibs = i3inspect.get_all_projects()
        rstfiles=[]

        for proj in inspectlibs:
            if not use_this_project(proj) or proj in args.skip_inspect:
                continue

            rst_out= os.path.join(sourcedir,"inspect",proj+".rst")
            rstfiles.append(rst_out)

            cmd = ["icetray-inspect",
                   proj,
                   "--sphinx","--subsection-headers","--sphinx-functions",
                   #--expand-segments,#"--verbose-docs" #these options might work in the future
                   "--title=",
                   "-o",rst_out]
            log.debug("Writing %s", rst_out)
            queue.call(cmd, timeout=10)

        queue.wait()

        #some projects don't have any output,
        #delete these so the docs look nice
        for rst_out in rstfiles:
            with open(rst_out) as f:
                filelength = len(f.read().strip())
            if filelength ==0 :
                os.unlink(rst_out)

    if not args.no_sphinx:
        doctreedir = os.path.join(builddir,"doctrees")
        finaldir = os.path.join(builddir,args.build_type)
        logdir = os.path.join(builddir,'logs')
        mkdir_p(doctreedir)

        if args.redirect_sphinx_output or args.verbose:
            mkdir_p(logdir)

        sphinx_kwargs = {}
        if args.redirect_sphinx_output:
            sphinx_kwargs["stdout"] = open(os.path.join(logdir,"sphinx_build_stdout.txt"),'w')
            sphinx_kwargs["stderr"] = open(os.path.join(logdir,"sphinx_build_stderr.txt"),'w')
        
        sphinx_cmd = [ "sphinx-build",
                        "-a",#all
                        "-j",str(args.j),
                        "-b",args.build_type,
                        "-d",doctreedir,
                        "-E",sourcedir
                        ]

        if args.verbose:
            warnfile = os.path.join(logdir,'sphinx_build_warnings.txt')
            sphinx_cmd += ["-v","-T","-w",warnfile]

        sphinx_cmd+=[finaldir]

        retvalue = call(*sphinx_cmd,**sphinx_kwargs)

        if args.open and not retvalue:
            if args.build_type=='html':
                if args.projects:
                    outfile = os.path.join(finaldir,"projects",args.projects[0],"index.html")
                else:
                    outfile = os.path.join(finaldir,"index.html")
            else:
                outfile = None

            if outfile:
                if sys.platform=='darwin':
                    opencmd = 'open'
                else:
                    opencmd = 'xdg-open'
                call(opencmd,outfile)

        return retvalue
    else:
        return 0


if __name__=="__main__":
    sys.exit(main())
