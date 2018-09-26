/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include "I3CutValuesConverter.h"

I3TableRowDescriptionPtr
I3CutValuesConverter::CreateDescription(const I3CutValues& cv)
{
	I3TableRowDescriptionPtr desc(new I3TableRowDescription());
	
	desc->AddField<uint16_t>("n_chan", "number", "Number of DOMs triggered");
	desc->AddField<uint16_t>("n_strings", "number", "Number strings triggered");
	desc->AddField<uint32_t>("n_hit", "number", "Number of launches/pulses/hits in the event");
	desc->AddField<uint32_t>("n_dir", "number", "Number launches/pulses/hits"
	    " in the 'direct' time window. This is poorly defined when there is"
	    " more than one pulse per DOM.");
	desc->AddField<double>("l_dir", "m", "Length of direct hits projected "
	    " along the reconstructed track. This is only meaningful for muons.");
	desc->AddField<double>("s_dir", "", "A quantity related to the spacing of "
	    " direct hits projected onto the reconstructed track. Smoothness 0 "
	    " means that hits are spaced uniformly along the track, positive "
	    " values indicate that spacings vary. This is only meaningful for muons.");
	desc->AddField<double>("s_all", "", "Smoothness of all (not just direct) hits.");
	desc->AddField<double>("cog_x", "m", "Mean x position of the launches/pulses/hits in the event.");
	desc->AddField<double>("cog_y", "m", "Mean y position of the launches/pulses/hits in the event.");
	desc->AddField<double>("cog_z", "m", "Mean z position of the launches/pulses/hits in the event.");
	
	return desc;
}

size_t
I3CutValuesConverter::FillRows(const I3CutValues& cv, I3TableRowPtr rows)
{
	rows->Set<uint16_t >("n_chan",    cv.Nchan);
	rows->Set<uint16_t >("n_strings", cv.Nstring);
	rows->Set<uint32_t >("n_hit",     cv.Nhit);
	rows->Set<uint32_t >("n_dir",     cv.Ndir);
	rows->Set<double   >("l_dir",     cv.Ldir);
	rows->Set<double   >("s_dir",     cv.Sdir);
	rows->Set<double   >("s_all",     cv.Sall);
	rows->Set<double   >("cog_x",     cv.cog.GetX());
	rows->Set<double   >("cog_y",     cv.cog.GetY());
	rows->Set<double   >("cog_z",     cv.cog.GetZ());
	
	return 1;
}

