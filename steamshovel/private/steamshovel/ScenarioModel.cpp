#include <QStringList>
#include <QColor>

#include "shovelart/Artist.h"
#include "shovelart/Scenario.h"

#include "ArtistSettings.h"
#include "ScenarioModel.h"
#include "moc_ScenarioModel.cpp"

ScenarioModel::ScenarioModel( Scenario* scenario, QObject* parent ) :
	QAbstractItemModel(parent),
	scenario_(scenario)
{
	QObject::connect( scenario_, SIGNAL(artistAdded(ArtistPtr)), this, SLOT(artistAdded(ArtistPtr)) );
	QObject::connect( scenario_, SIGNAL(artistRemoved(ArtistPtr)), this, SLOT(artistRemoved(ArtistPtr)) );
	QObject::connect( scenario_, SIGNAL(artistUpdated(ArtistPtr)), this, SLOT(artistUpdated(ArtistPtr)) );
	QObject::connect( scenario_, SIGNAL(inputsChanged(I3FramePtr)), this, SLOT(inputsChanged(I3FramePtr)) );
}

/* -------------------- Icky indexing internals -------------------- */

/* The ScenarioModel creates a tree structure as follows (QModelIndex details in parentheses):
 *
 * [ ] root node (i.e. invalid QModelIndex)
 *  |
 *  * [ Artist0 ] (row 0, column 0, parent = root )
 *         |
 *         * [ Key0 ] (row 0, column 0, parent = Artist1 )
 *         * [ Key1 ] (row 1, column 0, parent = Artist1 )
 *         * "2 Settings" (row 2, column 0, parent = Artist1 )
 *                |
 *                * [Name0] (r0, c0, parent="2 Settings") | [Setting0] (r0, c1, parent = "2 Settings")
 *                * [Name1] (r1, c0, parent="2 Settings") | [Setting1] (r1, c1, parent = "2 Settings")
 *  * [ Artist1 ] ( row 1, column 0, parent = root )
 *         |
 *         * [ Key0 ] (row 0, column 0, parent = Artist1 )
 *
 * An artist with N keys and no settings has N children;
 * An artist with N keys and M settings has N+1 children and 2M grandchildren,
 *    with all grandchildren being children of its last child (the "setting header")
 *    Column 0 grandchildren are setting names, and column 1 grandchildren are editable setting widgets
 *
 * Internally, we keep track of this state in QModelIndex's internalId as follows:
 * * Artist nodes have internalId == -1
 * * Key nodes and setting headers have internalId = n, where n is the artist's row number
 * * Setting nodes have internalid = n + ((m+1)<<24), where n is the artist's row number,
 *   and m is the number of keys that the artist has.  This allows a lookup of the artist's
 *   setting header.
 *
 * I believe it is safe to assume that there are never more than 2^24 artists in play,
 * or an artist with more than 2^8-1 keys.  If you prove me wrong I'll buy you a beer,
 * unless you read this comment first.
 */

static int index_mask = (1<<24) - 1;


static inline int keycountFromId( int id ){
	return (id >> 24) - 1;
}

static inline int artistFromId( int id ){
	return id & index_mask;
}

static inline int settingId( int artist, int keycount ){
	int id = ((keycount+1) << 24) + artist;
	assert( artist == artistFromId(id) );
	assert( keycount == keycountFromId(id) );
	assert( id > index_mask );
	return id;
}

QModelIndex ScenarioModel::index( int row, int column, const QModelIndex& parent ) const{

	if( !parent.isValid() && row < (int)scenario_->getArtists().size() && column == 0 ){
		return createIndex( row, column, -1 );
	}
	else if( isArtist( parent ) && column == 0 ){
		ArtistPtr artist = getArtist( parent );
		if( row < (int)artist->keys().size() ){
			return createIndex( row, column, parent.row() );
		}
		if( row == (int)artist->keys().size() && artist->settings().size() > 0 ){
			return createIndex( row, column, parent.row() );
		}
	}
	else if( isSettingHeader( parent ) && column <= 1 ){
		ArtistPtr artist = getArtist( parent.parent() );
		if( row < (int)artist->settings().size() )
			return createIndex( row, column, settingId( parent.parent().row(), artist->keys().size() ) );
	}

	return QModelIndex();
}

QModelIndex ScenarioModel::parent(const QModelIndex &child) const{
	qint64 id = child.internalId();
	if( id >= 0 && id <= index_mask )
		return createIndex( id, 0, -1 );
	else if( id > index_mask ){
		int artistid = artistFromId( id );
		int keycount = keycountFromId( id );
		return createIndex( keycount, 0, artistid );
	}
	else
		return QModelIndex();
}

/* -------------------- Interesting internal inquiries -------------------- */

/**
 * Return true if the index appears to be a valid artist index,
 * without checking if an artist of this kind really exists
 */
bool ScenarioModel::isArtist( const QModelIndex& index ) const{
	if( index.isValid() && !index.parent().isValid() ){
		return true;
	}
	return false;
}

/**
 * Return the artist at this index, throwing an exception if it doesn't exist
 */
ArtistPtr ScenarioModel::getArtist( const QModelIndex& index ) const {
	return artists_.at( index.row() );
}

/**
 * Return true if the index is an artist Key selection row
 */
bool ScenarioModel::isArtistKey( const QModelIndex& index ) const {
	if( index.isValid() && isArtist( index.parent() ) ){
		ArtistPtr parent = getArtist( index.parent() );
		return index.row() < (int)parent->keys().size();
	}
	return false;
}

/**
 * Return true if the index is an artist Setting header row
 */
bool ScenarioModel::isSettingHeader( const QModelIndex& index ) const{
	if( index.isValid() && isArtist( index.parent() ) ){
		ArtistPtr parent = getArtist( index.parent() );
		return index.row() == (int)parent->keys().size();
	}
	return false;
}

/**
 * Return true if the index is an artist Setting editor row
 */
bool ScenarioModel::isArtistSetting( const QModelIndex& index ) const{
	if( index.isValid() && isSettingHeader( index.parent() ) ){
		ArtistPtr parent = getArtist( index.parent().parent() );
		return index.row() < (int)parent->settings().size();
	}
	return false;
}

static Artist::SettingsMap::const_iterator getArtistSetting( ArtistPtr artist, const QModelIndex& index ){
	const Artist::SettingsMap& map = artist->settings();
	int idx = 0;
	Artist::SettingsMap::const_iterator i = map.begin();
	while( idx < index.row() ){
		i++; idx++;
	}
	if( i == map.end() ){
		log_error_stream( "Bad setting idx: " << idx );
	}
	return i;
}

/* -------------------- ItemModel interface -------------------- */

int ScenarioModel::rowCount(const QModelIndex &parent) const{
	if( !parent.isValid() ){
		return scenario_->getArtists().size();
	}
	else if( isArtist( parent ) ){
		ArtistPtr artist = getArtist(parent);
		if( artist->settings().size() ){
			return artist->keys().size() + 1;
		}
		else{
			return artist->keys().size();
		}
	}
	else if( isSettingHeader( parent ) ){
		ArtistPtr artist = getArtist(parent.parent());
		return artist->settings().size();
	}
	return 0;
}

int ScenarioModel::columnCount(const QModelIndex &parent) const{
	if( isSettingHeader( parent ) ){
		return 2;
	}
	// In other cases I very much wanted to return 1, which is more honest.
	// But the QTreeView is too dumb to notice when the maximum column count changes.
	return 2;
}
QVariant ScenarioModel::headerData( int section, Qt::Orientation orientation, int role ) const{
	if( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole ){
		return QString( "Artists" );
	}
	return QVariant();
}

Qt::ItemFlags ScenarioModel::flags( const QModelIndex& index ) const{
	if( isArtist( index ) ){
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
	}
	else if( isArtistKey( index ) ){
		return Qt::ItemIsEnabled | Qt::ItemIsEditable;
	}
	else if( isSettingHeader( index ) ){
		return Qt::ItemIsEnabled;
	}
	else if( isArtistSetting( index ) ){
		if( index.column() == 1 ){
			ArtistPtr a = getArtist( index.parent().parent() );
			return setting_QItemFlags( a, getArtistSetting( a, index )->name );
		}
		else return Qt::ItemIsEnabled;
	}
	log_warn("Unexpected flags()");
	return QAbstractItemModel::flags(index);
}

QVariant ScenarioModel::data(const QModelIndex &index, int role) const{
	if( !index.isValid() )
		return QVariant();

	if( isArtist(index) ){
		if( index.column() == 0 ){
			ArtistPtr a = getArtist(index);
			if( role == Qt::DisplayRole ){
				QString ret = QString::fromStdString( a->description() );
				ArtistKeyList custom_keys = scenario_->customKeys( a );
				if( custom_keys.size() > 0 ){
					ret += " (";
					for( unsigned i = 0; i < custom_keys.size(); ++i ){
						ret += QString::fromStdString(custom_keys[i]) + ", ";
					}
					ret.remove( ret.length()-2, 2 );
					ret += ")";
				}
				return ret;
			}
			else if( role == Qt::CheckStateRole ){
				// don't show a checkbox for WidgetArtists
				if( dynamic_cast<WidgetArtist*>(a.get()) )
					return QVariant();
				return a->isActive() ? Qt::Checked : Qt::Unchecked;
			}
			else if( role == ArtistPtrRole )
				return QVariant::fromValue(a);
		}
	}
	else if( isArtistKey( index ) ){
		ArtistPtr artist = getArtist(index.parent());
		std::string key = artist->keys().at(index.row());
		if( key == "" ){
			if( role == Qt::DisplayRole )
				return "Click to select...";
			else if( role == Qt::FontRole ){
				QFont f;
				f.setItalic(true);
				return f;
			}
		}
		else if( role == Qt::DisplayRole ){
			return QString::fromStdString(key);
		}
		if( role == Qt::EditRole ){
			ArtistKeyList validKeys = scenario_->validKeys( artist, index.row() );
			return QVariant::fromValue(
               ArtistKeyData( validKeys, artist->keys().at(index.row()) ) );
		}
	}
	else if( isSettingHeader( index ) ){
		if( role == Qt::DisplayRole ){
			ArtistPtr a = getArtist( index.parent() );
			int nsettings = a->settings().size();
			if( nsettings == 1 ){
				return QString( "1 Setting" );
			}
			else if( nsettings > 1 ){
				return QString::number(nsettings) + " Settings";
			}
		}
	}
	else if( isArtistSetting( index ) ){
		ArtistPtr a = getArtist( index.parent().parent() );
		Artist::SettingsMap::const_iterator i = getArtistSetting( a, index );
		if( index.column() == 0 ){
			if( role == Qt::DisplayRole )
				return QString::fromStdString( i->name );
		    //else if( role == Qt::SizeHintRole ){
			//	return QSize(150,12);
			//}
		}
		else if( index.column() == 1 ){
			return setting_QItemData( scenario_, a, i->name, role );
		}
	}

	return QVariant();
}

bool ScenarioModel::setData( const QModelIndex & index, const QVariant & value, int role ){
	if( !index.isValid() )
		return false;

	// We are supposed to explicitly emit dataChanged here;
	// it will be done within artistUpdated(), called from the Scenario.

	if( isArtist(index) && role == Qt::CheckStateRole ){
		ArtistPtr a = getArtist(index);
		scenario_->setIsActive( a, value.value<int>() );
	}
	else if( isArtistKey( index ) ){
		ArtistPtr artist = getArtist(index.parent());
		if( value.canConvert( QVariant::String ) && role == Qt::EditRole ){
			scenario_->setKey( artist, index.row(), value.toString().toStdString() );
			return true;
		}
		else{
			log_error( "Invalid QVariant" );
		}
	}
	else if( isArtistSetting( index ) && index.column() == 1 ){
		ArtistPtr a = getArtist( index.parent().parent() );
		Artist::SettingsMap::const_iterator i = getArtistSetting( a, index );
		return setting_QItemSetData( scenario_, a, i->name, value, role );
	}

	return false;
}

/* -------------------- Scenario-invoked slots -------------------- */

void ScenarioModel::artistAdded( ArtistPtr a ){

	int size = artists_.size();
	beginInsertRows( QModelIndex(), size, size );
	artists_.push_back(a);
	endInsertRows();

}

void ScenarioModel::artistRemoved( ArtistPtr a ){
	int idx = artists_.indexOf(a);
	if( idx < 0 ){
		log_fatal( "Bad artist ptr arrived in ScenarioModel" );
	}
	beginRemoveRows( QModelIndex(), idx, idx );
	artists_.remove(idx);
	endRemoveRows();
}

void ScenarioModel::artistUpdated( ArtistPtr a ){
	int idx = artists_.indexOf(a);
	if( idx < 0 ){
		log_fatal( "Bad artist ptr arrived in ScenarioModel" );
	}
	QModelIndex parent = index( idx, 0 );
	Q_EMIT dataChanged( parent, parent );
	if( a->keys().size() ){
		QModelIndex keystart = parent.child( 0, 0 );
		QModelIndex keyend   = parent.child( a->keys().size()-1, 0 );
		Q_EMIT dataChanged( keystart, keyend );
	}
	if( a->settings().size() ){
		QModelIndex settingheader = parent.child( a->keys().size(), 0 );
		QModelIndex settingStart = settingheader.child( 0, 1 );
		QModelIndex settingEnd = settingheader.child( a->settings().size()-1, 1 );
		Q_EMIT dataChanged( settingStart, settingEnd );
	}
	//Q_EMIT dataChanged( start, end );
}

void ScenarioModel::inputsChanged( I3FramePtr ){
	// trigger a redraw
	Q_EMIT dataChanged( index(0, 0),
	                    index(rowCount(), columnCount()) );
}
