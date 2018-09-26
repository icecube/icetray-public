#ifndef I3_SHOVEL_SCRIPTING_QMETA_H
#define I3_SHOVEL_SCRIPTING_QMETA_H

#include <stdexcept>

class QObject;

namespace scripting{

/**
 * Fully static class for automagical exposure of QObject to Python runtime.
 */
class QMeta{

public:
	/**
	 * Initialize the qmeta system; called once before Python is initialized.
	 */
	static void pre_init();

	/**
	 * Initialize the qmeta system; called once after Python is initialized.
	 */
	static void post_init();

	/**
	 * Export the properties and public slots of the given object.  The object will
	 * be added to the qmeta._all_objects list in Python; use addToGlobalNamespace
	 * or addToParent to expose the object in a more useful way.
	 */
	static void exportObject( QObject* q );

	/**
	 * Add the given object, which must have been previously passed to exportObject,
	 * to python's __main__ namespace with the given name.
	 */
	static void addToGlobalNamespace( QObject* q, const std::string& name );

	/**
	 * Set the child object as a python attribute of the parent object.  Both the
	 * objects must have previously been exported to python with exportObject.
	 */
	static void addToParent( QObject* parent, QObject* child, const std::string& attrname );

	/** General QMeta error type */
	class error : public std::runtime_error{
	public:
		error(): std::runtime_error("QMeta error"){}
		error(const std::string& err): std::runtime_error(err){}
	};

private:
	QMeta();
	QMeta(const QMeta&);
	const QMeta& operator=(const QMeta&);
};

} // namespace scripting

#endif /* I3_SHOVEL_SCRIPTING_QMETA_H */
