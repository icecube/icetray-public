#ifndef I3_SHOVEL_SCRIPTING_I3_METATYPES_H
#define I3_SHOVEL_SCRIPTING_I3_METATYPES_H

#include <vector>
#include <boost/any.hpp>
#include <boost/python/object_fwd.hpp>
#include <QGenericArgument>

class QByteArray;

namespace scripting{

void declare_i3_qt_metatypes();
QGenericArgument make_i3_qarg( const boost::python::object& pobj, const QByteArray& typestr,
                               std::vector<boost::any>& refs );

} // namespace scripting

#endif /* I3_SHOVEL_SCRIPTING_I3_METATYPES_H */
