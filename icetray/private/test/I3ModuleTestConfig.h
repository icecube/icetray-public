/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3ModuleTestConfig.h
 * @version $Revision$
 * @date $Date$
 * @author pretz
 */

#ifndef I3MODULETESTCONFIG_H
#define I3MODULETESTCONFIG_H

/**
 * @brief A simple struct for holding compile-time configurations.
 */
struct I3ModuleTestConfig
{
  /**
   * @brief The maximum number of frames that the system will put out.
   */
  static const int max_frames=20;
};

#endif
