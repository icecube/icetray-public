#!/usr/bin/env python
import sys, subprocess

def generate_dictionary(svn_externals):
    '''
    Takes in the output of an 'svn propget svn:externals <path>
    command and loads a dictionary where the key is the project
    name and the data element is the rest of the split.
    This will contain either one or two members depending on
    whether the path has an specified revision number or not.

    With revision numbers:
    <project_name> -r<revision_number> <project_url>
    
    Without revision numbers:
    <project_name> <project_url>

    Returns : {<project_name> : [...]}
    
    '''
    return_dict = dict()
    for line in svn_externals.split('\n'):
        if line.strip() == "" or line.startswith("#"): 
            continue
        return_dict[line.split()[0]] = line.split()[1:]
    return return_dict        

def get_revision_number(meta_project):
    '''
    This just calls 'svn info' and parses the output looking for
    the revision numbers.

    Returns : <revision_number>.
    '''
    svn_info = subprocess.check_output(["svn", "info", meta_project])
    for line in svn_info.split('\n'):
        if line.startswith("Revision:"):
            return int(line.split()[1])

def get_path_revision_number_pair(svn_info):
    '''
    Expects a list of everything except beyond the project name
    from the externals.  If the revision number is specified then
    just cast to an int and return.  Otherwise get the revision
    number from the repository.

    Returns : (<path>, <revision_number>)
    '''
    if len(svn_info) == 1:
        return svn_info[0], get_revision_number(svn_info[0])
    if len(svn_info) == 2:
        return svn_info[1], int(svn_info[0].strip("-r"))
        
if len(sys.argv) != 3:
    print("You must specify two arguments.")
    print("usage = meta-project-diff.py <meta-project-1> <meta-project-2>")
    print("        <meta-project-1> and <meta-project-2> are passed directly to svn,")
    print("        so anything you can pass to 'svn pg svn:externals' is valid.")
    sys.exit(1)

arg1_dict = generate_dictionary(subprocess.check_output(["svn","pg","svn:externals", sys.argv[1]]))
arg2_dict = generate_dictionary(subprocess.check_output(["svn","pg","svn:externals", sys.argv[2]]))

# check to see which projects the two meta-projects don't have in common.
for project_name in set(arg1_dict.keys()) ^ set(arg2_dict.keys()):
    if project_name in arg1_dict:
        print("Only in %s: %s" % (sys.argv[1], project_name))
    if project_name in arg2_dict:
        print("Only in %s: %s" % (sys.argv[2], project_name))
               
# now generate the diff checking only the projects the two meta-projects have in common
for project_name in set(arg1_dict.keys()) & set(arg2_dict.keys()):
    if arg1_dict[project_name] != arg2_dict[project_name]:
        # either the revision number or path (or both) have
        # changed so generate the diff and print it.
        svn_diff = subprocess.check_output(["svn","diff",
                                            "%s@%d" % get_path_revision_number_pair(arg1_dict[project_name]),
                                            "%s@%d" % get_path_revision_number_pair(arg2_dict[project_name])])
        print svn_diff
