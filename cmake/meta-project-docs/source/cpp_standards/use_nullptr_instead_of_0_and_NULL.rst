=======================================
Use `nullptr` instead of `0` and `NULL`
=======================================

Neither `0` nor `NULL` are by default of type pointer. 
Both will be interpreted as pointers if used in such a way, but the usage of `0` and `NULL` can cause problems in some cases. 

Example::
 
    void f(int);        // three versions of a function f
    void f(bool);
    void f(void*);

    f(0);               // call of f(int), not f(void*)
    f(NULL);            // might cause compilation error,
                        // or calls f(int)
    f(nullptr);         // calls f(void*)


You might also run into problems when you use templates. 
`0` and `NULL` will always be interpreted as `int` when a respective template is instantiated. 
`nullptr` on the other hand will always be interpreted as a pointer.     

Also: Try to avoid overloading with `int` and `pointers`, since some developers might still use `0` and `NULL` as pointers, which will cause problems when you overload functions in that way. 
