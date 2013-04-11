#ifndef I3_FILESTAGER_H_INCLUDED
#define I3_FILESTAGER_H_INCLUDED

#include <string>
#include <vector>

#include <icetray/I3PointerTypedefs.h>

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
     * Returns the URI scheme this stager handles.
     * (i.e. the in a URL before the colon, e.g. "http",
     * "ftp", "gsiftp", ...)
     */
    virtual std::vector<std::string> Schemes() const = 0;

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
    virtual std::string StageFile(const std::string &url) = 0;

    /**
     * Cleanup a previously staged file. Usually this
     * means just deleting it.
     */ 
    virtual void Cleanup(const std::string &filename) = 0;

private:
    I3FileStager(const I3FileStager &); // no copy constructor

};

I3_POINTER_TYPEDEFS(I3FileStager);

#endif
