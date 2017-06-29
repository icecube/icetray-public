#ifndef I3_FRAME_SEQUENCE_H_INCLUDED
#define I3_FRAME_SEQUENCE_H_INCLUDED

#include <string>
#include <vector>
#include <memory>

#include <icetray/I3Frame.h>

namespace dataio {

    //! Forward declaration of private implementation, an opaque class.
    class I3FrameSequenceImpl;

    /** An IceCube multi-file interface.
     *
     *  A class to easily access multiple files as if it was one large
     *  sequence of frames.  Only supports read access.
     */
    class I3FrameSequence
    {   
    public:
        //! Default constructor.
        I3FrameSequence();
    
        //! Copy constructor.
        I3FrameSequence(const I3FrameSequence&);

        /** A constructor
         *
         *  /param paths A vector of paths to files.
         *  /param size Size of cache
         */
        explicit I3FrameSequence(const std::vector<std::string>&, size_t = 1000);

        /** A constructor
         *
         *  /param size Size of cache
         */
        explicit I3FrameSequence(size_t);

        //! Destructor. Automatically closes the files.
        ~I3FrameSequence();

        //! Add a file.
        void add_file(const std::string&);

        //! Close all files.
        void close();

        //! Close the last file.
        void close_last_file();

        //! Go to the beginning of the files.
        void rewind();

        //! Test if there is another frame.
        bool more();

        //! Get a frame of a specific stream from a readable file.
        I3FramePtr pop_frame(I3Frame::Stream = I3Frame::None); 

        //! Get a DAQ frame
        inline I3FramePtr pop_daq() { return pop_frame(I3Frame::DAQ); }

        //! Get a Physics frame
        inline I3FramePtr pop_physics() { return pop_frame(I3Frame::Physics); }

        //! Seek to a frame by number
        void seek(size_t);

        //! vector-like indexing
        I3FramePtr operator[](size_t);

        //! Get the parent frames mixed into the current frame.
        std::vector<I3FramePtr> get_mixed_frames();

        //! Get the inclusive list of [parent ... current] frames
        std::vector<I3FramePtr> get_current_frame_and_deps();

        // Getters for private variables
        std::vector<std::string> get_paths() const;
        ssize_t get_frameno() const;
        ssize_t get_size() const; //!< will get size, or -1 if not known
        size_t get_cur_size() const; //!< will get current size
        I3Frame::Stream get_stream() const;

    private:
        std::unique_ptr<I3FrameSequenceImpl> impl_; //!< The actual implementation
    };

    using I3FrameSequencePtr = boost::shared_ptr<I3FrameSequence>;
    using I3FrameSequenceConstPtr = boost::shared_ptr<const I3FrameSequence>;

} // end namespace dataio


#endif // I3_FRAME_SEQUENCE_H_INCLUDED
