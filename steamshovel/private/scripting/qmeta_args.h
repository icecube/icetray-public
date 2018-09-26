#ifndef I3_SHOVEL_SCRIPTING_QMETA_ARGHELPER_H
#define I3_SHOVEL_SCRIPTING_QMETA_ARGHELPER_H

#include <vector>
#include <boost/any.hpp>
#include <boost/python/object_fwd.hpp>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QObject>

class QByteArray;

namespace scripting {

void register_metatypes();

boost::python::object make_metaproperty_reader( QMetaProperty m );

boost::python::object make_metaproperty_writer( QMetaProperty m );

boost::python::object make_invoker( QMetaMethod m );

namespace qmeta_detail {

/**
 * This class provides a fancy invoke()-able way of calling QMetaProperty.read()
 * and QMetaProperty.write(), and is used for thread synchronization when qmeta
 * properties are accessed outside the main thread.
 */
class MetapropertyThreadQueue : public QObject{
	Q_OBJECT;

	public:
	QMetaMethod read_method;
	QMetaMethod write_method;

	MetapropertyThreadQueue();

	public Q_SLOTS:
		QVariant read( QObject* q, QMetaProperty p );
		bool write( QObject* q, QMetaProperty p, const QVariant& value );
};

} // namespace qmeta_detail

} // namespace scripting

#endif /* I3_SHOVEL_SCRIPTING_QMETA_ARGHELPER_H */
