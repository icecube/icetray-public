#ifndef I3_SHOVEL_SCENARIO_DATAMODEL_H
#define I3_SHOVEL_SCENARIO_DATAMODEL_H

#include "shovelart/ArtistFwd.h"

#include <icetray/I3Frame.h>

#include <QVector>
#include <QAbstractItemModel>

class Scenario;

class ScenarioModel : public QAbstractItemModel {

Q_OBJECT;
public:
	static const int ArtistPtrRole = Qt::UserRole + 1;
	ScenarioModel( Scenario* scenario, QObject* parent = NULL );

	virtual QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags( const QModelIndex& index ) const ;
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

protected Q_SLOTS:
	// These slots correspond to the signals of Scenario
	void artistAdded( ArtistPtr a );
	void artistRemoved( ArtistPtr a );
	void artistUpdated( ArtistPtr a );
	void inputsChanged( I3FramePtr ptr );

protected:
	bool isArtist( const QModelIndex& index ) const;
	ArtistPtr getArtist( const QModelIndex& index ) const;
	bool isArtistKey( const QModelIndex& index ) const;
	bool isSettingHeader( const QModelIndex& index ) const;
	bool isArtistSetting( const QModelIndex& index ) const;

private:
	Scenario* scenario_;
	QVector< ArtistPtr > artists_;
};

#endif /* I3_SHOVEL_SCENARIO_DATAMODEL_H */
