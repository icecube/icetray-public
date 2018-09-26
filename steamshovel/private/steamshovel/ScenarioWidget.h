#ifndef I3_SHOVEL_SCENARIO_WIDGET_H
#define I3_SHOVEL_SCENARIO_WIDGET_H

#include <QTreeView>
#include <icetray/I3Logging.h>

class Scenario;
class ScenarioModel;
namespace scripting{ class PyConsole; }

namespace Ui{ class ScenarioWidget; }

class FilterWidget;
class ShovelMainWindow;
class QKeyEvent;

class ScenarioWidget : public QWidget{
	Q_OBJECT;
	SET_LOGGER("ScenarioWidget");

	Ui::ScenarioWidget* ui;
	Scenario* scenario_;

protected:
	void keyPressEvent( QKeyEvent* );

protected Q_SLOTS:
	void triggerAddAction( QAction* );
	void removeSelection( );
	void refreshArtistList( );

public:
	ScenarioWidget( QWidget* parent = NULL );
	~ScenarioWidget();

	FilterWidget* getFilter();
	void setScenario( Scenario* model );

	void disableFilter();
};

// displays the artist settings
class ScenarioView : public QTreeView{
	Q_OBJECT;

protected:
	void rowsInserted( const QModelIndex& parent, int start, int end );

protected Q_SLOTS:
	void bounceCurrentIndex( const QModelIndex& );

public:
	ScenarioView( QWidget* parent = NULL );
};

/// displays the sources
class ScenarioFrameView : public QTreeView {
	Q_OBJECT;

public:
	ScenarioFrameView( QWidget* parent = NULL ) :
		QTreeView( parent )
	{}
};


#endif /* I3_SHOVEL_SCENARIO_WIDGET_H */
