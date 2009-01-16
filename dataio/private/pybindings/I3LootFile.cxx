/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#include <icetray/open.h>

using namespace std;


#include <fstream>
#include <icetray/I3Frame.h>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/python/errors.hpp>

class I3LootFile
{
 public:
  
  enum Mode { Reading, Writing, Closed };
  I3LootFile() { };
  I3LootFile(const std::string& filename, Mode mode=Reading);
  ~I3LootFile();

  int open_file(const std::string& filename, Mode mode=Reading);
  void close();

  bool more();
  void push(I3FramePtr f);
  I3FramePtr pop_frame();
  I3FramePtr pop_frame(I3Frame::Stream s);
  I3FramePtr pop_physics();

  struct iterator 
  {
    boost::shared_ptr<I3LootFile> file;
    I3FramePtr next()
    {
      if (not file->more())
	{
	  PyErr_SetObject(PyExc_StopIteration, Py_None);
	  boost::python::throw_error_already_set();
	}
      return file->pop_frame();
    }
  };

  static iterator make_iterator(boost::shared_ptr<I3LootFile> f)
  {
    if (f->mode_ != Reading)
      throw std::runtime_error("Can only iterate over I3Files opened for reading"); 

    iterator iter;
    iter.file = f;
    return iter;
  }

 private:

  boost::iostreams::filtering_istream ifs_;
  boost::iostreams::filtering_ostream ofs_;
  
  Mode mode_;

};

void register_I3LootFile();

I3LootFile::~I3LootFile()
{
  close();
}

I3LootFile::I3LootFile(const std::string& filename, Mode mode)
{
  open_file(filename, mode);
}

void
I3LootFile::close()
{
  ifs_.reset();
  ofs_.reset();
  mode_ = Closed;
}

int
I3LootFile::open_file(const std::string& filename, Mode mode)
{
  mode_ = mode;
  if (mode_ == Reading)
    I3::dataio::open(ifs_, filename);
  else if (mode_ == Writing)
    I3::dataio::open(ofs_, filename);
    
  return 0;
}

I3FramePtr
I3LootFile::pop_frame()
{
  return this->pop_frame(I3Frame::None);
}

I3FramePtr
I3LootFile::pop_frame(I3Frame::Stream s)
{
  if (mode_ != Reading)
    log_fatal("can't pop from non-Reading file");
  if(ifs_.peek() == EOF) 
    return I3FramePtr();
  I3FramePtr f(new I3Frame);
  while(more()) {
    try {
      f->load(ifs_);  
      if (s == I3Frame::None || f->GetStop() == s)
	return f;
    } catch (const std::exception& e) {
      log_fatal("caught exception %s", e.what());
    }
  }
  return I3FramePtr();
}

void 
I3LootFile::push(I3FramePtr fp)
{
  if (mode_ != Writing)
    log_fatal("can't push onto nonwriting file");
  fp->save(ofs_);
}

I3FramePtr
I3LootFile::pop_physics()
{
  if (mode_ != Reading)
    log_fatal("can't pop from non-Reading file");
  I3FramePtr f;
  do {
    f = pop_frame();
  } while (f && f->GetStop() != I3Frame::Physics);
  return f;
}

bool
I3LootFile::more()
{
  return ifs_.peek() != EOF;
}

//
//
//   boost::python registration
//
//
using namespace boost::python;
namespace bp = boost::python;

void register_I3LootFile()
{
  scope in_i3lootfile = class_<I3LootFile, boost::noncopyable>
    ("I3File", 
     "Simple reader/writer for .i3 files, with optional on-the-fly gzipping",
     init<>("Create an I3File object that is not attached to any on-disk file")
     )
    .def(init<const std::string&>((bp::arg("filename (may be .i3 or .i3.gz)")),
				  "Create and open and I3File object for reading"))
    .def(init<const std::string&, I3LootFile::Mode>((bp::arg("filename (may be .i3 or .i3.gz)"), 
						     bp::arg("Mode")="Reading"),
						    "Create and open an I3File object, specifying the mode"))
    .def("open_file", &I3LootFile::open_file, 
	 (bp::arg("filename (may be .i3 or .i3.gz)"), 
	  bp::arg("Mode")="Reading"),
	 "Open a .i3 file")
    .def("close",&I3LootFile::close, 
	 "Close the file")
    .def("more", &I3LootFile::more, 
	 "Return True if there are more frames in the .i3 file")
    .def("push", &I3LootFile::push,
	 bp::arg("frame"),
	 "Push a frame to the file (if file opened with Mode.Writing)")
    .def("pop_frame", (I3FramePtr (I3LootFile::*)())&I3LootFile::pop_frame,
	 "Return the next frame on any stream from the file.")
    .def("pop_frame", (I3FramePtr (I3LootFile::*)(I3Frame::Stream))&I3LootFile::pop_frame,
	 (bp::arg("Stream")),
	 "Return the next frame on stream 'Stream' from the file.")
    .def("pop_physics", &I3LootFile::pop_physics,
	 "Return the next physics frame from the file, skipping frames on "
	 "other streams.")
    .def("__iter__", &I3LootFile::make_iterator)
    ;

  class_<I3LootFile::iterator>("iterator")
    .def("next", &I3LootFile::iterator::next)
    ;

  enum_<I3LootFile::Mode>("Mode")
    .value("Reading", I3LootFile::Reading)
    .value("Writing", I3LootFile::Writing)
    .value("Closed", I3LootFile::Closed)
    ;
}
  
