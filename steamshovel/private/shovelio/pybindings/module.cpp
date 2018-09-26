#include <boost/python.hpp>
using namespace boost::python;

#include <icetray/load_project.h>

#include "shovelio/File.h"
#include "shovelio/FileGroup.h"
#include "shovelio/I3FrameSequence.h"

void export_File();
void export_FileGroup();
void export_I3FrameSequence();

struct ns_raw {};

BOOST_PYTHON_MODULE(shovelio)
{
  load_project( "shovelio", false );
  boost::python::import("icecube.icetray");
  boost::python::import("icecube.dataio");

  {
    // simulate namespace with a dummy class
    scope raw_scope = class_< ns_raw >( "raw", no_init );
    export_File();
    export_FileGroup();
  }

  export_I3FrameSequence();
}
