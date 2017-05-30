If you Provide Any Class-Specific new, Provide All of the Standard Forms (plain, in-place, and nothrow)
-------------------------------------------------------------------------------------------------------

If a class defines any overload of operator ``operator new``, it should provide overloads of all three of plain, in-place, and non-throwing ``operator new``.  If you don't, they'll be hidden and unavailable to users of your class.

Say you define a class-specific ``operator new:``:: 

    class C
    {
        static void* operator new(size_t, MemoryPool&);///hides three normal forms 
    } 

￼Then, if someone tries to write an expression as boring as plain old ``new`` C, the compiler will reject it on grounds that it can't find plain old ``operator new``. Declaring the ``C::operator new`` overload that takes a ``MemoryPool`` hides all other overloads, including the familiar built-in global versions::

    void* operator new(std::size_t); //plain new
    void* operator new(std::size_t, std::nothrow_t) throw(); //nothrow new 
    void* operator new(std::size_t, void*); //in-place new

Alternatively, perhaps your class provides a class-specific version of one of these three operators new. In that case, by declaring one of them your class will by default also mask the other two.

Prefer to have class C explicitly bring into scope all three of the standard variants of operator new. Normally, all should have the same visibility. (The visibility can still be made private for individual forms, such as if you want to explicitly disable the plain or non-throwing operator new, but the purpose of this Item is to remind you not to hide them inadvertently.)

Note that you should always avoid hiding in-place new because STL containers use it extensively.
