.. highlight:: c++

Avoid inheriting from classes that were not designed to be base classes
-----------------------------------------------------------------------

Classes meant to be used standalone (utility) obey a different
blueprint than base classes (See Item 32: Be clear what kind of class
you are writing).  Using a standalone (utility) class as a base class
is a serious design error and should be avoided.  Keep these principles in mind:

- To add behavior, prefer to add nonmember functions instead of member functions (See Item 44: Prefer writing nonmember nonfriend functions).
- To add state, prefer composition instead of inheritance (See Item 34: Prefer composition to inheritance).
- Avoid inheriting from concrete base classes.

Adding behavior
~~~~~~~~~~~~~~~

To add behavior, prefer to add nonmember functions instead of member
functions.  It is easier to build generic methods if they are not part
of a class.  For example, why is ``std::find`` not a member of
``std::vector``?  Because then it wouldn't work for lists, maps, etc.


Example::

    class Car
    {
        Wheel frontLeft;
        Wheel frontRight;
        Wheel rearLeft;
        Wheel rearRight;
        Wheel spareInTrunk;

    public:
        void wheelsOnCar( list< Wheel > &wheels )
        {
            wheels.push_back( frontLeft );
            wheels.push_back( frontRight);
            wheels.push_back( rearLeft);
            wheels.push_back( rearRight);
        }
        const Wheel & getSpare(){ return spareInTrunk; }
        void setSpare( const Wheel &newSpare ){ spareInTrunk = newSpare; }
    };

    void wheelsRelatedToCar( Car *aCar, list< Wheel > &wheels )
    {
        aCar->wheelsOnCar( wheels );
        wheels.push_back( aCar->getSpare() );
    }


Adding state
~~~~~~~~~~~~

To add state, prefer composition instead of inheritance.

Inheritance:
++++++++++++

The ``Employee`` "is a" ``Person`` or inherits from ``Person``. All inheritance
relationships are "is-a" relationships.

.. code-block::

    using std::string;

    class Person 
    {
	string Title;
	string Name;
	int Age;
    }

    class Employee : Person 
    {
	int Salary;
	string Title;
    }

Composition:
++++++++++++

Composition is typically "has a" or "uses a" relationship. Here the
``Employee`` class has a ``Person``. It does not inherit from ``Person`` but
instead gets the ``Person`` object passed to it, which is why it "has a"
Person.

.. code-block::

    using std::string;

    class Person 
    {
        string Title;
        string Name;
        int Age;

        public Person(string title, string name, string age) 
        {
            this.Title = title;
            this.Name = name;
            this.Age = age;
        }
    }

    class Employee 
    {
        int Salary;
        private Person person;

        public Employee(Person p, int salary) 
        {
            this.person = p;
            this.Salary = salary;
        }
    }

    Person johnny = new Person ("Mr.", "John", 25);
    Employee john = new Employee (johnny, 50000);

Composition over Inheritance:
+++++++++++++++++++++++++++++

Now say you want to create a Manager type so you end up with::

    class Manager : Person, Employee 
    {
    }


This example will work fine, however, what if ``Person`` and
``Employee`` both declared ``Title``? Should ``Manager.Title`` return
"Manager of Operations" or "Mr."? Under composition this ambiguity is
better handled::

    Class Manager 
    {
        public Title;
        public Manager(Person p, Employee e)
        {
            this.Title = e.Title;
        }
    }


The Manager object is composed as an ``Employee`` and a
``Person``. The ``Title`` behaviour is taken from employee. This
explicit composition removes ambiguity among other things and you'll
encounter fewer bugs.


Avoid inheriting from concrete base classes.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Inheriting from a class with a public non-virtual destructor risks
littering your code with underfined behavior by deleting pointers to
base class objects that actually point to the derived objects.
