/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */

#include <icetray/I3Logging.h>
#include <icetray/I3Frame.h>
#include <dataio/I3File.h>
#include <boost/python/errors.hpp>

using namespace boost::python;
using namespace dataio;

namespace {

    object pass_through(object const& o) { return o; }

    class I3FileIterator
    {
    public:
        I3FileIterator(I3FilePtr);
        I3FileIterator(const I3FileIterator&);
        I3FramePtr next();
    private:
        I3FilePtr file_;
    };

    I3FileIterator::I3FileIterator(I3FilePtr f) :
        file_(f)
    { }

    I3FileIterator::I3FileIterator(const I3FileIterator& f) :
        file_(f.file_)
    { }

    I3FramePtr I3FileIterator::next()
    {
        if (!file_->more()) {
            PyErr_SetString(PyExc_StopIteration, "No more data.");
            throw_error_already_set();
        }
        return file_->pop_frame();
    }

    // I3File helpers

    std::string get_mode(const I3File& f)
    {
        switch(f.get_mode()) {
            case I3File::Mode::append:
                return "Write/Append";
            case I3File::Mode::create:
                return "Write/Create";
            case I3File::Mode::write:
                return "Write/Truncate";
            case I3File::Mode::read:
                return "Read";
        }
        log_fatal("unknown mode");
    }

    std::string get_type(const I3File& f)
    {
        switch(f.get_type()) {
            case I3File::Type::singlepass:
                return "Stream";
            case I3File::Type::multipass:
                return "File";
            case I3File::Type::empty:
                return "Empty";
            case I3File::Type::closed:
                return "Closed";
        }
        log_fatal("unknown type");
    }

    I3FileIterator make_iterator(I3FilePtr f)
    {
        if (f->get_mode() != I3File::Mode::read) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot iterate on non-reading file.");
            throw_error_already_set();
        }
        if (f->get_type() == I3File::Type::closed) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot iterate on closed file.");
            throw_error_already_set();
        }
        return I3FileIterator(f);
    }

    I3File& enter_context(I3File& f)
    {
        return f;
    }

    void exit_context(I3File& f, object, object, object)
    {
        f.close();
    }

}


void register_I3File()
{
  scope i3file_scope = class_<I3File, I3FilePtr>
    ("I3File", 
     "Simple reader/writer for .i3 files, with optional on-the-fly gzipping",
     no_init
     )
    .def(init<const I3File&>("Copy constructor"))
    .def(init<const std::string&>(
        (arg("Filename")),
        "Create and open an I3File object."))
    .def(init<const std::string&, const std::string&, optional<size_t> >(
        (arg("Filename"),
         arg("Mode (r, w, x, a)")="r",
         arg("Starting frame")=0),
        "Create and open an I3File object, specifying the mode."))
    .def("close", &I3File::close, 
         "Close the file")
    .def("more", &I3File::more, 
         "Return True if there are more frames in the file")
    .def("rewind", &I3File::rewind, 
         "Rewind to beginning of file and reopen")
    .def("push", (void(I3File::*)(boost::shared_ptr<I3Frame>))&I3File::push,
         arg("frame"),
         "Push a frame to the file (if file opened in writing mode)")
    .def("pop_frame", (I3FramePtr (I3File::*)(I3Frame::Stream))&I3File::pop_frame,
         (arg("Stream")=I3Frame::None),
         "Return the next frame on stream 'Stream' from the file.")
    .def("pop_daq", &I3File::pop_daq,
         "Return the next DAQ frame from the file, skipping frames on other streams.")
    .def("pop_physics", &I3File::pop_physics,
         "Return the next physics frame from the file, skipping frames on other streams.")
    .def("seek", &I3File::seek, 
         "Seek to a specific frame number")
    .def("get_mixed_frames", &I3File::get_mixed_frames,
         "Return the parent frames that are mixed into the current frame.")
    .def("get_current_frame_and_deps", &I3File::get_current_frame_and_deps,
         "Return the inclusive list of [parent ... current] frames")
    .add_property("path", &I3File::get_path)
    .add_property("frameno", &I3File::get_frameno)
    .add_property("size", &I3File::get_size)
    .add_property("stream", &I3File::get_stream)
    .add_property("mode", get_mode)
    .add_property("type", get_type)
    .def("__iter__", make_iterator)
    .def("__enter__", enter_context, return_value_policy<reference_existing_object>() )
    .def("__exit__", exit_context)
    .def("__len__", &I3File::get_size)
    ;

  class_<I3FileIterator>
    ("I3FileIterator",
     "Iterator for I3File",
     no_init
     )
    .def("__next__", &I3FileIterator::next, 
         "Return the next frame if one is available, else throw StopIteration")
    .def("__iter__", pass_through)
    ;
}
