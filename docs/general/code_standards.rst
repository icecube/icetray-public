.. _icecube-coding-standards:

IceCube Coding Standards
========================

.. image:: brazil.jpg

This is the third draft of the IceCube C++ Coding Standards. The first
draft was provided by Thomas Burgess, the second by Erik Blaufuss.  The
third is being expanded by the community.

Introduction
------------

These are coding standards and recommendations for developers of
IceCube software in C++ and Python.  There is a companion document for
:ref:`library_standards`.

This document refers primarily to offline software (including IceTray,
Dataclasses, Simulation and the event viewer).  Users are encouraged
to write their own private software to these standards, to make future
code maintence and the adoption of private code for production use
easier.

.. _python-coding-standards:

Python Coding Standards
-----------------------

Python coding standards follow the recommendations set forth in `PEP
0008 <https://peps.python.org/pep-0008/>`_. Where they cause
conflict, the C++ coding standards outlined below, take precedence.

.. _c-coding-standards:

C Coding Standards
------------------

The use of pure C is strongly discouraged.  This is not a statement about
the language, it's simply a choice we have to make based on the limited
resources available.  We have one compiled language (C++) and one
interpreted (Python) that covers everything we need.  We strive to ensure
that all production code is accessible to every member. C++ is no longer
"C with classes" but arguably a separate language.  At the very least,
coding styles differ greatly between the two communities and we have our
hands full with the two official languages we already have.

Should you find it necessary to write something in pure C, we ask that
you follow the Linux kernel coding style.

`Linux Kernel Coding Style <https://www.kernel.org/doc/Documentation/CodingStyle>`_

.. _c++-coding-standards:

C++ Coding Standards
--------------------

Good coding standards offer many interrelated advantages:

* *Improved code quality:* Encouranging developers to do the
  right things in a consistent way directly works to improve software
  quality and maintainability.
* *Faster development:* Developers dont need to always make
  decisions starting from first principles.
* *Better Teamwork:* They help reduce needless debates on
  inconsequential issues and make it easier for teammates to read and
  maintain each other's code.
* *Uniformity in the right dimension:* This frees developers
  to be creative in directions that matter.

Code reviews will make constant reference to *C++ Coding Standards* [#]_, by Herb Sutter and Andrei Alexandrescu.  If you submit code that
gets reviewed, you should have access to a copy: reviewers will refer
to the book, and everybody will save time if you can read the full
details/explanations yourself.

This document borrows pieces from the book's structure, and you will find direct
quotes from the book throughout (like the opening paragraph of this
section).  Each of the headings below corresponds to a several-page
long chapter in the book, where one can find Sutter's treatment, so
look there for more information.  Icecube-specific elaborations,
modifications, and excerpts from code reviews are found here.  You are
*not* expected to memorize them or to submit code in full compliance
with every single rule, as these are goals to strive for, and many are
achievably only in degree.  Do your best, and we will refer to this
document in reviews.

Organizational and Policy Issues
--------------------------------
.. toctree::
   :titlesonly:

   cpp_standards/dont_sweat_the_small_stuff
   cpp_standards/compile_cleanly_at_high_warning_levels
   cpp_standards/use_a_version_control_system
   cpp_standards/invest_in_code_reviews
   cpp_standards/temp_test_dir

Design Style
------------

.. toctree::
   :titlesonly:
   
   cpp_standards/dont_optimize_prematurely
   cpp_standards/minimize_global_data

Coding Style
------------

.. toctree::
   :titlesonly:

   cpp_standards/use_curly_braces
   cpp_standards/declare_variables_as_locally_as_possible
   cpp_standards/make_header_files_self_sufficient
   cpp_standards/always_write_internal_include_guards

Functions and Operators
-----------------------

.. toctree::
   :titlesonly:

   cpp_standards/take_parameters_appropriately
   cpp_standards/order_of_evaluation_of_function_args

Class Design and Inheritance
----------------------------
.. toctree::
   :titlesonly:

   cpp_standards/be_clear_what_kind_of_class_youre_writing
   cpp_standards/prefer_composition_to_inheritance
   cpp_standards/35_Avoid_Inheriting_From_Classes_Not_Base
   cpp_standards/consider_making_virtual_functions_nonpublic_and_public_functions_nonvirtual
   cpp_standards/40_Avoid_Providing_Implicit_Conversions
   cpp_standards/dont_give_away_your_internals
   cpp_standards/make_data_members_private_except_in_behaviorless_aggreggates
   cpp_standards/43_Pimpl_Judiciously
   cpp_standards/44_Prefer_Writing_Nonmember_Nonfriend_Functions
   cpp_standards/always_provide_new_and_delete_together
   cpp_standards/46_If_You_Provide_Any_Class_Specific_new_Provide_Standard_Forms

Construction, Destruction, and Copying
--------------------------------------
.. toctree::
   :titlesonly:

   cpp_standards/define_and_initialize_member_variables_in_the_same_order

Namespaces and Modules
----------------------
.. toctree::
   :titlesonly:

   cpp_standards/dont_write_namespace_usings

Templates and Genericity
------------------------
.. toctree::
   :titlesonly:

   cpp_standards/dont_write_unintentionally_nongeneric_code
   cpp_standards/blend_static_and_dynamic_polymorphism_judiciously
   cpp_standards/use_auto
   cpp_standards/lambda_expressions 

Error handling and Exceptions
-----------------------------
.. toctree::
   :titlesonly:

   cpp_standards/assert_liberally


STL: Containers
---------------

.. toctree::
   :titlesonly:

   cpp_standards/use_vector_by_default
   cpp_standards/use_vector_and_string_instead_of_arrays
   cpp_standards/use_vector_to_exchange_data_with_non_cpp_APIs
   cpp_standards/store_only_values_and_smart_pointers_in_containers

STL: Algorithms
---------------

.. toctree::
   :titlesonly:

   cpp_standards/use_checked_STL_implementation

Type Safety
-----------

.. toctree::
   :titlesonly:

   cpp_standards/dont_use_C_style_casts

Modern C++
----------------------
.. toctree::
   :titlesonly:

   cpp_standards/prefer_alias_declaration_over_typedef
   cpp_standards/use_nullptr_instead_of_0_and_NULL
   cpp_standards/smart_pointers


Useful links
------------

Bjarne Stroustrup's C++ pages <https://www.stroustrup.com/>
    The pages of the creator of C++ 

Effective C++ <https://www.aristeia.com/books.html>
    Effective C++ and More Effective C++ contains many very useful items
    for C++ programmers. 

C/C++ reference <https://www.cppreference.com/>
    General C/C++, standard C libraries, C++ I/0, strings and STL
    reference. 

C++ FAQ lite <http://www.parashift.com/c++-faq-lite/>
    Many useful answers on most C++ topics 

C++ Notes <http://www.fredosaurus.com/notes-cpp/index.html>
    Small useful examples illustrating basic C++ usage. 

Doxygen <https://doxygen.nl>
    Generates code documentation from source code comments 

Also handy for C++ programming GCC <http://www.gnu.org/software/gcc/>
    GNU Compiler Collection - many compilers, including the g++ C++
    compiler! 



.. rubric:: Footnotes

.. [#] The *C++ Coding Standards* homepage can be found at http://www.gotw.ca/publications/c++cs.htm
