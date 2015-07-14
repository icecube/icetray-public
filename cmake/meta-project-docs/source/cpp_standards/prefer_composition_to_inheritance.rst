
Prefer composition to inheritance
---------------------------------

Inheritance is the second-tightest coupling relationship in C++, second only to friendship. Tight coupling is undesirable and should be avoided where possible. Therefore, prefer composition to inheritance unless you know that the latter truly benefits your design.

Inheritance is often overused. A sound rule of software engineering is to minimize coupling: If a relationship can be expressed in more than one way, use the weakest relationship that's practical.

Example of composition::

    class A { /* ... */ };

    class B {
        A a_; // A is a member
        // additional members of B

    public:
        // A remains hidden, its interface is only accessed internally
    };

Example of inheritance::

    class A { /* ... */ };

    class B: public A {
        // state and interface are inherited from A
        // additional members of B

    public:
        // all of A's interface is accessible from the outside
    };

Composition prevents that changes to the public interface of A affect clients of B.
