#ifndef I3MULTISERVICEFACTORY_H_INCLUDED
#define I3MULTISERVICEFACTORY_H_INCLUDED

/*
 * @file
 *
 * Version $Id: I3MultiServiceFactory.h 36270 2007-08-31 21:20:32Z boersma $
 * $Date: 2007-08-31 16:20:32 -0500 (Fri, 31 Aug 2007) $
 *
 * This utility not directly essential to Gulliver. If it is deemed
 * useful for other projects, then this should be incorporated into
 * e.g. icetray or into some separate project.
 *
 * (c) IceCube Collaboration 2007
 */

#include <string>
#include "icetray/I3ServiceFactory.h"

/**
 * @class I3MultiServiceFactory
 * @author boersma (after an idea by juancarlo)
 *
 * This is a generic factory for services which need individual copies for
 * all "users", to allow those users to change the internal state without
 * affecting the behavior of the service object for other modules. That
 * is, a new service object gets created for every context (roughly:
 * one for every service and every module added to the tray), and it gets
 * put into all contexts.  You need to watch out with this if a service
 * allocates a lot of memory. E.g. if it allocates 100Mb, and your python
 * scripts adds 10 services/modules, then in total a gigabyte of memory
 * will be consumed by just creating service objects for each context.
 * 
 * The service objects are made available using the second argument of
 * the "AddService" call. That is, with the following python line you'd
 * create service objects, which modules can
 * retrieve their copy from their context (and use it) via the name "room101":
 * 
 * tray.AddService("I3MultiServiceFactory<I3RoomService>","myroom")(
 *     ("FridgeContents",["rum","tequila"]),
 * )
 *
 * Since there is one service object created per context, any actions
 * by a module which changes the state of the service object has no
 * consequences for another module which uses a service object from the
 * same factory. Say, if one module empties the fridge of its "myroom",
 * then the next module will find the fridge in its own "myroom" still as
 * full as the factory delivered it. If the modules can actually share
 * the same service object from a particular factory with a particular
 * configuration, without interfering with eachother, then you should
 * produce it with a I3SingleServiceFactory, because that is then more
 * efficient.
 *
 * To use this factory for some specific service, a service class MyService
 * needs to implement these methods:
 *
 * MyService(I3Context&); // constructor, declare config params
 * void Configure();      // get config params from context
 *
 * To facilitate adding and getting parameters to/from the context, you
 * can derive your class from I3ServiceBase, but that's not strictly
 * necessary. The base class defines a few convenience methods. If
 * the service is not happy with what it gets from the configuration,
 * it should call log_fatal during Configure().
 *
 * For the instantiation of the factory class you provide two template
 * parameters. The first should be the service class itself, it is used to
 * to instantiate the service object(s). Many service classes are derived
 * of some other service base classes, e.g. I3EventLogLikelihoodBase. If
 * the service factory should Put() the service object pointer into a
 * context as a pointer-to-base, then you can use the second template
 * argument to specify that base class. The second template argument
 * can also be omitted, by default it is identical to the first template
 *
 * The implementation file of the service class (say "I3RoomSercice") should
 * also contain a line of the form:
 *
 * I3_SERVICE_FACTORY(I3MultiServiceFactory<I3RoomService>)
 *
 * @sa I3SingleServiceFactory, I3ServiceFactory, I3Context, I3Configuration
 */

template<class MyService,class ItsBaseClass=MyService>
class I3MultiServiceFactory : public I3ServiceFactory {
    public:

        /// standard factory constructor with context
        I3MultiServiceFactory( const I3Context &context ):
          I3ServiceFactory(context),
          name_( context.template Get<I3Configuration>().InstanceName() ),
          myService_( new MyService(context_) ){ }

        /**
         * Let the datamember instance of the service configure itself.
         * (This instance is not put into an actual context, it is basically a
         * test of the configuration specified by the user in the tray.)
         */
        void Configure(){
            myService_->Configure();
        }

        ///  install a new instance of the service in a given context
        bool InstallService( I3Context &context ){

            // here it's different from I3SingleServiceFactory:
            // for every context we create a new service object
            myService_ = boost::shared_ptr<MyService>(new MyService(context));

            // this can be noisy
            myService_->Configure();

            // do it!
            bool success = context.template Put<ItsBaseClass>( myService_, name_ );
            if (!success){
                log_fatal( "FAILED to add %s to context (!?)",
                           name_.c_str());
            }

            return success;
        }

    private:

        /// name, used for logging message
        std::string name_;

        /// instance of the service used to test the configuration
        boost::shared_ptr<MyService> myService_;

        /// logging configuration thingy
        SET_LOGGER( "I3MultiServiceFactory" );
};

#endif /* I3MULTISERVICEFACTORY_H_INCLUDED */
