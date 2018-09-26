#include <boost/python.hpp>

#include "scripting/gil.h"
#include "I3FrameTableModel.h"
#include "moc_I3FrameTableModel.cpp"


I3FrameTableModel::I3FrameTableModel(QObject* parent):
	QAbstractTableModel(parent)
{
	setHeaderData( 0, Qt::Horizontal, QString("Name") );
	setHeaderData( 1, Qt::Horizontal, QString("Type") );
	setHeaderData( 2, Qt::Horizontal, QString("Value") );
}

void I3FrameTableModel::frameUpdate( I3FramePtr ptr ){
#if QT_VERSION >= 0x050000
	beginResetModel();
	frame_ = ptr;
	endResetModel();
#else
	frame_ = ptr;
	reset();
#endif
}

int I3FrameTableModel::rowCount( const QModelIndex& parent ) const {
	if( parent.isValid() ) return 0; // required by table model semantics

	if( !frame_ ) return 0;
	else return frame_->size();
}

int I3FrameTableModel::columnCount( const QModelIndex& parent ) const {
	if( parent.isValid() ) return 0; // required by table model semantics
	return 3;
}

QVariant I3FrameTableModel::headerData( int section, Qt::Orientation orientation, int role ) const{

	if( role != Qt::DisplayRole ){
		return QVariant();
	}

	if( orientation == Qt::Horizontal ){
		switch(section){
			case 0:
				return QString("Name");
			case 1:
				return QString("Type");
			case 2:
				return QString("Data");
			default:
				return QString("Huh?");
		}
	}
	else
		return section;
}

QVariant I3FrameTableModel::data( const QModelIndex& index, int role ) const{

	if( role != Qt::DisplayRole ){
		return QVariant();
	}
	if( index.isValid() ){
		int row = index.row();
		I3Frame::typename_iterator keyitem( frame_->typename_begin() );
		/* keyitem += row */
		for( int i = 0; i < row; ++i ) ++keyitem;
		if( index.column() == 0 ){
			return QString( keyitem->first.c_str() );
		}
		else if( index.column() == 1 ){
			return QString( keyitem->second.c_str() );
		}
		else if( index.column() == 2 ){
			scripting::ScopedGIL gil;
			namespace bp = boost::python;
			bp::object mod = bp::import("icecube.dataio.pprint");
			bp::object pprint = mod.attr("format_line");
			bp::object str = pprint(frame_, keyitem->first);
			std::string s = bp::extract<std::string>( str );
			return QString(s.c_str());
		}
	}
	return QString("Strange error!");
}

