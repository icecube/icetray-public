#ifndef I3_SHOVEL_FRAMEPICKER_SCENE
#define I3_SHOVEL_FRAMEPICKER_SCENE

#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>

#include <QGraphicsScene>
#include <QGraphicsView>

struct FrameInfo;
class QGraphicsItem;
class QKeyEvent;

class FramePickerScene : public QGraphicsScene
{
	Q_OBJECT ;

	SET_LOGGER("FramePicker");

	QGraphicsItem* beg_marker_;
	QGraphicsItem* end_marker_;
	QGraphicsItem* sel_marker_;
	unsigned markers_;

	QGraphicsItem* makeBracket( qreal );
	QGraphicsItem* makeSelectionMarker();
	void addFrame( const FrameInfo& );
	void addRuler( unsigned, unsigned );

public:
	FramePickerScene();
	void frameUpdate( I3FramePtr frame, unsigned idx, const FrameInfo& finfo );

public Q_SLOTS:
	void fileAdded();
	void newSegment( unsigned start, unsigned end, bool indexComplete );
};

class FramePicker : public QGraphicsView
{
	Q_OBJECT ;

	SET_LOGGER("FramePicker");

public:
	FramePicker( QWidget* parent );

	void keyPressEvent( QKeyEvent* ev );

	void movex( bool forward );
	void movey( bool down );

public Q_SLOTS:
	void clear();
};

#endif /* I3_SHOVEL_FRAMEPICKER_SCENE */
