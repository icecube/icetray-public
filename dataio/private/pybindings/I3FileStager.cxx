#include <dataio/I3FileStager.h>

#include <icetray/python/gil_holder.hpp>

namespace bp = boost::python;

std::string
I3TrivialFileStager::GenerateLocalFileName(const std::string &url, bool reading)
{
	return url;
}

void
I3TrivialFileStager::WillReadLater(const std::string &url, const std::string &fname)
{
}

void
I3TrivialFileStager::CopyFileIn(const std::string &url, const std::string &fname)
{
}

void
I3TrivialFileStager::CopyFileOut(const std::string &fname, const std::string &url)
{
}


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

    virtual std::string GenerateLocalFileName(const std::string &url, bool reading)
    {
        boost::python::detail::gil_holder gil;
        return this->get_override("GenerateLocalFileName")(url, reading);
    }

    virtual void WillReadLater(const std::string &url, const std::string &fname)
    {
        boost::python::detail::gil_holder gil;
        this->get_override("WillReadLater")(url, fname);
    }

    virtual void CopyFileIn(const std::string &url, const std::string &fname)
    {
        boost::python::detail::gil_holder gil;
        this->get_override("CopyFileIn")(url, fname);
    }
    
    virtual void CopyFileOut(const std::string &fname, const std::string &url)
    {
        boost::python::detail::gil_holder gil;
        this->get_override("CopyFileOut")(fname, url);
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
        .def("CanStageIn", &I3FileStager::CanStageIn)
        .def("CanStageOut", &I3FileStager::CanStageOut)
        .def("WillReadLater", (void (I3FileStager::*)(const std::string&))&I3FileStager::WillReadLater)
        .def("GetReadablePath", &I3FileStager::GetReadablePath)
        .def("GetWriteablePath", &I3FileStager::GetWriteablePath)
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
        bp::class_<I3FileStagerCollection, boost::shared_ptr<I3FileStagerCollection>, bp::bases<I3FileStagerWrapper>, boost::noncopyable>("I3FileStagerCollection", bp::no_init)
        .def("__init__", bp::make_constructor(&I3FileStagerCollection::create))
        ;
    }
    
    {
        bp::class_<I3TrivialFileStager, boost::shared_ptr<I3TrivialFileStager>, bp::bases<I3FileStagerWrapper>, boost::noncopyable>("I3TrivialFileStager", bp::no_init)
        .def("__init__", bp::make_constructor(&I3TrivialFileStager::create))
        ;
    }
    
    {
        using namespace I3::dataio;
        bp::class_<filehandle, shared_filehandle, boost::noncopyable>("filehandle", bp::no_init)
            .def(str(bp::self))
        ;
    }
    
    
}
