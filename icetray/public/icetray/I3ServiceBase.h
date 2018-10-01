#ifndef I3SERVICEBASE_H_INCLUDED
#define I3SERVICEBASE_H_INCLUDED

#include "icetray/I3Context.h"
#include "icetray/I3Configuration.h"

/**
 * @class I3ServiceBase
 * @brief Auxiliary base class for service classes
 *
 * When you derive your service classes from this base class then you can
 * configure them in exactly the same way as modules. You make a
 * factory class by doing a template instantiation of I3SingleServiceFactory,
 * e.g.:
 *
 * typedef
 * I3SingleServiceFactory<I3EventLogLikelihoodCombiner,I3EventLogLikelihoodBase>
 * I3EventLogLikelihoodCombinerFactory;
 * I3_SERVICE_FACTORY( I3EventLogLikelihoodCombinerFactory );
 *
 * It is recommended that you provide at least two constructors for your
 * service class: one which takes only a context and adds parameters
 * and uses the "factory" I3ServiceBase constructor, and another one
 * (using the "dummy" I3ServiceBase constructor) allows instantiating a
 * service object such that all configurable parameters are directly set
 * to specific values. The former is for regular icetray usage through
 * python scripts, the latter makes it easier to write unit tests.
 *
 * @sa I3ServiceFactory
 */

class I3ServiceBase {

    public:

        /**
         * Factory constructor
         *
         * Use this base class constructor to implement the derived class
         * constructor which calls AddParameter() to define configuration
         * parameters, just like an I3Module constructor.
         */
        I3ServiceBase(const I3Context &c):
            context_(c),
            configuration_(new I3Configuration), // deleted externally
            dummyContext_(false){}

        /**
         * Dummy constructor
         *
         * Use this base class constructor *only* to implement derived
         * class constructors which configure the service class *without*
         * using an I3Context. This is useful for unit tests.
         */
        I3ServiceBase(const std::string &name):
            context_(*(new I3Context)),
	    configuration_(new I3Configuration),
            dummyContext_(true){
		configuration_->InstanceName(name);
	}

        /// destructor
        virtual ~I3ServiceBase(){
            if ( dummyContext_ ) {
                delete &context_;
                delete configuration_;
            }
        }

        /**
         * Configure (get & check parameters)
         * This method should only be called if this object was created
         * with the "factory constructor", so that it has a usable
         * I3Context.
         */
        virtual void Configure() {}

        /// name (for log messages)
        const std::string GetName() const {
            return configuration_->InstanceName();
        }

    protected:

        /**
         * Convenience method: this allows defining a parameter
         * in the same way as in an I3Module.
         */
        template<typename ParamType>
        void AddParameter( const std::string &name,
                           const std::string &description,
                           const ParamType   &defval ){
            if ( dummyContext_ ){
                log_fatal( "(%s) object created with wrong constructor",
                           configuration_->InstanceName().c_str() );
            }
            configuration_->Add(name,description,defval);
        }

        /**
         * Convenience method: this allows retrieving a parameter
         * in the same way as in an I3Module.
         */
        template <typename T>
        typename boost::disable_if<boost::is_const<T>, void>::type
        GetParameter(const std::string& name, T& value) const
        {
            if ( dummyContext_ ){
                log_fatal( "(%s) object created with wrong constructor",
                           configuration_->InstanceName().c_str() );
            }
            try {
                value = configuration_->Get<T>(name);
            } catch (...) {
                try {
                    std::string context_name =
                      configuration_->Get<std::string>(name);
                    value = context_.Get<T>(context_name);
                    // NB: we got here by catching an error thrown by
                    // boost::python::extract(). All subsequent calls will fail
                    // unless we clean it up.
                    PyErr_Clear();
                } catch (...) {
                    log_error("Error in %s module '%s', getting parameter '%s'",
	                 I3::name_of(typeid(*this)).c_str(), GetName().c_str(),
                         name.c_str());
                    throw;
                }
           } 
        }

        const I3Configuration& GetConfiguration() const { return *configuration_; }
        const I3Context& GetContext() const { return context_; }

    protected:
        const I3Context& context_;
	I3Configuration *configuration_;

    private:
	template<class A, class B> friend class I3SingleServiceFactory;

        bool dummyContext_;
        I3ServiceBase();
        I3ServiceBase(const I3ServiceBase&);
        I3ServiceBase& operator=(const I3ServiceBase&);

};

#endif /* I3SERVICEBASE_H_INCLUDED */
