IceCube Software Development
============================

What to Expect When You're Expecting to Pass a Code Review
----------------------------------------------------------
This section is geared towards new (to software development) students and postdocs who are 
working on a implementation they hope to get into production; be it simulation, reconstruction, 
or standard processing.  The project is going to have to pass a code review at some point.  
Below are some useful tips to hopefully make that process smoother and increase the chances 
of passing review.

Development Tips
----------------

 - **Prototype in Python** - Even the most experienced C++ developers are going to be slower hacking C++ than python.  When coming up with a good design, you're not likely at all to get it right on the first shot.  You should be prepared to throw the first prototype away and start from scratch.  It's much easier to move quickly in python.  A common problem that's seen is a "first design" is kept and hacked on simply due to the effort it took to create it. 

 - **Get Feedback During the Process** - Don't code in a vacuum.  There are a lot of experienced developers on IceCube who are more than willing to help out with constructive feedback.  Get their input as often as possible.  The best place to go is our Slack channel.

 - **Write Tests as you Go** - This can't be stressed enough.  One of the criteria for passing code review is "decent" test coverage.  Many times developers request a code review and plan to write tests during the review.  Tests are not simply needed to placate reviewers.  They're a useful development tool and should a natural part of the development process.  If you find yourself writing a method or function that's difficult to test this points to a bad design.  If you're testing as you go this can be caught early on.

 - **Document as you Go** - This couples to tests and good design.  If you can clearly explain what a function, method, class, etc...does you should be able to write a test that illustrates it does its job.  If you find this difficult to do you should probably rethink your design.


Common IceCube Violations
-------------------------

Do yourself a favor and have your PI spring for a copy of "C++ Coding Standards : 101 Rules, 
Guidelines, and Best Practices" by H. Sutter and A. Alexandrescu.

Depending on your project a few (to many) of the coding standards may not apply. For instance 
the section on Design and Inheritance; most people don't design their own classes,
so I've made a list of the main violations that reviewers should look for.

Organizational and Policy Issues
................................
* **#1 Compile cleanly at high warning levels**

Design Style
............
* **#5 Give one entity one cohesive responsibility.**
* **#6 Correctness, simplicity, and clarity come first.**
* **#10 Minimize global and shared data.**

Coding Style
............
* **#15 Use "const" proactively.**
* **#17 Avoid magic numbers.**
* **#18 Declare variables as locally as possible.**
* **#19 Always initialize variables.**
* **#20 Avoid long functions. Avoid deep nesting.**
* **#23 Make header files self-sufficient.**

Functions and Operators
.......................
* **#25 Take parameters appropriately by value, (smart) pointer, or reference.**

Construction, Destruction, and Copying
......................................
* **#48 Prefer initialization to assignment in constructors.**

Namespaces and Modules
......................
* **#59 Don't write namespace 'using's in a header file before an #include.**

Error Handling and Exceptions
.............................
* **#68 Assert liberally to document internal assumptions and invariants.**
* **#69 Establish a rational error handling policy, and follow it strictly.**

STL: Containers
...............
* **#76 Use 'vector' be default.  Otherwise, choose and appropriate container.**
* **#77 Use 'vector' and 'string' instead of arrays.**
* **#78 Use 'vector' (and string::c_str) to exchange data with non-C++ APIs.**
* **#81 Prefer range operations to single-element operations.**

STL: Algorithms
...............
* **#83 Use a checked STL implementation.**
* **#84 Prefer algorithm calls to handwritten loops.**
* **#85 Use the right STL search algorithm.**

Type Safety
...........
* **#95 Don't use C-style casts.**
