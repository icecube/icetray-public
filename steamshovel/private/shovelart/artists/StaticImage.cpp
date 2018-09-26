#include <QImage>

#include "shovelart/Scene.h"
#include "shovelart/SceneObject.h"

#include "StaticImage.h"

StaticImage::StaticImage()
{
    addSetting( "source", FileSetting() );
    addSetting( "keep aspect ratio", true );
}

void StaticImage::create( I3FramePtr ptr, SceneGroup* group, const SceneState& state ){
    QString source = setting<FileSetting>( "source" );
    if( source.isEmpty() )
        source = ":/resources/IceCube_official_logo_greyLetters.png";
        // source = ":/resources/IceCube_official_logo.png";
    QImage imgfile( source );

    // Determine the visible output size of the logo in order to use high-quality scaling on it
    QSize resize_to;

    // query my overlay hints to see what the requested size of the output is
    const SceneState::OverlayHintsMap& hintsmap = state.overlayHintsMap_;
    SceneState::OverlayHintsMap::const_iterator foundhints = hintsmap.find( this );

    if( foundhints != hintsmap.end() ){
        SceneState::OverlayHints hints = foundhints->second;
        resize_to = hints.at(0).toQSize();
    } else {
        // set initial size to image size
        resize_to = imgfile.size();
    }

    // if the source is the IceCube logo, set initial width
    // to 95 pixels and determine height from aspect ratio
    if( source.contains( "IceCube_official_logo" ) ){
        resize_to = imgfile.size();
        resize_to.scale( 95, 95, Qt::KeepAspectRatioByExpanding );
    }

    ImageOverlay* overlay = new ImageOverlay( state.gl,
                                              QPixmap::fromImage(imgfile),
                                              resize_to );
    overlay->setFixedAspect( setting<bool>( "keep aspect ratio" ) );
    // grody size-setting approach: the resize() function takes a delta, rather than an absolute size.
    // also, since the overlay has a fixed aspect ratio, only the x coordinate of the resize function is used.
    // So we determine the size change necessary in X, and ignore Y since the ratio is fixed.
    overlay->resize( resize_to.width() - overlay->width(), 0 );
    group->add( overlay );
}
