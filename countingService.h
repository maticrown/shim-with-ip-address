//
// Created by DELL on 3/25/2020.
//

#ifndef FIRSTSHIM_COUNTINGSERVICE_H
#define FIRSTSHIM_COUNTINGSERVICE_H

#include "emane/shimlayerimpl.h"
#include "emane/utils/netutils.h"

#include "hiredis.h"

#include "emane/platformserviceprovider.h"
#include "emane/controls/r2rineighbormetriccontrolmessage.h"
#include "emane/controls/r2riqueuemetriccontrolmessage.h"
#include "emane/controls/r2riselfmetriccontrolmessage.h"
#include "emane/controls/flowcontrolcontrolmessage.h"

#include "emane/inetaddr.h"
#include "emane/radioserviceprovider.h"

#include <pcap/pcap.h>
#include <sstream>
#include <map>

#include <chrono>

#include <map>
#include <vector>
#include "showIP.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_tun.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define NETWORK_DEVICE_PATH "/dev/net/net"

namespace EMANE
{

struct ConfigParameterInfo
	{
// These are used later in the map
	const std::string type;
	std::string value;
	const std::string description;
	std::vector<unsigned int> to_iv() const
	{
		std::vector<unsigned int> vui;

		unsigned int ui{};
		std::stringstream ss(value);

		while(ss >> ui)
		{
			vui.push_back(ui);

			if(ss.peek()==',')
			ss.ignore();
		}
			if(!ss.eof())
			{
			throw std::invalid_argument(value);
			}
	return vui;
	}
	};

using ConfigParameterMapType = std::map<std::string, ConfigParameterInfo>;

    class CountingService
    {
    public:
        CountingService(NEMId id,
                        PlatformServiceProvider *pPlatformService,
                        RadioServiceProvider *pRadioService);

        ~CountingService();

        void configure(const ConfigurationUpdate & update);

        void start();

	void destroy();

        bool countDataMessages(DownstreamPacket & pkt);

	const ConfigParameterMapType & getConfigItems() const;

	char* getS();

	void postStart();

	int set_addr(const ConfigurationUpdate &update);
       // bool send_trap(DownstreamPacket & pkt);

	INETAddr & get_addr();

	INETAddr & get_mask();

	int open(const char*,const char*);

//	int close();

	int writev(const struct iovec*, size_t);

	int readv(struct iovec*, size_t);

	int get_handle();


    private:

	INETAddr address_;

//	INETAddr mask_;

        NEMId id_;

        PlatformServiceProvider * pPlatformService_;

        RadioServiceProvider * pRadioService_;

	ShowIP s;

        int count=0;

	int inet_addr_;

	int                   Handle_;
        std::string           Name_;
        std::string           Path_;
        std::string           Guid_;
        int                   Index_;
        INETAddr              Addr_;
        INETAddr              Mask_;

       /* redisContext *c;
        struct oid {
        	std::string oidName;
		std::string oidValue;
	};*/

// to store the oids in the cache
/*	std::list<oid> listOids ;
	std::uint64_t start_timer;
*/
// the specified interval period to update redis
//	std::uint64_t timeToUpdateRedis_;
//	INETAddr addressRedis_;

  // std::uint64_t global_counter_for_trap_sent;

// defining the time stamp for the trap
   //std::uint64_t time_stamp;

// for counting the Kps
  // std::uint64_t kps_;

   std::string interface_;

   char* addressV_;

   ConfigParameterMapType countingConfiguration_;

   float u64BitRate_;

  // void updateRedis();

 //  void deleteOidFromList(const char* key);

//!!!!!!!check what parmeters are needed

    };
}




#endif //FIRSTSHIM_COUNTINGSERVICE_H
