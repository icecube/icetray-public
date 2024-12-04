// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause
 
#ifndef TABLEIO_I3MAPICEACTKEYCONVERTER_H_INCLUDED
#define TABLEIO_I3MAPICEACTKEYCONVERTER_H_INCLUDED

#include "icetray/I3Bool.h"
#include "tableio/I3Converter.h"
#include "dataclasses/I3Map.h"
#include "dataclasses/I3String.h"
#include "dataclasses/I3Vector.h"
#include "dataclasses/IceActKey.h"
#include "dataclasses/physics/I3Trigger.h"
#include "dataclasses/physics/I3IceActMaps.h"


class I3IceActTriggerMapConverter : public I3ConverterImplementation<I3IceActTriggerMap>
{
    I3TableRowDescriptionPtr CreateDescription(const I3IceActTriggerMap& map) override
    {
        if (map.size() == 0)
            log_error("Warning:  I3IceActTriggerMap used for converter configuration is EMPTY, "
                      "nothing will be booked");
        I3TableRowDescriptionPtr desc(new I3TableRowDescription());
        desc->AddField<int32_t>("station", "", "Station number");
        desc->AddField<uint32_t>("telescope", "", "Telescope number");
        desc->AddField<double>("time", "ns", "time at which the trigger was issued");
        desc->AddField<double>("length", "ns", "duration of triggered readout window");
        desc->AddField<bool>("fired", "bool", "true, if trigger fired (used for simulations)");
        // TriggerKey
        MAKE_ENUM_VECTOR(sourceID,TriggerKey,TriggerKey::SourceID,TRIGGERKEY_H_TriggerKey_SourceID);
        MAKE_ENUM_VECTOR(typeID,TriggerKey,TriggerKey::TypeID,TRIGGERKEY_H_TriggerKey_TypeID);
        MAKE_ENUM_VECTOR(subtypeID,TriggerKey,TriggerKey::SubtypeID,TRIGGERKEY_H_TriggerKey_SubtypeID);
        desc->AddEnumField<TriggerKey::SourceID>("source_id", sourceID, "", "Enumeration describing what 'subdetector' issued a trigger");
        desc->AddEnumField<TriggerKey::TypeID>("type_id", typeID, "", "Enumeration describing what 'algorithm' issued a trigger");
        desc->AddEnumField<TriggerKey::SubtypeID>("subtype_id", subtypeID, "", "Enumeration describing how a software trigger was originally 'configured' within the TWR DAQ trigger system");
        desc->AddField<int32_t>("config_id", "", "Internal ID of the trigger settings in the DAQ. This can be used to retrieve the threshold, readout window, DOM set, etc from the DetectorStatus.");

        return desc;
    }
    size_t FillRows(const I3IceActTriggerMap& map, I3TableRowPtr rows) override
    {
        //typedef typename I3IceActTriggerMap::const_iterator citer_type;
        I3IceActTriggerMap::const_iterator mapiter = map.begin();
        const size_t maxindex = map.size();
        size_t currentRow;
        for(mapiter = map.begin(), currentRow = rows->GetCurrentRow(); mapiter != map.end(); mapiter++)
        {
            rows->SetCurrentRow(currentRow++);
            rows->Set<int32_t>("station", mapiter->first.GetStationID());
            rows->Set<uint32_t>("telescope", mapiter->first.GetTelescopeID());

            rows->Set<double>("time", mapiter->second.GetTriggerTime());
            rows->Set<double>("length", mapiter->second.GetTriggerLength());
            rows->Set<bool>("fired", mapiter->second.GetTriggerFired());

            const TriggerKey& key = mapiter->second.GetTriggerKey();

            rows->Set<TriggerKey::SourceID> ("source_id",   key.GetSource());
            rows->Set<TriggerKey::TypeID> ("type_id",       key.GetType());
            rows->Set<TriggerKey::SubtypeID> ("subtype_id", key.GetSubtype());
            if (key.CheckConfigID())
                rows->Set<int32_t>("config_id", key.GetConfigID());
        };
        return maxindex;
    }
    size_t GetNumberOfRows(const I3IceActTriggerMap& map) override {
        return map.size();
    }
};

class I3IceActRecoPulseSeriesMapConverter : public I3ConverterImplementation<I3IceActRecoPulseSeriesMap>
{
    size_t getI3IceActRecoPulseSeriesMapLength(const I3IceActRecoPulseSeriesMap &map)
    {
        size_t size = 64;
        BOOST_FOREACH(const I3IceActRecoPulseSeriesMap::value_type &pair, map)
        {
            if (pair.second.size() > size ) size = pair.second.size();
        }
        return size;
    }

    I3TableRowDescriptionPtr CreateDescription(const I3IceActRecoPulseSeriesMap& map)
    {
        if (map.size() == 0)
            log_error("Warning:  I3IceActRecoPulseSeries used for converter configuration is EMPTY, nothing will be booked");

        I3TableRowDescriptionPtr desc(new I3TableRowDescription());

        //Determining the size works on chunks so if Chunk does not include enough pixel table is short.
        size_t mapSize=getI3IceActRecoPulseSeriesMapLength(map);

        if (mapSize == 0)
            log_error("Got some zero length I3IceActRecoPulseSeries.");

        desc->isMultiRow_ = true;

        desc->AddField<int32_t>("station", "", "Station number");
        desc->AddField<uint32_t>("telescope", "", "Telescope number");

        desc->AddField<uint32_t>("pixel", "", "Pixel number",mapSize);
        desc->AddField<double>("time", "ns", "Leading-edge time of the pulse",mapSize);
        desc->AddField<double>("width", "ns", "Duration of the pulse",mapSize);
        desc->AddField<double>("charge", "", "Highest charge in the pulse",mapSize);
        desc->AddField<bool>("existingpulses", "", "Mask for pulses which exist for each event",mapSize);

        return desc;
    }

    size_t FillRows(const I3IceActRecoPulseSeriesMap& map, I3TableRowPtr rows)
    {
        I3IceActRecoPulseSeriesMap::const_iterator mapiter = map.begin();

        const I3TableRowDescription &desc = *rows->GetDescription();
        const size_t rpssize = desc.GetFieldArrayLengths()[desc.GetFieldColumn("pixel")];

        for(mapiter = map.begin();mapiter != map.end();mapiter++)
        {
            if (mapiter->second.size() == 0)
                continue;
            rows->Set<int32_t>("station", mapiter->first.GetStationID());
            rows->Set<uint32_t>("telescope", mapiter->first.GetTelescopeID());

            const std::map<unsigned int, std::vector<I3RecoPulse> >& rps = mapiter->second;
            std::map<unsigned int, std::vector<I3RecoPulse> >::const_iterator rpsiter;

            uint32_t* buffer_pixel = rows->GetPointer<uint32_t>("pixel");
            double* buffer_time    = rows->GetPointer<double>("time");
            double* buffer_width   = rows->GetPointer<double>("width");
            double* buffer_charge  = rows->GetPointer<double>("charge");
            bool*   buffer_exists  = rows->GetPointer<bool>("existingpulses");
            unsigned i = 0;
            for (rpsiter = rps.begin(); i < rpssize && rpsiter != rps.end(); ++i, ++rpsiter++)
            {
                buffer_pixel[i]  = rpsiter->first;
                buffer_time[i]   = rpsiter->second[0].GetTime();
                buffer_width[i]  = rpsiter->second[0].GetWidth();
                buffer_charge[i] = rpsiter->second[0].GetCharge();
                buffer_exists[i] = true;
            }
        };
        return 1;
    }
};

class I3IceActTelescopePixelUnsignedMapConverter : public I3ConverterImplementation<I3IceActTelescopePixelUnsignedMap>
{
    I3TableRowDescriptionPtr CreateDescription(const I3IceActTelescopePixelUnsignedMap& map) override {
        I3TableRowDescriptionPtr desc(new I3TableRowDescription());

        //Determining the size works on chunks so if Chunk does not include enough pixel table is short.
        size_t mapSize=getI3IceActTelescopePixelUnsignedMapLength(map);

        if (mapSize == 0)
            log_error("Got some zero length I3IceActTelescopePixelUnsignedMap.");

        desc->isMultiRow_ = true;

        desc->AddField<int32_t>("station", "", "Station number");
        desc->AddField<uint32_t>("telescope", "", "Telescope number");

        desc->AddField<uint32_t>("pixel", "", "Pixel number",mapSize);
        desc->AddField<unsigned int>("value", "", "Value of the list",mapSize);
        desc->AddField<bool>("existingpulses", "", "Mask for pulses which exist for each event",mapSize);

        if (map.size() == 0)
            log_error("Warning:  I3IceActTelescopePixelUnsignedMap used for converter configuration is EMPTY, nothing will be booked");
        return desc;
    }

    size_t FillRows(const I3IceActTelescopePixelUnsignedMap& map, I3TableRowPtr rows) override {
        I3IceActTelescopePixelUnsignedMap::const_iterator mapiter = map.begin();

        const I3TableRowDescription &desc = *rows->GetDescription();
        const size_t rpssize = desc.GetFieldArrayLengths()[desc.GetFieldColumn("pixel")];
        const size_t maxindex = map.size();
        size_t currentRow;
        for(mapiter = map.begin(), currentRow = rows->GetCurrentRow(); mapiter != map.end(); mapiter++)
        {
            if (mapiter->second.size() == 0)
            continue;
            rows->SetCurrentRow(currentRow++);
            rows->Set<int32_t>("station", mapiter->first.GetStationID());
            rows->Set<uint32_t>("telescope", mapiter->first.GetTelescopeID());

            const std::map<unsigned int, unsigned int >& rps = mapiter->second;
            std::map<unsigned int, unsigned int >::const_iterator rpsiter;

            uint32_t* buffer_pixel = rows->GetPointer<uint32_t>("pixel");
            unsigned int* buffer_value   = rows->GetPointer<unsigned int>("value");
            bool*   buffer_exists  = rows->GetPointer<bool>("existingpulses");
            unsigned i = 0;
            for (rpsiter = rps.begin(); i < rpssize && rpsiter != rps.end(); ++i, ++rpsiter++)
            {
                buffer_pixel[i]  = rpsiter->first;
                buffer_value[i]   = rpsiter->second;
                buffer_exists[i] = true;
            }
        };
        return maxindex;
    }
    size_t GetNumberOfRows(const I3IceActTelescopePixelUnsignedMap& map) override {
        return map.size();
    }
    size_t getI3IceActTelescopePixelUnsignedMapLength(const I3IceActTelescopePixelUnsignedMap &map)
    {
        size_t size = 64;
        BOOST_FOREACH(const I3IceActTelescopePixelUnsignedMap::value_type &pair, map){
            if (pair.second.size() > size ) size = pair.second.size();
        }
        return size;
    }
};

class I3IceActTelescopePixelDoubleMapConverter : public I3ConverterImplementation<I3IceActTelescopePixelDoubleMap>
{
    I3TableRowDescriptionPtr CreateDescription(const I3IceActTelescopePixelDoubleMap& map) override
    {
        I3TableRowDescriptionPtr desc(new I3TableRowDescription());

        //Determining the size works on chunks so if Chunk does not include enough pixel table is short.
        size_t mapSize=getI3IceActTelescopePixelDoubleMapLength(map);

        if (mapSize == 0)
            log_error("Got some zero length I3IceActTelescopePixelDoubleMap.");

        desc->isMultiRow_ = true;

        desc->AddField<int32_t>("station", "", "Station number");
        desc->AddField<uint32_t>("telescope", "", "Telescope number");

        desc->AddField<uint32_t>("pixel", "", "Pixel number",mapSize);
        desc->AddField<double>("value", "", "Value of the list",mapSize);
        desc->AddField<bool>("existingpulses", "", "Mask for pulses which exist for each event",mapSize);

        if (map.size() == 0)
            log_error("Warning:  I3IceActTelescopePixelDoubleMap used for converter configuration is EMPTY, nothing will be booked");
        return desc;
    }

    size_t FillRows(const I3IceActTelescopePixelDoubleMap& map, I3TableRowPtr rows) override
    {
        I3IceActTelescopePixelDoubleMap::const_iterator mapiter = map.begin();

        const I3TableRowDescription &desc = *rows->GetDescription();
        const size_t rpssize = desc.GetFieldArrayLengths()[desc.GetFieldColumn("pixel")];

        size_t currentRow;
        const size_t maxindex = map.size();
        for(mapiter = map.begin(), currentRow = rows->GetCurrentRow(); mapiter != map.end(); mapiter++)
        {
            if (mapiter->second.size() == 0)
            continue;
            rows->SetCurrentRow(currentRow++);
            rows->Set<int32_t>("station", mapiter->first.GetStationID());
            rows->Set<uint32_t>("telescope", mapiter->first.GetTelescopeID());

            const std::map<unsigned int, double >& rps = mapiter->second;
            std::map<unsigned int, double >::const_iterator rpsiter;

            uint32_t* buffer_pixel = rows->GetPointer<uint32_t>("pixel");
            double* buffer_value   = rows->GetPointer<double>("value");
            bool*   buffer_exists  = rows->GetPointer<bool>("existingpulses");
            unsigned i = 0;
            for (rpsiter = rps.begin(); i < rpssize && rpsiter != rps.end(); ++i, ++rpsiter++)
            {
                buffer_pixel[i]  = rpsiter->first;
                buffer_value[i]  = rpsiter->second;
                buffer_exists[i] = true;
            }
        };
        return maxindex;
    }
    size_t GetNumberOfRows(const I3IceActTelescopePixelDoubleMap& map) override {
        return map.size();
    }
    size_t getI3IceActTelescopePixelDoubleMapLength(const I3IceActTelescopePixelDoubleMap &map)
    {
        size_t size = 64;
        BOOST_FOREACH(const I3IceActTelescopePixelDoubleMap::value_type &pair, map){
            if (pair.second.size() > size ) size = pair.second.size();
        }
        return size;
    }
};

class I3IceActTelescopePixelVectorIntMapConverter : public I3ConverterImplementation<I3IceActTelescopePixelVectorIntMap>
{
    I3TableRowDescriptionPtr CreateDescription(const I3IceActTelescopePixelVectorIntMap& map) override {
        I3TableRowDescriptionPtr desc(new I3TableRowDescription());
        //Determining the size works on chunks so if Chunk does not include enough pixel table is short.
        size_t vecSize=40;
        desc->isMultiRow_ = true;
        desc->AddField<int32_t>("station", "", "Station number");
        desc->AddField<uint32_t>("telescope", "", "Telescope number");
        I3MapIntVectorInt::const_iterator citer = map.begin()->second.begin();
        for (citer = map.begin()->second.begin(); citer != map.begin()->second.end(); ++citer)
        {
            desc->AddField<int>(std::to_string(citer->first), "", "",vecSize);
            desc->AddField<bool>("existingvalues_"+std::to_string(citer->first), "", "",vecSize);
        }
        if (map.size() == 0)
            log_error("Warning:  I3IceActTelescopePixelVectorIntMap used for converter configuration is EMPTY, nothing will be booked");
        return desc;
    }
    size_t FillRows(const I3IceActTelescopePixelVectorIntMap& map, I3TableRowPtr rows) override {
        I3IceActTelescopePixelVectorIntMap::const_iterator mapiter = map.begin();
        size_t currentRow;
        const size_t maxindex = map.size();
        for(mapiter = map.begin(), currentRow = rows->GetCurrentRow(); mapiter != map.end(); mapiter++)
        {
            if (mapiter->second.size() == 0)
                continue;
            rows->SetCurrentRow(currentRow++);
            rows->Set<int32_t>("station", mapiter->first.GetStationID());
            rows->Set<uint32_t>("telescope", mapiter->first.GetTelescopeID());
            I3MapIntVectorInt::const_iterator citer = map.begin()->second.begin();
            for (citer = map.begin()->second.begin(); citer != map.begin()->second.end(); ++citer)
            {
                I3VectorInt::const_iterator viter = citer->second.begin();
                int* buffer_vector  = rows->GetPointer<int>(std::to_string(citer->first));
                bool* buffer_exists = rows->GetPointer<bool>("existingvalues_"+std::to_string(citer->first));
                unsigned i = 0;
                for (viter = citer->second.begin (); viter != citer->second.end ();++i, ++viter)
                {
                    buffer_vector[i] = *viter;
                    buffer_exists[i] = true;
                }
            }
        };
        return maxindex;
    };
    size_t GetNumberOfRows(const I3IceActTelescopePixelVectorIntMap& map) override {
        return map.size();
    }
};

class I3IceActTelescopeStringDoubleMapConverter : public I3ConverterImplementation<I3IceActTelescopeStringDoubleMap>
{
    I3TableRowDescriptionPtr CreateDescription(const I3IceActTelescopeStringDoubleMap& map) override {
        I3TableRowDescriptionPtr desc(new I3TableRowDescription());
        desc->isMultiRow_ = true;
        desc->AddField<int32_t>("station", "", "Station number");
        desc->AddField<uint32_t>("telescope", "", "Telescope number");
        I3MapStringDouble::const_iterator citer = map.begin()->second.begin();
        for (citer = map.begin()->second.begin(); citer != map.begin()->second.end(); ++citer)
        {
            desc->AddField<double>(citer->first, "", "");
        }
        if (map.size() == 0)
            log_error("Warning:  I3IceActRecoHillasMap used for converter configuration is EMPTY, nothing will be booked");
        return desc;
    };

    size_t FillRows(const I3IceActTelescopeStringDoubleMap& map, I3TableRowPtr rows) override {
        I3IceActTelescopeStringDoubleMap::const_iterator mapiter = map.begin();
        size_t currentRow;
        const size_t maxindex = map.size();
        for(mapiter = map.begin(), currentRow = rows->GetCurrentRow(); mapiter != map.end(); mapiter++)
        {
            if (mapiter->second.size() == 0)
                continue;
            rows->SetCurrentRow(currentRow++);
            rows->Set<int32_t>("station", mapiter->first.GetStationID());
            rows->Set<uint32_t>("telescope", mapiter->first.GetTelescopeID());
            I3MapStringDouble::const_iterator citer = mapiter->second.begin();
            for (citer = mapiter->second.begin(); citer != mapiter->second.end(); ++citer)
            {
                rows->Set<double>(citer->first, citer->second);
            }
        };
        return maxindex;
    };
    size_t GetNumberOfRows(const I3IceActTelescopeStringDoubleMap& map) override {
        return map.size();
    }
};

template<typename ValueType>
class I3MapIceActKeyConverter : public I3ConverterImplementation<I3Map<IceActKey, ValueType> >
{
    I3TableRowDescriptionPtr CreateDescription(const I3Map<IceActKey, ValueType>& map)
    {
        if (map.size() == 0)
            log_error("Warning:  I3Map<IceActKey, ...> used for converter configuration is EMPTY, "
                      "nothing will be booked");
        I3TableRowDescriptionPtr desc(new I3TableRowDescription());
        desc->AddField<int32_t>("station", "", "Station number");
        desc->AddField<uint32_t>("telescope", "", "Telescope number");
        desc->AddField<ValueType>("value", "", "");

        return desc;
    }

    size_t FillRows(const I3Map<IceActKey, ValueType>& map, I3TableRowPtr rows)
    {
        typedef typename I3Map<IceActKey, ValueType>::const_iterator citer_type;
        for(citer_type mapiter = map.begin(); mapiter != map.end(); mapiter++)
        {
            rows->Set<int32_t>("station", mapiter->first.GetStationID());
            rows->Set<uint32_t>("telescope", mapiter->first.GetTelescopeID());
            rows->Set<ValueType>("value", mapiter->second);
        };
        return 1;
    }
};

typedef I3MapIceActKeyConverter<double>        I3IceActDoubleMapConverter;
typedef I3MapIceActKeyConverter<int32_t>       I3IceActIntMapConverter;
typedef I3MapIceActKeyConverter<uint32_t>      I3IceActUnsignedMapConverter;
typedef I3MapIceActKeyConverter<bool>          I3IceActBoolMapConverter;

#endif // TABLEIO_I3MAPICEACTKEYCONVERTER_H_INCLUDED
