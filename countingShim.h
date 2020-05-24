#ifndef EMANEMODEL_FIRSTSHIM_COUNTINGSHIM_HEADER  
#define EMANEMODEL_FIRSTSHIM_COUNTINGSHIM_HEADER  
#include "emane/shimlayerimpl.h" 
#include "countingService.h"
#include <thread>


namespace EMANE
{
    namespace Transports
    {
        namespace COUNTING
        {
            // remember to include emane/shimlayerimpl for ShimLayerImplementor
            class CountingShim : public ShimLayerImplementor
            {
            public:
                // check what needs to be included for NEM and the rest to be recognized
                CountingShim(NEMId id,
                        PlatformServiceProvider * platformService,
                        RadioServiceProvider * pRadioServiceprovider);

                ~CountingShim() override;

                void initialize(Registrar & registar) override;

                void configure(const ConfigurationUpdate & update) override;

                void start() override ;

		void postStart() override;

                void stop() override ;

                void destroy() throw() override;

                void processUpstreamControl(const ControlMessages & msgs) override;

                void processDownstreamControl(const ControlMessages & msgs) override;

                void processUpstreamPacket(UpstreamPacket & pkt,
                                               const ControlMessages & msgs) override;

                void processDownstreamPacket(DownstreamPacket & pkt,
                                             const ControlMessages & msgs) override;

            private:
            CountingService countingService_;

            INETAddr address_;

	    //INETAddr mask_;

	    CountingService * countingS_;

	    void readDevice();

	    std::thread thread_;

	    std::string sDeviceName_;

	    std::string sDevicePath_;

//	    INETAddr addressRedis_;

//	    std::uint64_t timeToUpdateRedis_;

            };
        }
    }
}

#endif
