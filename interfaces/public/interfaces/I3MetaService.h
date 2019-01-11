/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3MetaService.h 17239 2006-03-14 17:51:36Z troy $
 *
 * @file I3MetaService.h
 * @version $Revision:$
 * @date $Date: 2006-03-14 18:51:36 +0100 (Tue, 14 Mar 2006) $
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
