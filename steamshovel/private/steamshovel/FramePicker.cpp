#include "shovelart/BindSignal.h"
#include "SteamshovelApp.h"
#include "FileService.h"
#include "FramePicker.h"
#include "moc_FramePicker.cpp"

#include <dataclasses/physics/I3EventHeader.h>

#include <map>
#include <vector>
#include <boost/foreach.hpp>

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>
#include <QString>
#include <QVector>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPushButton>

static const qreal RADIUS = 20;
static const qreal YOFF = RADIUS * 0.8;
static const qreal XOFF = RADIUS * 0.75;

static QColor StreamColor( const I3Frame::Stream& s )
{
	QColor qc;

	if( s == I3Frame::Geometry )
		qc = QColor(Qt::blue);
	else if( s == I3Frame::Calibration )
		qc = QColor(Qt::red);
	else if( s == I3Frame::DetectorStatus )
		qc = QColor(Qt::cyan);
	else if( s == I3Frame::DAQ )
		qc = QColor(Qt::green);
	else if( s == I3Frame::Physics )
		qc = QColor(Qt::yellow);
	else
		qc = QColor(Qt::gray);

	return qc.light();
}

class PhysicsSubColor {
	typedef std::map<std::string, QColor> Map;
	Map map_;

public:
	QColor operator()( const std::string& sub_event_stream )
	{
		Map::iterator it = map_.find(sub_event_stream);
		if( it == map_.end() ){
			// this is a substream we haven't seen, give it a color
			QColor val = QColor( Qt::GlobalColor(Qt::red + map_.size()) ).light();
			map_.insert(std::make_pair( sub_event_stream, val ));
			return val;
		}
		return it->second;
	}

} physicsSubColor;

class StreamOffset {
	std::vector< I3Frame::Stream > streams_;
public:
	int operator()( const I3Frame::Stream& stream )
	{
		int stream_offset = -1;
		for( unsigned i = 0; i < streams_.size(); ++i ){
			if(stream == streams_.at(i))
				stream_offset = i + 2;
		}
		if (stream_offset == -1 ){
			// trayinfo is always on line 2
			if( stream == I3Frame::TrayInfo ){
				stream_offset = 2;
			}
			// gcd is on line 1
			else if( stream == I3Frame::Geometry ||
			         stream == I3Frame::Calibration ||
			         stream == I3Frame::DetectorStatus ){
				stream_offset = 1;
			}
			// any other frame types are on lines 2+,
			// stacked downwards in the order they are encountered
			else{
				streams_.push_back( stream );
				stream_offset = streams_.size() + 1;
			}
		}
		return stream_offset;
	}

	void reset() { streams_.clear(); }
	int max() { return 1 + streams_.size(); }
} streamOffset;

class FrameBall : public QGraphicsEllipseItem
{
public:
	int idx_;
	QColor inner_;
	QString label_;

	FrameBall( unsigned idx, unsigned streamIdx, const FrameInfo& finfo ):
		QGraphicsEllipseItem( -RADIUS / 2, -RADIUS / 2, RADIUS, RADIUS ),
		idx_( idx ), label_( finfo.stream_.id() )
	{
		if( !finfo.sub_stream_.empty() )
			inner_ = physicsSubColor( finfo.sub_stream_ );
		setBrush( StreamColor( finfo.stream_ ) );
		setFlag( QGraphicsItem::ItemIsSelectable, false );
		moveBy( XOFF*idx, YOFF*streamIdx );
	}

	void paint( QPainter* painter, const QStyleOptionGraphicsItem* option,
	            QWidget* widget ){
		QGraphicsEllipseItem::paint( painter, option, widget );
		const QRectF box = boundingRect();
		if( inner_.isValid() ){
			painter->setPen( QPen(Qt::transparent) );
			painter->setBrush( inner_ );
			painter->drawEllipse( box.center(), RADIUS / 4, RADIUS / 4 );
		}
		QFont font;
		font.setPixelSize( 15 );
		painter->setFont( font );
		painter->setPen( QPen() );
		painter->setBrush( QBrush() );
		painter->drawText( box, Qt::AlignHCenter | Qt::AlignVCenter, label_ );
	}

protected:
	void mousePressEvent( QGraphicsSceneMouseEvent* ev )
	{
		SteamshovelApp::instance()->files().selectFrame( idx_ );
	}
};

FramePickerScene::FramePickerScene() :
	QGraphicsScene(),
	beg_marker_(makeBracket(5)),
	end_marker_(makeBracket(-5)),
	sel_marker_(makeSelectionMarker()),
	markers_(0)
{
	beg_marker_->setPos(-1.0 * XOFF, 0);

	FileService* fileService = &SteamshovelApp::instance()->files();
	connect( fileService,
	         SIGNAL(newSegment(unsigned, unsigned, bool)),
	         SLOT(newSegment(unsigned, unsigned, bool)) );
	connect( fileService,
			 SIGNAL( fileAdded( QString, bool ) ),
			 SLOT( fileAdded() ) );

	setItemIndexMethod( NoIndex );
	//setBspTreeDepth(13);
}

QGraphicsItem* FramePickerScene::makeBracket( qreal dx )
{
	QPainterPath p;
	// draw [
	const qreal y2 = 4 * YOFF;
	const qreal y1 = 0.1 * YOFF;
	p.moveTo( dx, y2 );
	p.lineTo( 0, y2 );
	p.lineTo( 0, y1 );
	p.lineTo( dx, y1 );
	QGraphicsItem* m = addPath( p, QPen() );
	m->setCacheMode( QGraphicsItem::NoCache );
	m->setZValue( -2 );
	m->hide();
	return m;
}

QGraphicsItem* FramePickerScene::makeSelectionMarker()
{
	const double selrad = (RADIUS * 1.6);
	const double selrad2 = selrad / 2.0;
	const double selrad4 = selrad / 4.0;
	QPainterPath path;
	path.moveTo( selrad2, 0 );
	path.arcTo( QRectF( -selrad2, -selrad2, selrad, selrad ), 0, 270 );
	path.lineTo( selrad4, selrad2 );
	path.arcTo( QRectF( 0, 0, selrad2, selrad2 ), 270, 90 );
	path.closeSubpath();
	QGraphicsPathItem* m = addPath( path );
	m->setBrush( QColor( Qt::GlobalColor( Qt::blue ) ) );
	m->setZValue(-1);
	m->hide();
	return m;
}

void FramePickerScene::addFrame( const FrameInfo& finfo )
{
	const I3Frame::Stream& stream = finfo.stream_;
	const int stream_offset = streamOffset( stream );
	QGraphicsItem* ball = new FrameBall( markers_, stream_offset, finfo );
	++markers_;
	addItem( ball );
}

void FramePickerScene::addRuler( unsigned start, unsigned stop )
{
	const int height = 15;
	QFont font;
	font.setPixelSize( height );
	for( unsigned i = start / 5, n = stop / 5; i < n; ++i ){
		if( i % 2 ){
			const qreal x = (i * 5) * XOFF;
			addLine( x, -0.5 * height, x, 0 );
		}
		else{
			const qreal x = (i * 5) * XOFF;
			QGraphicsSimpleTextItem* t = addSimpleText( QString::number(i * 5), font );
			QPointF ctr = t->boundingRect().center();
			t->setPos( x - ctr.x(), -height );
		}
	}
}

void FramePickerScene::fileAdded()
{
	end_marker_->hide();
	if( !beg_marker_->isVisible() )
		beg_marker_->show();
}

void FramePickerScene::newSegment( unsigned start, unsigned stop, bool complete )
{
	log_trace_stream( "newSegment begins start="
	                  << start << ", stop=" << stop
	                  << " complete=" << complete );

	log_trace_stream( "sel visible " << sel_marker_->isVisible());

	if( !beg_marker_->isVisible() )
		beg_marker_->show();

	for( unsigned i = start; i < stop; ++i ){
		const FrameInfo& finfo = SteamshovelApp::instance()->files().frameIndex()[i];
		addFrame( finfo );
		// auto-select first physics frame in sequence
		if( !sel_marker_->isVisible() && finfo.stream_ == I3Frame::Physics ){
			// triggers FramePickerScene::select
			SteamshovelApp::instance()->files().selectFrame( i );
		}
	}

	if( complete ){
		end_marker_->setPos( (markers_ + 0.1)  * XOFF, 0 );
		end_marker_->show();
		// auto-select first frame in sequence
		if( !sel_marker_->isVisible() && stop > 0 )
			// triggers FramePickerScene::select
			SteamshovelApp::instance()->files().selectFrame( 0 );
	}

	addRuler( start, stop );

	log_trace( "newSegment ends" );
}

void FramePickerScene::frameUpdate( I3FramePtr ptr,
                                    unsigned idx,
                                    const FrameInfo& finfo )
{
	if( idx == unsigned(-1) ) return;

	int stream_offset = streamOffset( finfo.stream_ );
	sel_marker_->setPos( idx * XOFF, stream_offset * YOFF );
	sel_marker_->show();
	sel_marker_->ensureVisible();

	if( !ptr ){
		// returned frame is null, draw a little cross
		QGraphicsSimpleTextItem* x = addSimpleText(QString(180 /* == x */));
		x->setPos( (idx - 0.3) * XOFF, stream_offset * YOFF );
		QPointF ctr = -x->boundingRect().center();
		x->moveBy( ctr.x(), ctr.y() );
		x->setFont( QFont("Symbol", 15, QFont::Bold) );
		x->setBrush( QColor( Qt::GlobalColor( Qt::red )));
		x->setZValue( 10 );
	}

	update( sceneRect() );
}

FramePicker::FramePicker( QWidget* parent ):
	QGraphicsView( parent )
{
	setScene( new FramePickerScene() );
	connect( &SteamshovelApp::instance()->files(),
	         SIGNAL( clearFrames() ),
	         SLOT( clear() ) );
}

void
FramePicker::clear()
{
	log_trace("clear called");
	streamOffset.reset();
	delete scene(); // need to delete scene to make viewport shrink :(
	setScene( new FramePickerScene() );
}

void
FramePicker::keyPressEvent( QKeyEvent* ev )
{
	FileService& fs = SteamshovelApp::instance()->files();
	if( fs.currentIndex() == unsigned(-1) ){
		ev->ignore();
		return;
	}

	switch( ev->key() ){
	case Qt::Key_Left:
		ev->accept();
		if( ev->modifiers() == Qt::ShiftModifier )
			fs.advanceFrame(-1);
		else
			movex(false);
	break;
	case Qt::Key_Right:
		ev->accept();
		if( ev->modifiers() == Qt::ShiftModifier )
			fs.advanceFrame(1);
		else
			movex(true);
	break;
	case Qt::Key_Up:
		ev->accept();
		movey(false);
	break;
	case Qt::Key_Down:
		ev->accept();
		movey(true);
	break;
	default:
		QGraphicsView::keyPressEvent( ev );
	};
}

void
FramePicker::movex( bool forward )
{
	FileService& fs = SteamshovelApp::instance()->files();
	const FrameIndex& index = fs.frameIndex();
	unsigned idx = fs.currentIndex();
	const int level = streamOffset( index[idx].stream_ );

	forward ? ++idx: --idx;
	// loop works even if idx is reduced below zero
	for( ; idx < index.size(); forward ? ++idx : --idx ){
		log_trace_stream("movex: trying " << idx);
		if( streamOffset( index[idx].stream_ ) == level ){
			fs.selectFrame( idx );
			log_trace("movex: success");
			return;
		}
	}
}

void
FramePicker::movey( bool down )
{
	FileService& fs = SteamshovelApp::instance()->files();
	const FrameIndex& index = fs.frameIndex();
	unsigned idx = fs.currentIndex();
	const int level = streamOffset( index[idx].stream_ );
	if( (down && level == streamOffset.max()) ||
	    (!down && level == 1) )
		return;

	down ? ++idx: --idx;
	// loop works even if idx is reduced below zero
	for( ; idx < index.size(); down ? ++idx : --idx ){
		log_trace_stream("movey: trying " << idx);
		if( (down && streamOffset( index[idx].stream_ ) > level) ||
		    (!down && streamOffset( index[idx].stream_ ) < level) ){
			fs.selectFrame( idx );
			log_trace("movey: success");
			return;
		}
	}
}
