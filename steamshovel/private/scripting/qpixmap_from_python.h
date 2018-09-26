#ifndef I3_SHOVEL_QPIXMAP_FROM_PYTHON_H
#define I3_SHOVEL_QPIXMAP_FROM_PYTHON_H

#include <boost/python/object_fwd.hpp>
#include <QPixmap>

namespace scripting{

/**
 * Create a qpixmap from raw bitmap data stored in a Python object
 */
QPixmap qpixmap_from_python( PyObject* datastr, int w, int h );

}

#endif
