
Don't write unintentionally nongeneric code
-------------------------------------------

Commit to abstractions, not to details: Use the most generic 
and abstract means to implement a piece of functionality.
More generic code is more reusable and more resilient to
changes in its surroundings.

Some examples:

* Use `empty()` instead of `size() == 0`

* Write const-correct code using `const&` parameters

* Prefer iteration to indexed access

* Use the highest class in the hierarchy that offers what you need

Exceptions:

    Sometimes indexing is faster, but check before you blindly do it.
