Making a class serializable
===========================

.. highlight:: c++

Classes that are to make the round-trip to disk inside .i3 files need serialize()
methods. Pending more documentation here, see the `boost::serialization documentation <http://www.boost.org/doc/libs/1_57_0/libs/serialization/doc/index.html>`_
directly for general information on writing serialization methods, or follow
one of the many examples in project dataclasses.

.. note::

   Even though we've switched from boost::serialization to icecube::serialization,
   the docs linked previously are still valid.  That links to boost 1.57 docs,
   which is where we're frozen at.  A simple change of the namespace from boost
   to icecube should be pretty much all the change you'll need.

I3_SERIALIZABLE
---------------

This is a convenience macro that instantiates the serialization methods for its argument. In order to do its job, this macro must be located somewhere after the full definition of the serialization method for it's argument. Take I3Position.cxx for example::

 #include <icetray/serialization.h>
 #include <dataclasses/I3Position.h>

 // not here
 // I3_SERIALIZABLE(I3Position);

 template <class Archive>
 void
 I3Position::serialize(Archive& ar, unsigned version)
 {
   ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
   ar & make_nvp("X", x_);
   ar & make_nvp("Y", y_);
   ar & make_nvp("Z", z_);
 }

 // here.
 I3_SERIALIZABLE(I3Position);

How to write serialization methods
-----------------------------------

To be able to be written to or read from an i3 file, a serialization method
must be supplied.

A basic serialize method
^^^^^^^^^^^^^^^^^^^^^^^^

Writing a new class means writing a serialize method so that your
class or struct can be written to an i3 file.  Mostly this is done through example.

I use I3Hit as an example of the simplest case. I3Hit has only two data
members. A double and an integer.  In I3Hit.h::

 static const unsigned i3hit_version_ = 0;
 ....
 class Hit {
 ....
 private:
  double time_;
  int hitID_;
 ....
 }

Note that the only purpose of using a static const to specify the class version # (above) is to allow a check (see "if" statement in serialization method below) to ensure an obsolete executable does not attempt to handle a newer, likely incompatible, class version.
To be able to serialize this class' data members you only need to add the following lines in the class' private area in I3Hit.h::

 ....
 private:
  ...
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);
  ...

Then in the implementation of I3Hit.cxx::

 #include <dataclasses/BoostHeaders.h>  //Gets all the BOOST stuff needed

  template <class Archive>
  void serialize(Archive& ar, unsigned version)
  {
    if (version>i3hit_version_)
        log_fatal("Attempting to read version %zu from file but running version %zu of I3Hit class.",version,i3hit_version_);
    ar & make_nvp("Time", time_);
    ar & make_nvp("HitID", hitID_);
  }

  I3_SERIALIZABLE(I3Hit);


Each basic data type in the serialize method needs a "named value pair",
where you specify a name (which will also be the name in XML-ized output)
and the name of the variable from the class or structure

The I3_SERIALIZABLE() macro registers the class as something the
BOOST system can write out.

My class includes other classes or structures
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If your class has objects for data members and you want to serialize those as well you'll simply need to add a serialize method to those classes as well. Then they can be serialized just like primitive types. For example, say you want to add a data member to I3Hit of type I3Time. I3Time
already has a serialization method so we don't need to add one.

The relevant code for I3Hit then looks like::

  double time_;
  int hitID_;
  I3Time i3time_; //Our new data member


The serialization method is modified as shown below::


  template <class Archive>
  void serialize(Archive& ar, unsigned version)
  {
    if (version>i3hit_version_)
        log_fatal("Attempting to read version %zu from file but running version %zu of I3Hit class.",version,i3hit_version_);
    ar & make_nvp("Time", time_);
    ar & make_nvp("HitID", hitID_);
    ar & make_nvp("I3Time", i3time_); //I3Time's serialization method gets called here
    //NB: If I3Time did not have a serialization method we would have to add one to that class
  }

My object is an I3FrameObject (inherits from I3FrameObject)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If your class is an I3FrameObject (inherits from I3FrameObject), you need
to a little more in your serialize method.  To be able to live as
a standalone object in the Frame, an object must be an I3FrameObject.  Take the I3Particle class as an example::

 class I3Particle : public I3FrameObject
 {
  ...
 private:

  int ID_;
  int parentID_;
  int primaryID_;
  ...
 }

In the serialize method, you need to add one thing::

 template <class Archive>
  void I3Particle::serialize(Archive& ar, unsigned version)
  {
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("ID",ID_);
    ar & make_nvp("parentID",parentID_);
    ar & make_nvp("primaryID",primaryID_);
    ...
  }

Here, the I3FrameObject is serialized as a base_object using it's serialize
method and included in the serialized output of this class.

A few other things
^^^^^^^^^^^^^^^^^^

If you use a typedef to add other ways to represent your class, for example::

 typedef I3Vector<I3Particle> I3VectorI3Particle;

Then you need to make sure that this class is also serializeable.
Add to the bottom of I3Particle.cxx::

 I3_SERIALIZABLE(I3VectorI3Particle);

The BOOST system knows how to handle things like std::vectors and
std::maps, as long as the class they contain is serializable.  In
this case, I3Particle has a serialize method we already talked about,
so the typedef of vector<I3Particle> is now serializable with this one line.

.. caution::

   Once you specify a serializable name with I3_SERIALIZABLE that's that type's name forever more.  If you write a file with this::

    class C { ... }
    I3_SERIALIZABLE(C);

   then if you try to read it with this::

    class C { ... }
    typedef C TypedefofC;
    I3_SERIALIZABLE(TypedefofC);

   it wont work.

Schema Evolution or Class Versioning
-------------------------------------

Suppose you want to add a member variable to your class,
but you want the new code to be able to read old data files.
You do this by simply adding the macro I3_CLASS_VERSION
to your class.  By default the version of all classes is 0, so
the first modifcation will be version 1.

For example, you added "double new_var\_" to your class I3MyClass.
You would add the following lines to your class::

 static const unsigned i3myclass_version_ = 1;

 BOOST_CLASS_VERSION(I3MyClass,i3myclass_version_)

.. note::

   The static const assignment and the `BOOST_CLASS_VERSION` macro need
   to be in the header file (.h) that defines your class/structure.

...and modify the serialize method accordingly::

  template<class Archive>
    void I3MyClass::serialize(Archive& ar, unsigned version){
      ar & make_nvp("Position", position);
      //etc.
      if(version > 0){
       ar & make_nvp("NewVar",new_var_);
      }
    }

Check out the boost docs for more information.
http://www.boost.org/libs/serialization/doc/index.html
