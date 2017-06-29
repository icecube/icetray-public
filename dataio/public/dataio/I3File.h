#ifndef I3_FILE_H_INCLUDED
#define I3_FILE_H_INCLUDED

#include <string>
#include <vector>

#include <boost/iostreams/filtering_stream.hpp>

#include <icetray/I3Frame.h>
#include <icetray/I3FrameMixing.h>

namespace dataio {

    // forward declaration of I3FrameSequence
    class I3FrameSequence;

    /** An IceCube file interface.
     *
     *  A class to easily access an IceCube (.i3) file. Provides read/write
     *  support and can open compressed files, web addresses, or raw sockets.
     */
    class I3File
    {
    public:
        //! An enum type for the file mode.
        enum class Mode : char {
            read = 'r', //!< reading (default)
            write = 'w', //!< writing, truncating if file exists
            create = 'x', //!< writing, fail if file exists
            append = 'a' //!< writing, append to end of file if file exists
        };

        //! An enum type for the file type.
        enum class Type : uint8_t {
            closed, //!< file is closed
            empty, //!< file is empty
            singlepass, //!< file is a stream
            multipass //!< file can be rewinded
        };

        //! Copy constructor.
        I3File(const I3File&);

        /** A constructor
         *
         *  /param path The path to the file.
         */
        explicit I3File(const std::string&);
        // ^ need this constructor to make boost::python happy

        /** A constructor
         *
         *  /param path The path to the file.
         *  /param mode The file mode (r, w, x, a).
         *  /param frames The number of frames to skip.
         */
        explicit I3File(const std::string&, const std::string&, size_t = 0);

        /** A special constructor for controlling mixing.
         * 
         *  /param path The path to the file.
         *  /param mode The file mode using the Mode enum.
         *  /param frames The number of frames to skip.
         *  /param mixing Whether to use mixing mode or tracking mode.
         */
        explicit I3File(const std::string&, Mode, size_t = 0, bool = true);

        //! Destructor. Automatically closes the file.
        ~I3File();

        //! Close the file.
        void close();

        //! Go to the beginning of a MultiPass file.
        void rewind();

        //! Test if there is another frame in a readable file.
        bool more();

        //! Push a frame onto a writable file.
        void push(I3FramePtr);

        //! Get a frame of a specific stream from a readable file.
        I3FramePtr pop_frame(I3Frame::Stream = I3Frame::None); 

        //! Get a DAQ frame from a readable file.
        inline I3FramePtr pop_daq() { return pop_frame(I3Frame::DAQ); }

        //! Get a Physics frame from a readable file.
        inline I3FramePtr pop_physics() { return pop_frame(I3Frame::Physics); }

        //! Seek to a frame by number.
        void seek(size_t);

        //! Get the parent frames mixed into the current frame.
        std::vector<I3FramePtr> get_mixed_frames();

        //! Get the inclusive list of [parent ... current] frames
        std::vector<I3FramePtr> get_current_frame_and_deps();

        // Getters for private variables
        std::string get_path() const;
        ssize_t get_frameno() const;
        size_t get_size() const;
        I3Frame::Stream get_stream() const;
        Mode get_mode() const;
        Type get_type() const;

    private:
        boost::iostreams::filtering_istream ifs_; //!< input file stream
        boost::iostreams::filtering_ostream ofs_; //!< output file stream
        std::string path_; //!< file path
        I3FrameMixer cache_; //!< frame cache
        I3FramePtr curr_frame_; //!< current frame
        size_t frameno_; //!< current frame number
        size_t size_; //!< file size
        Mode mode_; //!< file mode
        Type type_; //!< file type

        //! Open currently specified file. Used by constructors.
        void open_file();

        //! Skip frames in the current file.
        void skip_frames(size_t);
    };

    using I3FilePtr = boost::shared_ptr<I3File>;
    using I3FileConstPtr = boost::shared_ptr<const I3File>;

} // end namespace dataio

#endif //I3_FILE_H_INCLUDED
