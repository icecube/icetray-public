#ifndef I3_SHOVEL_GRAPHICS_MATH_H
#define I3_SHOVEL_GRAPHICS_MATH_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Direction.h>

typedef QVector2D vec2d;

typedef QVector3D vec3d;

typedef QVector4D vec4d;

/**
 * Rotate v1 around v2 by angle radians
 */
vec3d rotate_around( const vec3d& v1, const vec3d& v2, qreal angle );

vec3d from_I3Position( const I3Position& p );

vec3d from_I3Direction( const I3Direction& p );

std::ostream& operator<<( std::ostream&, const vec3d& v );

#endif /* I3_SHOVEL_GRAPHICS_MATH_H */
