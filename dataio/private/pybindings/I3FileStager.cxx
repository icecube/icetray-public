#include <dataio/I3FileStager.h>

#include <icetray/python/gil_holder.hpp>

namespace bp = boost::python;

struct I3FileStagerWrapper : I3FileStager, bp::wrapper<I3FileStager>
{
    // pure virtual
    virtual std::vector<std::string> Schemes() const
    {
        boost::python::detail::gil_holder gil;
        return this->get_override("Schemes")();
    }

    virtual void WillReadLater(const std::string &url)
    {
        boost::python::detail::gil_holder gil;
        this->get_override("WillReadLater")(url);
    }

    virtual std::string StageFile(const std::string &url)
    {
        boost::python::detail::gil_holder gil;
        return this->get_override("StageFile")(url);
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
        .def("Schemes", bp::pure_virtual(&I3FileStager::Schemes))
        .def("WillReadLater", bp::pure_virtual(&I3FileStager::WillReadLater))
        .def("StageFile", bp::pure_virtual(&I3FileStager::StageFile))
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
}
