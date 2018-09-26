#ifndef I3_SHOVEL_RENDERING_SCENARIO_H
#define I3_SHOVEL_RENDERING_SCENARIO_H

#include <QObject>
#include <QMetaType>
#include <QStringList>
#include <QString>

#include <boost/function.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <string>
#include <utility>

#include <icetray/I3Logging.h>

#ifndef Q_MOC_RUN
#include "Scene.h"
#endif
#include "ArtistFwd.h"

class I3Frame;
class OutputsChangedEmitter;
class QWidget;

/** Class to hold and edit all Artists that apply to a Scene
 *
 * All creation and modification of Artists happens through a Scenario
 *
 */
class Scenario : public QObject {

	Q_OBJECT;
	SET_LOGGER( "Scenario" );

	friend class OutputsChangedEmitter;

public:
	Scenario( QObject* parent, boost::shared_ptr<Scene> scene ):
		QObject( parent ), scene_(scene)
	{}

// ---------------------- Basic queries -------------------

	/** Return the current artists */
	const std::vector< ArtistPtr >& getArtists() const{ return artists_; }

	/** Return the valid keys for Artist p's key_idx'th key slot */
	ArtistKeyList validKeys( ArtistPtr p, unsigned key_idx ) const;

	// /** Return the valid keys that match KeySetting */
	// ArtistKeyList validKeys( const KeySetting& keySetting );

	/** Return a list of current artists that are actively displaying the given list of keys */
	std::vector< ArtistPtr > findArtistsByInput( const ArtistKeyList& keys ) const;

	/** Return the WidgetArtist connected to the passed QWidget */
	WidgetArtist* findWidgetArtist( const QWidget* );

// ---------------------- Default-key related queries ----------------------

	/** Return those keys in Artist p for which more than one option exists */
	ArtistKeyList customKeys( ArtistPtr p );

	/**
	 * Return a list of artist Factories that could render the given list of keys for the current frame.
	 * If expand_default_keys is true, then default keys are included via expandDefaultKeys().
	 */
	std::vector< const ArtistFactory* > findUsableArtistsByKeys( const ArtistKeyList& keys, bool expand_keys = true );

	/**
	 * Given an artist factory and an (optional) list of input keys, return an expanded list in which blank keys that
	 * could be filled using the Scenario's default keys map are filled with those defaults.  The returned list *may*
	 * be a valid key set for the given artist type, but it may not be; some keys may be blank, or the list may be
	 * empty.
	 * */
	ArtistKeyList expandDefaultKeys( const ArtistFactory* factory, const ArtistKeyList* expansion_keys = NULL ) const;

// ----------------------- Changes to Artist settings ----------------------------
//           (these cannot be Qt slots because they are templated)

	template< typename T >
	void changeSetting( ArtistPtr p, const std::string& key, T new_value ){
		p->settings_.find(key)->setting.setValue<T>( new_value );
		perhapsRegen(p);
		Q_EMIT artistUpdated( p );
	}

	template< typename T >
	void changeComplexSetting( ArtistPtr p, const std::string& key,
	                           const typename T::Value& new_value ){
		p->settings_.find(key)->setting.setValueOfComplex<T>( new_value );
		perhapsRegen(p);
		Q_EMIT artistUpdated( p );
	}

	/**
	 * Unconditionally force an artist to re-create() itself.
	 */
	void forceRegen( ArtistPtr, OutputsChangedEmitter& );
	void forceRegen( ArtistPtr p );

	// regenerate all artists
	void forceRegen();

	/**
	 * Get access to the associated SceneState, first ensuring it's up to date
	 */
	const SceneState& accessState() const { scene_->updateAllHints(); return scene_->state(); }

// -------------------------  Primary slot-based API -------------------------

public Q_SLOTS:

	/// Add a new artist to the Scenario
	void add( ArtistPtr p );
	/// Remove an artist from the Scenario
	void remove( ArtistPtr p );
	/// Remove all artists from the Scenario
	void clear();
	/// Update an artist's nth key to str
	void setKey( ArtistPtr a, int n, const std::string& str );
	/// Set an artist's activeness state
	void setIsActive( ArtistPtr, bool );
	/// Update the currently selected frame
	void setFrame( I3FramePtr ptr );
	/// Update the user-selected set of keys
	void selectOMKeys( const OMKeySet& keys );
	/// Update an artist's overlayhints
	void setOverlaySizeHints( ArtistPtr p, const SceneState::OverlayHints& hints );
	/// Update widget artists to current time
	void updateWidgetArtists( const TimeWindow& );

Q_SIGNALS:
	/** Emitted when an artist added */
	void artistAdded( ArtistPtr a );
	/** Emitted when an artist is about to be removed; all listeners should drop their references */
	void artistRemoved( ArtistPtr a );
	/** Emitted when an artists's keys, settings, or isActive changes */
	void artistUpdated( ArtistPtr a );
	/** Emitted when anything occurs to alter the graphical output of the Scene */
	void outputsChanged();
	/** Emitted when the current frame changes, indicating that artist key valid key lists may have changed */
	void inputsChanged( I3FramePtr );
	/** Emitted when a WidgetArtist was added */
	void widgetArtistAdded( QString, QWidget* );
	/** Emitted when a WidgetArtist is about to be removed */
	void widgetArtistRemoved( QWidget* );
	/** Emitted when the user needs to be notified about a serious error */
	void showLog();

private:
	// bool perhapsRegen( ArtistPtr, bool quiet = false );
	void perhapsRegen( ArtistPtr );
	void perhapsRegen( ArtistPtr, OutputsChangedEmitter& );
	bool isValid( ArtistPtr );
	bool canCreate( ArtistPtr );

	boost::shared_ptr<Scene> scene_;
	I3FramePtr frame_;
	std::vector< ArtistPtr > artists_;
	std::vector< WidgetArtist* > widget_artists_;
	TimeWindow last_time_window_;
	typedef boost::unordered_map< std::pair<ArtistPtr, int>, ArtistKeyList > ValidKeyCache;
	mutable ValidKeyCache valid_key_cache_;
};

#endif /* I3_SHOVEL_RENDERING_SCENARIO_H */
