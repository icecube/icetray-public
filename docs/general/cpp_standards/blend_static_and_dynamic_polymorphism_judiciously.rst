Blend static and dynamic polymorphism judiciously
-------------------------------------------------

Both static and dynamic polymorphism have their uses.  Don't
overuse one when the other would be more appropriate.

Advantages of dynamic polymorphism
""""""""""""""""""""""""""""""""""

* Manipulation based on superset / subset relationship:

    A function that works on base objects also
    works on derived objects.

* Dynamic binding and separate compilation:

    Code can be compiled separately. So if one derived
    class changes, only it needs to be recompiled.

* Binary interfacing:

    Modules can be linked statically or dynamically.


Advantages of static polymorphism
"""""""""""""""""""""""""""""""""

* Manipulation based on syntactic and semantic interface:

    Interfaces are implicit, so any type (or type substitution)
    that fits a given syntax will work.

* Static type checking:

    All types are checked statically.

* Efficiency:

    Compile-time evaluation allows increased optimization.

