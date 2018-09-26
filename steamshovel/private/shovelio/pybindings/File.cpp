#include "shovelio/File.h"
using namespace shovelio;

#include <boost/python.hpp>
using namespace boost::python;

static const raw::FilterList kNoFilter;

I3FramePtr
RawFile_next(raw::File& self, bool fast)
{
  I3FramePtr p(new I3Frame());
  if( !self.Next(p, fast, kNoFilter) ){
    PyErr_SetNone(PyExc_StopIteration);
    throw_error_already_set();
  }
  return p;
}

I3FramePtr
RawFile_at_stream_pos(raw::File& self, int x)
{
  I3FramePtr p(new I3Frame());
  self.AtStreamPos(p, x, kNoFilter);
  return p;
}


void export_File()
{
  class_< raw::File,
          boost::noncopyable > PyFile( "File", init<const std::string&, raw::File::Type>() )
    ;

  scope file_scope = PyFile;

  enum_< raw::File::Type > ("Type")
    .value("Unknown", raw::File::Unknown)
    .value("Bad", raw::File::Bad)
    .value("Empty", raw::File::Empty)
    .value("SinglePass", raw::File::SinglePass)
    .value("MultiPass", raw::File::MultiPass)
    .value("Seekable", raw::File::Seekable)
    ;

  PyFile
    .def( init<const std::string&>() )
    .def( "next", RawFile_next )
    .def( "at_stream_pos", RawFile_at_stream_pos )
    .add_property( "type", &raw::File::GetType )
    .add_property( "current_stream_pos", &raw::File::CurrentStreamPos )
    .add_property( "has_size", &raw::File::HasSize )
    .add_property( "size", &raw::File::GetSize )
    ;
}
