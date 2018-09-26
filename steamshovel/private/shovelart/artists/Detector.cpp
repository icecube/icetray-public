#include <boost/foreach.hpp>

#include <QFile>
#include <QTextStream>

#include <dataclasses/geometry/I3Geometry.h>
#include <icetray/OMKey.h>

#include "Detector.h"
#include "shovelart/Scene.h"
#include "shovelart/SceneObject.h"

#include "scripting/gil.h"
#include <boost/python.hpp>

namespace {

	typedef std::vector<vec3d> vec3d_list;

	class NameMap {
		std::map<OMKey, std::string> namemap_;
		std::string empty;

		// The dom list, doms.txt, comes from the phys-service project.
		// It is pulled into steamshovel's program resources via resources/steamshovel.qrc.
		// We delay initialization until the map gets its first request.
		void init(){
			log_debug( "Creating dom names map" );
			QFile qf(":/resources/doms.txt");
			qf.open(QIODevice::ReadOnly);
			QTextStream str(&qf);
			QString line;
			while( !str.atEnd() ) {
				QString line = str.readLine();
				// line format is NN-MM SERIAL NAME HASH
				// we want to make an OMKey out of NN,MM
				QStringList tokens = line.split( QRegExp("\\s+") );
				QStringList IDs = tokens[0].split('-');
				namemap_[ OMKey(IDs[0].toInt(), IDs[1].toInt()) ] = tokens[2].toStdString();
			}
		}
	public:
		const std::string& operator()( OMKey key ){
			if( namemap_.empty() )
				init();
			if( namemap_.find(key) != namemap_.end() ){
				return namemap_[key];
			}
			return empty;
		}

	} dom_names;

	void draw_string(SceneGroup* g, const vec3d_list& points,
	                 VariantQColorPtr string_color,
	                 VariantFloatPtr string_width,
	                 bool show_cross){
		StaticLineObject* line = new StaticLineObject( points );
		line->setColor( string_color );
		line->setLineWidth( string_width );
		g->add( line );
		// perhaps draw a little cross on top of the string
		if( show_cross ){
			const double w = 20; // meter
			vec3d_list xp(2, points.front());
			xp[0] -= vec3d(w, 0, 0);
			xp[1] += vec3d(w, 0, 0);
			StaticLineObject* xline = new StaticLineObject( xp );
			xline->setColor( string_color );
			xline->setLineWidth( string_width );
			g->add( xline );
			vec3d_list yp(2, points.front());
			vec3d dy(0, w, 0);
			yp[0] -= vec3d(0, w, 0);
			yp[1] += vec3d(0, w, 0);
			StaticLineObject* yline = new StaticLineObject( yp );
			yline->setColor( string_color );
			yline->setLineWidth( string_width );
			g->add( yline );
		}
	}

} // namespace

Detector::Detector(){
	addSetting( "DOM labels", false );
	addSetting( "DOM radius", RangeSetting(0.0, 10.0, 100, 1.0) );
	addSetting( "DOM color", QColor::fromRgbF(1.0, 1.0, 1.0, 1.0) );
	addSetting( "string color", QColor::fromRgbF(1.0, 1.0, 1.0, 0.3) );
	addSetting( "string width", RangeSetting(0.0, 10.0, 100, 1.0) );
	addSetting( "string cross", false );
	addSetting( "outline width", RangeSetting(0.0, 10.0, 100, 0.0) );
	addSetting( "high quality DOMs", true );
	addSetting( "hide", ChoiceSetting("nothing;IceCube;IceTop", 0) );
}

void Detector::create( I3FramePtr ptr, SceneGroup* g, const SceneState& state ){

	I3GeometryConstPtr geo = ptr->Get<I3GeometryConstPtr>( keys()[0] );

	VariantQColorPtr string_color( new SceneConstant<QColor>( setting<QColor>("string color") ) );
	VariantQColorPtr dom_color( new SceneConstant<QColor>( setting<QColor>("DOM color") ) );
	VariantFloatPtr string_width( new SceneConstant<float>( setting<RangeSetting>("string width") ) );
	VariantFloatPtr outline_width( new SceneConstant<float>( setting<RangeSetting>("outline width") ) );
	const float domrad = setting<RangeSetting>("DOM radius");

	const bool shaded = setting<bool>("high quality DOMs");
	const bool show_doms = domrad > 0.0f;
	const bool show_labels = setting<bool>("DOM labels");
	const bool show_strings = setting<RangeSetting>("string width") > 0.0f;
	const bool show_cross = setting<bool>("string cross");
	const bool show_outline = setting<RangeSetting>("outline width") > 0.0f;
	const bool hide_icecube = setting<ChoiceSetting>("hide") == 1;
	const bool hide_icetop = setting<ChoiceSetting>("hide") == 2;

	vec3d_list string_accum;
	vec3d_list icetop_points, top_points, bottom_points;

	int string = 0;
	BOOST_FOREACH( I3OMGeoMap::const_reference ref, geo->omgeo )
	{
		OMKey key = ref.first;
		const I3OMGeo geo = ref.second;

		if( geo.omtype == I3OMGeo::IceTop ){
		  if ( hide_icetop )
		    continue;
		}else if ( geo.omtype == I3OMGeo::IceCube) {
		  if( hide_icecube )
		    continue;
		}else{
		  //only show IceCube and IceTop hide anything we don't understand 
		  continue;
		}
		
		if( string != key.GetString() ){
			if( string_accum.size() ){
				bottom_points.push_back( string_accum.back() );
				top_points.push_back( string_accum.front() );
				if( show_strings )
					draw_string( g, string_accum, string_color,
					             string_width, show_cross );
				string_accum.clear();
			}
			string = key.GetString();
		}

		const I3Position& pos = geo.position;
		vec3d pos3d( pos.GetX(), pos.GetY(), pos.GetZ() );

		if( show_doms ){
			SphereObject* dom = new SphereObject( domrad, pos3d );
			if( !shaded )
				dom->setShader( SphereObject::UNSHADED_GEOM );
			dom->setColor( dom_color );
			dom->setSelectionContent( key );
			g->add( dom );
		}

		if( show_doms && show_labels ){
			SceneObject* domlabel = new TextObject( state.gl,
			                                        key.str().substr(5) + " " + dom_names(key),
			                                        pos3d );
			g->add( domlabel );
		}

		if( geo.omtype == I3OMGeo::IceTop ) {
		  icetop_points.push_back(pos3d);
		} else if ( geo.omtype == I3OMGeo::IceCube ){
		  string_accum.push_back(pos3d);
		}
	}

	// create the final string
	if( string_accum.size() ){
		bottom_points.push_back( string_accum.front() );
		top_points.push_back( string_accum.back() );
		if( show_strings )
			draw_string( g, string_accum, string_color,
			             string_width, show_cross );
		string_accum.clear();
	}

	if( show_outline ){
		using namespace boost::python;

		scripting::ScopedGIL gil;
		object mod = import( "icecube.steamshovel.artists.util" );
		object result = mod.attr( "detector_outline" )( bottom_points, top_points );
		vec3d_list points[2] = {
			extract<vec3d_list>( result[0] ),
			extract<vec3d_list>( result[1] )
		};

		if( !points[0].empty() && points[0].size() == points[1].size() ){
			// draw vertical edges
			for( int i = 0, n = points[0].size(); i < n; ++i ){
				vec3d_list vert(2);
				vert[0] = points[0][i];
				vert[1] = points[1][i];
				StaticLineObject* line = new StaticLineObject( vert );
				line->setColor( string_color );
				line->setLineWidth( outline_width );
				g->add( line );
			}

			// draw top and bottom
			for( int i = 0; i < 2; ++i ){
				points[i].push_back( points[i].front() ); // close the polygon
				StaticLineObject* line = new StaticLineObject( points[i] );
				line->setColor( string_color );
				line->setLineWidth( outline_width );
				g->add( line );
			}
		}
	}
}
