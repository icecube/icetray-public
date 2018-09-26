#ifndef I3_SHOVEL_BIND_SIGNAL_MAPPER
#define I3_SHOVEL_BIND_SIGNAL_MAPPER

#include <boost/any.hpp>
#include <boost/function.hpp>

#include <QObject>
#include <QFont>
#include <QString>

/** Interace from Qt signals to boost::functions
 *
 * This class is used as a proxy object allowing Qt signals to be attached to
 * arbitrary boost::functions of zero or one argument, typically created using
 * boost::bind.  The bound function may be a Qt slot but need not be.  To use functions
 * with an argument of type T, this class must implement a call(T) slot, so it
 * may be necessary to add more such slots in the future if new type requirements
 * arise.
 *
 * This messiness would probably be unnecessary in Qt5, so any future efforts
 * to port to that library should consider replacing this class with Qt5's more
 * expressive signal/slot system.
 */
class BindSignal : public QObject
{

Q_OBJECT;

protected:
	boost::any the_func_;

	template <typename T>
	void call_impl( T t ){
		typedef boost::function<void(T)> f_type;
		boost::any_cast<f_type>(the_func_)(t);
	}

	void call_impl_void(){
		typedef boost::function<void(void)> f_type;
		boost::any_cast<f_type>(the_func_)();
	}

public:
	template < typename T >
	explicit BindSignal( boost::function<void(T)> f, QObject* parent = NULL ) :
		QObject( parent ), the_func_( boost::any(f) )
	{}

	explicit BindSignal( boost::function<void(void)> f, QObject* parent = NULL ) :
		QObject( parent ), the_func_( boost::any(f) )
	{}

public Q_SLOTS:
	/* Qt4 does not support templated slots, nor does our infrastructure let us use the
	 * preprocessor here.  So, manually add as many slot types as needed for the project.
	 **/
	void call(const QString& t){ call_impl<const QString&>(t); }
	void call( const QFont& f ){ call_impl<const QFont&>(f); }
	void call(bool b){ call_impl<bool>(b); }
	void call(int t){ call_impl<int>(t); }
	void call(float v){ call_impl<float>(v); }
	void call(){ call_impl_void(); }

};

#endif /* I3_SHOVEL_BIND_SIGNAL_MAPPER */
