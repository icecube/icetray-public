/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3MetaService.h
 * @version $Revision$
 * @date $Date$
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
