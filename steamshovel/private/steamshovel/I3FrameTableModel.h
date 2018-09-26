#ifndef I3_SHOVEL_I3_FRAME_TABLE_MODEL_H
#define I3_SHOVEL_I3_FRAME_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <icetray/I3Frame.h>

/**
 * Table model for the steamshovel "spreadsheet view," showing the contents of an I3Frame
 * by key, type, and value
 */
class I3FrameTableModel : public QAbstractTableModel {
	Q_OBJECT;

private:
	I3FramePtr frame_;

public:
	I3FrameTableModel( QObject* parent = NULL );
	int rowCount( const QModelIndex& parent = QModelIndex() ) const;
	int columnCount( const QModelIndex& parent = QModelIndex() ) const;
	QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
	QVariant headerData( int section, Qt::Orientation orientation, int rol = Qt::DisplayRole ) const;

	void frameUpdate( I3FramePtr frame );
};

#endif /* I3_SHOVEL_I3_FRAME_TABLE_MODEL_H */
