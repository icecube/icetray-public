#include <algorithm>
#include <boost/foreach.hpp>

#include <icetray/I3Logging.h>
#include <icetray/name_of.h>
#include <icetray/Singleton.h>
#include <icetray/serialization.h>

#include <QWidget>

#include "scripting/gil.h"
#include "scripting/pyerr.h"
#include "Scene.h"
#include "Artist.h"
#include "ArtistRegistry.h"
#include "Scenario.h"
#include "moc_Scenario.cpp"

// Collects outputsChanged() calls
class OutputsChangedEmitter {
	Scenario* s_;
	bool emit_;
public:
	OutputsChangedEmitter( Scenario* s ):
		s_(s), emit_(false) {}
	void operator()(){ emit_ = true; };
	~OutputsChangedEmitter(){ if( emit_) s_->outputsChanged(); }
};

/** Return a list of artist Factories that could render the given list of keys for the current frame */
std::vector<const ArtistFactory*>
Scenario::findUsableArtistsByKeys( const ArtistKeyList& keys,
                                   bool expand_default_keys ){
	// scripting::ScopedGIL gil; // gil seems to be obsolete here?
	std::vector<const ArtistFactory*> ret;
	ArtistRegistry::const_iterator factory;
	for( factory = ArtistRegistry::global().begin();
	     factory != ArtistRegistry::global().end(); ++factory ){
		if( factory->keyCount >= keys.size() ){
			ArtistKeyList test_keys = ArtistKeyList(factory->keyCount,"");
			if( expand_default_keys ){
				test_keys = expandDefaultKeys( &(*factory), &keys );
			}
			if( test_keys.size() != factory->keyCount )
				continue;

			// now that test_keys are fully expanded, test them with the artist
			bool okay = true;
			int idx = 0;
			for( ArtistKeyList::const_iterator key = test_keys.begin();
			     okay && key != test_keys.end(); ++key ){
				try{
					okay = frame_ && frame_->Has(*key) &&
					       factory->prototype->isValidKey( frame_, idx, *key );
				}catch(...){
					log_warn_stream("Could not load " << *key << ", probably unregistered type");
				}
				idx++;
			}
			if( okay ){
				// we can use this factory for the given keylist
				ret.push_back( &(*factory) );
				log_trace_stream( factory->description << " accepted" );
			}
		}
	}
	return ret;
}

ArtistKeyList Scenario::expandDefaultKeys( const ArtistFactory* factory,
                                           const ArtistKeyList* expansion_keys ) const {
	// scripting::ScopedGIL gil; // gil seems to be obsolete here?
	ArtistKeyList keys = ArtistKeyList(factory->keyCount, "");
	ArtistPtr p = factory->prototype;
	unsigned blank_keys = keys.size();
	for( unsigned key_idx = 0; key_idx < factory->keyCount; ++key_idx ){
		ArtistKeyList vkeys = validKeys( p, key_idx );
		if( vkeys.size() == 1 ){
			log_trace_stream( factory->description << ": default key "
			                  << vkeys.front() << " at idx=" << key_idx );
			// Special exception: if expansion_keys[idx] == this default key, don't apply it yet;
			// it will be applied in the if statement below
			if( expansion_keys && expansion_keys->size() > key_idx &&
			    expansion_keys->at(key_idx) == vkeys.front() )
				continue;
			keys.at(key_idx) = vkeys.front();
			--blank_keys;
		}
	}

	if( expansion_keys ){
		if( expansion_keys->size() != blank_keys ){
			log_trace_stream( factory->description << ": Expansion keys are present but are too many" );
			return ArtistKeyList();
		}

		ArtistKeyList::const_iterator k = expansion_keys->begin();
		for( ArtistKeyList::iterator i = keys.begin(); i != keys.end(); ++i ){
			if( *i == "" ){
				*i = *k;
				++k;
			}
		}
		assert( k == expansion_keys->end() );
	}
	return keys;
}

ArtistKeyList Scenario::customKeys( ArtistPtr artist ){
	ArtistKeyList ret;
	if( frame_ ){
		for( unsigned key_idx = 0; key_idx < artist->keys().size(); ++key_idx ){
			if( validKeys( artist, key_idx ).size() > 1 )
				ret.push_back( artist->keys()[key_idx] );
		}
	}
	return ret;
}

std::vector< ArtistPtr > Scenario::findArtistsByInput( const ArtistKeyList& keys ) const{
	std::vector< ArtistPtr > matches;
	BOOST_FOREACH( ArtistPtr a, artists_ ){
		if( keys == a->keys() ){
			matches.push_back( a );
		}
	}
	return matches;
}

WidgetArtist* Scenario::findWidgetArtist( const QWidget* widget ){
	BOOST_FOREACH( WidgetArtist* w, widget_artists_ )
		if( w->widget_ == widget )
			return w;
	return 0;
}

void Scenario::add( ArtistPtr p ){
	if( WidgetArtist* wa = dynamic_cast<WidgetArtist*>( p.get() ) ){
		try{
			wa->widget_ = wa->create();
			wa->widget_->setEnabled(0);
			wa->widget_->setSizePolicy( QSizePolicy::Expanding,
			                            QSizePolicy::Expanding );
			wa->widget_->setFocusPolicy( Qt::ClickFocus );
			Q_EMIT widgetArtistAdded(
			    QString::fromStdString( wa->description() ),
			    wa->widget_
			);
			widget_artists_.push_back( wa );
			wa->updateColors( last_time_window_ );
		}
		catch( boost::python::error_already_set& e ){
			log_error_stream( p->description() 
			                  << "::create(...) threw a Python error:\n"
			                  << e );
			Q_EMIT showLog();
			return;
		}
		catch( std::exception& e ){
			log_error_stream( p->description() 
			                  << "::create(...) threw an exception:\n"
			                  << e.what() );
			Q_EMIT showLog();
			return;
		}
	}
	artists_.push_back( p );
	perhapsRegen( p );
	Q_EMIT artistAdded( p );
}

void Scenario::remove( ArtistPtr p ){
	if( !p )
		log_fatal( "ArtistPtr is null" );

	log_debug_stream("Remove " << p.get() );
	Q_EMIT artistRemoved( p );
	// does not delete p yet, just erases shared_ptr from collection
	artists_.erase( std::find( artists_.begin(), artists_.end(), p ) );
	if( WidgetArtist* wa = dynamic_cast<WidgetArtist*>( p.get() ) ){
		// implicitely calls back into validate, so it is
		// important that artist_.erase(...) runs before
		widget_artists_.erase( std::find( widget_artists_.begin(),
		                                  widget_artists_.end(), wa ) );
		Q_EMIT widgetArtistRemoved( wa->widget_ );
		scripting::ScopedGIL gil; // Python might be invoked in the destructor
		delete wa->widget_;
	}
	else { // GLArtist
		scene_->dropSource( p );
	}
	p->cleanUp();
	Q_EMIT outputsChanged();	
}

void Scenario::clear(){
	while( !artists_.empty() ){
		remove( artists_.back() );
	}
}

ArtistKeyList Scenario::validKeys( ArtistPtr artist, unsigned key_idx )const{
	ArtistKeyList ret;
	if( frame_ && key_idx < artist->numRequiredKeys() ){
		ValidKeyCache::const_iterator iter =
			valid_key_cache_.find( std::make_pair(artist, key_idx) );
		if( iter == valid_key_cache_.end() ){ // cache miss
			const ArtistKeyList& fkeys = frame_->keys();
			BOOST_FOREACH( const std::string& k, fkeys ){
				try{
					if( artist->isValidKey( frame_, key_idx, k ) )
						ret.push_back( k );
				}catch(...){
					log_warn_stream("Could not load " << k << ", probably unregistered type");
				}
			}
			valid_key_cache_[std::make_pair( artist, key_idx )] = ret;
		}else{ // return cached result
			ret = iter->second;
		}
	}
	return ret;
}

void Scenario::setKey( ArtistPtr artist, int key_idx, const std::string& key ){
	if( !artist )
		log_fatal( "ArtistPtr is null" );

	log_debug( "Set artist key %u to %s", key_idx, key.c_str() );
	artist->keys_.at(key_idx) = key;
	perhapsRegen( artist );
	Q_EMIT artistUpdated( artist );
}

void Scenario::setFrame( I3FramePtr ptr ){
	frame_ = ptr;
	valid_key_cache_.clear();
	log_debug_stream( "Setting up Scenario with " << artists_.size() << " artists" );
	OutputsChangedEmitter emit(this);
	BOOST_FOREACH( ArtistPtr artist, artists_ )
		perhapsRegen( artist, emit );
	Q_EMIT inputsChanged( ptr );
}

void Scenario::selectOMKeys( const OMKeySet& keys ){
	log_debug_stream( "Selecting " << keys.size() << " OMKeys" );
	OutputsChangedEmitter emit(this);
	BOOST_FOREACH( ArtistPtr artist, artists_ )
	{
		for( Artist::SettingsMap::iterator i = artist->settings_.begin();
		     i != artist->settings_.end(); ++i )
		{
			Artist::Setting& setting = i->setting;
			if( setting.is< OMKeySet >() ){
				setting.setValue( keys );
				forceRegen( artist, emit );
				Q_EMIT artistUpdated( artist );
			}
		}
	}
}

void Scenario::forceRegen( ArtistPtr p ){
	OutputsChangedEmitter emit(this);
	forceRegen( p, emit );
}

void Scenario::forceRegen( ArtistPtr p, OutputsChangedEmitter& emit ){
	log_debug( "Forcing artist to regenerate output" );
	p->settings_valid_ = false;
	perhapsRegen( p, emit );
}

void Scenario::forceRegen(){
	OutputsChangedEmitter emit(this);
	BOOST_FOREACH( ArtistPtr artist, artists_ )
		forceRegen( artist, emit );
}

void Scenario::setIsActive( ArtistPtr artist, bool active ){
	if( !artist )
		log_fatal( "ArtistPtr is null" );

	if( dynamic_cast<WidgetArtist*>(artist.get()) )
		return; // WidgetArtists ignore isActive setting

	if( active == artist->active_ )
		return;

	artist->active_ = active;
	if( active ){
		// redraw this artist
		forceRegen( artist );
	}
	else{
		scene_->dropOutputs(artist);
		Q_EMIT outputsChanged();
	}
	Q_EMIT artistUpdated( artist );
}

void Scenario::setOverlaySizeHints( ArtistPtr p, const SceneState::OverlayHints& hints ){
	if( !p )
		log_fatal( "ArtistPtr is null" );

	scene_->dropOutputs(p);
	scene_->state_.overlayHintsMap_[p] = hints;
	forceRegen(p);
}

void Scenario::updateWidgetArtists( const TimeWindow& tw ){

	if( tw.start() != last_time_window_.start() ||
	    tw.end() != last_time_window_.end() ||
	    tw.colorStart() != last_time_window_.colorStart() ||
	    tw.colorEnd() != last_time_window_.colorEnd() ){

		log_debug( "updating widget artists" );

		BOOST_FOREACH( WidgetArtist* wa, widget_artists_ )
			if( wa->widget_->isEnabled() ){
				try{
					wa->updateColors( tw );
				}
				catch( boost::python::error_already_set& e ){
					log_error_stream( wa->description()
					                  << "::updateColors(...) threw a Python exception:\n"
					                  << e );
				}
				catch( std::exception& e ){
					log_error_stream( wa->description()
					                  << "::updateColors(...) threw an exception:\n"
					                  << e.what() );
				}
			}

		// hack, to disappear soon:
		// MPLArtists need manual update on TimeWindow change to update their colormap. :(
		BOOST_FOREACH( ArtistPtr a, artists_ ){
			const Artist::SettingsMap::const_iterator it =
				a->settings_.find( "TWUpdate" );

			if ( it == a->settings_.end() || it->setting.value<bool>() == false )
				continue;

			forceRegen( a ); 
		}
	}

	if( tw.current() != last_time_window_.current() ){
		BOOST_FOREACH( WidgetArtist* wa, widget_artists_ )
			try{
				wa->updateTime( tw.current() );
			}
			catch( boost::python::error_already_set& e ){
				log_error_stream( wa->description()
				                  << "::updateTime(...) threw a Python exception:\n"
				                  << e );
			}
			catch( std::exception& e ){
				log_error_stream( wa->description()
				                  << "::updateTime(...) threw an exception:\n"
				                  << e.what() );
			}
	}

	last_time_window_ = tw;
}

/* Return true if the previously generated content of this Artist
 * is equivalent to what would be regenerated by calling create() now
 */
bool Scenario::isValid( ArtistPtr artist ){
	// An artist's previous output is valid if all keys match
	// existing sources, and no change has happened to the settings

	unsigned num_required_keys = artist->numRequiredKeys();
	if( num_required_keys == 0 ){
		return artist->settings_valid_;
	}
	else if( !frame_ ){
		// artist requires data but frame is null; assume update is needed.
		return false;
	}
	else if( (num_required_keys != artist->sources_.size()) ||
	         (num_required_keys != artist->keys_.size()) ){
		// The iteration below only makes sense if the sizes match
		return false;
	}

	Artist::sourcelist::iterator si = artist->sources_.begin();
	ArtistKeyList::iterator ki = artist->keys_.begin();

	bool stillValid = artist->settings_valid_;
	for( ; stillValid && si != artist->sources_.end(); ++si, ++ki ){
		if( frame_->Has( *ki ) ){
			try{
				stillValid &= frame_->Get<I3FrameObjectConstPtr>(*ki) == *si;
			} catch(...) {
				log_warn_stream( "Could not load " << *ki << ", "
				                 "probably unregistered type" );
				stillValid = false;
			}
		}else{
			stillValid = false;
		}
	}

	return stillValid;
}

bool Scenario::canCreate( ArtistPtr artist ){
	// sources have changed: determine if we can use this Artist on this frame
	for( ArtistKeyList::iterator ki = artist->keys_.begin();
	     ki != artist->keys_.end(); ++ki){
		if( !frame_ || !frame_->Has( *ki ) )
			return false;
		try{
			if( !artist->isValidKey( frame_, ki-(artist->keys_.begin()), *ki ) ){
				log_error_stream( artist->description()
				                  << ": for key " << *ki << " "
				                  << " got incompatible type "
				                  << frame_->type_name(*ki) );
				return false;
			}
		}catch(...){
			log_warn_stream( "Could not load " << *ki << ", "
			                 "probably unregistered type" );
		}
	}
	return true;
}

void Scenario::perhapsRegen( ArtistPtr artist ){
	OutputsChangedEmitter emit(this);
	perhapsRegen( artist, emit );
}

void Scenario::perhapsRegen( ArtistPtr artist, OutputsChangedEmitter& emit ){
	if( !artist->active_ ){
		log_debug_stream( artist->description() 
		                  << " is inactive" );
		return;
	}

	if( isValid(artist) ){
		log_debug_stream( artist->description()
		                  << "'s output is still valid" );
		return;
	}

	artist->sources_.clear();

	if( WidgetArtist* wartist = dynamic_cast<WidgetArtist*>(artist.get()) ){
		if( !wartist->widget_->isEnabled() ) {
			log_debug_stream( artist->description() << " not enabled" );
			return;
		}

		bool canBeCreated = canCreate( artist );
		if( canBeCreated ){
			BOOST_FOREACH( const std::string& key, artist->keys_ ){
				try{
					artist->sources_.push_back( frame_->Get<I3FrameObjectConstPtr>(key) );
				}
				catch(...){
					log_warn_stream( "Could not load " << key << ", "
					                 "probably unregistered type" );
				}
			}
		} else
			log_debug_stream( artist->description() << " cannot be created" );

		try{
			// WidgetArtist must be able to handle empty I3FramePtr
			wartist->updateFrame( frame_ );
			log_debug_stream( artist->description() << " redrawn" );
			artist->settings_valid_ = true;
		}
		catch( boost::python::error_already_set& e ){
			log_error_stream( artist->description()
			                  << "::update(...) threw a Python exception:\n"
			                  << e );
		}
		catch( boost::bad_any_cast& ){
			log_error_stream( artist->description()
			                  << "::update(...) threw a bad cast, "
			                  << "problem exists with a Setting" );
		}
		catch( std::exception& e ){
			log_error_stream( artist->description()
			                  << "::update(...) threw an exception:\n"
			                  << e.what() );
		}
		catch( ... ){
			log_error_stream( artist->description()
			                  << "::update(...) threw an unknown exception" );
		}
	}
	else
	if( GLArtist* glartist = dynamic_cast<GLArtist*>(artist.get()) ){
		scene_->dropOutputs( artist );

		if( canCreate( artist ) ){
			BOOST_FOREACH( const std::string& key, artist->keys_ ){
				try{
					artist->sources_.push_back( frame_->Get<I3FrameObjectConstPtr>(key) );
				}catch(...){
					log_warn_stream( "Could not load " << key << ", "
					                 "probably unregistered type" );
				}
			}

			try{
				SceneGroup* group = new SceneGroup( artist );
				glartist->create( frame_, group, scene_->state() );
				scene_->add( group ); // scene takes ownership of g
				artist->settings_valid_ = true;
				log_debug_stream( artist->description() << " added" );
			}
			catch( boost::bad_any_cast& ){
				log_error_stream( artist->description()
				                  << "::create(...) threw a bad cast, "
				                  "problem exists with a Setting" );
			}
			catch( std::exception& e ){
				log_error_stream( artist->description() 
				                  << "::create(...) threw an exception:\n"
				                  << e.what() );
			}
			catch( ... ){
				log_error_stream( artist->description() 
				                  << "::create(...) threw an unknown exception" );
			}
		}else
			log_debug_stream( artist->description() << " could not be created" );
	}else
		log_fatal( "Could not cast artist to any known subclass" );

	emit();
}
