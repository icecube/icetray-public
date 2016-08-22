Smart pointers
==============

Standard pointers, or *raw pointers*, in C++ do not indicate ownership: a
pointer does not tell you who owns the object the pointer is referring to, who
should delete it and how. In C++11, so called *smart pointers*, which wrap
around raw pointers, where introduced to overcome these limitations. They are
contained in the ``<memory>`` header; an alternative implementation ships with
Boost and is commonly used within the IceCube software framework.

There are three different types of smart pointers:

    * Exclusive ownership: ``unique_ptr``,
    * Shared ownership: ``shared_ptr``,
    * *Cyclic references*: break cycles with ``weak_ptr``.

Exclusive ownership
-------------------

The ``unique_ptr`` provides the semantics of strict ownership: it owns the
object it is referring to.

.. code-block:: cpp

    // initialize smart pointer with a raw pointer
    std::unique_ptr<double> p{new double{42.}};

The memory allocated for the double above is automatically released once the
pointer expires (goes out of scope). One consequence of C++'s definition of
strict ownership is that unique pointers cannot be copied, only moved.

.. note::

    Unique pointers accept a custom deleter class as a second template
    parameter, which is called to destroy the owned object.

Shared ownership
----------------

The ``shared_ptr`` provides the semantics of shared ownership: it manages
memory that is used in common by multiple parties. The shared ownership is
realized based on a reference counting system: the :cpp:func:`use_count` member
returns the number of shared pointers referring to the same managed object;
memory is automatically released as soon as no shared pointer is referring to
the data any longer; shared pointers can be copied as often as desired.

If possible, shared pointers should be created via ``make_shared``:

.. code-block:: cpp

    #include <iostream>
    #include <memory>

    struct Circle
    {
        Circle() : radius(0.) {};
        Circle(double rnew) : radius(rnew) {};
        double radius;
    };

    int main()
    {
        auto c = std::make_shared<Circle>(5.);
        std::cout << "Circle of radius: " << c->radius << std::endl;
    }

This way management and data are stored together in memory and the memory
caching is more efficient.

Temporary shared ownership
--------------------------

The ``weak_ptr`` gives you the possibility to temporary "control" an object
that is managed by shared pointers: it can be converted to a shared pointer via
the :cpp:func:`lock` member. This way the object's lifetime is extended until
the temporary shared pointer is destroyed. Weak pointers are useful in
situations where you do not need to own the object all the time except for
certain tasks.

Recommendations
---------------

#. Prefer unique pointers to shared pointers.
#. Prefer ordinary scoped objects to objects on the heap owned by a unique
   pointer.
