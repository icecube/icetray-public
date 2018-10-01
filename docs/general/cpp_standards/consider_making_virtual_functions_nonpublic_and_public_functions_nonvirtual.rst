
Consider making virtual functions nonpublic, and public functions nonvirtual
----------------------------------------------------------------------------

In base classes with a high cost of change (particularly ones in libraries and frame-works): Prefer to make public functions nonvirtual. Prefer to make virtual functions private, or protected if derived classes need to be able to call the base versions.

A **public virtual** function inherently has two different and competing responsibilities:

* *It specifies interface*: Being public, it is directly part of the interface the class presents to the rest of the world.

* *It specifies an implementation detail*: Being virtual, it provides a hook for derived classes to replace the base implementation of that function; it is a customization point.

In general, one function cannot fulfill both responsibilities perfectly. Here is an example, where both responsibilities are separated::

    class A {
    public:
        something(/* args */) {
            // pre-conditions, e.g. check args; may be added later
            doSomething(/* args */);
            // post-conditions, e.g. check internal state; may be added later
        }
    protected:
        virtual doSomething(/* args */) = 0; // the customization point
    };

**Note:** This advice does not apply to destructors. Destructors should be public and virtual, if want to be able to delete an instance of a derived class with a base pointer.
