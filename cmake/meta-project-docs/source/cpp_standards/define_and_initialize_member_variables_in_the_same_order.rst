
Define and initialize member variables in the same order
--------------------------------------------------------

Agree with your compiler : Member variables are always initialized in the o
order they are declared in the class definition; the order in which you write
them in the constructor initialization list is ignored.  Make sure the constructor
code doesn't confusingly specify a different order.
