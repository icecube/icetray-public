#ifndef I3_FILESTAGER_H_INCLUDED
#define I3_FILESTAGER_H_INCLUDED

#include <string>
#include <vector>
#include <map>

#include <icetray/I3PointerTypedefs.h>
#include <icetray/I3Logging.h>

/**
 * A base class interface for staging files
 * to local storage from remote locations.
 */
class I3FileStager
{
public:
    I3FileStager();
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
    virtual void WillReadLater(const std::string &url) = 0;

    /**
     * Read a file from a URL and copy it to a local
     * scratch directory. Block until finished and return
     * a full path to the local copy.
     */
    virtual std::string StageFileIn(const std::string &url) = 0;

   /**
    * Allows a writer to tell us that it wants to write to
    * the given URL. Return a local filename that the writer
    * should use instead.
    */ 
    virtual std::string WillWrite(const std::string &url) = 0;
   
   /**
    * Copy the local file associated with the given url to
    * its destination, and block until complete
    */
    virtual void StageFileOut(const std::string &url) = 0;

    /**
     * Cleanup a previously staged file. Usually this
     * means just deleting it.
     */ 
    virtual void Cleanup(const std::string &filename) = 0;

private:
    I3FileStager(const I3FileStager &); // no copy constructor

};

I3_POINTER_TYPEDEFS(I3FileStager);

class I3FileStagerCollection : public I3FileStager
{
public:
    I3FileStagerCollection(const std::vector<I3FileStagerPtr>&);
    virtual ~I3FileStagerCollection();
    
    virtual std::vector<std::string> ReadSchemes() const;
    virtual std::vector<std::string> WriteSchemes() const;
    virtual void WillReadLater(const std::string &url);
    virtual std::string StageFileIn(const std::string &url);
    virtual std::string WillWrite(const std::string &url);
    virtual void StageFileOut(const std::string &url);
    virtual void Cleanup(const std::string &filename);
private:
    SET_LOGGER("I3FileStagerCollection");
    I3FileStagerCollection(const I3FileStagerCollection &);
    
    std::vector<I3FileStagerPtr> stagers_;
    typedef std::map<std::string, I3FileStagerPtr> protocol_map_t;
    protocol_map_t readers_;
    protocol_map_t writers_;
    
    const I3FileStagerPtr& GetReader(const std::string& url) const;
    const I3FileStagerPtr& GetWriter(const std::string& url) const;
};

class I3StagingReader : public I3Module {
public:
    I3StagingReader(const I3Context&);
    void Configure();
    // void OpenNextFile()
protected:
    std::vector<std::string> filenames_;
    std::vector<std::string>::iterator filenames_iter_;
    std::string current_filename_;
    I3FileStager file_stager_;
};

#endif
