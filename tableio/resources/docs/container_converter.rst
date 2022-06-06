.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$

Writing a converter for an I3Map or an I3Vector
***************************************************

.. highlight:: c++

In order to simplify the creation of converters for objects of type I3...Series
or I3...SeriesMap (i.e. I3Map<OMKey, std::vector<...> >), two converter
templates have been implemented in C++: I3VectorConverter<T> and
I3MapOMKeyVectorConverter<T>. The template parameter T is either a simple struct
to convert the stored data as defined below, or an existing I3Converter
implementation. If the objects stored in the I3Vector are I3FrameObjects, then
an I3Converter for those is prefered or might already exist. If it is not
an I3FrameObject, the convert-struct approach has to be used. This struct needs
to fulfill the following requirements:

* define a type called booked_type, which is the type converted by
  this struct;
* has two functions
  ::
 
    void AddFields(I3TableRowDescriptionPtr);
    void AddFields(I3TableRowDescriptionPtr, const booked_type&);
 
  that add the required fields to the given I3TableRowDescription - the first
  one can of course be replaced by adding a default value to the second
  parameter of the second version; and
* has a function
  ::

    void FillSingleRow(const booked_type&, I3TableRowPtr);

  that takes a single object of booked_type and fills the given table row.

The same struct can be used both for the I3VectorConverter and the
I3MapOMKeyVectorConverter.

.. note::
    I3MapConverter has parameters bookGeometry and bookToParticle (you name the particle) in its constructor, that
    allow users to book X/Y/Z coordinates or perpendicular/longitunal distances to a particle track/vertex, 
    together with the data in the map.
    They default to not booking these things (bookGeometry=false, and bookToParticle="").
    To make this option available in python, use the I3_MAP_CONVERTER_EXPORT or
    I3_MAP_CONVERTER_EXPORT_DEFAULT macros instead of I3CONVERTER_EXPORT or
    I3CONVERTER_EXPORT_DEFAULT.



Example using I3MapOMKeyVectorConverter: I3RecoPulseSeriesMapConverter
_______________________________________________________________________

.. highlight:: c++

The following example shows the conversion struct for the I3RecoPulseSeriesMap
converter using the implemented I3MapOMKeyVectorConverter<T>. Mind the required typedef booked_type.
::

    #include <dataclasses/physics/I3RecoPulse.h>
    #include <tableio/converter/I3MapConverter.h>

    struct convert_I3RecoPulse {

      typedef I3RecoPulse booked_type;

      void AddFields(I3TableRowDescriptionPtr desc, const I3RecoPulse& = I3RecoPulse())
      {
        desc->AddField<double>("time", "ns", "Leading-edge time of the pulse");
        desc->AddField<double>("width", "ns", "Duration of the pulse");
        desc->AddField<double>("charge", "PE", "Integrated pulse charge");
        desc->AddField<int32_t>("id", "generic", "hit id");
      }

      void FillSingleRow(const booked_type& pulse, I3TableRowPtr row)
      {
        row->Set<double>("time", pulse.GetTime());
        row->Set<double>("width", pulse.GetWidth());
        row->Set<double>("charge", pulse.GetCharge());
        row->Set<int32_t>("id", pulse.GetID());
      }
    
    };

    typedef I3MapOMKeyVectorConverter< convert_I3RecoPulse >  I3RecoPulseSeriesMapConverter;



Example using I3VectorConverter: I3MMCTrackListConverter
__________________________________________________________

.. highlight:: c++

The I3MMCTrack does not inherit from I3FrameObject, so a converter for the I3MMCTrackList (an I3Vector<I3MMCTrack>) 
also needs a struct representing the I3MMCTrack. The following example shows the conversion struct for the I3MMCTrackList
converter using the implemented I3VectorConverter<T>. Mind the required typedef booked_type.
::

    #include <simclasses/I3MMCTrack.h>
    #include <tableio/converter/I3VectorConverter.h>
    
    struct convert_I3MMCTrack {
    
        typedef I3MMCTrack booked_type;
    
        void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = I3MMCTrack())
        {
            desc->AddField<double>("Ec", "GeV", "Muon Energy at closest point to detector center");
            desc->AddField<double>("Ef", "GeV", "Muon Energy at leaving detector");
            desc->AddField<double>("Ei", "GeV", "Muon Energy at entering detector");
            desc->AddField<double>("Elost", "GeV", "Muon energy loss in detector");
            ...
        }
    
        void FillSingleRow(const booked_type& track, I3TableRowPtr row)
        {
            row->Set<double>("Ec", track.GetEc());
            row->Set<double>("Ef", track.GetEf());
            row->Set<double>("Ei", track.GetEi());
            row->Set<double>("Elost", track.GetElost());
            ...
        }
    
    };
    
    typedef I3VectorConverter< convert_I3MMCTrack > I3MMCTrackListConverter;

