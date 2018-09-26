#ifndef I3EVENTINFOCONVERTER_H_INCLUDED
#define I3EVENTINFOCONVERTER_H_INCLUDED

/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include <tableio/I3Converter.h>

#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3Waveform.h>

template<class HitType>
class I3EventInfoConverter : public I3ConverterImplementation<I3Map<OMKey, std::vector<HitType> > > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3Map<OMKey, std::vector<HitType> >& hitmap) {
        I3TableRowDescriptionPtr desc(new I3TableRowDescription());
        desc->AddField<uint32_t>("nchan",      "",   "number of hit channels (calculated as the size of the hit map)");
        //desc->AddField<uint32_t>("nom",        "",   "");
        desc->AddField<uint32_t>("nchan_1hit", "",   "number of channels with only one hit");
        desc->AddField<uint32_t>("nstrings",   "",   "number of strings with hit DOMs");
        desc->AddField<uint32_t>("nhit",       "",   "number of hits/pulses/launches in the hit map");
        desc->AddField<double>  ("tot_charge", "pe", "summed charge of all hits/pulses, nan for I3DOMLaunches");
        desc->AddField<double>  ("first_time", "ns", "time of the earliest hit/pulse/launch in the hit map");
        desc->AddField<double>  ("length",     "ns", "time between the latest and the earliest hit/pulse/launch");

        return desc;

    }
    size_t FillRows(const I3Map<OMKey, std::vector<HitType> >& hitmap, I3TableRowPtr rows) {
        
        double first_time = +HUGE_VAL;
        double last_time = -HUGE_VAL;
        double tot_charge = 0.;
        uint32_t nhit = 0;
        uint32_t nchan_1hit = 0;
        std::set<int> strings; 

        typename I3Map<OMKey, std::vector<HitType> >::const_iterator i_map;
        typename std::vector<HitType>::const_iterator i_vec;

        for( i_map = hitmap.begin(); i_map != hitmap.end(); ++i_map) {
            const OMKey& key = i_map->first;
            const typename std::vector<HitType>& vect = i_map->second;
            
            strings.insert(key.GetString());
            if (vect.size() == 1)
                ++nchan_1hit;

            nhit += vect.size();

            for (i_vec = vect.begin(); i_vec != vect.end(); ++i_vec) {
                tot_charge += i_vec->GetCharge();
                double time = i_vec->GetTime();
                if (time < first_time)
                    first_time = time;
                if (time > last_time)
                    last_time = time;

            }
        }

        rows->Set<uint32_t>("nchan", hitmap.size());
        rows->Set<uint32_t>("nchan_1hit", nchan_1hit);
        rows->Set<uint32_t>("nstrings", strings.size());
        rows->Set<uint32_t>("nhit", nhit);
        rows->Set<double>("tot_charge", tot_charge);
        rows->Set<double>("first_time", first_time); 
        rows->Set<double>("length", last_time - first_time);    

        return 1;
    }
};
    
typedef I3EventInfoConverter<I3RecoPulse> I3EventInfoConverterFromRecoPulses;

#endif  // I3EVENTINFOCONVERTER_H_INCLUDED
