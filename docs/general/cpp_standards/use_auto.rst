.. highlight:: c++

Use Auto
========

When a variable is declared using auto, auto plays the role of ``T``
in the template, and the type specifier for the variable acts as
``ParamType``. This is easier to show than to describe, so consider
this example::
   
 auto x = 27;

Here, the type specifier for ``x`` is simply ``auto`` by itself. On
the other hand, in this declaration::
   
 const auto cx = x;

the type specifier is ``const auto``. And here::
   
 const auto& rx = x;

the type specifier is ``const auto&``. To deduce types for ``x``,
``cx``, and ``rx`` in these examples, compilers act as if there were a
template for each declaration as well as a call to that template with
the corresponding initializing expression.

Using ``auto`` can be particularly advantageous for complicated types
as found in IceCube Software such as iterators for container
classes.

.. code-block::
   
   auto it=mcTree.begin();

instead of::
   
   I3MCTree::const_iterator it=mcTree.begin();

In concept, ``auto`` is as simple as simple can be, but it’s more
subtle than it looks. Using it saves typing, sure, but it also
prevents correctness and performance issues that can bedevil manual
type declarations. Furthermore, some of ``auto``’s type deduction
results, while dutifully conforming to the prescribed algorithm, are,
from the perspective of a programmer, just wrong. When that’s the
case, it’s important to know how to guide auto to the right answer,
because falling back on manual type declarations is an alternative
that’s often best avoided.

As you can see, ``auto`` type deduction works like template type
deduction. They’re essentially two sides of the same coin. Except for
the one way they differ. We’ll start with the observation that if you
want to declare an ``int`` with an initial value of ``27``, C++98
gives you two syntactic choices::
   
  int x1 = 27;
  int x2(27);

C++11, through its support for uniform initialization, adds these::
   
  int x3 = { 27 };
  int x4{ 27 };
  
All in all, four syntaxes, but only one result: an ``int`` with value
``27``. There are advantages to declaring variables using ``auto``
instead of fixed types, so it’d be nice to replace int with ``auto``
in the above variable declarations. Straightforward textual
substitution yields this code::
   
  auto x1 = 27;
  auto x2(27);
  auto x3 = { 27 };
  auto x4{ 27 };

These declarations all compile, but they don’t have the same meaning
as the ones they replace. The first two statements do, indeed, declare
a variable of type ``int`` with value ``27``. The second two, however,
declare a variable of type ``std::initializer_list<int>`` containing a
single element with value ``27``!

.. code-block::
   
   auto x1 = 27;     // type is int, value is 27
   auto x2(27);      // ditto
   auto x3 = { 27 }; // type is std::initializer_list<int>
   auto x4{ 27 };    // ditto

Specific Recommendations:

- There is no need to replace initializers in functioning code with ``auto``.
- Prefer ``auto`` to explicit type declarations, auto variables must be initialized, are generally immune to type mismatches that can lead to portability or efficiency problems, can ease the process of refactoring, and typically require less typing than variables with explicitly specified types.
- Do not use ``auto`` with braced initializers, ``auto`` type deduction is usually the same as template type deduction, but ``auto`` type deduction assumes that a braced initializer represents a ``std::initializer_list``, and template type deduction doesn’t.
- Use the explicitly typed initializer idiom when ``auto`` deduces undesired types. “Invisible” proxy types can cause auto to deduce the “wrong” type for an initializing expression. The explicitly typed initializer idiom forces auto to deduce the type you want it to have.
