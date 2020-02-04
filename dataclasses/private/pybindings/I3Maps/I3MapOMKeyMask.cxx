
#include <boost/foreach.hpp>
#include <dataclasses/I3MapOMKeyMask.h>
#include <icetray/python/dataclass_suite.hpp>
#include <icetray/python/function.hpp>

namespace bp = boost::python;

I3RecoPulseSeriesMapPtr
underhanded_apply(const I3RecoPulseSeriesMapMask& mask, const I3Frame &frame)
{
	return boost::const_pointer_cast<I3RecoPulseSeriesMap>(mask.Apply(frame));
}

bp::list
getbits(const I3RecoPulseSeriesMapMask &mask)
{
	bp::list usermask;
	BOOST_FOREACH(const boost::dynamic_bitset<uint8_t> &bits, mask.GetBits()) {
		bp::list elements;
		for (size_t i = 0; i < bits.size(); i++)
			elements.append(bits.test(i));
		usermask.append(elements);
	}
	
	return usermask;
}

I3RecoPulseSeriesMapMaskPtr
from_callable(const I3Frame &frame, const std::string &key, bp::object callable)
{
	typedef boost::function<bool (const OMKey&, size_t, const I3RecoPulse&)> callback_t;
	boost::shared_ptr<callback_t> predicate = bp::detail::function_from_object<callback_t>(callable);
	return I3RecoPulseSeriesMapMaskPtr(new I3RecoPulseSeriesMapMask(frame, key, *predicate));
}

void register_I3RecoPulseSeriesMapMask()
{
	void (I3RecoPulseSeriesMapMask::*set_om_all)(const OMKey&, bool) = &I3RecoPulseSeriesMapMask::Set;
	void (I3RecoPulseSeriesMapMask::*set_om_by_idx)(const OMKey&, const unsigned, bool) = &I3RecoPulseSeriesMapMask::Set;
	void (I3RecoPulseSeriesMapMask::*set_om_by_value)(const OMKey&, const I3RecoPulse&, bool) = &I3RecoPulseSeriesMapMask::Set;
	
	typedef boost::function<bool (const OMKey&, size_t, const I3RecoPulse&)> callback_t;
	
	bp::scope mask_scope =
	bp::class_<I3RecoPulseSeriesMapMask, bp::bases<I3FrameObject>,
	    I3RecoPulseSeriesMapMaskPtr>("I3RecoPulseSeriesMapMask",
	    bp::init<const I3Frame&, const std::string &>(bp::args("frame", "key")))
		.def("__init__", bp::make_constructor(&from_callable))
		.def(bp::init<const I3Frame&, const std::string &, const I3RecoPulseSeriesMap &>())
		.def(bp::init<const I3Frame&, const std::string &, callback_t>())
		.add_property("source", &I3RecoPulseSeriesMapMask::GetSource)
		.add_property("bits", &getbits)
#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif
		.def(bp::self &  bp::self)
		.def(bp::self &= bp::self)
		.def(bp::self |  bp::self)
		.def(bp::self |= bp::self)
		.def(bp::self ^  bp::self)
		.def(bp::self ^= bp::self)
#ifdef __clang__
#pragma GCC diagnostic pop
#endif              
		.def("remove", &I3RecoPulseSeriesMapMask::Remove)
		.def("has_ancestor", &I3RecoPulseSeriesMapMask::HasAncestor)
		.def("repoint", &I3RecoPulseSeriesMapMask::Repoint)
		.def("apply", &underhanded_apply, "Apply the mask to an I3Frame, returning an I3RecoPulseSeries.")
		.def("any", &I3RecoPulseSeriesMapMask::GetAnySet, "Are any of the bits set in the mask?")
		.def("all", &I3RecoPulseSeriesMapMask::GetAllSet, "Are all of the bits set in the mask?")
		.def("sum", &I3RecoPulseSeriesMapMask::GetSum, "Get the number of set bits in the mask.")
		.def("set", set_om_all, "Set/unset all bits for this OMKey.")
		.def("set", set_om_by_idx, "Set/unset the bit at this index.")
		.def("set", set_om_by_value, "Set/unset the bit corresponding to this RecoPulse.")
		.def("set_none", &I3RecoPulseSeriesMapMask::SetNone, "Unset all bits in the mask.")
		.def(bp::dataclass_suite<I3RecoPulseSeriesMapMask>())
	;
	
	bp::def_function<callback_t>("Predicate");
	
	register_pointer_conversions<I3RecoPulseSeriesMapMask>();
}
