#ifndef _MulticastSender_h
#define _MulticastSender_h

#include <vector>
#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>

class MulticastSender
{
public:
   MulticastSender(const boost::asio::ip::address& addr, const unsigned short port, const std::string& bindIp = "127.0.0.1");
   virtual ~MulticastSender();

   void send(const std::vector<unsigned char>& buffer);

private:
   boost::asio::ip::udp::endpoint myEndpoint;
   boost::asio::io_service myService;
   boost::scoped_ptr<boost::asio::ip::udp::socket> mySocket;
};

#endif
