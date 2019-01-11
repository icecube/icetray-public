/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3ModuleTestConfig.h 165886 2018-10-01 14:37:58Z nwhitehorn $
 *
 * @file I3ModuleTestConfig.h
 * @version $Revision: 165886 $
 * @date $Date: 2018-10-01 07:37:58 -0700 (Mon, 01 Oct 2018) $
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
