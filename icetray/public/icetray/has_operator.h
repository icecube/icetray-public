#ifndef ICETRAY_HAS_OPERATOR_H_INCLUDED
#define ICETRAY_HAS_OPERATOR_H_INCLUDED

/**
 * Type traits for detecting whether types can be used with particular operators.
 */
namespace has_operator {
    typedef char yes;
    typedef struct { char array[2]; } no;
    
    struct anyx { template <class T> anyx(const T &); };
    no operator << (const anyx &, const anyx &);
    
    template <class T> yes check(T const&);
    no check(no);
  
    /**
     * Determine whether a type can be output to a stream.
     * \tparam StreamType the type of stream which is the target
     * \tparam T the ype to be inserted into the stream
     */
    template <typename StreamType, typename T>
    struct insertion {
        static StreamType & stream;
        static T & x;
        static const bool value = sizeof(check(stream << x)) == sizeof(yes);
    };
}

#endif //ICETRAY_HAS_OPERATOR_H_INCLUDED