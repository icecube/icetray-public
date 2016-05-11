#!/usr/bin/env python

import argparse
         
parser = argparse.ArgumentParser(description='Generate Documentation for IceTray')
pgroup = parser.add_mutually_exclusive_group(required=True)
pgroup.add_argument('--projects', nargs='+',metavar='proj',
                    help='only generate documentation for these projects')
pgroup.add_argument('--all', action='store_true',
                    help='generate docs for all the projects (will take a long time)')
parser.add_argument('--no-project-docs', action='store_true',
                    help="don't generate docs from individual projects `docs` dir")
parser.add_argument('--no-python', action='store_true',
                    help="don't generate python API reference docs")
parser.add_argument('--no-cpp', action='store_true',default=True,
                    help="don't generate C++ API reference docs from doxygen XML")
parser.add_argument('--no-inspect', action='store_true',
                    help="don't generate IceTray reference docs using icetray-inspect")
parser.add_argument('--no-html',action='store_true',
                    help='convenience option to generate the html')
parser.add_argument('--open',action='store_true',
                    help='convenience option to open a browser with the output of the html')
args = parser.parse_args()

import os.path,sys,pkgutil,subprocess
from glob import glob
from icecube.icetray import i3inspect

def mkdir_p(dir):
    if not os.path.isdir(dir):
        print "making directory "+dir
        os.mkdir(dir)

def symlink(src,dst):
    if os.path.islink(dst):
        os.unlink(dst)
    print "linking {}->{}".format(src,dst)
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
    
I3_BUILD = os.environ["I3_BUILD"]
I3_SRC = os.environ["I3_SRC"]

outdir=os.path.join(I3_BUILD,"sphinx_build")
sphinxdir=os.path.join(I3_BUILD,"sphinx_src")
sourcedir = os.path.join(outdir,"source")

autodoctxt="""
.. automodule:: {}
    :members:
    :undoc-members:
    :show-inheritance:
"""

cppautodoctxt = """
{PROJECT_NAME} C++ API Reference
================================================================================

.. doxygenindex::
    :path: {DOXYGEN_PROJECT_PATH}/xml
"""
    
subprocess.call(['rm','-rfv',outdir])

mkdir_p(outdir)
mkdir_p(sourcedir)
mkdir_p(sourcedir+'/python')
mkdir_p(sourcedir+'/projects')
mkdir_p(sourcedir+'/inspect')
mkdir_p(sourcedir+'/cpp/')

symlink(os.path.join(sphinxdir,"source","conf.py"),
        os.path.join(sourcedir,"conf.py"))
symlink(os.path.join(I3_SRC,'docs'),
        os.path.join(sourcedir,"metaproject"))
for pyfile in ["plot_directive.py","script_directive.py",
               "sphinx_cmake.py","toctree_sort.py"]:
    symlink(os.path.join(I3_SRC,'cmake',"meta-project-docs",pyfile),
            os.path.join(outdir,pyfile))
symlinkdir(os.path.join(I3_SRC,"cmake","meta-project-docs","source"),sourcedir)

if not args.no_project_docs:
    print "symlink projocts' docs dir"
    for projdocdir in glob(os.path.join(os.environ["I3_SRC"],"*","resources","docs")):
        proj = projdocdir.split(os.sep)[-3]
        if not use_this_project(proj):
            continue
        
        if os.path.isdir(projdocdir):
            symlink(projdocdir,os.path.join(sourcedir,"projects",proj))

if not args.no_python:
    print "Generating Python references"
    pymoduledir = os.path.join(I3_BUILD,"lib","icecube")
    
    for pymodule in pkgutil.iter_modules([pymoduledir]):
        _, name, ispkg = pymodule
        
        if not use_this_project(name):
            continue
        
        modulepath = os.path.join(pymoduledir,name)        
        sublibs = ["icecube."+name]
        for a in pkgutil.walk_packages([modulepath]):
            sublibs.append("icecube."+name+"."+a[1])

        title = name+" Python API Reference"            
        txt = title + "\n"+len(title)*"="+"\n"
        for mod in sublibs:
            txt+=autodoctxt.format(mod)
            
        outfilename = sourcedir+"/python/"+name+".rst"        
        print "writing", outfilename
        with open(outfilename,'wt') as f:
            f.write(txt)

if not args.no_cpp:
    print "Generating C++ references from Doxygen XML"
    for doxygen_dir in glob(os.path.join(I3_BUILD,"docs","doxygen","*")):
        project_name = os.path.basename(doxygen_dir)
        if not use_this_project(project_name):
            continue
    
        if os.path.isdir(os.path.join(doxygen_dir,"xml")):
            outfilename = sourcedir+"/cpp/"+project_name+".rst"
            print "writing",outfilename
            with open(outfilename,'wt') as f:
                f.write(cppautodoctxt.format(PROJECT_NAME=project_name,
                                             DOXYGEN_PROJECT_PATH=doxygen_dir))

if not args.no_inspect:
    print "Generating icetray reference from icetray-inspect"        
    cpp_libs, python_libs, python_mods = i3inspect.get_inspectable_projects()

    inspectlibs = cpp_libs + python_libs + python_mods
    inspectlibs  = sorted(set([a.replace('-','_') for a in inspectlibs]))

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
        print "Writing",rst_out
        subprocess.call(cmd)

        #some projects don't have any output,
        #delete these so the docs look nice
        with open(rst_out) as f:
            filelength = len(f.read().strip())
        if filelength ==0 :
            os.unlink(rst_out)

    quick_rst = os.path.join(sourcedir,"icetray_quick_reference.rst")
    cmd = ["icetray-inspect",
           "--sphinx","--sphinx-references","--no-params",
           "--title=IceTracy Quick Reference",
           "-o",quick_rst]
    if args.projects:
        cmd += args.projects
    else:
        cmd += ["--all"]
    print "Writing", quick_rst
    subprocess.call(cmd)
    
if not args.no_html:
    doctreedir = os.path.join(outdir,"doctrees")
    htmldir = os.path.join(outdir,"html")
    
    mkdir_p(doctreedir)
    mkdir_p(htmldir)

    subprocess.call(["sphinx-build",
                     "-N","-a","-b","html",
                     "-d",doctreedir,
                     "-E",sourcedir,
                     htmldir])
    
    if args.open:
        if sys.platform=='darwin':
            opencmd = 'open'
        else:
            opencmd = 'xdg-open'
        subprocess.call([opencmd,
                         os.path.join(htmldir,"index.html")])
