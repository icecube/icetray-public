/**
 * $Id$
 * (c) 2005 IceCube Collaboration
 * @author D.Turcan
 */

#ifndef I3CUTPARAMSMODULE_H
#define I3CUTPARAMSMODULE_H

#include <icetray/I3ConditionalModule.h>
#include "icetray/I3Tray.h"
#include "dataclasses/physics/I3Particle.h"


/**
 * @brief
 *
 */
class I3CutsModule : public I3ConditionalModule
{
public:

  I3CutsModule(const I3Context& ctx);
  ~I3CutsModule() { }
  void Configure();
  void Physics(I3FramePtr frame);

private:

  I3CutsModule();
  I3CutsModule(const I3CutsModule& source);
  I3CutsModule& operator=(const I3CutsModule& source);

  /**
   * Parameter: name(s) of the I3Particle(s) that the module writes to tree(s).
   * Multiple names can be included, separated by ",".  If no name is set,
   * all of the I3Particles from the event will be written.
   */
  std::string particleName_;

  /**
   * List of I3Particle names that the module writes to trees.
   */
  std::vector<std::string> particleNameList_;

  /**
   * Parameter: name of the hit series map to use in the calculation of the
   * cut parameters.
   */
  std::string hitsName_;

  /**
   * Parameter: name of the pulse series map to use in the calculation of
   * the cut parameters.
   */
  std::string pulsesName_;

  /**
   * Parameter: Optional tag for the name of the object inserted by this module
   * into the frame.
   */
  std::string nameTag_;

  /**
   * Parameter: time range for calculating direct hits.
   */
  std::vector<double> timeRange_;

  /**
   * Named stream for my logging messages.  Allows my messages to be
   * sent to a particular log file, to be set to a special threshold, etc.
   */
  SET_LOGGER("I3CutsModule");

};

#endif
