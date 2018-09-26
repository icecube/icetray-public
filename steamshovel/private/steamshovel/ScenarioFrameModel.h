#ifndef I3_SHOVEL_SCENARIO_FRAME_DATAMODEL_H
#define I3_SHOVEL_SCENARIO_FRAME_DATAMODEL_H

#include <QList>
#include <QAbstractItemModel>

#include <icetray/I3Logging.h>
#include <icetray/IcetrayFwd.h>


class Scenario;

class ScenarioFrameModel : public QAbstractItemModel {

Q_OBJECT;
SET_LOGGER( "ScenarioFrameModel" );

public:
	struct RowDatum;

	ScenarioFrameModel( Scenario* s, QObject* parent = NULL );

	virtual QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags( const QModelIndex& index ) const ;
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

protected Q_SLOTS:
	void artistAdded( ArtistPtr a );
	void artistRemoved( ArtistPtr a );
	void artistUpdated( ArtistPtr a );
	void inputsChanged( I3FramePtr ptr );

private:
	RowDatum* rowDatum( const QModelIndex& index ) const;

	Scenario* scenario_;
	QList< RowDatum* > row_data_;
};

#endif /* I3_SHOVEL_SCENARIO_FRAME_DATAMODEL_H */
