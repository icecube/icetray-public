#ifndef INSTALL_H
#define INSTALL_H
/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include <icetray/I3Context.h>

template <typename T>
struct I3InstallService {
	static bool
	func(boost::shared_ptr<T> what, I3ContextPtr ctx, const std::string& where)
	{
		return ctx->Put(where, what);
	}	
};

#endif
