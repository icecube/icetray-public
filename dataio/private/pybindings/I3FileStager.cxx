#include <dataio/I3FileStager.h>

#include <icetray/python/gil_holder.hpp>

namespace bp = boost::python;

struct I3FileStagerWrapper : I3FileStager, bp::wrapper<I3FileStager>
{
    // pure virtual
    virtual std::vector<std::string> ReadSchemes() const
    {
        boost::python::detail::gil_holder gil;
        return this->get_override("ReadSchemes")();
    }
    
    virtual std::vector<std::string> WriteSchemes() const
    {
        boost::python::detail::gil_holder gil;
        return this->get_override("WriteSchemes")();
    }

    virtual void WillReadLater(const std::string &url)
    {
        boost::python::detail::gil_holder gil;
        this->get_override("WillReadLater")(url);
    }

    virtual std::string StageFileIn(const std::string &url)
    {
        boost::python::detail::gil_holder gil;
        return this->get_override("StageFileIn")(url);
    }
    
    virtual std::string WillWrite(const std::string &url)
    {
        boost::python::detail::gil_holder gil;
        return this->get_override("WillWrite")(url);
    }
    
    virtual void StageFileOut(const std::string &url)
    {
        boost::python::detail::gil_holder gil;
        this->get_override("StageFileOut")(url);
    }

    virtual void Cleanup(const std::string &filename)
    {
        boost::python::detail::gil_holder gil;
        this->get_override("Cleanup")(filename);
    }

};

void register_I3FileStager()
{
    {
        bp::scope I3FileStager_scope = 
        bp::class_<I3FileStagerWrapper, boost::shared_ptr<I3FileStagerWrapper>, boost::noncopyable>
        (
            "I3FileStager",
            "A base class interface for staging files to local storage from remote locations."
        )
        .def("ReadSchemes", bp::pure_virtual(&I3FileStager::ReadSchemes))
        .def("WriteSchemes", bp::pure_virtual(&I3FileStager::WriteSchemes))
        .def("WillReadLater", bp::pure_virtual(&I3FileStager::WillReadLater))
        .def("StageFileIn", bp::pure_virtual(&I3FileStager::StageFileIn))
        .def("WillWrite", bp::pure_virtual(&I3FileStager::WillWrite))
        .def("StageFileOut", bp::pure_virtual(&I3FileStager::StageFileOut))
        .def("Cleanup", bp::pure_virtual(&I3FileStager::Cleanup))
        ;
    }
    
    bp::implicitly_convertible<boost::shared_ptr<I3FileStagerWrapper>, boost::shared_ptr<const I3FileStager> >();
    bp::implicitly_convertible<boost::shared_ptr<I3FileStagerWrapper>, boost::shared_ptr<I3FileStager> >();
    bp::implicitly_convertible<boost::shared_ptr<I3FileStagerWrapper>, boost::shared_ptr<const I3FileStagerWrapper> >();

    bp::class_<std::vector<I3FileStagerPtr> >("vector_I3FileStager")
        .def(bp::vector_indexing_suite<std::vector<I3FileStagerPtr>, true>())
    ;
    from_python_sequence<std::vector<I3FileStagerPtr>, variable_capacity_policy>();
    
    {
        bp::class_<I3FileStagerCollection, boost::shared_ptr<I3FileStagerCollection>, boost::noncopyable>("I3FileStagerCollection", bp::init<const std::vector<I3FileStagerPtr>& >())
        ;
    }
    
    bp::implicitly_convertible<boost::shared_ptr<I3FileStagerCollection>, boost::shared_ptr<const I3FileStager> >();
    bp::implicitly_convertible<boost::shared_ptr<I3FileStagerCollection>, boost::shared_ptr<I3FileStager> >();
    bp::implicitly_convertible<boost::shared_ptr<I3FileStagerCollection>, boost::shared_ptr<const I3FileStagerCollection> >();
    
    
    
}
