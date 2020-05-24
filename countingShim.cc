#include "emane/controls/serializedcontrolmessage.h"
#include "countingShim.h"
#include "emane/controls/flowcontrolcontrolmessage.h"
#include "emane/startexception.h"
#include "emane/downstreamtransport.h"
#include "emane/downstreampacket.h"

//Needed to run different threads that can run together as shims
// are each a thread and they are deep cloned in order to change the
// parameters effectively
namespace
{
   const char* __MODULE__ ="FirstShim::countingShim";
   EMANE::ControlMessages clone(const EMANE::ControlMessages & msgs)
   {
    EMANE::ControlMessages clones;
    for(const auto & msg : msgs)
        {
            clones.push_back(msg->clone());
        }
    return clones;
   }
}

// constructor inheriting from the shimlayerImplemetor defining the
// base format of a shim
EMANE::Transports::COUNTING::CountingShim::CountingShim(NEMId id,
                                                        PlatformServiceProvider * pPlatformService,
                                                        RadioServiceProvider * pRadioService):
                      ShimLayerImplementor{id, pPlatformService, pRadioService},
                      countingService_{id, pPlatformService, pRadioService}
{}

// Default destructor
EMANE::Transports::COUNTING::CountingShim::~CountingShim()
= default;

//initializing the shim layer as defined in shimlayerimplementor
void EMANE::Transports::COUNTING::CountingShim::initialize(Registrar & registrar)
{
 LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                         DEBUG_LEVEL,
                         "SHIMI %03hu %s::%s", id_, __MODULE__, __func__);

 // Checking the first item is indeed the address
 try
 {
     auto & configRegistrar = registrar.configurationRegistrar();

     configRegistrar.registerNonNumeric<INETAddr>("address",
             ConfigurationProperties::NONE,
                                               {},
                                               "IPv4 or IPv6 virtual device address");
    /*configRegistrar.registerNonNumeric<INETAddr>("addressRedis",
	ConfigurationProperties::NONE,
	{},
	"Redis address");
	configRegistrar.registerNumeric<std::uint64_t>("kps",
		ConfigurationProperties::NONE,
		{},
		"Send trap for over 100 Kps stream");*/

// registering the bit rate
	configRegistrar.registerNumeric<float>("bitrate",
             ConfigurationProperties::DEFAULT,
                                               {0.0},
		"bit rate of data");

//function for map items
   const EMANE::ConfigParameterMapType & countingConfiguration{countingService_.getConfigItems()};
//checking the counting library for items
for(auto & item : countingConfiguration)
{
	auto value = item.second.value;

	LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
        	             DEBUG_LEVEL,
                	     "SHIMI %03hu %s::%s item: %s, value: %s, description: %s",
                     	     id_, __MODULE__, __func__,
			     item.first.c_str(),
			     value.c_str(),
			     item.second.description.c_str());

	configRegistrar.registerNonNumeric<std::string>(item.first,
				ConfigurationProperties::DEFAULT,
				{{value}},
				item.second.description);


}

 }
 // Throwing proper exception in case it isn't
 catch (const EMANE::Exception & ex)
 {
     LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                     ERROR_LEVEL,
                     "SHIMI %03hu %s::%s caught %s",
                     id_, __MODULE__, __func__,
                     ex.what());
 }
}

// Configuring the shim layer based on the definition of the shimlayerimplementor
void EMANE::Transports::COUNTING::CountingShim::configure(const ConfigurationUpdate & update)
{
    LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                            "SHIMI %03hu %s::%s", id_, __MODULE__, __func__);
    for(const auto & item : update)
    {
        if(item.first == "address")
        {
            address_ = item.second[0].asINETAddr();
            LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                                    INFO_LEVEL,
                                    "SHIMI %03hu %s::%s %s=%s",
                                    id_,
                                    __MODULE__,
                                    __func__,
                                    item.first.c_str(),
                                    address_.str(false).c_str());
        }

    }


// Calling the configure function of the service
    countingService_.configure(update);

    INETAddr addr_=countingService_.get_addr();

    INETAddr mas_=countingService_.get_mask();

   LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                            "SHIMI %03hu %s::%s configure address", id_, __MODULE__, __func__);


    countingService_.set_addr(update);

    //thread_ = std::thread{&VirtualTransport::readDevice,this};

	if(!address_.isAny())
	{
		if(countingS_->set_addr(update)<0)
		{
		std::stringstream ssDescription;
		ssDescription << "could not set address "<<addr_.str();
		ssDescription << "mask " << mas_.str()
		<<std::ends;
		throw StartException(ssDescription.str());
		}
	}


}


// Starting the shimlayer implemtation
void EMANE::Transports::COUNTING::CountingShim::start()
{
    LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                            "SHIMI %03hu %s::%s", id_, __MODULE__, __func__);

// Calling the service starting function
    countingService_.start();

}

void EMANE::Transports::COUNTING::CountingShim::postStart()
{
    LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                            "SHIMI %03hu %s::%s", id_, __MODULE__, __func__);


   countingService_.postStart();
}

// Stoppping the implementation
void EMANE::Transports::COUNTING::CountingShim::stop()
{
    LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                            "SHIMI %03hu %s::%s", id_, __MODULE__, __func__);
}

// Destroying the stopped implementation
void EMANE::Transports::COUNTING::CountingShim::destroy() throw()

    {
        LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                                DEBUG_LEVEL,
                                "SHIMI %03hu %s::%s", id_, __MODULE__, __func__);
    }


// Proccessing downstream control messages with the proper function
void EMANE::Transports::COUNTING::CountingShim::processDownstreamControl(const ControlMessages &msgs)
{
    LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                            "SHIMI %03hu %s::%s pass thru %zu msgs",
                            id_, __MODULE__, __func__, msgs.size());
    sendDownstreamControl(clone(msgs));
}

//Proccessing downstream data packets with the proper function,
// also adding the counting function defined in the service
void EMANE::Transports::COUNTING::CountingShim::processDownstreamPacket(DownstreamPacket &pkt, const ControlMessages &msgs)
{
    LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                            "SHIMI %03hu %s::%s pass thru %zu msgs with size %zu",
                            id_, __MODULE__, __func__, msgs.size(),pkt.length());
    sendDownstreamPacket(pkt, clone(msgs));

    // Here I'm calling the function which counts the packets going downstream
    // as required
    countingService_.countDataMessages(pkt);

    // calling the trap
 //   countingService_.send_trap(pkt);

}
// Proccessing upstream control messages with the proper function
void EMANE::Transports::COUNTING::CountingShim::processUpstreamControl(const ControlMessages &msgs)
{
    LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                            "SHIMI %03hu %s::%s pass thru %zu msgs",
                            id_, __MODULE__, __func__, msgs.size());
    sendUpstreamControl(clone(msgs));
}
//Proccessing downstream data packets with the proper function
void EMANE::Transports::COUNTING::CountingShim::processUpstreamPacket(UpstreamPacket &pkt, const ControlMessages &msgs)
{
    LOGGER_STANDARD_LOGGING(pPlatformService_->logService(),
                            DEBUG_LEVEL,
                            "SHIMI %03hu %s::%s pass thru pkt len %zu and %zu msgs",
                            id_, __MODULE__, __func__, pkt.length(), msgs.size());

    sendUpstreamPacket(pkt, clone(msgs));
}

DECLARE_SHIM_LAYER(EMANE::Transports::COUNTING::CountingShim);
