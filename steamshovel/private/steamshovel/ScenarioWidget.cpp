#include "ScenarioWidget.h"
#include "moc_ScenarioWidget.cpp"
#include "ui_scenario.h"

#include "SteamshovelApp.h"
#include "ScenarioModel.h"
#include "ScenarioFrameModel.h"
#include "ArtistDelegate.h"
#include "FilterWidget.h"
#include "FileService.h"

#include "shovelart/Artist.h"
#include "shovelart/Scenario.h"
#include "shovelart/BindSignal.h"

#include "scripting/qmeta.h"
// #include "scripting/data.h"
#include "scripting/embed.h"

#include <boost/bind.hpp>
#include <boost/python/object.hpp>

#include <algorithm>

#include <QMenu>
#include <QAction>
#include <QKeyEvent>

ScenarioWidget::ScenarioWidget( QWidget* parent ) :
	QWidget(parent), ui( new Ui::ScenarioWidget() ), scenario_(NULL)
{
	ui->setupUi(this);
	ui->addButton->setMenu( new QMenu( ui->addButton ) );

	connect( ui->addButton->menu(),
	         SIGNAL(triggered(QAction*)), 
	         SLOT(triggerAddAction(QAction*)) );
	connect( ui->removeButton,
	         SIGNAL(pressed()), 
	         SLOT(removeSelection()) );
	connect( &ArtistRegistry::global(),
	         SIGNAL(artistRegistered()),
	         SLOT(refreshArtistList()) );

	FileService* fileService = &SteamshovelApp::instance()->files();
	connect( ui->frame_filter, SIGNAL(sendFilter(boost::python::object)),
	         fileService, SLOT(setFrameFilter(boost::python::object)) );
}

ScenarioWidget::~ScenarioWidget(){
	delete ui;
}

void ScenarioWidget::keyPressEvent( QKeyEvent* ev ){
	switch( ev->key() ){
	case Qt::Key_Delete:
		ev->accept();
		removeSelection();
	break;
	default:
		QWidget::keyPressEvent( ev );
	};
}

FilterWidget* ScenarioWidget::getFilter(){
	return ui->frame_filter;
}

void ScenarioWidget::triggerAddAction( QAction* action ){
	const ArtistFactory* factory = action->data().value<const ArtistFactory*>();
	if( scenario_ ){
		ArtistKeyList default_keys = scenario_->expandDefaultKeys( factory );
		scenario_->add( factory->make( default_keys ) );
	}
}

void ScenarioWidget::removeSelection(){
	if( !scenario_ ) return;

	// once we start removing artists from the Scenario, our QModelIndices become invalid.
	// so cache things to remove and do so at the end.
	std::vector< ArtistPtr > to_remove;
	QModelIndexList sels = ui->treeView->selectionModel()->selectedIndexes();
	for( int i = 0; i < sels.size(); ++i ){
		QModelIndex idx = sels[i];
		QVariant artist = idx.data( ScenarioModel::ArtistPtrRole );
		if( artist.canConvert<ArtistPtr>() ){
			to_remove.push_back( artist.value<ArtistPtr>() );
		}
		else{
			log_error( "Bogus data received from tree model selection" );
		}
	}
	for( std::vector<ArtistPtr>::iterator i = to_remove.begin(); i != to_remove.end(); ++i ){
		scenario_->remove( *i );
	}
}

void ScenarioWidget::setScenario( Scenario* s ){
	scenario_ = s;
	ui->treeView->setModel( new ScenarioModel( scenario_, this ) );
	ui->frameView->setModel( new ScenarioFrameModel( scenario_, this ) );

	refreshArtistList();
}

void ScenarioWidget::refreshArtistList( ){
	ui->addButton->menu()->clear();

	for( ArtistRegistry::const_iterator
			iter = ArtistRegistry::global().begin(),
			end = ArtistRegistry::global().end();
		 iter != end; ++iter ){
		QAction* action = new QAction( QString::fromStdString( iter->description ),
		                               ui->addButton->menu() );
		action->setData( QVariant::fromValue( &(*iter) ) );
		ui->addButton->menu()->addAction ( action );
	}
}

void ScenarioWidget::disableFilter(){
	ui->tabWidget->setTabEnabled( 2, false );
}

ScenarioView::ScenarioView( QWidget* parent ) :
	QTreeView(parent)
{
	setItemDelegate( new ArtistDelegate(this) );
	setIconSize( QSize(128, 18) );
	setSelectionBehavior( QAbstractItemView::SelectItems );
	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setFocusPolicy(Qt::StrongFocus);
	BindSignal* bs = new BindSignal( boost::bind( &ScenarioView::resizeColumnToContents, this, 0 ), this );
	QObject::connect( this, SIGNAL(expanded(const QModelIndex& )), bs, SLOT(call()) );
	QObject::connect( this, SIGNAL(collapsed(const QModelIndex& )), bs, SLOT(call()) );

	// see comments for bounceCurrentIndex
	setEditTriggers( QAbstractItemView::CurrentChanged );
	QObject::connect(this, SIGNAL(clicked(const QModelIndex&)),
	                 this, SLOT(bounceCurrentIndex(const QModelIndex&)));
}

/**
 * Given an index, first set the current index to nothing, and then to this idx.
 * This works around the problem that our key/setting editor widgets are summoned
 * only by a change in the current index (edit trigger QAbstractItemView::CurrentChanged),
 * but it's possible to dismiss the editor widgets for the current idx without changing it.
 * So this avoids the obnoxious situation where one must click a different editor to
 * retrieve the editor that is actually desired.
 */
void ScenarioView::bounceCurrentIndex( const QModelIndex& idx ){
	setCurrentIndex( QModelIndex() );
	setCurrentIndex( idx );
}

void ScenarioView::rowsInserted( const QModelIndex& parent, int start, int end ){
	QTreeView::rowsInserted( parent, start, end );
	if( !parent.isValid() || !parent.parent().isValid() ){
		for( int i = start; i <= end; ++i ){
			setFirstColumnSpanned( i, parent, true );
		}
	}
	resizeColumnToContents(0);
}
