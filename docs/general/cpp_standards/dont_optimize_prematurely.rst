Don't Optimize Prematurely
--------------------------

As Donald Knuth [quoting Hoare] said:

  "Premature optimization is the root of all evil."
  
Premature optimization is:

* making designs/code more complex (and less readable) than it needs to be

* when the effort is not justified by proven performance need

* adds no value

Always remember, "it is far, far easier to make a correct program fast
than it is to make a fast program correct."

So when writing code:

1. Focus on readability and correctness.

2. Measure whether optimization is needed.

3. *Only if justified*, add optimization.

Also remember that the compiler is good at basic optimizations and can do
them for you.  Do not sacrifice readability for something the compiler
can do.
