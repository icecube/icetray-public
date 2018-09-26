#include <boost/bind.hpp>

#include <QVector>
#include <QFont>

#include "shovelart/Artist.h"
#include "shovelart/Scenario.h"

#include "ScenarioFrameModel.h"
#include "moc_ScenarioFrameModel.cpp"

struct ScenarioFrameModel::RowDatum {
	int row;
	RowDatum* parent;
	QList< RowDatum* > children;

	RowDatum(int r) : row(r), parent(NULL) {}
	virtual ~RowDatum(){ qDeleteAll(children); }

	virtual ArtistPtr artist() const { return ArtistPtr(); }
	bool isArtist() const { return (bool)artist(); }

	virtual Qt::ItemFlags flags() const = 0;
	virtual QVariant data( int role ) const = 0;
	virtual bool setData( const QVariant& data, int role, Scenario* scenario ){ return false; }

	virtual QModelIndex index( const ScenarioFrameModel& model ) const {
		return model.index( row, 0, parent ? parent->index(model) : QModelIndex() );
	}

	/**
	 * Given an artist that has recently been added or changed, make any corresponding changes needed
	 * in this row.  Return true if a visible change occured
	 */
	virtual bool update( ArtistPtr a ){ return false; }

	/**
	 * Given an artist that has just disappeared from the Scenario, make any changes needed in this row.
	 * @return true if a visible change occurred
	 */
	virtual bool drop( ArtistPtr a ){ return false; }

};

namespace{

/** A row with a key, and possibly some child ArtistRows */
struct KeyRow : public ScenarioFrameModel::RowDatum {

	QString key;

	KeyRow( int r, const QString& k ):
		RowDatum(r), key(k)
	{}

	virtual Qt::ItemFlags flags() const{
		if( children.size() ){
			return Qt::ItemIsEnabled;
		}
		else
			return Qt::NoItemFlags;
	}

	virtual QVariant data( int role ) const{
		switch(role){
		case Qt::DisplayRole:
			return key;
		}
		return QVariant();
	}

};

static ArtistPtr findMatch( const std::vector<ArtistPtr>& artists,
                            const ArtistFactory* factory,
                            const ArtistKeyList& keys )
{
	for( std::vector<ArtistPtr>::const_iterator i = artists.begin(); i != artists.end(); ++i ){
		if( (*i)->description() == factory->description && (*i)->keys() == keys ){
			return *i;
		}
	}
	return ArtistPtr();
}

/** A row that represents a single Artist, with settings as children */
struct ArtistRow : public ScenarioFrameModel::RowDatum {

	/// The factory to create an artist for this row, if any
	const ArtistFactory* factory;
	/// The keylist to pass to the factory if we need to make an artist for this row
	ArtistKeyList trigger_keylist;
	/// If an artist is currently representing this row, this points to it
	ArtistPtr active_artist;

	ArtistRow( int r, Scenario* s, const ArtistFactory* f, const ArtistKeyList* keys ) :
		RowDatum(r), factory(f),
		trigger_keylist( s->expandDefaultKeys( factory, keys ) )
	{
		active_artist = findMatch( s->getArtists(), factory, trigger_keylist );
	}


	virtual Qt::ItemFlags flags() const {
		return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
	}

	virtual QVariant data( int role ) const {
		switch(role){

		case Qt::DisplayRole:
			return QString::fromStdString(factory->description);
			break;

		case Qt::FontRole:
			if( active_artist ){
				QFont ret; ret.setBold(true); return ret;
			}
			else
				return QFont();
			break;

		case Qt::CheckStateRole:
			if( active_artist && active_artist->isActive() )
				return Qt::Checked;
			else
				return Qt::Unchecked;
			break;
		}
		return QVariant();
	}

	virtual bool setData( const QVariant& value, int role, Scenario* scenario ){
		if( role == Qt::CheckStateRole ){
			int set = value.value<int>();
			if( active_artist ){
				scenario->setIsActive( active_artist, set );
				return true;
			}
			else if( set && factory ){
				log_debug_stream( "Creating artist " << factory->description );
				scenario->add( factory->make( trigger_keylist ) );
				return true;
			}
		}
		return false;
	}

	virtual bool update( ArtistPtr a ){
		if( a == active_artist ){
			if( a->keys() != trigger_keylist ){
				active_artist = ArtistPtr();
				qDeleteAll( children );
				return true;
			}
		}
		else if( !active_artist ){
			if( a->keys() == trigger_keylist && a->description() == factory->description ){
				active_artist = a;
				return true;
			}
		}
		return false;
	}

	virtual bool drop( ArtistPtr a ){
		if( a == active_artist ){
			active_artist = ArtistPtr();
			qDeleteAll( children );
			return true;
		}
		return false;
	}
};

/** A row representing a single artist, with its key displayed alongside */
struct KeyArtistRow : public ArtistRow {
	QString key;

	KeyArtistRow( int r, QString k, Scenario* s, const ArtistFactory* f, const ArtistKeyList* keys ):
		ArtistRow( r, s, f, keys ), key( k )
	{}

	virtual QVariant data( int role ) const {
		if( role == Qt::DisplayRole ){
			return key + " (" + QString::fromStdString(factory->description) + ")";
		}
		return ArtistRow::data(role);
	}
};

} // namespace

ScenarioFrameModel::ScenarioFrameModel( Scenario* s, QObject* parent ) :
	QAbstractItemModel( parent ), scenario_(s)
{
	QObject::connect( scenario_, SIGNAL(artistAdded(ArtistPtr)), this, SLOT(artistAdded(ArtistPtr)) );
	QObject::connect( scenario_, SIGNAL(artistRemoved(ArtistPtr)), this, SLOT(artistRemoved(ArtistPtr)) );
	QObject::connect( scenario_, SIGNAL(artistUpdated(ArtistPtr)), this, SLOT(artistUpdated(ArtistPtr)) );
	QObject::connect( scenario_, SIGNAL(inputsChanged(I3FramePtr)), this, SLOT(inputsChanged(I3FramePtr)) );
}

ScenarioFrameModel::RowDatum* ScenarioFrameModel::rowDatum( const QModelIndex& index ) const {
	RowDatum* rd = static_cast<RowDatum*>(index.internalPointer());
	return rd;
}

QModelIndex ScenarioFrameModel::index( int row, int column, const QModelIndex& parent ) const{
	if( !parent.isValid() ){
		if( column == 0 && row < row_data_.size() ){
			return createIndex( row, column, row_data_.at(row) );
		}
	}
	else{
		RowDatum* rd = rowDatum(parent);
		if( column == 0 && rd ){
			if( rd->children.size() ){
				return createIndex( row, column, rd->children.at(row) );
			}
		}
	}
	return QModelIndex();
};

QModelIndex ScenarioFrameModel::parent(const QModelIndex &child) const{
	RowDatum* rd = rowDatum(child);
	if( rd && rd->parent != NULL ){
		return rd->parent->index(*this);
	}
	return QModelIndex();
}

int ScenarioFrameModel::rowCount(const QModelIndex &parent ) const{
	if( !parent.isValid() )
		return row_data_.size();
	RowDatum* rd = rowDatum(parent);
	if( rd ){
		if( rd->children.size() ) return rd->children.size();
	}
	return 0;
}


int ScenarioFrameModel::columnCount(const QModelIndex &parent ) const{
	return 1;
}

QVariant ScenarioFrameModel::headerData( int section, Qt::Orientation orientation, int role ) const{
	return QVariant();
}

QVariant ScenarioFrameModel::data(const QModelIndex &index, int role ) const{
	RowDatum* rd = rowDatum(index);
	if( rd ){
		return rd->data( role );
	}
	return QVariant();
}


Qt::ItemFlags ScenarioFrameModel::flags( const QModelIndex& index ) const{
	RowDatum* rd = rowDatum(index);
	if( rd )
		return rd->flags();
	return QAbstractItemModel::flags(index);
}

bool ScenarioFrameModel::setData ( const QModelIndex & index, const QVariant & value, int role ){
	RowDatum* rd = rowDatum(index);
	if( rd ){
		return rd->setData( value, role, scenario_ );
	}
	return false;
}


void ScenarioFrameModel::artistAdded( ArtistPtr a ){
	log_trace( "Added artist" );
	for( QList<RowDatum*>::iterator i = row_data_.begin(); i != row_data_.end(); ++i ){
		RowDatum* row = *i;
		if( row->update( a ) ){
			log_trace( "Row became an artist" );
			QModelIndex idx = row->index(*this);
			Q_EMIT dataChanged( idx, idx );
		}
		if( row->children.size() ){
			for( QList<RowDatum*>::iterator j = row->children.begin(); j != row->children.end(); ++j ){
				if( (*j)->update(a) ){
					log_trace( "Row child became an artist" );
					QModelIndex idx = (*j)->index(*this);
					Q_EMIT dataChanged( idx, idx );
				}
			}
		}
	}
}

void ScenarioFrameModel::artistRemoved( ArtistPtr a ){
	log_trace( "Dropped artist" );
	for( QList<RowDatum*>::iterator i = row_data_.begin(); i != row_data_.end(); ++i ){
		RowDatum* row = *i;
		if( row->drop( a ) ){
			log_trace( "Row lost an artist" );
			QModelIndex idx = row->index(*this);
			Q_EMIT dataChanged( idx, idx );
		}
		if( row->children.size() ){
			for( QList<RowDatum*>::iterator j = row->children.begin(); j != row->children.end(); ++j ){
				if( (*j)->drop(a) ){
					log_trace( "Row child lost an artist" );
					QModelIndex idx = (*j)->index(*this);
					Q_EMIT dataChanged( idx, idx );
				}
			}
		}
	}
}

void ScenarioFrameModel::artistUpdated( ArtistPtr a ){
	log_trace( "Updated artist" );
	for( QList<RowDatum*>::iterator i = row_data_.begin(); i != row_data_.end(); ++i ){
		RowDatum* row = *i;
		if( row->update( a ) ){
			log_trace( "Row changed" );
			QModelIndex idx = row->index(*this);
			Q_EMIT dataChanged( idx, idx );
		}
		if( row->children.size() ){
			for( QList<RowDatum*>::iterator j = row->children.begin(); j != row->children.end(); ++j ){
				if( (*j)->update(a) ){
					log_trace( "Row child changed" );
					QModelIndex idx = (*j)->index(*this);
					Q_EMIT dataChanged( idx, idx );
				}
			}
		}
	}
}


void ScenarioFrameModel::inputsChanged( I3FramePtr ptr ){
	log_trace( "Reading new frame..." );

	beginResetModel();
	qDeleteAll( row_data_ );
	row_data_.clear();

	QVector< std::string > frame_keys;
	if( ptr ){
		frame_keys = QVector<std::string>::fromStdVector( ptr->keys() );
		qSort( frame_keys );
	}

	int current_toplevel_row = 0;

	ArtistKeyList nokeys;
	std::vector< const ArtistFactory* > decorations = scenario_->findUsableArtistsByKeys( nokeys, false );
	for( int i = 0; i < (signed)decorations.size(); ++i ){
		ArtistRow* dec = new ArtistRow( current_toplevel_row++, scenario_, decorations[i], &nokeys );
		row_data_.push_back( dec );
	}

	for( int i = 0; i < frame_keys.size(); ++i ){
		RowDatum* this_row = NULL;
		// a keylist of just this key
		ArtistKeyList keys( 1, frame_keys.at(i) );
		//artists that could be created to display this key
		std::vector< const ArtistFactory* > potential_artists = scenario_->findUsableArtistsByKeys( keys );
		log_trace_stream( "For key " << frame_keys[i] << " " << potential_artists.size() << " artists exist" );

		if( potential_artists.size() == 1 ){
			// make a key-artist row
			this_row = new KeyArtistRow( current_toplevel_row++, QString::fromStdString(frame_keys[i]),
			                             scenario_, potential_artists.at(0), &keys );
		}
		else{
			// zero or several artists
			this_row = new KeyRow( current_toplevel_row++, QString::fromStdString(frame_keys[i]) );
			for( int j = 0; j < (signed)potential_artists.size(); ++j ){
				ArtistRow* child = new ArtistRow( j, scenario_, potential_artists[j], &keys );
				child->parent = this_row;
				this_row->children.push_back(child);
			}
		}

		assert( this_row != NULL );
		row_data_.push_back( this_row );
	}

	endResetModel();

	log_trace("done");
}
