#include "shovelio/I3FrameSequence.h"

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

void export_I3FrameSequence()
{
  using namespace shovelio;
  using namespace boost::python;

  class_< I3FrameSequence,
          boost::shared_ptr<I3FrameSequence>,
          boost::noncopyable > PyI3FrameSequence( "I3FrameSequence", init<>() )
    ;
  scope this_scope = PyI3FrameSequence;

  class_< I3FrameSequence::IndexItem >( "IndexItem", no_init )
    .def_readonly( "pos", &I3FrameSequence::IndexItem::pos_ )
    .def_readonly( "stream", &I3FrameSequence::IndexItem::stream_ )
    .def_readonly( "parent", &I3FrameSequence::IndexItem::parent_ )
    ;

  class_< I3FrameSequence::LockedIndex,
          boost::noncopyable >( "LockedIndex", init<I3FrameSequence&>() )
    .def( "__getitem__",
          make_function( &I3FrameSequence::LockedIndex::operator[],
                         return_internal_reference<>() ) 
    )
    .def( "__len__", &I3FrameSequence::LockedIndex::GetSize )
    ;

  PyI3FrameSequence
    .def( "add_file", &I3FrameSequence::AddFile )
    .def( "__len__", &I3FrameSequence::GetSize )
    .def( "__getitem__", &I3FrameSequence::operator[] )
    .add_property( "is_indexing", &I3FrameSequence::IsIndexing )
    ;
}
