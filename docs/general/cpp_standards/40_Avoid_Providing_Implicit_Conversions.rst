Avoid Providing Implicit Conversions
------------------------------------

Constructors that can be called with one argument and are not declared ``explicit`` interact poorly with overloading and foster invisible temporary objects that pop up all over.  Conversions defined as member functions of the form ``operator T`` (where "T" is a type) are no better -- they interact poorly with implicit constructors and can allow all sorts of nonsensical code to compile.

.. highlight:: c++

Example::

    struct Bar
    {
        Bar();      // default constructor
        Bar( int ); // value constructor with implicit conversion
    };

    void func( const Bar& );

    Bar b;
    b = 1; // expands to b.operator=( Bar( 1 ));
    func( 10 ); // expands to func( Bar( 10 ));


``b = 1;`` is allowed and is bad because simple syntax hides potentially expensive operations - construction of temporary, copy, destruction of temporary.  To avoid this use explicit: ``explicit Bar( int );``

