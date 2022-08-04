.. _icecube-code-reviews:

Code Reviews
============

The purpose of these guidelines is to give some guidance as to what should be 
considered in a code review. This helps in creating reviews of a consistent style 
and quality. These guidelines are not fixed, however, and may change as we gain 
experience with code reviews. If you are still uncertain whether code reviews 
are really necessary, please read https://www.nature.com/articles/467775a.

In the following, various items that reviewers should pay attention to, are listed.

Method of Code Review Process
-----------------------------

In general, a code review should be communicated and published on the software mailing list software@icecube.wisc.edu.  
A copy of the reviews (in ReST) should be include in the projects 'resources/docs/' directory and linked to the 
project's ReST docs.  This allows us to easily locate and add to it for future reviews.

Introduction
------------

FAQ: What does and does not need to be reviewed? 

A:  At the very least all projects that are going to be part of a production meta-project (i.e. offline, 
simulation, or icerec) needs to be reviewed, regardless of the language ( C++, pyython, etc...).


Ten Commandments
----------------
.. _The 10 commandments of navigating code reviews: https://techbeacon.com/app-dev-testing/10-commandments-navigating-code-reviews

Stolen from `The 10 commandments of navigating code reviews`_ by Angie Jones, paraphrased.

1. Don't take it personnally
2. Don't marry your code
3. Consider all feedback
4. Articulate your rationale
5. Be willing to compromise
6. Contribute to others' code reviews
7. Treat submitters how you'd like to be treated
8. Don't be intimidated by the number of comments
9. Don't repeat the same mistakes
10. Embrace the nitpickers (within reason)
    
With respect to the 10th commandment, I think it's worth asking reviewers not to be nitpicky.
Keep comments constructive and helpful.  Don't sweat the small style and don't try to legislate
style, except when the chosen style hinders readability.  Focus on correctness, readability,
and maintainability.

    
Points to be addressed during Code Review
-----------------------------------------

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
