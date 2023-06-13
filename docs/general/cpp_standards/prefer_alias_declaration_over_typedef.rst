.. highlight:: c++

Prefer alias declaration over typedef
-------------------------------------

The Gist
~~~~~~~~

If you do not want to write out long and tedious type names (like ``std::unique_ptr<std::unordered_map<std::string, std::string>>``),
or short and easy type names (like ``double``) for that matter, you can give them a name of your own.
The best way to do this is to use an alias declaration (since C++11) with the keyword ``using``.
Earlier, one would use ``typedef`` for this purpose, but this is not recommended anymore.


Using Alias Declarations
~~~~~~~~~~~~~~~~~~~~~~~~

How to use alias declarations as synonyms for other types::
   
    using mydouble   = double;
    using mydouble10 = double[10];
    using UPtrMapSS  = std::unique_ptr<std::unordered_map<std::string, std::string>>;

You can also use an alias declaration as a synonym for a function, or pointer to a function::
   
    // myfunc is a synonym to a function that takes a float and an int as arguments and returns a float
    using myfunc = float (float, int);
    // myfuncptr is a synonym to a pointer to a function that takes an int and a const std::string& and returns nothing
    using myfuncptr = void (*)(int, const std::string&);

Alias declaratiuons can also be templatized (then called alias templates), unlike ``typedef``::
   
    // vector and matrix are synonyms for a tensor of order 1 and 2, respectively
    template <unsigned Order, typename Value>
    class tensor { ... };
    template <typename Value>
    using vector = tensor<1, Value>;
    template <typename Value>
    using matrix = tensor<2, Value>;

Then a vector or matrix could be declared::
   
    vector<...> myvector;
    matrix<...> mymatrix;

Additionally, vectors and matrices can be created inside templates::
   
    template<typename T>
    class mywidget{
    private:
        vector<T> myvector;
        matrix<T> mymatrix;
        ...
    };


Hacking templatization with typedef
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Firstly: **Don't do this!**

A template with a ``typedef`` could certainly be hacked together, using a ``typedef`` nested inside a struct::
   
    template<typename Value>
    struct vector{
        typedef tensor<1, Value> type;
    };

In order to use the newly defined type called vector one would have to add the ``::type`` suffix::
   
    vector<...>::type myvector;

Additionally, when creating a vector inside a template it must be preceeded by ``typename`` in order for the compiler to udnerstand that it actually represents a type::
   
    template<typename T>
    class mywidget{
    private:
        typename vector<T>::type myvector;
    ...
    };

This leads us into the problem with hacking together templates with ``typedef``, namely that
the compiler doesn't know if ``vector<...>::type`` refers to an actual type or something else, e.g. a data member.

Please see *Meyers*, pp 63-67, for further explanations.


Readability difference
~~~~~~~~~~~~~~~~~~~~~~

There is also a (subjective) readability improvement when using alias declarations over ``typedef``, especially when it comes to functions.
Below are the alias declaration examples from above, but rewritten to use ``typedef``::
   
    typedef double mydouble;
    typedef double mydouble[10];
    typedef std::unique_ptr<std::unordered_map<std::string, std::string>> UPtrMapSS;
    // myfunc is a synonym to a function that takes a float and an int as arguments and returns a float
    typedef float myfunc(float, int);
    // myfuncptr is a synonym to a pointer to a function that takes an int and a const std::string& and returns nothing
    typedef void (*myfuncptr)(int, const std::string&);


Examples taken from
~~~~~~~~~~~~~~~~~~~

S. Meyers, *Effective Modern C++*

P. Gottschling, *Discovering Modern C++*
