#!/usr/bin/env python
import sys
import subprocess

def generate_dictionary(svn_dump):
    return_dict = {}
    for line in svn_dump.split('\n'):
        if line.strip() == "" or line.startswith("#"): 
            continue
        return_dict[line.split()[0]] = line.split()[1:]
    return return_dict

first = sys.argv[1]
second = sys.argv[2]

first_output = subprocess.check_output(["svn","pg","svn:externals", first])
second_output = subprocess.check_output(["svn","pg","svn:externals", second])

first_dict = generate_dictionary(first_output)        
second_dict = generate_dictionary(second_output)

for project_name, svn_info in second_dict.iteritems():
    if project_name not in first_dict:
        print("project %s added." % project_name)
    else:
        if svn_info != first_dict[project_name]:
            # something changed
            first_revision_number = int(svn_info[0].strip("-r"))
            first_path = svn_info[1]
            second_revision_number = int(first_dict[project_name][0].strip("-r"))
            second_path = first_dict[project_name][1]
            svn_diff = subprocess.check_output(["svn","diff",
                                               "%s@%d" % (first_path, first_revision_number),
                                               "%s@%d" % (second_path, second_revision_number)])
            print svn_diff
