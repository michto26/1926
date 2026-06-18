/* -*-Mode:C++;c-file-style:"gnu";indent-tabs-mode:nil; -*-*/

//Adding required header files
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-layout-module.h"

//Add namespace declaration
using namespace ns3;

//Specify name of log file where log message will be saved
NS_LOG_COMPONENT_DEFINE ("Star");

//main function
int main (int argc, char *argv[])
{

 Config::SetDefault("ns3::OnOffApplication::PacketSize",UintegerValue(137));
 Config::SetDefault("ns3::OnOffApplication::DataRate",StringValue("14kb/s"));
 
 //Set time resolution to 1 nano second
  Time::SetResolution (Time::NS);
  
  uint32_t nSpokes=8;
  
  //Read and parse
  CommandLine cmd(__FILE__);
  cmd.Parse (argc, argv);
  
  //Display message
  NS_LOG_INFO("Buid Star Topology,,");
  
   //Configure point-to-point net devices and channels
   PointToPointHelper pointToPoint;
   pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
   
   //install Configured net devices and channel on nodes
   PointToPointStarHelper star (nSpokes, pointToPoint);
   
   //install protocol stack on nodes
   NS_LOG_INFO("install internet stack an all nodes");
   InternetStackHelper  stack;
   star.InstallStack (stack);
   
   NS_LOG_INFO("Assign IP Addresses");
   star.AssignIpv4Addresses (Ipv4AddressHelper ("10.0.0.0", "255.0.0.0"));
   
   //Configure the application on hub node
   
   NS_LOG_INFO("Create Application");
   
   
   uint16_t port=50000;
   
   //Configure Socket Address of hub
   Address hubLocalAddress (InetSocketAddress(Ipv4Address::GetAny(),port));
   PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory",hubLocalAddress);
   
   ApplicationContainer hubApp = packetSinkHelper.Install (star.GetHub());
   
   hubApp.Start (Seconds (1.0));
   hubApp.Stop (Seconds (10.0));
   
   //Configure on off applicaion
   OnOffHelper onOffHelper ("ns3::TcpSocketFactory", Address ());
   
   onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
   
   onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
   
   
   //install OnOffApplication
   ApplicationContainer spokeApps;
   
   for(uint32_t i=0; i<star.SpokeCount (); ++i)
   {
     AddressValue remoteAddress (InetSocketAddress (star.GetHubIpv4Address (i), port));
     
     onOffHelper.SetAttribute ("Remote", remoteAddress);
     
     spokeApps.Add (onOffHelper.Install (star.GetSpokeNode (i)));
   }
   
   
   //Configure start and stop time of onOff App
   spokeApps.Start (Seconds (1.0));
   spokeApps.Stop (Seconds (10.0));
   
   
   //turn on global static routing so packets can actually be routed accross the star
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
   
   //Configure topology for Animation
   star.BoundingBox (1,1,100,100);
   
   AnimationInterface anim ("mystar.xml");
   
   NS_LOG_INFO("Run Simulation");
   Simulator::Run ();
   Simulator::Destroy ();
   NS_LOG_INFO("Done");
   
   return 0;

}

