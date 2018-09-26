#ifndef I3_SHOVEL_TIMELINE_WIDGET_H
#define I3_SHOVEL_TIMELINE_WIDGET_H

#include "shovelart/timewindow.h"

#include <QWidget>
#include <QGraphicsView>
#include <QResizeEvent>
#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>

Q_DECLARE_METATYPE( TimeWindow );

namespace Ui{ class TimelineWidget; }
namespace timeline{ class RangeFinder; }

class TimelineWidget : public QWidget
{

Q_OBJECT;
SET_LOGGER( "Timeline" );

private:
	Ui::TimelineWidget* ui;
	I3FramePtr frame_ptr_;
	std::auto_ptr<timeline::RangeFinder> rangefinder_;
	TimeWindow timewin_;
	double nanoseconds_per_second_;
	bool animation_active_;
	bool looping_;
	struct LoopMode {
		enum Enum {
			OneFrame,
			AllFrames,
			AllXFrames
		};
	};
	LoopMode::Enum loopMode_;

private:
	void resetAnimationWindow();

public:
	TimelineWidget( QWidget* parent );

	double getTime() const { return timewin_.current(); }
	void setTime(double newcur);
	Q_PROPERTY( double time READ getTime WRITE setTime NOTIFY timeWindowChanged );

	double getMaxTime() const { return timewin_.colorEnd(); }
	void setMaxTime(double newmax);
	Q_PROPERTY( double maxTime READ getMaxTime WRITE setMaxTime NOTIFY timeWindowChanged );

	double getMinTime() const { return timewin_.colorStart(); }
	void setMinTime( double m );
	Q_PROPERTY( double minTime READ getMinTime WRITE setMinTime NOTIFY timeWindowChanged );

	double getEventBeginTime() const { return timewin_.start(); }
	Q_PROPERTY( double eventBeginTime READ getEventBeginTime WRITE setEventBeginTime NOTIFY timeWindowChanged );

	double getEventEndTime() const { return timewin_.end(); }
	Q_PROPERTY( double eventEndTime READ getEventEndTime WRITE setEventEndTime NOTIFY timeWindowChanged );

	QString getRangeFinder() const;
	Q_PROPERTY( QString rangeFinder READ getRangeFinder WRITE setRangeFinder );

	double getAnimationRate() const { return nanoseconds_per_second_; }
	Q_PROPERTY( double animationRate READ getAnimationRate WRITE setAnimationRate );

	bool isPlaying() const { return animation_active_; }
	Q_PROPERTY( bool isPlaying READ isPlaying );

	Q_CLASSINFO( "qmeta_scriptable_props", "time,maxTime,minTime,eventBeginTime,eventEndTime,rangeFinder,animationRate,isPlaying" );
	Q_CLASSINFO( "qmeta_scriptable_slots", "play,stop,fastForward,rewind,stepForward,stepBack,reset,setEventTimeWindow" );

	const TimeWindow& getTimeWindow() const { return timewin_; }
	void frameUpdate( I3FramePtr ptr );

public Q_SLOTS:
	void play( double nanoseconds_per_second = 0 );
	void stop();
	void togglePlay( bool on, double nanoseconds_per_second = 0 ){
		on ? play(nanoseconds_per_second) : stop();
	}
	void fastForward();
	void rewind();
	void stepForward();
	void stepBack();
	void reset();
	void setEventTimeWindow( double start, double stop );
	void setEventBeginTime( double m );
	void setEventEndTime( double m );

	void setAnimationRate( double nanoseconds_per_second );

	void setLooping( bool on ){ looping_ = on; }
	void setLoopOne(){ loopMode_ = LoopMode::OneFrame; }
	void setLoopAll(){ loopMode_ = LoopMode::AllFrames; }
	void setLoopAllX(){ loopMode_ = LoopMode::AllXFrames; }

protected Q_SLOTS:
	void animationStep();
	void populateComboBox();
	void setRangeFinder(const QString&);
	void emitSubSignals();

Q_SIGNALS:

	void timeWindowChanged( const TimeWindow& );
	void timeHistogramChanged( const std::vector<float>& );

	// the below are provided for easy connections to UI elements that want to display data from the TimeWindow.
	// Program-internal classes should not connect to these signals and prefer timeWindowChanged()
	void eventBeginChanged(double);
	void eventEndChanged(double);

	// signalled with true when animation starts, false when it stops
	void animationRunning( bool );

	void animationRateChanged( double );

};

class WithPlayPaused {
	TimelineWidget* w_;
	bool wasPlaying_;

public:
	WithPlayPaused( TimelineWidget* w ) :
		w_( w ),
		wasPlaying_( w->isPlaying() )
	{ w->stop(); }

	~WithPlayPaused()
	{ if( wasPlaying_ ) w_->play(); }
};

namespace timeline {

/* These classes would live entirely in TimelineWidget.cpp, but moc insists
 * on having them in a header file. */

/**
 * A QGraphicsView that keeps the scene box (0,0) to (1,1) fixed in the view.
 * The .ui file specifies that TimelineWidget should use these views, and
 * sets their scrollsbars to never appear.
 */
class TimelineView : public QGraphicsView {
	Q_OBJECT;
public:
	TimelineView(QWidget* parent) : QGraphicsView(parent) {}

protected:
	void resizeEvent( QResizeEvent* event );

public Q_SLOTS:
	void fitView( const QRectF& );
};

class TimelineLegendItem;
class TimelineControl;

class TimelineScene : public QGraphicsScene
{
	Q_OBJECT;
	SET_LOGGER( "Timeline" );

protected:
	QGraphicsItem* timebar_;
	QGraphicsSimpleTextItem* timelabel_;
	TimelineLegendItem* legend_;
	TimelineControl *left_handle_, *right_handle_;
	double lower_, upper_, mark_;
	QGraphicsItemGroup* hist_group_;
	TimelineWidget* owner_;
	TimelineScene( TimelineWidget* w );
	void pickNewTime( QGraphicsSceneMouseEvent* event );
	virtual void mouseMoveEvent( QGraphicsSceneMouseEvent* event );
	virtual void mousePressEvent( QGraphicsSceneMouseEvent* event );
	friend class ::TimelineWidget;

public:
	QRectF timeWindow() const{ return QRectF( lower_, 0, upper_-lower_, 1 ); }

protected Q_SLOTS:
	void setTimeWindow( const TimeWindow& );
	void setHistogram( const std::vector<float>& );
};

} // namespace timeline


#endif /* I3_SHOVEL_TIMELINE_WIDGET_H */
