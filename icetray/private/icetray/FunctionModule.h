#ifndef FUNCTION_MODULE_H
#define FUNCTION_MODULE_H

#include <set>
#include <icetray/I3ConditionalModule.h>
#include <boost/function.hpp>

/**
 * A wrapper class which turns a callable object into an I3Module
 * in the same manner as PythonFunction. The wrapped callable may
 * return nothing or a boolean, and in the latter case the processed
 * frame is dropped if the result is false.
 */
class FunctionModule : public I3ConditionalModule{
public:
	FunctionModule(const I3Context& context,
	               boost::function<void(boost::shared_ptr<I3Frame>)>);
	FunctionModule(const I3Context& context,
	               boost::function<bool(boost::shared_ptr<I3Frame>)>);
	virtual ~FunctionModule(){}
	virtual void Configure();
	virtual void Process();
private:
	///Whether the wrapped function returns a boolean flag
	bool funcReturns;
	///The wrapped function if it returns void
	boost::function<void(boost::shared_ptr<I3Frame>)> vf;
	///The wrapped function if it returns bool
	boost::function<bool(boost::shared_ptr<I3Frame>)> bf;
	///The streams on which this module should run
	std::set<I3Frame::Stream> streams;
	
	SET_LOGGER("FunctionModule");
};

#endif

