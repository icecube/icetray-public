#ifndef FUNCTION_MODULE_H
#define FUNCTION_MODULE_H

#include <set>
#include <functional>
#include <icetray/I3ConditionalModule.h>

/**
 * A wrapper class which turns a callable object into an I3Module.
 * The wrapped callable may return nothing or a boolean, and in the latter case
 * the processed frame is dropped if the result is false.
 */
class FunctionModule : public I3ConditionalModule{
public:
	FunctionModule(const I3Context& context,
	               std::function<void(boost::shared_ptr<I3Frame>)>);
	FunctionModule(const I3Context& context,
	               std::function<bool(boost::shared_ptr<I3Frame>)>);
	virtual void Configure();
	virtual void Process();
private:
	FunctionModule(const I3Context& context);
	///The wrapped function
	std::function<void(boost::shared_ptr<I3Frame>)> func;
	///The streams on which this module should run
	std::set<I3Frame::Stream> streams;
};

#endif

