Vector Math
===========

I3Position and I3Direction
--------------------------

New operations:

* addition
* subtraction
* scalar multiplication
* dot products
* cross products
* magnitudes
* conversions

Works with normal math operators in C++ and Python.

example::

    I3Position p(1,2,3), q(4,5,6);
    std::cout << p+q << std::endl;
    // I3Position(5,7,9)
    std::cout << p-q << std::endl;
    // I3Position(-3,-3,-3)
    std::cout << 2*p << std::endl;
    // I3Position(2,4,6)
    std::cout << p*q << std::endl;
    // 32.0
    std::cout << I3Direction(p) << std::endl;
    // I3Direction(Zenith (deg): 143.301, Asimuth(deg): 243.435)
    
