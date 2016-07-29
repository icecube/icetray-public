
Prefer alias declaration over typedef
---------------------------------------------------------------------------

In C++98 you can use ``typedef`` in order to set your own name to a type that you don't want to write out (perhaps it's very long).
An example:

  typedef double your_name;
  typedef double your_name[10];
  typedef std::unique_ptr<std::unordered_map<std::string, std::string>> UPtrMapSS;

However, in C++11 you can switch to using alias declarations instead, using the ``using`` keyword:

  using your_name = double;
  using your_name = double[10];
  using UPtrMapSS = std::unique_ptr<std::unordered_map<std::string, std::string>>;

Another example is to set your type as a synonym to a function:

  // func is a synonym to a function that takes a float and an int as arguments and returns a float
  typedef float func(float, int);   // typedef
  using func = float (float, int);  // alias declaration

  // func_point is a synonym to a pointer to a function that takes an int and a const std::string& and returns nothing
  typedef void (*func_point)(int, const std::string&);   // typedef
  using func_point = void (*)(int, const std::string&);  // alias declaration

So far the only advantage of ``using`` over ``typedef`` is to increase readabilty.
However, you can also use ``using`` directly in a template, which is not true for ``typedef``:

  // MyAllocList<T> is a synonym for std::list<T, MyAlloc<T>>
  template<typename T>
  using MyAllocList = std::list<T, MyAlloc<T>>;

  // vector and matrix are synonyms for a tensor of order 1 and 2, respectively
  template <unsigned Order, typename Value>
  class tensor { ... };

  template <typename Value>
  using vector = tensor<1, Value>;

  template <typename Value>
  using matrix = tensor<2, Value>;

**Important:**

There are additional reasons why using ``typedef`` could make the code confusing both for
humans and compilers, but in short **do not use ``typedef``, use alias declarations with ``using`` instead!**


Examples taken from:
S. Meyers, *Effective Modern C++*
P. Gottschling, *Discovering Modern C++*
