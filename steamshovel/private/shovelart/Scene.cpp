#include <iostream>
#include <icetray/I3Logging.h>

#include "Artist.h"
#include "Scenario.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Camera.h"
#include "gl.h"

SceneGroup::SceneGroup( ArtistPtr ptr ) :
	std::vector<SceneObject*>(), source_(ptr), overlays_(false)
{}

SceneGroup::~SceneGroup(){
	for( iterator i = begin(); i!=end(); ++i ){
		delete *i;
	}
}

/** sorting functor for SceneObject draw order
 *
 * Draw order is as follows:
 *   - First draw objects in order by render pass, as defined in SceneObject
 *   - Within the SOLID pass, draw in order by shader ID to minimize shader transitions
 *   - within the TRANSPARENT pass, draw in order by camera distance (furthest first)
 *     to get transparency to look right
 *   - within the OVERLAY pass, draw in order by camera distance, which will simply be
 *     z-coordinates
 *   - otherwise order is arbitrary (sort by pointer value)
 */
struct scenesort{

	double vt_;
	const I3Camera& c_;

	scenesort( double vistime, const I3Camera& c ): vt_(vistime), c_(c) {}

	bool operator()( SceneObject* s1, SceneObject* s2 ){
		if( s1 == s2 ) return false;
		SceneObject::RenderPass ps1 = s1->pass(vt_), ps2 = s2->pass(vt_);
		if( ps1 == ps2 ){
			if( ps1 == SceneObject::SOLID ){
				return s1->shader(vt_) < s2->shader(vt_);
			}
			if( ps1 == SceneObject::TRANSPARENT || ps1 == SceneObject::OVERLAY ){
				return s1->cameraDistance( vt_, c_ ) > s2->cameraDistance( vt_, c_ );
			}
			else{
				return s1 < s2;
			}
		}
		else{
			return ps1 < ps2;
		}
	}

};


void Scene::draw( double vistime, const I3Camera& camera ){

	std::sort( objects_.begin(), objects_.end(), scenesort(vistime,camera) );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.doProjection();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.doLookAt();
	glColor3f(1,1,1);
	const int shader = camera.shader;

	// pass 1, solid objects, including those with shaders, and lines
	std::vector< SceneObject* >::iterator i = objects_.begin();

	while( i != objects_.end() && (*i)->pass(vistime) < SceneObject::TRANSPARENT ){
		SceneObject* s = *(i++);
		if( s->isVisible(vistime) ){
			if(shader) ShaderManager::ensureBound( s->shader(vistime) );
			s->draw(vistime, camera);
		}
	}

	// pass 2, transparent object in back-to-front order, depth write disabled
	glDepthMask( false );
	while( i != objects_.end() && (*i)->pass(vistime) == SceneObject::TRANSPARENT ){
		SceneObject* s = *(i++);
		if( s->isVisible(vistime) ){
			if(shader) ShaderManager::ensureBound( s->shader(vistime) );
			s->draw(vistime, camera);
		}
	}

	// pass 3, draw the overlay objects with an orthographic projection and depth write enabled
	glColor3f(1,1,1);
	glDepthMask( true );
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.doScreenOrtho();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	while( i != objects_.end() ){
		SceneObject* s = *(i++);
		if( s->isVisible(vistime) ){
			if(shader) ShaderManager::ensureBound( s->shader(vistime) );
			s->draw(vistime, camera);
		}
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	ShaderManager::release();
}

// Collect the screen location of any overlays in this group,
// so that they may be restored in the future
void Scene::gatherHints( SceneState::OverlayHints& hints, SceneGroupPtr group ){
	if( fixed_state_ ){
		log_debug( "gathering no hints because state is fixed" );
		return;
	}
	hints.clear();
	for( SceneGroup::iterator i = group->begin(); i != group->end(); ++i ){
		SceneOverlay* overlay = dynamic_cast<SceneOverlay*>( *i );
		if( overlay && overlay->width() > 5 && overlay->height() > 5 ){
			hints.push_back( overlay->makeHint() );
		}
	}
	log_trace_stream( "Gathered " << hints.size() << " overlay hints" );
}

// Apply previously collected overlay location hints to a new group
void Scene::applyHints( SceneState::OverlayHints& hints, SceneGroupPtr group ){

	log_debug_stream( "Applying " << hints.size() << " overlay hints" );
	SceneGroup::iterator objects = group->begin();
	for( SceneState::OverlayHints::iterator i = hints.begin(); i!= hints.end(); ++i ){
		SceneOverlay* overlay = NULL;
		while( !overlay && objects != group->end() ){
			overlay = dynamic_cast<SceneOverlay*>( *(objects++) );
		}
		if( overlay ){
			overlay->applyHint( *i );
		}
		else
			break;
	}

}

void Scene::add( SceneGroupPtr s ){
	groups_.insert(s);
	objects_.insert( objects_.end(), s->begin(), s->end() );
	log_debug_stream( "Added SceneGroup, size now " << objects_.size() );

	if( s->overlays_ ){
		log_debug( "  SceneGroup has overlays..." );
		SceneState::OverlayHints& hints = state_.overlayHintsMap_[ s->source_ ];
		applyHints( hints, s );
	}
}

void Scene::dropOutputs( ArtistPtr ptr ){

	objects_.clear();

	// collect the set of SceneGroups sourced from this ptr
	std::set<SceneGroupPtr> to_drop;
	for( std::set<SceneGroupPtr>::iterator i = groups_.begin();
	     i != groups_.end(); ++i )
	{
		// either mark the group for dropping, or reinsert its contents into objects_
		if( (*i)->getSource() == ptr ){
			to_drop.insert(*i);
		}
		else{
			objects_.insert( objects_.end(), (*i)->begin(), (*i)->end() );
		}
	}
	log_trace_stream( "Dropping " << to_drop.size() << " SceneGroups" );

	// erase them from groups_ and grab overlay hints, if any
	for( std::set<SceneGroupPtr>::iterator i = to_drop.begin();
	     i != to_drop.end(); ++i )
	{
		if( (*i)->overlays_ ){
			gatherHints( state_.overlayHintsMap_[ptr], (*i) );
		}
		groups_.erase( *i );
	}
}

void Scene::dropSource( ArtistPtr ptr ){
	dropOutputs( ptr );
	if( !fixed_state_ )
		state_.overlayHintsMap_.erase( ptr );
}

void Scene::updateAllHints(){
	if( fixed_state_ )
		return;
	state_.overlayHintsMap_.clear();
	for( std::set<SceneGroupPtr>::iterator i = groups_.begin(); i != groups_.end(); ++i ){
		if( (*i)->overlays_ ){
			gatherHints( state_.overlayHintsMap_[(*i)->source_], (*i) );
		}
	}
}

void Scene::dropAll(){
	log_trace("dropAll");
	updateAllHints();
	objects_.clear();
	groups_.clear();
}

ArtistPtr Scene::overlaySource( SceneOverlay* overlay ){
	log_debug( "An overlay was resized" );
	ArtistPtr source;
	for( std::set<SceneGroupPtr>::iterator i = groups_.begin(); i != groups_.end() && !source; ++i ){
		SceneGroupPtr group = *i;
		if( group->overlays_ ){
			for( SceneGroup::iterator j = group->begin(); j != group->end(); ++j ){
				if( *j == overlay ){
					source = group->getSource();
					break;
				}
			}
		}
	}

	if( !source ){
		throw std::runtime_error("No source found for overlay");
	}
	return source;

}

// ewwww
#define SELECTBUF_SIZE 30000

boost::any Scene::pick( double vistime, const I3Camera& camera, int winx, int winy ){

	// adapted from the OpenGL Programming Guide (5th ed., ch 13)

	GLuint selectBuf[ SELECTBUF_SIZE ];
	GLint hits;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	log_trace( "Viewport: %d, %d, %d, %d", viewport[0], viewport[1], viewport[2], viewport[3] );

	glSelectBuffer(SELECTBUF_SIZE, selectBuf);
	glRenderMode( GL_SELECT );

	glInitNames();
	glPushName(-1);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix( winx, viewport[3] - winy, 2, 2, viewport );
	camera.doProjection();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	camera.doLookAt();

	// Disable the shader to run faster on Intel platforms
	//const int shader = camera.shader;
	const int shader = 0;

	log_trace("Rendering for pick at %d, %d...", winx, winy );
	size_t len = objects_.size();
	size_t i;
	for( i = 0; i < len && objects_[i]->pass(vistime) < SceneObject::OVERLAY; ++i ){
		if( objects_[i]->isSelectable(vistime) ){
			glLoadName(i);
			if(shader) ShaderManager::ensureBound( objects_[i]->shader(vistime) );
			objects_[i]->draw(vistime, camera);
		}
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix( winx, viewport[3] - winy, 2, 2, viewport );
	camera.doScreenOrtho();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for( ; i < len; ++i ){
		if( objects_[i]->isSelectable(vistime) ){
			glLoadName(i);
			if(shader) ShaderManager::ensureBound( objects_[i]->shader(vistime) );
			objects_[i]->draw(vistime, camera);
		}
	}

	ShaderManager::release();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glFlush();
	hits = glRenderMode(GL_RENDER);

	log_trace("Picking got %d hits", hits);
	GLuint *ptr = selectBuf;
	GLuint minz = UINT_MAX;
	boost::any ret;
	for( int i = 0; i < hits; ++i ){
		GLuint numnames = *ptr++;
		GLuint z = *ptr++;
		/*int maxz = * */ ptr++;
		GLuint name = *ptr;
		if( numnames != 1 ){
			log_error( "Got more than one name in a selection hit record" );
		}
		for( GLuint j = 0; j < numnames; ++j ){
			++ptr;
		}
		if( minz > z ){
			log_trace_stream( "New min: " << z << " i=" << i );
			minz = z;
			ret = objects_.at(name)->selectionContent();
		}
	}

	{
		GLenum errCode;
		const GLubyte* errString;
		if( (errCode = glGetError()) != GL_NO_ERROR ){
			errString = gluErrorString(errCode);
			log_error("GL error while picking: %s", errString );
		}
	}

	return ret;

}

Scene::WithOverlaysScaled::WithOverlaysScaled( const boost::shared_ptr<Scene>& parent,
                                               Scenario* scenario, double scaling_factor ):
	parent_(parent), scenario_(scenario)
{
	scenario_->blockSignals(true);
	parent_->updateAllHints();
	old_hints_ = parent_->state().overlayHintsMap_;
	SceneState::OverlayHintsMap new_hints;
	for( SceneState::OverlayHintsMap::iterator i = old_hints_.begin(); i != old_hints_.end(); ++i ){
		SceneState::OverlayHints scaled_hint;
		for( SceneState::OverlayHints::iterator j = i->second.begin(); j != i->second.end(); ++j ){
			scaled_hint.push_back( j->scaled(scaling_factor) );
		}
		new_hints[i->first] = scaled_hint;
	}
	parent_->state_.scaling_factor = scaling_factor;
	parent_->state_.overlayHintsMap_ = new_hints;
	parent_->fixed_state_ = true;
	for( SceneState::OverlayHintsMap::iterator i = new_hints.begin(); i != new_hints.end(); ++i ){
		scenario->forceRegen( i->first );
	}
}

Scene::WithOverlaysScaled::~WithOverlaysScaled(){
	parent_->state_.scaling_factor = 1.0;
	parent_->state_.overlayHintsMap_ = old_hints_;
	for( SceneState::OverlayHintsMap::iterator i = old_hints_.begin(); i != old_hints_.end(); ++i ){
		scenario_->forceRegen( i->first );
	}
	parent_->fixed_state_ = false;
	scenario_->blockSignals(false);
}
