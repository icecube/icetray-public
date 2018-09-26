#include <sstream>

#include "TimeLabel.h"
#include "shovelart/Texture.h"
#include "shovelart/Text.h"
#include "shovelart/Scene.h"
#include "shovelart/SceneObject.h"

/**
 * Textual label displaying the current visible time.
 * Settings for font and font size
 */
class TimeLabelOverlay : public TextOverlay{

protected:
	double t_;

public:
	static std::string toStr( double w ){
		std::stringstream str;
		str << "t = " << long(w);
		return str.str();
	}

	TimeLabelOverlay( QGLWidget* w, double t, QFont font = QFont() ) :
		TextOverlay(w, toStr(t), font), t_(t)
	{}

	virtual void draw( double vistime, const I3Camera& camera ){
		if(vistime != t_){
			text_ = toStr( vistime );
			needs_update_ = true;
			t_ = vistime;
		}
		TextOverlay::draw(vistime,camera);
	}

};

TimeLabel::TimeLabel(){
	QFont font;
	addSetting( "font", font );
	addSetting( "fontsize", font.pointSize() );
	addSetting( "color", QColor::fromRgb(255, 255, 255, 255) );
}

void TimeLabel::create( I3FramePtr ptr, SceneGroup* group, const SceneState& state ){
	QFont f = setting<QFont>("font");
	f.setPointSize( setting<int>("fontsize") );
	TimeLabelOverlay *tl = new TimeLabelOverlay( state.gl, 0, f );
	tl->setColor( setting<QColor>("color") );
	group->add( tl );
}
