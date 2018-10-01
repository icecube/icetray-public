/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3ModuleTestConfig.h 14186 2006-01-09 18:25:42Z troy $
 *
 * @file I3ModuleTestConfig.h
 * @version $Revision: 1.1 $
 * @date $Date: 2006-01-09 19:25:42 +0100 (Mon, 09 Jan 2006) $
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
