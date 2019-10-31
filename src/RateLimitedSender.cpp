#include "RateLimitedSender.h"

#include <iostream>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h>
#include <boost/bind.hpp>

#include "MulticastSender.h"

RateLimitedSender::RateLimitedSender(int packetSize, MulticastSender& dest, int packetsPerInterval, int intervalMillis):
   myPacketsPerInterval(packetsPerInterval),
   myInterval(intervalMillis),
   myDestination(dest),
   myBuffer(packetSize, 0),
   myIoService(),
   myTimer(myIoService, myInterval),
   myCallback(boost::bind(&RateLimitedSender::send, this, boost::asio::placeholders::error)),
   myPacketCount(0),
   mySequence(0),
   mySequenceId(0)
{
   myTimer.async_wait(myCallback);
}

RateLimitedSender::~RateLimitedSender()
{
}

void RateLimitedSender::start()
{
   myIoService.run();
}

void RateLimitedSender::stop()
{
   myIoService.stop();
}

void RateLimitedSender::send(const boost::system::error_code& /*e*/)
{
   for(int i = 0; i < myPacketsPerInterval; ++i)
   {
      // Embed a sequence number in the packet, if it's large enough
      if(myBuffer.size() >= sizeof(mySequence) + sizeof(mySequenceId))
      {
         uint16_t* netSeq(reinterpret_cast<uint16_t*>(myBuffer.data()));
         *netSeq = htons(++mySequence);

         if(mySequence == 0)
         {
            uint16_t* netSeqId(reinterpret_cast<uint16_t*>(myBuffer.data() + sizeof(mySequence)));
            *netSeqId = htons(++mySequenceId);
         }
      }
      myDestination.send(myBuffer);
   }
   myPacketCount += myPacketsPerInterval;

   myTimer.expires_at(myTimer.expires_at() + myInterval);
   myTimer.async_wait(myCallback);

   if(myPacketCount % 50000 == 0)
   {
      struct timeval tp;
      gettimeofday(&tp, NULL);
      unsigned long millis(tp.tv_sec * 1000 + tp.tv_usec / 1000);

      std::cout << "Sent a total of " << myPacketCount << " packets as of: " << millis << std::endl;
   }
}
