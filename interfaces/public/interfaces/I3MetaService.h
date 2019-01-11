/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3MetaService.h 165886 2018-10-01 14:37:58Z nwhitehorn $
 *
 * @file I3MetaService.h
 * @version $Revision: 165886 $
 * @date $Date: 2018-10-01 07:37:58 -0700 (Mon, 01 Oct 2018) $
 * @author pretz
 */

#ifndef INTERFACES_I3METASERVICE_H_INCLUDED
#define INTERFACES_I3METASERVICE_H_INCLUDED

#include <icetray/I3Frame.h>
#include <icetray/I3DefaultName.h>

class I3MetaService
{
 public:

  /**
   * @brief indicates whether or not there are more events to find.
   * 
   */
  virtual I3FramePtr PopMeta() = 0;

  virtual ~I3MetaService();
};

I3_DEFAULT_NAME(I3MetaService);
I3_POINTER_TYPEDEFS(I3MetaService);

#endif
