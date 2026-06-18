/*-*-Mode:C++;c-file-style:"gnu";indent-tabs-mode:nil;-*-*/

//Adding required header files
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

//add namespace declaration
using namespace ns3;

//specify name of log file where log message will be saved

NS_LOG_COMPONENT_DEFINE ("PointTo PointExample");

int main (int argc,char *argv[])
{
//reading and parsing command line arguments
CommandLine cmd (__FILE__);
cmd.Parse (argc,argv);

//Set Time Resolution to 1 nanosecond
Time::SetResolution (Time::NS);

//enable logging for client and server applications
LogComponentEnable ("UdpEchoClientApplication",LOG_LEVEL_INFO);
LogComponentEnable ("UdpEchoServerApplication",LOG_LEVEL_INFO);

//create 2 nodes
NodeContainer nodes;
nodes.Create (2);

//configure point to point  net devices and channel
PointToPointHelper PointToPoint;
PointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
PointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

//install net devices on nodes and connect them using channel
NetDeviceContainer devices;
devices = PointToPoint.Install (nodes);
//configure and install protocols stack
InternetStackHelper stack;
stack.Install (nodes);

//configure IP addresses
Ipv4AddressHelper address;
address.SetBase ("10.0.0.0", "255.0.0.0");

//Assign IP Addresses to the interfaces of net devices
Ipv4InterfaceContainer interfaces = address.Assign (devices);

//configure server application
UdpEchoServerHelper echoServer (9);

//install server application on node 1
ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));

//configure start and stop time of server application
serverApps.Start (Seconds (1.0));
serverApps.Stop (Seconds (10.0));

//configure the client application
UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);

echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

//install client application on node 0
ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));

//configure start and stop time of client application
clientApps.Start(Seconds(2.0));
clientApps.Stop(Seconds(10.0));

//animate the topology
AnimationInterface anim("p2p_Animation.xml");
anim.SetConstantPosition(nodes.Get(0),10.0,10.0);
anim.SetConstantPosition(nodes.Get(1),30.0,10.0);

//Run simulation
Simulator::Run ();

//Destroy resources
Simulator::Destroy ();
return 0;
}
 
