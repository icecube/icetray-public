Code Reviews
============

The purpose of these guidelines is to give some guidance as to what should be 
considered in a code review. This helps in creating reviews of a consistent style 
and quality. These guidelines are not fixed, however, and may change as we gain 
experience with code reviews. If you are still uncertain whether code reviews 
are really necessary, please read http://www.nature.com/nature/journal/v467/n7317/pdf/467775a.pdf.

In the following, various items that reviewers should pay attention to, are listed.

Method of Code Review Process
-----------------------------

In general, a code review should be communicated and published on the software mailing list software@icecube.wisc.edu.  
A copy of the reviews (preferably is rst) should be include in the projects 'resources/code_review' directory.  This
allows us to easily locate and add to in for future reviews.

Introduction
------------

Code review work is divided into four roles :
 - **Author**
 - **Sponsor**
 - **Consumers**
 - **Everyone Else** 

If you are involved in a review process, think about what role(s) you are most likely assuming and 
look at what your tasks are.

The Author
..........

First, we have the **Author**. The Author (or Authors) is obviously the author of the project 
that is getting reviewed. The Author is responsible for ensuring that their code is ready to be reviewed.
Tests have been written. Documentation has been written. Coding Standards have been followed.

The Sponsor
...........

Next, we have the **Sponsor**. The Sponsor is responsible for ensuring that the Author has done their 
job (Tests, docs, code standards, etc), for collating the responses and comments of the review, and for 
making sure the Author implements them. Finally the Sponsor is responsible for ensuring there are 
enough "sign offs" to finalize and close the review. Sponsors will get a "Signed-off by:" line in the 
SVN commit.

The Author, of course, can be self-sponsored. But, ideally this role will be taken on by someone other 
than the Author. Additionally, the Sponsor can be a primary **Consumer**.

Consumers
.........

**Consumers** are those projects/work/good-night's-sleep/lives will be effected by promotion and adoption 
of the project under review. This is the primary group of people who should be involved in the review process 
(but often aren't). The review is the time to raise objections, not after promotion, adoption and implementation.

Everyone Else
.............

Finally, we have **Everyone Else**. This is, well, everyone else. Code reviews are the responsibility of 
everyone who uses any code at all. We are in a collaboration after all. Additionally, code reviews are a 
fantastic way to get involved, and/or get your feet wet with IceCube code. Are you super organized, and 
can herd cats like no one else? You can help! Are you annoyed by the quality of some of the documentation? 
Here's your chance to make sure good docs get included! Are you new to IceCube and confused about why we 
do some of the things we do? Here's your chance to learn the how's and the why's!

Examples
--------

Project CommonVariables
.......................
 - Author: Martin Wolf
 - Sponsor: Emanuel Jacobi (assisted by Don)

A Consumer should download and see how CommonVariables will help their project. When they see that 
CommonVariables::Foo() breaks their Widget::Bar() they should send an email to dataclasses 
(referencing the code review) that demonstrates the breakage. This can then be used fix a bug in either 
project or as a new way of achieving Foo().

An 'Everyone Else' who likes clear documentation can send a patch that reformats Martin's docs so that 
they're more clear to a general user.

Project pybdt
.............
 - Author: Mike Richman
 - Sponsor: Andreas Homeier (as a primary Consumer, and assisted by Don)

Andreas, who wants to run pybdt at pole, needs to help Mike ensure that his code runs as expected on 
the pole test systems. In the mean time he needs to ensure that other Consumers are looking at Mike's code too.

Points to be addressed during Code Review (mostly by the Sponsor)
-----------------------------------------------------------------

First glance
............
When you check out the project, note down the path and revision you are reviewing. This is important because 
it allows others to understand what you are writing about.

Completeness of the project:
 * Documentation (doxygen and/or rst)
 * Example scripts
 * Unit tests and/or test scripts. Unit tests are preferred, test scripts are optional, but can act as example scripts.

Then check:
 * Does the code compile without warnings in both debug and release mode?
 * Do the tests compile without warnings?
 * Do all tests succeed?


Documentation
.............
Read the documentation provided with the source code. Does it explain in reasonable detail what the project 
does? Is it understandable for non-experts? In case further physics documentation is available for example 
in the form of an internal report, it should be linked in the documentation.

An important question to answer at that point is, whether the project provides some new and useful algorithm. 
Does the whole thing make sense or is it already obsolete? E.g. we probably don't need any more icepicks.

Look at the output of **icetray-inspect**. Are the docstrings useful for a person reasonably acquainted 
with the purpose of the project? Is the usage clear?


Source Code
...........
Directory Structure
...................
The code should be organized in the following way:
 * public/<project_name> - All *public* header files of the project should go into this directory. Header files for internal use do not belong in here. This directory is optional: if there are no public header files, it does not need to exist.
 * private/<project_name> - The project's implementation goes into this directory. Conventionally, tableio converters belong into private/<project_name>/converter.
 * private/pybindings - This is the directory for the python bindings.
 * private/test - Unit tests go in here.
 * python - All pure python library code goes in here. If it exists, it must contain an __init__.py file that also loads the C++ pybindings library and the project library itself.
 * resources/scripts - This is the directory for utility scripts. 
 * resources/test - Python test scripts go into this directory. Scripts to be run as tests must work without command line parameters.
 * resources/examples - All other scripts should be put into this directory. However, it is not a dumping ground for people's scripts. Only useful, commented, example scripts should be here.
 * resources/docs - All documentation should be placed here (as the name suggests). Plain text README files are not useful since they will not show up on automatically generated documentation pages. The preferred form of documentation is either rst or Doxygen. This is also not a good place for pdf files and other reports. 

Structure
.........
Is the code well-structured? Every function should fulfil one specific purpose and endless Physics
routines are discouraged. Ideally Physics only puts the different pieces together: get the data from 
the frame, call functions to do the calculations, put the results into the frame.

Dead code: Blocks of code that are commented out or disabled by "#if 0 ... #endif" blocks should be removed. 
They are disruptive and potentially dangerous.

Implementations go into .cxx files, not headers. With the exception of template code and maybe one line 
get/set methods. Header files that are not part of the module interface (e.g. classes/functions used 
internally by the module) go into the private directory.

Authors should be encouraged to write python bindings and tableio converters for their data classes.

Coding standards
................
Is the code in accordance with our coding standards? Consistent naming of variables, classes, etc across 
projects helps people when they read other peoples code. Since transparency is important in an experiment 
like IceCube, it is essential that physicists can understand what their colleagues did.

This also means that variables should have meaningful names: rpn, agv, etc are not useful. As the coding 
standards say, normal IceCube abbreviations like DOM, ATWD, etc are okay.

Also, keep an eye on the use of logging statements. Excessive logging will unnecessarily increase the size 
of processing log files and make it hard to find important error or warning messages. In situations that 
may occur during processing (i.e. almost everything but configuration errors) the code should not log_fatal().

Build time failures should be preferred over runtime failures.

In general the project should be written in an reasonable C++ style.

Readability
...........
Can you follow the logic of the code? If you cannot, your colleagues will very likely also be unable to 
understand the code. Although some people argue that excessive commenting can disrupt the code, some comments 
can actually improve the readability and structure of the code. Obvious things don't need to be commented, though.

Could readability be improved in some way? Code duplication should be avoided both because it is bad for 
readability and because it is a potential source of error.

Are error and warning messages understandable?

Usability
.........
This concerns mostly input parameters. Some modules have an awful lot of parameters. Not all of them might 
be necessary. Other modules use bitmasks as parameters. These are extremely hard to understand, error prone, 
and thus from a usability point of view absolutely **verboten!!!**

Potential sources of error
..........................
Although the task of the code review is not to review the physics of a project, you should still report 
errors if you see them. Also, if you see potential bugs in the code, please report them. Here is a list
of typical problems:
 - When objects are retrieved from the frame, their existence needs to be checked.
 - PushFrame() needs to be called before ''every'' return from Physics.
 - Unsafe code: Some operations have undefined results that may compile fine and work perfectly in some circumstances but fail e.g. at different optimization levels or if compiled with a different compiler.
 - Use of C arrays: 
  - Prefer STL containers.  If code uses C-arrays ask the author to justify their choice. 
  - Do loops run over the end of the array? 
  - Character arrays and C string functions: Why not use std::string? Is there enough room at the end of the string for the trailing '\0'?

Conclusions
...........
Do you have ideas to further improve the project? Can specific code be made more general (e.g. by using 
templates to treat both I3RecoHits and I3RecoPulses)? Do you have suggestions for further unit tests?
