#include <QPixmap>
#include <QPainter>

// needs to be before any 'boost/python*' or else triggers https://bugs.python.org/issue10910
#include <icetray/I3Logging.h>

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
//#include <boost/container/flat_map.hpp>

#include "ColorVariant.h"

#include "scripting/gil.h"
#include "scripting/pyerr.h"

ColorMap::mapList ColorMap::python_maps_;

namespace scripting{

namespace bp = boost::python;

class MatplotlibColorMap : public ColorMap {
	typedef std::pair<float,float> Vpair;
	//typedef boost::flat_map<float,float[2]> channel_map;
	typedef std::map<float,Vpair> channel_map;
private:

	channel_map rmap_, gmap_, bmap_;

	void make_channel( bp::object source, channel_map& dest ){
		for( int i = 0; i < bp::len(source); ++i ){
			bp::object entry = source[i];
			Vpair vals = std::make_pair(bp::extract<float>(entry[1])(), bp::extract<float>(entry[2])());
			float key = bp::extract<float>(entry[0]);
			dest[key] = vals;
		}
	}

	float get_value( float v, channel_map& cmap ){
		channel_map::iterator hi = cmap.upper_bound(v);
		if( hi == cmap.end() ){
			return cmap.rbegin()->second.second;
		}
		else if ( hi == cmap.begin() ){
			return hi->second.first;
		}

		channel_map::iterator lo(hi);
		lo--;

		float lov = lo->first, hiv = hi->first;
		float loValue = lo->second.second, hiValue = hi->second.first;
		float normv = (v-lov) / (hiv-lov);

		return (loValue * (1.f-normv)) + (hiValue * normv);

	}

public:
	MatplotlibColorMap( bp::object cm ){
		bp::object r = cm[0];
		make_channel( r, rmap_ );
		bp::object g = cm[1];
		make_channel( g, gmap_ );
		bp::object b = cm[2];
		make_channel( b, bmap_ );
	}

	QColor value( float v ){
		return QColor::fromRgbF( get_value(v,rmap_), get_value(v,gmap_), get_value(v,bmap_) );
	}

};

static bool pythonMapsGathered = false;

static void gatherPythonCMaps( ColorMap::mapList& mlist ){
	pythonMapsGathered = true;
	ScopedGIL gil;
	try{
		bp::object colormod = bp::import("icecube.steamshovel.util.colormaps");
		bp::object cmlist = colormod.attr("get_matplotlib_colormaps")();
		typedef bp::stl_input_iterator<bp::tuple> cmlist_iter;
		cmlist_iter begin(cmlist), end;
		for( cmlist_iter i = begin; i!=end; ++i){
			std::string name = bp::extract<std::string>((*i)[0])();
			ColorMapPtr map = ColorMapPtr( new MatplotlibColorMap( (*i)[1] ) );
			mlist.push_back( ColorMap::NamedMap( name, map ) );
		}

	}
	catch( bp::error_already_set& e){
		log_error_stream(e);
		return;
	}

}

} // namespace scripting

ColorMap::mapList ColorMap::availableNativeMaps(){
	// this function used to return an unchanging global list instead of regenerating it each time.
	// That works fine for the I3TimeColorMap, but users of this function need the returned constant color maps
	// to be independent in memory.
	ColorMap::mapList native_maps;
	native_maps.push_back( NamedMap("Shovel colormap", ColorMapPtr( new I3TimeColorMap()) ) );
	native_maps.push_back( NamedMap("Constant color", ColorMapPtr( new ConstantColorMap( QColor::fromRgbF(.5,.5,.5) ) ) ) );
	return native_maps;
}

const ColorMap::mapList& ColorMap::availablePythonMaps(){
	if( scripting::pythonMapsGathered == false ){
		scripting::gatherPythonCMaps(python_maps_);
	}
	return python_maps_;
}


ColorMapPtr ColorMap::getPythonMapByName( const std::string& str ){
	// call this rather than using python_maps_ to ensure its populated.
	const ColorMap::mapList& pymaps = availablePythonMaps();
	for( ColorMap::mapList::const_iterator i = pymaps.begin(); i != pymaps.end(); ++i ){
		if( str == i->name ){
			return i->map;
		}
	}
	log_error_stream( "Could not find a python color map named " << str );
	return ColorMapPtr( new I3TimeColorMap() );
}

std::string ColorMap::toString( ColorMapPtr ptr ){
	if( dynamic_cast<I3TimeColorMap*>( ptr.get() ) )
		return "I3TimeColorMap()";
	else if ( dynamic_cast<ConstantColorMap*>( ptr.get() ) ){
		QColor c = ptr->value(0);
		std::stringstream s;
		s <<  "ConstantColorMap( PyQColor( " 
		  << c.red() << ',' << c.green() << ',' << c.blue() 
		  << ',' << c.alpha() << " ) )";
		return s.str();
	}
	else if( dynamic_cast<scripting::MatplotlibColorMap*>( ptr.get() ) ){
		for( ColorMap::mapList::iterator i = python_maps_.begin(); i != python_maps_.end(); ++i ){
			if( ptr == i->map ){
				std::stringstream s;
				s << "ColorMap.getPythonMapByName('" << i->name << "')";
				return s.str();
			}
		}
	}

	log_error_stream( "Could not create a string representation of a color map" );
	return "I3TimeColorMap()";
}


QPixmap ColorMap::image( int x, int y ){
	QPixmap ret( x, y );
	QPainter p(&ret);
	for( int i = 0; i < x; ++i ){
		p.fillRect(i,0,1,y,value(float(i)/x));
	}
	return ret;
}
