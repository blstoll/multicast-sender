#include <iostream>
#include <unistd.h>
#include <csignal>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "MulticastSender.h"
#include "RateLimitedSender.h"

namespace
{
   int SUCCESS = 0;
   int ERROR = -1;

   RateLimitedSender* ourRateSender = 0;
}

void signalHandler(int signum)
{
   std::cout << std::endl << "Iterrupt signal (" << signum << ") received." << std::endl;
   if(ourRateSender)
   {
      std::cout << "Stopping the flow of data..." << std::endl;
      ourRateSender->stop();
   }
}

int main(int argc, char* argv[])
{
   namespace opts = boost::program_options;
   opts::options_description desc("Options");
   opts::variables_map values;

   desc.add_options()
   ("help,h", "Print help messages") 
   ("destination,d", opts::value<std::string>()->default_value("224.33.44.4"), "Multicast Destination Address") 
   ("port,p", opts::value<unsigned short>()->default_value(1234),"Port")
   ("bind,b", opts::value<std::string>()->default_value("127.0.0.1"),"Bind address")
   ("rate,r", opts::value<unsigned int>()->default_value(24),"Packets per interval")
   ("interval,i", opts::value<unsigned int>()->default_value(1),"Packet interval in ms")
   ("size,s", opts::value<unsigned int>()->default_value(1080),"Packet size in bytes");

   try
   {
      opts::store(opts::parse_command_line(argc, argv, desc), values);
      if(values.count("help"))
      {
         std::cout << "Multicast sender application" << std::endl << desc << std::endl;
         return SUCCESS;
      }

      // Throws on command line error
      opts::notify(values);
   }
   catch(opts::error& e)
   {
      std::cerr << "Error: " << e.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;

      return ERROR;
   }

   // Register a signal for clean shutdown
   signal(SIGINT, signalHandler);

   std::string multicastAddress(values["destination"].as<std::string>());
   std::string bindAddress(values["bind"].as<std::string>());
   unsigned short port(values["port"].as<unsigned short>());
   unsigned int packetRate(values["rate"].as<unsigned int>());
   unsigned int interval(values["interval"].as<unsigned int>());
   unsigned int size(values["size"].as<unsigned int>());

   double mbps(static_cast<double>(packetRate) * size * 8 / interval / 1000);

   boost::asio::ip::address addr(boost::asio::ip::address::from_string(multicastAddress));
   
   std::cout << "Sending " << size << " byte packets to: "
      << addr << ":" << port << " on interface: " << bindAddress
      << " every " << interval << "ms" << std::endl;
   std::cout << "Average rate: " << mbps << " Mbps" << std::endl;

   std::cout << std::endl << "Type Ctrl+C to exit" << std::endl << std::endl;

   MulticastSender sender(addr, port, bindAddress);
   RateLimitedSender rate(size, sender, packetRate, interval);
   ourRateSender = &rate;

   rate.start();

   return 0;
}
