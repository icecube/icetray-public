#include <icetray/I3Logging.h>
#include <icetray/name_of.h>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3LinearizedMCTree.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Vector.h>
#include <dataclasses/I3Constants.h>

#include "Particles.h"
#include "shovelart/Scene.h"
#include "shovelart/SceneObject.h"
#include "shovelart/ColorVariant.h"
#include "scripting/gil.h"
#include "shovelart/Camera.h"

#include <QString>

#include <boost/make_shared.hpp>

namespace {

  const int kNeutrinoStippleScale = 10;
  const float kArrowHeadRatio = 0.3;
  const QColor kCherenkovColor = QColor::fromRgbF(0.2f, 0.2f, 0.95f, 0.4f);

  float ParseEnergy(QString s)
  {
    if( s.isEmpty() )
      return 0.0;

    // parse string, format: <number> <unit>
    QStringList l = s.split(" ", QString::SkipEmptyParts);
    float result = 0.0;
    if( l.size() >= 1 ){
      result = l[0].toDouble();

      if( l.size() == 2 ){
        const QString& unit = l[1];

        if( unit == "eV" )
          result *= 1e-9;
        else if( unit == "keV" )
          result *= 1e-6;
        else if( unit == "MeV" )
          result *= 1e-3;
        else if( unit == "TeV" )
          result *= 1e3;
        else if( unit == "PeV" )
          result *= 1e6;
        else if( unit == "EeV" )
          result *= 1e9;
      }
    }

    return result;
  }

  class LightFrontRadius : public SceneVariant<float> {
    int t0_;
    float speed_;

  public:
    LightFrontRadius(float t0, float n)
      : t0_(t0), speed_(I3Constants::c/n)
    {}

    virtual float value( double vistime ) {
      if (vistime < t0_ || (vistime-t0_)*speed_ > 1e3)
        return 0.;
      else
        return (vistime-t0_)*speed_;
    }
  };

  class LightFrontColor : public SceneVariant<QColor> {
    const TimeWindow* win_;
    int t_;
    float speed_;
    float attenuationLength_;
    float intensity_;
    ColorMapPtr cm_;
    bool use_cherenkov_blue_;
  public:
    LightFrontColor( const TimeWindow* win, int t, float n, float attenuationLength,
                     float intensity, ColorMapPtr cm, bool use_cherenkov_blue ) :
      win_(win), t_(t),
      speed_(I3Constants::c/n),
      attenuationLength_(attenuationLength),
      intensity_(intensity),
      cm_(cm),
      use_cherenkov_blue_(use_cherenkov_blue)
    {}

    virtual QColor value( double vistime ){
      QColor v;
      if( use_cherenkov_blue_ ){
        v = QColor("RoyalBlue");
      }
      else{
        float normalized_t = ( t_ - win_->colorStart() ) / ( win_->colorEnd() - win_->colorStart() );
        normalized_t = std::min( 1.f, std::max( 0.f, normalized_t ) );
        v = cm_->value(normalized_t);        
      }
      v.setAlphaF( getAlpha(vistime) );
      return v;
    }

  private:
    float getAlpha( double vistime ) const
    {
      float alpha = intensity_*std::exp(-std::max(vistime-t_, 0.0)*speed_/attenuationLength_);
      return alpha;
    }
  };

  class LightFrontSphere : public SphereObject{
  public:
    LightFrontSphere( float r, const vec3d& loc ) :
      SphereObject(r,loc)
    {}

    float cameraDistance( double vistime, const I3Camera& camera ){
      // Sort light front spheres differently than standard spheres,
      // Ensure light front spheres are drawn in order by size, smallest first
      return 1.0f / scale_->value(vistime);
    }
  };

  class ConeHead: public CylinderObject {
    const I3ParticlePtr particle_;
    WindowTime time_;

  public:
    ConeHead(const I3ParticlePtr p,
             const TimeWindow* window,
             float length, float ratio)
      : CylinderObject( vec3d(0,0,0),
                        length * from_I3Direction( p->GetDir() ),
                        length * ratio ),
        particle_(p), time_(window, TimeWindow::CURRENT)
    {
      setLocation( VariantVec3dPtr(
        new ParticlePoint(p, window, TimeWindow::CURRENT, -length)
      ));
    }

    virtual bool isVisible( double vistime ){
      double t = time_.value(vistime);
      I3Particle::ParticleShape shape = particle_->GetShape();

      if (shape != I3Particle::InfiniteTrack &&
          shape != I3Particle::StoppingTrack &&
          t < particle_->GetTime())
        return false;

      if (shape == I3Particle::StoppingTrack &&
          t > particle_->GetTime())
        return false;

      if (particle_->GetLength() < particle_->GetSpeed() * (t - particle_->GetTime()))
        return false;

      return true;
    }
  };

  class ParticleArrow: public ArrowObject {
    WindowTime time_;
    I3ParticlePtr particle_;

  public:
    ParticleArrow( I3ParticlePtr p,
                   const TimeWindow* window,
                   float length ):
      ArrowObject( VariantVec3dPtr( new ParticlePoint(p, window, TimeWindow::START) ),
                   VariantVec3dPtr( new ParticlePoint(p, window, TimeWindow::CURRENT) ),
                   std::atan(kArrowHeadRatio), length ),
      time_(window, TimeWindow::CURRENT),
      particle_(p)
    {}

    bool isHeadVisible( double vistime ){
      double t = time_.value(vistime);
      I3Particle::ParticleShape shape = particle_->GetShape();

      if (shape != I3Particle::InfiniteTrack &&
          shape != I3Particle::StoppingTrack &&
          t < particle_->GetTime())
        return false;

      if (shape == I3Particle::StoppingTrack &&
          t > particle_->GetTime())
        return false;

      if (particle_->GetLength() < particle_->GetSpeed() * (t - particle_->GetTime()))
        return false;

      return true;
    }
  };
}

Particles::Particles() {
  addSetting( "colormap", ColorMapPtr( new I3TimeColorMap() ) );
  addSetting( "color by type", true );
  addSetting( "color", QColor::fromRgbF(0.95, 0.2, 0.2, 0.6) );
  addSetting( "linewidth", RangeSetting(1, 10, 10, 3.0) );
  addSetting( "power", RangeSetting(0.0, 0.5, 50, 0.15) );
  addSetting( "scale", RangeSetting(0.0, 100.0, 100, 10.0) );
  addSetting( "show light fronts", false );
  addSetting( "blue light fronts", true );
  addSetting( "Cherenkov cone size", RangeSetting(0.0, 500.0, 100, 0.0) );
  addSetting( "arrow head size", RangeSetting(0.0, 400.0, 100, 120.0) );
  addSetting( "vertex size", RangeSetting(0.0, 100.0, 100, 0.0) );
  addSetting( "incoming/outgoing", true );
  addSetting( "labels", true );
  addSetting( "min. energy [track]", QString() );
  addSetting( "min. energy [cascade]", QString() );
}

bool
Particles::isValidKey( I3FramePtr ptr, int key_idx, const std::string& key )
{
  const std::string type_name = ptr->type_name(key);
  return type_name == I3::name_of<I3Particle>() ||
         type_name == I3::name_of<I3Vector<I3Particle> >() ||
         type_name == I3::name_of<I3MCTree>() || // new I3MCTree
         type_name == "I3Tree<I3Particle>" || // old I3MCTree
         type_name == I3::name_of<I3LinearizedMCTree>();
}

void
Particles::create( I3FramePtr ptr, SceneGroup* g, const SceneState& state )
{
  const float min_energy_cascade = ParseEnergy( setting<QString>("min. energy [cascade]") );
  const float min_energy_track = ParseEnergy( setting<QString>("min. energy [track]") );
  const bool incoming_outgoing = setting<bool>( "incoming/outgoing" );

  const std::string key = keys()[0];
  if (I3ParticleConstPtr p = ptr->Get<I3ParticleConstPtr>(key))
    drawParticle( *p, g, state, incoming_outgoing,
                  min_energy_cascade, min_energy_track );
  else
  if (I3MCTreeConstPtr t = ptr->Get<I3MCTreeConstPtr>(key))
    drawCollection( *t, g, state, false, min_energy_cascade, min_energy_track );
  else
  if (I3LinearizedMCTreeConstPtr t = ptr->Get<I3LinearizedMCTreeConstPtr>(key))
    drawCollection( *t, g, state, false, min_energy_cascade, min_energy_track );
  else
  if (I3VectorI3ParticleConstPtr v = ptr->Get<I3VectorI3ParticleConstPtr>(key))
    drawCollection( *v, g, state, incoming_outgoing,
                    min_energy_cascade, min_energy_track );
  return;
}

template <typename Container>
void Particles::drawCollection( Container& container,
                                SceneGroup *g,
                                const SceneState& state,
                                bool incoming_outgoing,
                                float min_energy_cascade,
                                float min_energy_track )
{
  for (typename Container::const_iterator
         p = container.begin();
       p != container.end(); p++) {
    drawParticle( *p, g, state, incoming_outgoing, min_energy_cascade, min_energy_track );
  }
}

void Particles::drawParticle( const I3Particle& p,
                              SceneGroup *g,
                              const SceneState& state,
                              bool incoming_outgoing,
                              float min_energy_cascade,
                              float min_energy_track )
{
  VariantQColorPtr color( new SceneConstant<QColor>(setting<QColor>("color")) );

  const float power = setting<RangeSetting>("power");
  const float intensity = p.GetEnergy() > 0.0 ?
    std::pow(p.GetEnergy() / I3Units::GeV, (double)power) : 1.0;

  VariantFloatPtr linewidth(
    new SceneConstant<float>(setting<RangeSetting>("linewidth") * intensity));
  const float arrowsize = setting<RangeSetting>("arrow head size") * std::sqrt(intensity);

  if( p.IsCascade() ){
    if( p.GetEnergy() < min_energy_cascade )
      return;

    vec3d pos( from_I3Position(p.GetPos()) );
    SphereObject* bubble;
    if( setting<bool>("show light fronts") ){
      bubble = new LightFrontSphere(1, pos);
      bubble->setSize( new LightFrontRadius( p.GetTime(), I3Constants::n_ice_group ) );
      const float intensity = std::max( 0.0, 1.0 - std::pow(p.GetEnergy(), -(double)power) );
      bubble->setColor( new LightFrontColor( state.times,
                                             p.GetTime(),
                                             I3Constants::n_ice_group,
                                             100.f,
                                             intensity,
                                             setting<ColorMapPtr>("colormap"),
                                             setting<bool>("blue light fronts") ) );
    }
    else{
      bubble = new SphereObject( 1, pos );
      SceneStepFunction<float>* sizef = new SceneStepFunction<float>( 0 );
      const float size = intensity * setting<RangeSetting>("scale");
      sizef->add( size, p.GetTime() );
      bubble->setSize( sizef );
      if( setting<bool>("color by type") ){
        switch( p.GetType() ){
        case I3Particle::Brems:
          bubble->setColor(QColor("Red"));
          break;
        case I3Particle::DeltaE:
          bubble->setColor(QColor("Green"));
          break;
        case I3Particle::PairProd:
          bubble->setColor(QColor("Blue"));
          break;
        case I3Particle::NuclInt:
          bubble->setColor(QColor("Magenta"));
          break;
        default:
          log_warn ("Unknown Energy Loss Type: %i", p.GetType());
          break;
        }
      }
      else{
        bubble->setColor( new TimeWindowColor( state.times, p.GetTime(),
                                               setting<ColorMapPtr>("colormap") ) );
      }
    }
    g->add( bubble );

    if( setting<bool>("labels") ){
      std::ostringstream label;
      label << p.GetEnergy() << "GeV " << p.GetTypeString();
      SceneObject* clabel = new TextObject( state.gl, label.str(), pos );
      g->add(clabel);
    }
  } else
  if( p.IsTrack() || p.IsNeutrino() ){
    
    if( p.GetEnergy() < min_energy_track )
      return;
    
    I3ParticlePtr pp( new I3Particle(p) );
    ParticleArrow* obj = new ParticleArrow( pp, state.times, arrowsize );
    obj->setColor(color);
    
    obj->setLineWidth(linewidth);
    if (p.IsNeutrino())
      obj->setStippleScale(kNeutrinoStippleScale);
    g->add( obj );

    // add a Cherenkov Cone if so requested
    
    const float cconesize = setting<RangeSetting>("Cherenkov cone size");
    const double beta = p.GetSpeed() / I3Constants::c;
    const double base_length_ratio = tan(acos(
      1.0 / (I3Constants::n_ice_phase * beta)
    ));
    if (cconesize && p.IsTrack() && !std::isnan(base_length_ratio)) {
      ConeHead* obj = new ConeHead( pp, state.times, cconesize, base_length_ratio );
      obj->setColor( new SceneConstant<QColor>( kCherenkovColor ) );
      g->add(obj);
    }
  }

  // add vertex point if so requested
  const float vertexsize = setting<RangeSetting>( "vertex size" );
  if( !p.IsCascade() && vertexsize && p.HasPosition() ){
    SphereObject* vertex = new SphereObject( vertexsize, from_I3Position( p.GetPos() ) );
    vertex->setColor(color);

    SceneStepFunction<SphereObject::ShadeMode>* shade =
      new SceneStepFunction<SphereObject::ShadeMode>( SphereObject::RING );
    shade->add( SphereObject::MATTE, p.GetTime() );
    vertex->setShader( shade );

    g->add( vertex );
  }

  // add incoming/outgoing dashed lines for tracks with start and/or end points
  if( incoming_outgoing && p.IsTrack() &&
      p.GetShape() != I3Particle::InfiniteTrack ){
    I3Particle::ParticleShape sh = p.GetShape();
    I3ParticlePtr p2[2]; // in, out

    if( sh == I3Particle::StartingTrack || 
        sh == I3Particle::ContainedTrack ||
        sh == I3Particle::MCTrack ){
      // has a starting point
      p2[0].reset( new I3Particle() );
      p2[0]->SetShape( I3Particle::StoppingTrack );
      p2[0]->SetPos( p.GetStartPos() );
      p2[0]->SetTime( p.GetStartTime() );
    }

    if( sh == I3Particle::StoppingTrack ||
        ( ( sh == I3Particle::ContainedTrack ||
            sh == I3Particle::MCTrack )
          && p.GetLength() > 0 ) ){
      // has a stopping point
      p2[1].reset( new I3Particle() );
      p2[1]->SetShape( I3Particle::StartingTrack );
      p2[1]->SetPos( p.GetStopPos() );
      p2[1]->SetTime( p.GetStopTime() );     
    }

    for( int i = 0; i < 2; ++i ){
      if( !p2[i] ) continue;
      p2[i]->SetDir( p.GetDir() );
      p2[i]->SetSpeed( p.GetSpeed() );
      ParticleArrow* obj = new ParticleArrow( p2[i], state.times, arrowsize );
      obj->setColor( color );
      obj->setLineWidth( linewidth );
      obj->setStippleScale( kNeutrinoStippleScale );
      g->add( obj );
    }
  }
}

