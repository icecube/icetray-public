#ifndef I3SINGLESERVICEFACTORY_H_INCLUDED
#define I3SINGLESERVICEFACTORY_H_INCLUDED

/*
 * @file
 *
 * Version $Id: I3SingleServiceFactory.h 36270 2007-08-31 21:20:32Z boersma $
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
 * @class I3SingleServiceFactory
 * @author boersma (after an idea by juancarlo)
 *
 * This is a generic factory for services which do not need individual
 * copies for all "users". That is, only one service object gets created
 * per factory object, and it gets put into all contexts.  So it is still
 * possible to have two different service objects of the same class but
 * with different configurations; just define two separate factories
 * with the different configuration parameters in your python script.
 * 
 * The service objects are made available using the second argument of
 * the "AddService" call. That is, with the following python lines you'd
 * create two differently configured service objects, which modules can
 * retrieve and use via the names "room101" and "room102", respectively.
 * 
 * tray.AddService("I3SingleServiceFactory<I3RoomService>","room101")(
 *     ("FridgeContents",["vodka","caviar"]),
 * )
 * tray.AddService("I3SingleServiceFactory<I3RoomService>","room102")(
 *     ("FridgeContents",["yoghurt","granola"]),
 * )
 *
 * Since there is only one service object created per factory, any
 * action by a module which changes the state of the service object
 * has consequences for another module which uses the same service
 * object. Say, if one module empties the fridge, then the next module
 * will find it empty. If each module should have full control over the
 * service object as configured by the factory, then you should produce
 * it with a I3MultiServiceFactory.
 *
 * To use this factory for a specific service, the service class needs
 * to implement at least an I3Context-using constructor and a Configure()
 * method.
 *
 * MyService::MyService(I3Context&);
 * void MyService::Configure(I3Context&);
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
 * argument.
 *
 * The implementation file of the service class (say "I3RoomSercice") should
 * also contain a line of the form:
 * I3_SERVICE_FACTORY(I3SingleServiceFactory<I3RoomService>)
 *
 * @sa I3MultiServiceFactory, I3ServiceFactory, I3Context, I3Configuration
 */


template<class MyService,class ItsBaseClass=MyService>
class I3SingleServiceFactory : public I3ServiceFactory {
    public:

        /// standard factory constructor with context
        I3SingleServiceFactory( const I3Context &context ):
          I3ServiceFactory(context),
          name_( context.template Get<I3Configuration>().InstanceName() ),
          myService_( new MyService(context_) ){
        }

        /// Let the datamember instance of the service configure itself.
        void Configure(){
            myService_->Configure();
        }

        ///  install the service object in a given context
        bool InstallService( I3Context &context ){
            if (!myService_){
                log_fatal( "Failed to create %s service object",
                           name_.c_str());
            }
            // here it's different from I3SingleServiceFactory:
            // for every context we use the *same* service object
            log_debug("adding service %s to the context",
                      name_.c_str() );

            // do it!
            bool success =
                context.template Put<ItsBaseClass>( myService_, name_ );
            if (!success){
                log_fatal( "FAILED to add %s to context (!?)",
                           name_.c_str());
            }

            return success;
        }
    private:

        /// name, used for logging message
        std::string name_;

        /// the instance of the service (one per factory object)
        boost::shared_ptr<MyService> myService_;

        /// logging configuration thingy
        SET_LOGGER("I3SingleServiceFactory");
};

#endif /* I3SINGLESERVICEFACTORY_H_INCLUDED */
