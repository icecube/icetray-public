#include <map>
#include <string>
#include <boost/python.hpp>
#include <boost/python/docstring_options.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "icetray/python/dataclass_suite.hpp"
#include "icetray/memory.h"

namespace {
    typedef std::map<std::string,size_t> Snapshot;
    typedef std::pair<size_t,Snapshot > TimedSnapshot;
    typedef std::vector<TimedSnapshot > Timeline;

    class MemoryTimeline
    {
      public:
        MemoryTimeline();
        ~MemoryTimeline();

        /**
         * Start keeping track of snapshots
         *
         * interval in microseconds
         */
        void start(unsigned long interval);

        /**
         * Stop keeping track of snapshots
         */
        void stop();

        /**
         * Get the timeline data
         *
         * Trust the user to not manipulate this, since
         * we can't pass a const to python.
         */
        Timeline& get_timeline();

      private:
        boost::posix_time::time_duration _interval;
        boost::shared_ptr<boost::thread> _thread;
        Timeline _data;

        // run the thread behind the scenes
        void _thread_run();
    };

    MemoryTimeline::MemoryTimeline()
        : _interval(boost::date_time::not_a_date_time)
    { }

    MemoryTimeline::~MemoryTimeline()
    {
        stop();
    }

    void
    MemoryTimeline::start(unsigned long interval)
    {
        if (!_thread) {
            _interval = boost::posix_time::microseconds(interval);
            _thread = boost::shared_ptr<boost::thread>(
                new boost::thread(boost::bind(
                &MemoryTimeline::_thread_run, this)));
        }
    }

    void
    MemoryTimeline::stop()
    {
        if (_thread) {
            _thread->interrupt();
            _thread->join();
            _interval = boost::date_time::not_a_date_time;
            _thread.reset();
        }
    }

    Timeline&
    MemoryTimeline::get_timeline()
    {
        return _data;
    }

    void
    MemoryTimeline::_thread_run()
    {
        boost::posix_time::ptime t(
            boost::posix_time::microsec_clock::local_time());
        while(1) {
            _data.push_back(std::make_pair(
                (boost::posix_time::microsec_clock::local_time() - t).total_microseconds(),
                memory::get_extents()));
            boost::this_thread::sleep(_interval);
        }
    }
}

struct Memory{};
void register_Memory(){
  boost::python::docstring_options doc_options;
  doc_options.disable_cpp_signatures();
  boost::python::scope m = boost::python::class_<Memory>("memory")
    .def("get_extents", &memory::get_extents, "Get a snapshot of memory extents")
    .staticmethod("get_extents")
    .def("set_label", &memory::set_label, "Set the current memory label")
    .staticmethod("set_label")
    ;

  boost::python::class_<Snapshot >("Snapshot")
    .def(boost::python::dataclass_suite<Snapshot >())
    ;

  boost::python::class_<TimedSnapshot >("TimedSnapshot")
    .add_property("first", &TimedSnapshot::first)
    .add_property("second", &TimedSnapshot::second)
    .def(boost::python::dataclass_suite<TimedSnapshot >())
    ;

  boost::python::class_<Timeline >("Timeline")
    .def(boost::python::dataclass_suite<Timeline >())
    ;

  boost::python::class_<MemoryTimeline >("MemoryTimeline")
    .def("start", &MemoryTimeline::start)
    .def("stop", &MemoryTimeline::stop)
    .def("get_timeline", &MemoryTimeline::get_timeline,
         boost::python::return_internal_reference<>())
    ;
}
