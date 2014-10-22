#ifndef I3_FILESTAGER_H_INCLUDED
#define I3_FILESTAGER_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <icetray/I3PointerTypedefs.h>
#include <icetray/I3Logging.h>
#include <icetray/I3DefaultName.h>

class I3FileStager;

namespace I3 { namespace dataio {
class filehandle : public std::string
{
public:
    filehandle(const std::string &path);
    virtual ~filehandle();
    
private:
    filehandle(const std::string &path, boost::function<void()> on_destroy);
    friend class ::I3FileStager;
    
    boost::function<void()> callback_;
};
typedef boost::shared_ptr<filehandle> shared_filehandle;
}}

class I3FileStagerCollection;

/**
 * A base class interface for staging files
 * to local storage from remote locations.
 */
class I3FileStager : public boost::enable_shared_from_this<I3FileStager>
{
public:
    virtual ~I3FileStager();

    /**
     * Returns the URI scheme this stager can read.
     * (i.e. the in a URL before the colon, e.g. "http",
     * "ftp", "gsiftp", ...)
     */
    virtual std::vector<std::string> ReadSchemes() const = 0;
    virtual bool CanStageIn(const std::string &url);
    
    /**
     * Returns the URI scheme this stager can write.
     */
    virtual std::vector<std::string> WriteSchemes() const = 0;
    virtual bool CanStageOut(const std::string &url);

    /**
     * Allows the reader to tell us what files will
     * be read in the future (in the order this is called).
     */ 
    void WillReadLater(const std::string &url);

    /**
     * Read a file from a URL and copy it to a local
     * scratch directory. Block until finished and return
     * a full path to the local copy. When the last reference
     * to the return value is deleted, the file will be removed.
     */
    I3::dataio::shared_filehandle GetReadablePath(const std::string &url);

   /**
    * Allows a writer to tell us that it wants to write to
    * the given URL. Return a local filename that the writer
    * should use instead. When the last reference
    * to the return value is deleted, the file will be uploaded
    * to its destination and then removed.
    */ 
    I3::dataio::shared_filehandle GetWriteablePath(const std::string &url);

protected:
    I3FileStager();
    
    virtual std::string GenerateLocalFileName(const std::string &url, bool reading) = 0;
    virtual void WillReadLater(const std::string &url, const std::string &fname) = 0;
    virtual void CopyFileIn(const std::string &url, const std::string &fname) = 0;
    virtual void CopyFileOut(const std::string &fname, const std::string &url) = 0;
    
    
private:
    void Cleanup(const std::string &fname);
    void StageFileOut(const std::string &url);
    I3FileStager(const I3FileStager &);
    friend class I3FileStagerCollection;
    
    typedef boost::weak_ptr<I3::dataio::filehandle> weak_filehandle;
    typedef std::pair<std::string, weak_filehandle> handle_pair;
    std::map<std::string, handle_pair>::iterator GetLocalFileName(const std::string &url, bool reading);

    std::map<std::string, handle_pair> url_to_handle_;
};

I3_POINTER_TYPEDEFS(I3FileStager);
I3_DEFAULT_NAME(I3FileStager);

class I3TrivialFileStager : public I3FileStager {
public:
    static boost::shared_ptr<I3FileStager> create();
    virtual ~I3TrivialFileStager();
    
    virtual std::vector<std::string> ReadSchemes() const;
    virtual std::vector<std::string> WriteSchemes() const;
protected:
    virtual std::string GenerateLocalFileName(const std::string &url, bool reading);
    virtual void WillReadLater(const std::string &url, const std::string &fname);
    virtual void CopyFileIn(const std::string &url, const std::string &fname);
    virtual void CopyFileOut(const std::string &fname, const std::string &url);
private:
    I3TrivialFileStager();
};

class I3FileStagerCollection : public I3FileStager
{
public:
    static boost::shared_ptr<I3FileStager> create(const std::vector<I3FileStagerPtr>&);
    virtual ~I3FileStagerCollection();
    
    virtual std::vector<std::string> ReadSchemes() const;
    virtual std::vector<std::string> WriteSchemes() const;
protected:
    virtual std::string GenerateLocalFileName(const std::string &url, bool reading);
    virtual void WillReadLater(const std::string &url, const std::string &fname);
    virtual void CopyFileIn(const std::string &url, const std::string &fname);
    virtual void CopyFileOut(const std::string &fname, const std::string &url);
private:
    SET_LOGGER("I3FileStagerCollection");
    
    I3FileStagerCollection(const std::vector<I3FileStagerPtr>&);
    I3FileStagerCollection(const I3FileStagerCollection &);
    
    std::vector<I3FileStagerPtr> stagers_;
    typedef std::map<std::string, I3FileStagerPtr> protocol_map_t;
    protocol_map_t readers_;
    protocol_map_t writers_;
    
    const I3FileStagerPtr& GetReader(const std::string& url) const;
    const I3FileStagerPtr& GetWriter(const std::string& url) const;
};

#endif
