#include "graphicsmath.h"

vec3d rotate_around( const vec3d& v1, const vec3d& v2, qreal angle )
{
	QQuaternion q = QQuaternion::fromAxisAndAngle( v2, angle );
	return q.rotatedVector( v1 );
}

vec3d from_I3Position( const I3Position& p ) { return vec3d(p.GetX(),p.GetY(),p.GetZ()); }
vec3d from_I3Direction( const I3Direction& p ) { return vec3d(p.GetX(),p.GetY(),p.GetZ()); }

std::ostream& operator<<( std::ostream& out, const vec3d& v ){
	out << "[" << v.x() <<", " << v.y() << ", " << v.z() << "]";
	return out;
}
