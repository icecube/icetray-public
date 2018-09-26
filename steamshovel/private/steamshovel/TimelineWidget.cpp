#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <boost/foreach.hpp>
#include <boost/python.hpp>

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QFont>
#include <QTime>
#include <QTimer>

#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <icetray/I3Frame.h>

#include "scripting/gil.h"
#include "scripting/pyerr.h"

#include "shovelart/ColorVariant.h"

#include "ShovelMainWindow.h"
#include "SteamshovelApp.h"
#include "FileService.h"
#include "TimelineWidget.h"
#include "ui_timeline.h"
#include "moc_TimelineWidget.cpp"

/***********************************************
 * TimelineWidget support classes
 ***********************************************/
namespace timeline {

namespace bp = boost::python;

/**
 *  Extracts a valid event time range from an I3Frame
 */
class RangeFinder {

	SET_LOGGER("RangeFinder");

	bp::object pythonGetRange_;
	bp::object pythonGetHistogram_;
	typedef std::map<std::string, bool> TypeMap;
	TypeMap typemap_; // remember hasRange results

	void set( const std::string& name, bool hasRange )
	{ typemap_.insert( std::make_pair(name, hasRange) ); }

public:
	RangeFinder() {
		scripting::ScopedGIL gil;
		try {
			bp::object mod = bp::import( "icecube.steamshovel.util.rangefinder" );
			pythonGetRange_ = mod.attr( "getTimeRange" );
			pythonGetHistogram_ = mod.attr( "getRangeHistogram" );
		} catch ( bp::error_already_set& e ) {
			log_error_stream(e);
		}

		// prepare for known types with high de-serialization cost
		set( "I3Geometry", false );
		set( "I3Calibration", false );
		set( "I3Map<OMKey, vector<I3MCPE> >", true );
		set( "I3Tree<I3Particle>", true );
		set( "I3RecoPulseSeriesMap", true );
		set( "I3RecoPulseSeriesMapMask", true );
		set( "I3RecoPulseSeriesMapUnion", true );
	}

	bool hasRange( I3FramePtr frame, const QString& key ){
		// fast check for known types
		const std::string type_name = frame->type_name( key.toStdString() );
		TypeMap::const_iterator it = typemap_.find( type_name );
		if( it != typemap_.end() ){
			log_trace_stream( "hasRange: fast decision for "
			                  << key.toStdString() << ": " << it->second );
			return it->second;
		}

		// explicit check for unknown types
		double a = 0, b = 0;
		getRange( frame, key, a, b );
		const bool hasRange = a < b;

		log_trace_stream( "hasRange: explicit check for "
		                  << key.toStdString() << ": " << hasRange );
		typemap_.insert( std::make_pair(type_name, hasRange) );

		return hasRange;
	}

	void getRange( I3FramePtr frame, const QString& key, double& a, double& b ) {
		log_trace_stream( "getRange called for " << key.toStdString() );
		scripting::ScopedGIL gil;

		try {
			bp::object ret = pythonGetRange_( frame, key );
			a = bp::extract<double>(ret[0]);
			b = bp::extract<double>(ret[1]);
		} catch ( bp::error_already_set& ) {
			// ignore Python exceptions
			PyErr_Clear();
		}
	};

	void getHistogram( I3FramePtr frame, const QString& key,
	                   std::vector<float>& hist ) {
		log_trace_stream( "getHistogram called for " << key.toStdString() );
		scripting::ScopedGIL gil;

		try {
			bp::object ret = pythonGetHistogram_( frame, key,
			                                      pythonGetRange_(frame, key) );
			if (ret) {
				I3Vector<float>& dat = bp::extract< I3Vector<float>& >(ret);
				hist.swap( dat );
				return;
			}
		}
		catch( bp::error_already_set& ) {
			// ignore Python exceptions
			PyErr_Clear();
		}
	}

};

/***********************************************
 * TimelineWidget QGraphicsScenes
 ***********************************************/

class TimelineControl : public QGraphicsEllipseItem
{
public:
	enum Type{LOWER,UPPER};
	Type type_;
	TimelineWidget* w_;

public:
	TimelineControl(Type t, TimelineWidget* w) :
		QGraphicsEllipseItem( -10, -10, 20, 20 ), type_(t), w_(w)
	{
		setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
		setBrush( type_ == LOWER ? Qt::red : Qt::blue );
	}

protected:

	virtual void mouseMoveEvent( QGraphicsSceneMouseEvent* event ){
		qreal x = event->scenePos().x();
		try{
			if( type_ == LOWER ){
				w_->setMinTime( x );
			}
			else
				w_->setMaxTime( x ) ;
		}
		catch( TimeWindowError& e ){
			log_trace_stream( "Caught " << e.what() );
		}
		event->accept();
	}

	// an implementation of this is required to receive mouseMoveEvents, so do not remove
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event ){
		event->accept();
	}
};

class TimelineLegendItem : public QGraphicsItemGroup
{
protected:
	ColorMapPtr cmap_;
	QGraphicsRectItem *left_legend_, *legend_, *right_legend_;

public:
	TimelineLegendItem() : cmap_( new I3TimeColorMap() )
	{
		QLinearGradient gradient(QPointF(0,0), QPointF(1,0));
		const int steps = 512;
		for( int i = 0; i < steps; ++i ){
			qreal stage = qreal(i) / steps;
			gradient.setColorAt(stage, QColor::fromHsvF(stage*0.7,.5,1));
		}

		QPen no_pen(Qt::NoPen);
		legend_ = new QGraphicsRectItem( 0, .7, 1, .3 );
		legend_->setBrush( gradient );
		//legend_->setBrush( cmap_->value(.5) );
		legend_->setPen( no_pen );
		addToGroup(legend_);

		left_legend_ = new QGraphicsRectItem( -1, .7, 1, .3 );
		left_legend_->setBrush( gradient );
		left_legend_->setPen( no_pen );
		addToGroup( left_legend_ );

		right_legend_ = new QGraphicsRectItem( 1, .7, 1, .3 );
		right_legend_->setBrush( gradient );
		right_legend_->setPen( no_pen );
		addToGroup( right_legend_ );

		setZValue( -1 );
	}


	void setTimeWindow( const TimeWindow& window ){
		QTransform t = QTransform().translate(window.colorStart(),0)
		                           .scale( window.colorEnd()-window.colorStart(), 1 );
		legend_->setTransform(t);

		if( window.colorStart() != window.start() ){
			left_legend_->setVisible(true);
			QTransform lt = QTransform().translate(window.start(),0)
			                            .scale( window.colorStart() - window.start(), 1 )
			                            .translate(1,0);
			left_legend_->setTransform(lt);
		}
		else{
			left_legend_->setVisible(false);
		}

		if( window.colorEnd() != window.end() ){
			right_legend_->setVisible(true);
			QTransform rt = QTransform().translate( window.colorEnd(), 0 )
			                            .scale( window.end() - window.colorEnd(), 1 )
			                            .translate( -1, 0 );
			right_legend_->setTransform(rt);
		}
		else{
			right_legend_->setVisible(false);
		}

	}
};

TimelineScene::TimelineScene(TimelineWidget* w) : QGraphicsScene(w),
	lower_(-2), upper_(2), mark_(0), owner_(w)
{
	timebar_ = addLine( 0,0,0,1 );
	timebar_->show();

	timelabel_ = addSimpleText( QString::number(0) );
	timelabel_->setFlag( QGraphicsItem::ItemIgnoresTransformations );
	QFont font;
	font.setPixelSize( 15 );
	timelabel_->setFont( font );

	legend_ = new TimelineLegendItem();
	addItem( legend_ );

	left_handle_ = new TimelineControl( TimelineControl::LOWER, w );
	addItem( left_handle_ );

	right_handle_ = new TimelineControl( TimelineControl::UPPER, w );
	addItem( right_handle_ );

	hist_group_ = new QGraphicsItemGroup();
	addItem( hist_group_ );

}

void TimelineScene::setHistogram( const std::vector<float>& hist ){
	removeItem( hist_group_ );
	delete hist_group_;
	hist_group_ = new QGraphicsItemGroup();

	QPen blue_pen(Qt::darkCyan);
	QBrush blue_brush(Qt::darkCyan);

	if( hist.size() > 0 ){
		double span = sceneRect().width();
		double xbase = sceneRect().x();
		double spandiv = span / hist.size();

		for( unsigned int i = 0; i < hist.size(); ++i ){
			double x = xbase + spandiv * i;
			double h = hist[i] * 0.7;
			if( h < 0.001 ) continue;
			QGraphicsRectItem* rect = new QGraphicsRectItem( x, .7-h, spandiv, h, hist_group_ );
			rect->setPen(blue_pen);
			rect->setBrush(blue_brush);
			hist_group_->addToGroup( rect );
		}
	}

	hist_group_->setZValue( -5 );
	addItem( hist_group_ );
}

void TimelineScene::setTimeWindow( const TimeWindow& window ){
	mark_ = window.current();
	lower_ = window.start();
	upper_ = window.end();
	timebar_->setPos( mark_, 0 );

	timelabel_->setText( QString::number(mark_) );
	timelabel_->setPos( QPointF( mark_, 0 ) );
	setSceneRect( lower_, 0, upper_-lower_, 1 );

	// Move the timelabel_ left of the timebar_ if it's close to the edge of the view
	QRectF labelrect = timelabel_->sceneBoundingRect();
	QTransform viewport = views().at(0)->viewportTransform(); // there's always exactly one view, right? :)
	QRectF labelrect_scene = viewport.inverted().mapRect( labelrect );
	labelrect.setWidth( labelrect_scene.width() );
	// clamp Y values to anything within [0,1], since we only need to compare on X values
	labelrect.setY( 0.1 );
	labelrect.setHeight( 0.8 );
	if( !sceneRect().contains( labelrect ) ){
		timelabel_->moveBy( -labelrect.width(), 0 );
	}

	legend_->setTimeWindow( window );
	left_handle_->setPos( window.colorStart(), .7 );
	right_handle_->setPos( window.colorEnd(), .7 );

	update();
}

void TimelineScene::pickNewTime( QGraphicsSceneMouseEvent* event ){
	qreal x = event->scenePos().x();
	try{
		owner_->setTime( x );
	}
	catch( TimeWindowError& e ){
		log_debug_stream( x << " -- " << e.what() );
	}
}

void TimelineScene::mouseMoveEvent( QGraphicsSceneMouseEvent* event ){
	QGraphicsScene::mouseMoveEvent( event );
	if( !event->isAccepted() )
		pickNewTime(event);
}

void TimelineScene::mousePressEvent( QGraphicsSceneMouseEvent* event ){
	QGraphicsScene::mousePressEvent( event );
	if( !event->isAccepted() )
		pickNewTime(event);
}

/***********************************************
 * TimelineView methods
 ***********************************************/

void TimelineView::fitView( const QRectF& r ){
	fitInView( r );
}

void TimelineView::resizeEvent( QResizeEvent* event ){
	QGraphicsView::resizeEvent( event );
	fitView( scene()->sceneRect() );
}

} // namespace timeline
using timeline::RangeFinder;
using timeline::TimelineScene;

/***********************************************
 * TimelineWidget members
 ***********************************************/

TimelineWidget::TimelineWidget(QWidget* parent): QWidget(parent),
	ui(new Ui::TimelineWidget),
	rangefinder_(new RangeFinder()),
	animation_active_(false),
	looping_(false),
	loopMode_(LoopMode::OneFrame)
{
	log_debug( "Creating timeline widget" );
	ui->setupUi(this);
	connect( this, SIGNAL(timeWindowChanged(const TimeWindow&)),
	         SLOT(emitSubSignals()) );
	connect( ui->timesourceComboBox, SIGNAL(activated(const QString&)),
	         SLOT(setRangeFinder(const QString&)));
	connect( this, SIGNAL(eventBeginChanged(double)),
	         ui->eventStartSpinner, SLOT(setValue(double)) );
	connect( this, SIGNAL(eventEndChanged(double)),
	         ui->eventEndSpinner, SLOT(setValue(double)) );
	connect( ui->eventStartSpinner, SIGNAL(valueChanged(double)),
	         SLOT(setEventBeginTime(double)) );
	connect( ui->eventEndSpinner, SIGNAL(valueChanged(double)),
	         SLOT(setEventEndTime(double)) );

	TimelineScene* scene = new TimelineScene(this);
	ui->timePlot->setScene( scene );
	ui->timePlot->show();
	connect( this, SIGNAL(timeWindowChanged(const TimeWindow&)),
	         scene, SLOT(setTimeWindow(const TimeWindow&)) );
	connect( this, SIGNAL(timeHistogramChanged( const std::vector<float>& )),
	         scene, SLOT(setHistogram(const std::vector<float>&)) );
	connect( scene, SIGNAL(sceneRectChanged(const QRectF&)),
	         ui->timePlot, SLOT(fitView(const QRectF&)) );

	populateComboBox();
	setAnimationRate( 0.0 ); // sets the default rate
}

void TimelineWidget::populateComboBox(){
	log_debug("updating combo box");

	using timeline::RangeFinder;

	// remember previous key
	const QString prev_key = ui->timesourceComboBox->currentText();

	ui->timesourceComboBox->clear();

	// add keys to the box which RangeFinder can handle
	if( frame_ptr_ ){
		std::vector<std::string> keys = frame_ptr_->keys();
		sort(keys.begin(), keys.end());
		BOOST_FOREACH(const std::string& k, keys) {
			QString key = QString::fromStdString(k);
			if (rangefinder_->hasRange( frame_ptr_, key )) {
				ui->timesourceComboBox->addItem( key );
				log_debug( "added key %s", k.c_str() );
			}
		}
	}

	// add default key
	ui->timesourceComboBox->addItem( "Default" );

	// ensure previous selection remains
	if ( !prev_key.isEmpty() )
		setRangeFinder( prev_key );
	else
		setRangeFinder( "Default" );
}

void TimelineWidget::setRangeFinder( const QString& name ){
	log_debug( "setRangeFinder with argument: %s", name.toStdString().c_str() );

	double a = 0, b = 40000;
	std::vector<float> hist;

	int idx = ui->timesourceComboBox->findText( name );
	if (idx == -1) {
		idx = ui->timesourceComboBox->findText( "Default" );
		ui->timesourceComboBox->insertItem( idx, name );
	} else {
		if (name != "Default") {
			rangefinder_->getRange( frame_ptr_, name, a, b );
			if (a < b)
				rangefinder_->getHistogram( frame_ptr_, name, hist );
		}
	}

	ui->timesourceComboBox->setCurrentIndex( idx );
	setEventTimeWindow(a, b);
	Q_EMIT timeHistogramChanged( hist );
}

// private reset operation; callers must ensure signals are sent
void TimelineWidget::resetAnimationWindow(){
	double start = timewin_.start();
	double end = timewin_.end();
	timewin_.setWindow( start, start, end, end, end );
}

// public reset slot
void TimelineWidget::reset(){
	resetAnimationWindow();
	Q_EMIT timeWindowChanged(timewin_);
}

void TimelineWidget::emitSubSignals(){
	// update the spinboxes here to ensure they do not allow users to specify invalid values
	// their signals need to be blocked while these changes go through
	ui->eventStartSpinner->blockSignals(true);
	ui->eventEndSpinner->blockSignals(true);
	ui->eventStartSpinner->setMaximum( timewin_.end() - 1 );
	ui->eventEndSpinner->setMinimum( timewin_.start() + 1 );
	// called when timeWindowChanged() emits; this provides signals for individual integers
	// for widgets that really need access to them
	Q_EMIT eventBeginChanged( timewin_.start() );
	Q_EMIT eventEndChanged( timewin_.end() );

	ui->eventStartSpinner->blockSignals(false);
	ui->eventEndSpinner->blockSignals(false);
}

void TimelineWidget::setEventBeginTime( double t ){
	timewin_.setCurrent( std::max( timewin_.current(), t ) );
	timewin_.setColorStart( std::max( timewin_.colorStart(), t) );
	timewin_.setStart( t );
	Q_EMIT timeWindowChanged(timewin_);
}

void TimelineWidget::setEventEndTime( double t ){
	timewin_.setCurrent( std::min( timewin_.current(), t ) );
	timewin_.setColorEnd( std::min( timewin_.colorEnd(), t ) );
	timewin_.setEnd( t );
	Q_EMIT timeWindowChanged(timewin_);
}

void TimelineWidget::setEventTimeWindow( double start, double stop ){
	double current = timewin_.current();
	if( animation_active_ )
		current = start;
	else{
		current = std::min( current, stop );
		current = std::max( current, start );
	}
	timewin_.setWindow( start, start, current, stop, stop );
	Q_EMIT timeWindowChanged(timewin_);
}

void TimelineWidget::setMinTime( double m ){
	timewin_.setColorStart( m );
	Q_EMIT timeWindowChanged(timewin_);
}

void TimelineWidget::setMaxTime( double m ){
	timewin_.setColorEnd( m );
	Q_EMIT timeWindowChanged(timewin_);
}

void TimelineWidget::setTime( double m ){
	timewin_.setCurrent( m );
	Q_EMIT timeWindowChanged(timewin_);
}

QString TimelineWidget::getRangeFinder() const {
  return ui->timesourceComboBox->currentText();
}

void TimelineWidget::setAnimationRate( double nanoseconds_per_second ){
	if( nanoseconds_per_second > 0.0 )
		nanoseconds_per_second_ = nanoseconds_per_second;
	else
		nanoseconds_per_second_ = 1000.0;
	Q_EMIT animationRateChanged( nanoseconds_per_second_ );
}

void TimelineWidget::play( double nanoseconds_per_second ){
	log_debug( "Play: %.0f", nanoseconds_per_second );
	if( nanoseconds_per_second > 0 )
		setAnimationRate( nanoseconds_per_second );
	if( !animation_active_ && (timewin_.current() >= timewin_.end()) )
		setTime( timewin_.start() );
	animation_active_ = true;
	animationStep();
	Q_EMIT animationRunning( true );
}

void TimelineWidget::stop(){
	log_debug( "Stop" );
	animation_active_ = false;
	Q_EMIT animationRunning( false );
}

void TimelineWidget::animationStep(){
	if( !animation_active_ )
		return;

	QTime now; // measure execution time of animation step
	now.start();

	const double frames_per_second = 30.0;
	const double nanoseconds_per_frame = nanoseconds_per_second_ / frames_per_second;

	const double next_time = timewin_.current() + nanoseconds_per_frame;
	FileService& fs = SteamshovelApp::instance()->files();
	if( next_time > timewin_.end() ){
		if( !looping_ ){
			stop();
			setTime( timewin_.end() );
		}else{
			switch( loopMode_ ){
			case LoopMode::OneFrame:
				setTime( timewin_.start() );
				break;
			case LoopMode::AllFrames:
				setTime( timewin_.start() );
				// wrap around at end
				if( !fs.advanceFrame(1) )
					fs.selectFrame(0);
				break;
			case LoopMode::AllXFrames:
				setTime( timewin_.start() );
				// wrap around at end
				const FrameIndex& index = fs.frameIndex();
				const I3Frame::Stream stop = index[fs.currentIndex()].stream_;
				if( !fs.nextMatchingFrame( stop ) ){
					for( unsigned i = 0; i < index.size(); ++i ){
						if( index[i].stream_ == stop ){
							fs.selectFrame(i);
							break;
						}
					}
				}
				break;
			}
		}
	}
	else{
		setTime( next_time );
	}

	QCoreApplication::processEvents();
	// trigger the next update, take execution time into account
	const int next_real_time = 1000.0 / frames_per_second - now.elapsed();
	QTimer::singleShot( std::max(next_real_time, 0), this, SLOT(animationStep()) );
}

void TimelineWidget::fastForward(){
	setTime( timewin_.colorEnd() );
}

void TimelineWidget::rewind(){
	setTime( timewin_.colorStart() );
}

void TimelineWidget::stepForward(){
	setTime( std::min( timewin_.colorEnd(), timewin_.current()+1 ) );
}

void TimelineWidget::stepBack(){
	setTime( std::max( timewin_.colorStart(), timewin_.current()-1 ) );
}

void TimelineWidget::frameUpdate( I3FramePtr ptr ){
	frame_ptr_ = ptr;

	populateComboBox();

	/* If animation is not active, then set the current time to max */
	if( !animation_active_ ){
		setTime( timewin_.colorEnd() );
	}
}
