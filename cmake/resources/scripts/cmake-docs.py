#!/usr/bin/env python

import os.path,sys,pkgutil,time,subprocess
from collections import deque
from glob import glob
from icecube.icetray import i3inspect
    

class command_queue:
    def __init__(self,max_processes=1):
        self.queue = deque([])
        self.running = []
        self.max_processes = max_processes
    def check(self):
        newrunning = []
        for args,popen in self.running:
            st = popen.poll()
            if st is None:
                newrunning.append((args,popen))
            else:
                if st:
                    print("\033[1;31mWARNING\033[0mExit Status",args,":",st)
        self.running = newrunning
        while len(self.queue) and len(self.running) < self.max_processes:
            args = self.queue.popleft()
            print("running ",args)
            self.running.append((args,subprocess.Popen(*args)))
    def call(self,*args):
        self.queue.append(args)
        self.check()
    def wait(self,polltime=0.001):
        while len(self.running):
            self.check()
            time.sleep(polltime)

def mkdir_p(dir):
    if not os.path.isdir(dir):
        print("making directory "+dir)
        os.mkdir(dir)

def symlink(src,dst):
    if os.path.islink(dst):
        os.unlink(dst)
    print("linking {}->{}".format(src,dst))
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

def call(*args):
    print("calling",str(args))
    status = subprocess.call(args)
    if status:
        print("###ERROR### {} returned {}".format(args[0],status))


cppautodoctxt = """
{PROJECT_NAME} C++ API Reference
================================================================================

.. doxygenindex::
    :path: {DOXYGEN_PROJECT_PATH}
"""        
    
def main():

    import argparse
         
    parser = argparse.ArgumentParser(description='Generate Documentation for IceTray')
    parser.add_argument('--verbose', '-v', action='count')
    pgroup = parser.add_mutually_exclusive_group(required=True)
    pgroup.add_argument('--projects', nargs='+',metavar='proj',
                        help='only generate documentation for these projects')
    pgroup.add_argument('--all', action='store_true',
                        help='generate docs for all the projects (will take a long time)')
    parser.add_argument('--build-type', default='html',
                        help="type of output to build (see "
                        "http://www.sphinx-doc.org/en/stable/invocation.html"
                        "#invocation-of-sphinx-build for complete list)")
    parser.add_argument('--build-dir',default="sphinx_build",
                        help="sub-directory of ${I3_BUILD} to build the "
                        "documentation source code in")
    parser.add_argument('--no-general-docs',action='store_true',
                        help="don't generate the general interest docs")
    parser.add_argument('--no-meta-project-docs', action='store_true',
                        help="don't generate the docs from the meta-project's "
                        "base directory")
    parser.add_argument('--no-project-docs', action='store_true',
                        help="don't generate docs from individual projects `docs` dir")
    parser.add_argument('--no-python', action='store_true',
                        help="don't generate python API reference docs")
    parser.add_argument('--no-doxygen', action='store_true',default=True,
                        help="don't generate doxygen XML files")
    parser.add_argument('--no-cpp', action='store_true',default=True,
                        help="don't generate C++ API reference docs from doxygen XML")
    parser.add_argument('--no-inspect', action='store_true',
                        help="don't generate IceTray reference docs using icetray-inspect")
    parser.add_argument('--no-html',action='store_true',
                        help='convenience option to generate the html')
    parser.add_argument('--open',action='store_true',
                        help='convenience option to open a browser with the output of the html')
    parser.add_argument('-j',default=1,type=int,
                        help='number of parallel processes to run')
                        
    global args
    args = parser.parse_args()
    
    I3_BUILD = os.environ["I3_BUILD"]
    I3_SRC = os.environ["I3_SRC"]

    print(args.build_dir)
    builddir=os.path.join(I3_BUILD,args.build_dir)
    sphinxdir=os.path.join(I3_BUILD,"sphinx_src")
    sourcedir = os.path.join(builddir,"source")
    doxygendir=os.path.join(builddir,"doxygen")

    queue = command_queue(args.j)

    call("rm","-rfv",builddir)

    mkdir_p(builddir)
    mkdir_p(sourcedir)
    mkdir_p(sourcedir+'/python')
    mkdir_p(sourcedir+'/projects')
    mkdir_p(sourcedir+'/inspect')
    mkdir_p(sourcedir+'/cpp/')

    symlink(os.path.join(sphinxdir,"source","conf.py"),
            os.path.join(sourcedir,"conf.py"))

    symlink(os.path.join(sphinxdir,"source","svn-externals.txt"),
            os.path.join(sourcedir,"svn-externals.txt"))
        
    for pyfile in ["plot_directive.py","script_directive.py",
                   "sphinx_cmake.py","toctree_sort.py"]:
        symlink(os.path.join(I3_SRC,'cmake',"meta-project-docs",pyfile),
                os.path.join(builddir,pyfile))

    if not args.no_general_docs:
        symlinkdir(os.path.join(I3_SRC,"cmake","meta-project-docs","source"),sourcedir)
        symlink(os.path.join(sphinxdir,"source","metaproject_metainfo.rst"),
                os.path.join(sourcedir,"metaproject_metainfo.rst"))
    
    else:
        for x in ["index.rst","icetray","_static","_templates","ICE_CUBE_LOGO_4c.png",
                  "icetray_reference.rst","cpp_reference.rst"]:
            symlink(os.path.join(I3_SRC,"cmake","meta-project-docs","source",x),
                    os.path.join(sourcedir,x))
    
    if not args.no_meta_project_docs:
        symlink(os.path.join(I3_SRC,'docs'),
                os.path.join(sourcedir,"metaproject"))

    if not args.no_project_docs:
        print("symlink projocts' docs dir")
        for projdocdir in glob(os.path.join(os.environ["I3_SRC"],"*","resources","docs")):
            proj = projdocdir.split(os.sep)[-3]
            if not use_this_project(proj):
                continue
        
            if os.path.isdir(projdocdir):
                symlink(projdocdir,os.path.join(sourcedir,"projects",proj))

    if not args.no_python:
        print("Generating Python references")
        pymoduledir = os.path.join(I3_BUILD,"lib")

        #call program which generates rsts for all python moudles in libdir
        call("sphinx-apidoc",
             "-l","-M",
             "-H","Python API Reference",
             "-o",sourcedir+"/python/",
             pymoduledir)

        #delete the ones we dont need
        if not args.all:
            for rstfile in glob(sourcedir+"/python/*"):
                s= os.path.basename(rstfile).split('.')
                if s[0]=='icecube':
                    if s[1]!='rst' and not use_this_project(s[1]):
                        os.unlink(rstfile)
                else:
                    if s!=['modules', 'rst'] and not use_this_project(s[0]):
                        os.unlink(rstfile)
    
    if not args.no_doxygen:

        mkdir_p(doxygendir)

        for projectdir in glob(os.path.join(I3_SRC,"*")):
            project = os.path.basename(projectdir)

            if os.path.isdir(projectdir) and use_this_project(project):
                doxyfile = os.path.join(doxygendir,project+'.doxyfile')
                copy_replace(os.path.join(I3_SRC,"cmake","doxyfile.new.in"),
                             doxyfile,
                             {"@PROJECT_NAME@":project,
                              "@DOXYGEN_OUTPUT_PATH@":os.path.join(doxygendir,project),
                              "@CMAKE_CURRENT_SOURCE_DIR@":projectdir,
                             })
                queue.call(["doxygen",doxyfile])

        queue.wait()

    if not args.no_cpp:
        print("Generating C++ references from Doxygen XML")
        for doxygen_dir in glob(os.path.join(doxygendir,"*","xml")):


            project_name = os.path.basename(os.path.dirname(doxygen_dir))
            print("@",doxygen_dir,project_name)
            if not use_this_project(project_name):
                continue    
            outfilename = sourcedir+"/cpp/"+project_name+".rst"
            print("writing",outfilename)
            with open(outfilename,'wt') as f:
                f.write(cppautodoctxt.format(PROJECT_NAME=project_name,
                                             DOXYGEN_PROJECT_PATH=doxygen_dir))

                #symlink(os.path.join(I3_SRC,"cmake","meta-project-docs","source","cpp_reference.rst"),
                #os.path.join(sourcedir,x))

    if not args.no_inspect:
        print("Generating icetray reference from icetray-inspect")
        quick_rst = os.path.join(sourcedir,"icetray_quick_reference.rst")
        cmd = ["icetray-inspect",
               "--sphinx","--sphinx-references","--no-params",
               "--title=IceTracy Quick Reference",
               "-o",quick_rst]
        if args.projects:
            cmd += args.projects
        else:
            cmd += ["--all"]
        print("Writing", quick_rst)
        queue.call(cmd)
        
        inspectlibs = i3inspect.get_all_projects()

        for proj in inspectlibs:
            if not use_this_project(proj):
                continue
        
            rst_out= os.path.join(sourcedir,"inspect",proj+".rst")
        
            cmd = ["icetray-inspect",
                   proj,
                   "--sphinx","--subsection-headers","--sphinx-functions",
                   #--expand-segments,#"--verbose-docs" #these options might work in the future
                   "--title=",
                   "-o",rst_out]
            print("Writing",rst_out)
            queue.call(cmd)

            #symlink(os.path.join(I3_SRC,"cmake","meta-project-docs","source","icetray_reference.rst"),
            #    os.path.join(sourcedir,x))
        queue.wait()

        #some projects don't have any output,
        #delete these so the docs look nice
        for proj in inspectlibs:
            if not use_this_project(proj):
                continue
            rst_out= os.path.join(sourcedir,"inspect",proj+".rst")
            with open(rst_out) as f:
                filelength = len(f.read().strip())
            if filelength ==0 :
                os.unlink(rst_out)



    
    if not args.no_html:
        doctreedir = os.path.join(builddir,"doctrees")
        finaldir = os.path.join(builddir,args.build_type)
    
        mkdir_p(doctreedir)

        call("sphinx-build",
             #"-N",#no color
             "-a",#all
             "-j",str(args.j),
             "-b",args.build_type,
             "-d",doctreedir,
             "-E",sourcedir,
             #"-c",configdir,
             finaldir)
    
        if args.open:
            if args.build_type=='html':
                outfile = os.path.join(finaldir,"index.html")
            else:
                outfile = None

            if outfile:
                if sys.platform=='darwin':
                    opencmd = 'open'
                else:
                    opencmd = 'xdg-open'
                call(opencmd,outfile)
        
if __name__=="__main__":
    main()
