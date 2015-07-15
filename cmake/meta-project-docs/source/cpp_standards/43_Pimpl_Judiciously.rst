Pimpl Judiciously
-----------------

Pimpl is an abbreviation of 'pointer to implementation'.

The idea of the Pimpl idiom is to give a class (for example 'Lizard') an opaque (smart) pointer to the actual implementation (for example 'LizardImpl'). The opaque (smart) pointer enables it to do a forward declaration of the implementation class only, without the compiler needing to know the actual type. This shortens build times. All the public member functions of the Pimpl class will (often) call the member functions of the implementation class with the same name.
 
The advantages of using the Pimpl idiom are:
* Shorten build times 
* Remove visibility of private member functions 
* Improved error handling and safety 
 
The disadvantage of using the Pimpl idiom is the cost of an extra level of indirection, so Pimpl judiciously.
