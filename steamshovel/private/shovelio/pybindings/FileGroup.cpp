#include "shovelio/FileGroup.h"
using namespace shovelio;

#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>
using namespace boost::python;

static const raw::FilterList kNoFilter;

object
RawFileGroup_init(tuple args, dict kwargs)
{
  object self = args[0];
  args = tuple(args.slice(1,_));

  std::vector<std::string> filenames;
  filenames.reserve(len(args));
  for( unsigned i = 0; i < len(args); ++i )
    filenames.push_back(extract<std::string>(args[i])());

  return self.attr("__init__")(filenames);
}

I3FramePtr
RawFileGroup_next(raw::FileGroup& self, bool fast)
{
  I3FramePtr p(new I3Frame());
  if (!self.Next(p, fast, kNoFilter)) {
    PyErr_SetNone(PyExc_StopIteration);
    throw_error_already_set();
  }
  return p;
}

I3FramePtr
RawFileGroup_at_stream_pos(raw::FileGroup& self, int x)
{
  I3FramePtr p(new I3Frame());
  self.AtStreamPos(p, x, kNoFilter);
  return p;
}

uint64_t (raw::FileGroup::*size)() const = &raw::FileGroup::GetSize;

void export_FileGroup()
{
  class_< raw::FileGroup,
          boost::noncopyable >( "FileGroup", no_init )
    .def( "__init__", raw_function(RawFileGroup_init) )
    .def( init<const std::vector<std::string>&>() )
    .def( "add_file", make_function(&raw::FileGroup::AddFile,
                                    return_internal_reference<>()) )
    .def( "next", RawFileGroup_next )
    .def( "at_stream_pos", RawFileGroup_at_stream_pos )
    .add_property( "current_stream_pos", &raw::FileGroup::CurrentStreamPos )
    .add_property( "has_size", &raw::FileGroup::HasSize )
    .add_property( "size", size )
    ;
}
