/**
 *  $Id: I3File.cxx 2376 2016-01-04 21:26:32Z david.schultz $
 *
 *  Copyright (C) 2007 Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Copyright (C) 2007 the IceCube Collaboration <http://www.icecube.wisc.edu>
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

#include "pybindings.hpp"
#include <stdexcept>
#include <icetray/I3Logging.h>
#include <icetray/I3Frame.h>
#include <dataio/I3FrameSequence.h>
#include <boost/python/errors.hpp>

using namespace boost::python;
using namespace dataio;

namespace {

    object pass_through(object const& o) { return o; }

    class I3FrameSequenceIterator
    {
    public:
        I3FrameSequenceIterator(I3FrameSequencePtr);
        I3FrameSequenceIterator(const I3FrameSequenceIterator&);
        I3FramePtr next();
    private:
        I3FrameSequencePtr file_;
    };

    I3FrameSequenceIterator::I3FrameSequenceIterator(I3FrameSequencePtr f) :
        file_(f)
    { }

    I3FrameSequenceIterator::I3FrameSequenceIterator(const I3FrameSequenceIterator& f) :
        file_(f.file_)
    { }

    I3FramePtr I3FrameSequenceIterator::next()
    {
        if (!file_->more()) {
            PyErr_SetString(PyExc_StopIteration, "No more data.");
            throw_error_already_set();
        }
        return file_->pop_frame();
    }

    // I3FrameSequence helpers

    I3FrameSequenceIterator make_iterator(I3FrameSequencePtr f)
    {
        return I3FrameSequenceIterator(f);
    }

    I3FrameSequence& enter_context(I3FrameSequence& f)
    {
        return f;
    }

    void exit_context(I3FrameSequence& f, object, object, object)
    {
        f.close();
    }

    I3FramePtr fs_getitem(I3FrameSequence& f, long i)
    {
        if (i < 0) {
            PyErr_SetString(PyExc_TypeError, "Negative indexes invalid");
            throw_error_already_set();
        }
        I3FramePtr ret;
        try {
            ret = f[static_cast<size_t>(i)];
        } catch (const std::runtime_error&) {
            PyErr_SetString(PyExc_IndexError, "No frame at that index");
            throw_error_already_set();
        } catch (const std::exception &e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
            throw_error_already_set();
        }
        return ret;
    }

    bool fs_contains(I3FrameSequence& f, long i)
    {
        if (i < 0) {
            return false;
        }
        try {
            I3FramePtr ret = f[static_cast<size_t>(i)];
            if (!ret) {
                return false;
            } else {
                return true;
            }
        } catch (const std::runtime_error&) {
            return false;
        }
    }
}


void register_I3FrameSequence()
{
  scope i3file_scope = class_<I3FrameSequence, I3FrameSequencePtr>
    ("I3FrameSequence",
     "Reader for multiple .i3 files",
     init<>()
     )
    .def(init<const I3FrameSequence&>("Copy constructor"))
    .def(init<const std::vector<std::string>&, size_t>(
        (arg("Filenames"),
         arg("Cache_size") = 1000),
        "Create and open an I3FrameSequence object."))
    .def("close", &I3FrameSequence::close,
         "Close all files")
    .def("add_file", &I3FrameSequence::add_file,
         "Add a file")
    .def("close_last_file", &I3FrameSequence::close_last_file,
         "Close the last file")
    .def("more", &I3FrameSequence::more,
         "Return True if there are more frames in the file")
    .def("rewind", &I3FrameSequence::rewind,
         "Rewind to beginning of file and reopen")
    .def("pop_frame", &I3FrameSequence::pop_frame,
         (arg("Stream")=I3Frame::None),
         "Return the next frame on stream 'Stream' from the file.")
    .def("pop_daq", &I3FrameSequence::pop_daq,
         "Return the next DAQ frame from the file, skipping frames on other streams.")
    .def("pop_physics", &I3FrameSequence::pop_physics,
         "Return the next physics frame from the file, skipping frames on other streams.")
    .def("seek", &I3FrameSequence::seek,
         "Seek to a specific frame number")
    .def("get_mixed_frames", &I3FrameSequence::get_mixed_frames,
         "Return the frames that are mixed into the current frame.")
    .add_property("paths", &I3FrameSequence::get_paths)
    .add_property("frameno", &I3FrameSequence::get_frameno)
    .add_property("size", &I3FrameSequence::get_size)
    .add_property("cur_size", &I3FrameSequence::get_cur_size)
    .add_property("stream", &I3FrameSequence::get_stream)
    .def("__iter__", make_iterator)
    .def("__enter__", enter_context, return_value_policy<reference_existing_object>() )
    .def("__exit__", exit_context)
    .def("__len__", &I3FrameSequence::get_cur_size)
    .def("__getitem__", fs_getitem)
    .def("__contains__", fs_contains)
    ;

  class_<I3FrameSequenceIterator>
    ("I3FrameSequenceIterator",
     "Iterator for I3FrameSequence",
     no_init
     )
    .def("__next__", &I3FrameSequenceIterator::next,
         "Return the next frame if one is available, else throw StopIteration")
    .def("__iter__", pass_through)
    ;
}
