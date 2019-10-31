#ifndef _RateLimitedSender_h_
#define _RateLimitedSender_h_

#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

class MulticastSender;

class RateLimitedSender
{
public:
   RateLimitedSender(int packetSize, MulticastSender& dest, int packetsPerInterval=24, int intervalMillis=1);
   virtual ~RateLimitedSender();

   void start();
   void stop();

protected:
   void send(const boost::system::error_code&);

private:
   int myPacketsPerInterval;
   boost::posix_time::milliseconds myInterval; 
   MulticastSender& myDestination;

   std::vector<unsigned char> myBuffer;

   boost::asio::io_service myIoService;
   boost::asio::deadline_timer myTimer;

   boost::function<void (const boost::system::error_code&)> myCallback;

   unsigned long long myPacketCount;
   uint16_t mySequence;
   uint16_t mySequenceId;
};
#endif
