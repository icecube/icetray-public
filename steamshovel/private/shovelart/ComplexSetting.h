#ifndef I3_SHOVEL_COMPLEX_SETTING_H
#define I3_SHOVEL_COMPLEX_SETTING_H

/** Base class for complex settings, which encapsulate a value of 
 *  type T, but also need to provide meta information for the editor.
 *  Derive from this and add a public "typedef T Value" in your
 *  derived class. Also make it a friend of the corresponding editor.
 *  See examples: KeySetting, FileSetting, ...
 */

template<typename T>
class ComplexSetting {
private:
    T value_;
public:
    ComplexSetting(T val): value_(val) {}

    // only to used internally
    void setValue( T val ) { value_ = val; }

    // for C++ users: implicit conversion to value type
    operator T() const { return value_; }
};

#endif
