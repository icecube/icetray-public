#ifndef I3SINGLESERVICEFACTORY_H_INCLUDED
#define I3SINGLESERVICEFACTORY_H_INCLUDED

/*
 * @file
 *
 * Version $Id$
 * $Date$
 *
 * (c) IceCube Collaboration 2007
 */

#include <icetray/I3ServiceFactory.h>
#include <string>

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
          I3ServiceFactory(context) {
            myService_ = boost::shared_ptr<MyService>(new MyService(context));
            // Synchronize the two configurations via the back door
            myService_->ReplaceConfiguration(configuration_);
        }

        /// Let the datamember instance of the service configure itself.
        void Configure(){
            myService_->Configure();
        }

        ///  install the service object in a given context
        bool InstallService( I3Context &context ){
            if (!myService_){
                log_fatal( "Failed to create %s service object",
                  GetName().c_str());
            }
            log_debug("adding service %s to the context", GetName().c_str() );

            // do it!
            bool success =
                context.template Put<ItsBaseClass>( myService_,
                  GetName().c_str());
            if (!success){
                log_fatal( "FAILED to add %s to context (!?)",
                           GetName().c_str() );
            }

            return success;
        }

    protected:
        /// the instance of the service (one per factory object)
        boost::shared_ptr<MyService> myService_;

    private:
        /// logging configuration thingy
        SET_LOGGER("I3SingleServiceFactory");
};

#endif /* I3SINGLESERVICEFACTORY_H_INCLUDED */
