#include "MulticastSender.h"

#include <iostream>
#include <vector>

MulticastSender::MulticastSender(const boost::asio::ip::address& addr, const unsigned short port, const std::string& bindIp):
   myEndpoint(addr, port),
   myService(),
   mySocket(new boost::asio::ip::udp::socket(myService, myEndpoint.protocol()))
{
   boost::asio::socket_base::send_buffer_size sendSize(16777216 * 2);
   mySocket->set_option(sendSize);

   boost::asio::socket_base::send_buffer_size actualSendSize;
   mySocket->get_option(actualSendSize);
   std::cout << "Acutual send buffer: " << actualSendSize.value() << std::endl;

   boost::asio::ip::address bindAddr(boost::asio::ip::address::from_string(bindIp));
   mySocket->bind(boost::asio::ip::udp::endpoint(bindAddr, 0));
}

MulticastSender::~MulticastSender()
{
}

void MulticastSender::send(const std::vector<unsigned char>& data)
{
   mySocket->send_to(boost::asio::buffer(data), myEndpoint);
}
