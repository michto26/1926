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

//Add namespace declaration
using namespace ns3;

//Specify name of log file where log message will be saved
NS_LOG_COMPONENT_DEFINE ("BusTopologyExample");

//main function
int main (int argc, char *argv[])
{
   //reading and parsing cmd line arguments
   CommandLine cmd (__FILE__);
   cmd.Parse (argc,argv);
   
   //Set time resolution to 1 nano second
   Time::SetResolution (Time::NS);
   
   //Enable logging for client and server applications
   LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
   LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

   //Create p2p nodes
   NodeContainer p2pNodes;
   p2pNodes.Create (2);

   //Configure point-to-point net devices and channels
   PointToPointHelper pointToPoint;
   pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

   //Install p2p net devices on p2p nodes and connect them using channel
   NetDeviceContainer p2pDevices;
   p2pDevices = pointToPoint.Install (p2pNodes);

   //Configure and install protocols stack on p2p node 0
   InternetStackHelper stack;
   stack.Install (p2pNodes.Get (0));

   //creating bus topology
   //create nodes in bus topology
   NodeContainer busNodes; 
   busNodes.Add (p2pNodes.Get (1));//adds p2p node 1 to the bus topology
   busNodes.Create (3);//Create 3 additional nodes in bus topology

        
   // Configure net devices and shared channel(csma) in bus topology
   CsmaHelper csma;
   csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
   csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

   // Install net devices on bus nodes and connect them using csma channel
   NetDeviceContainer busDevices;
   busDevices = csma.Install (busNodes);

   // Install protocol stack on bus nodes:
   stack.Install (busNodes);

   //Configure IP address for interfaces of bus devices
   Ipv4AddressHelper address;
   address.SetBase ("10.0.0.0", "255.0.0.0");
   
   //assign ip address to interfaces of p2p devices
   Ipv4InterfaceContainer p2pInterfaces;
   p2pInterfaces = address.Assign (p2pDevices);
   
   //Configure IP address for interfaces of bus devices
   address.SetBase ("20.0.0.0", "255.0.0.0");
   
   //configure IP address for interfaces of bus devices
   Ipv4InterfaceContainer busInterfaces;
   busInterfaces = address.Assign (busDevices);

   //Configure server application
   UdpEchoServerHelper echoServer (9);
   
   //install server application on bus node n4
   ApplicationContainer serverApps;
   serverApps = echoServer.Install (busNodes.Get (3));
   
   //Configure start and stop time of server
   serverApps.Start (Seconds (1.0));
   serverApps.Stop (Seconds (10.0));

   //Configure client application
   UdpEchoClientHelper echoClient (busInterfaces.GetAddress (3), 9);
   echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
   echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
   echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

   //insatall client application on p2p node 0
   ApplicationContainer clientApps;
   clientApps = echoClient.Install (p2pNodes.Get (0));
   
   //Configure start and stop time of client
   clientApps.Start (Seconds (2.0));
   clientApps.Stop (Seconds (10.0));

   //configure the route from p2p network to bus network
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

   //Animate the topology
   AnimationInterface anim ("bus_Animator.xml");
   anim.SetConstantPosition (p2pNodes.Get (0), 10.0, 10.0);
   anim.SetConstantPosition (busNodes.Get (0), 10.0, 10.0);
   anim.SetConstantPosition (busNodes.Get (1), 20.0, 10.0);
   anim.SetConstantPosition (busNodes.Get (2), 30.0, 10.0);
   anim.SetConstantPosition (busNodes.Get (3), 40.0, 10.0);

   Simulator::Run ();
   Simulator::Destroy ();

   return 0;
}

