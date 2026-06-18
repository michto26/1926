/*-*-Mode:C++;c-file-style:"gnu";indent-tabs-mode:nil;-*-*/  

//adding Header Files
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

//Add namespace declaration
using namespace ns3;

//define name of the file that store log messages 
NS_LOG_COMPONENT_DEFINE ("UDPPontToPointExample");

int main(int argc, char *argv[])
{
	//reading and parsing command line arguments
	CommandLine cmd (__FILE__);
	cmd.Parse (argc, argv);
	
	//set time resolution to 1 nanosecond
	Time::SetResolution (Time::NS);
	
	//enable the logging for applications
	LogComponentEnable("UdpClient",LOG_LEVEL_INFO);
	LogComponentEnable("UdpServer",LOG_LEVEL_INFO);
	
	//Create 2 nodes 
	NodeContainer nodes;
	nodes.Create (2);
	
	//Configura and create the net devices and channel
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute ("DataRate",StringValue ("5Mbps"));
	pointToPoint.SetChannelAttribute ("Delay",StringValue ("2ms"));
	
	//install Configured net devices and channel on nodes
	NetDeviceContainer devices;
	devices = pointToPoint.Install (nodes);
	
	//Configure and install protocol stack on nodes
	InternetStackHelper  stack;
	stack.Install (nodes);
	
	//Configure IP Addresses
	Ipv4AddressHelper address;
	address.SetBase ("10.0.0.0","255.0.0.0");
	
	//Assign IP addresses to interfaces of net devices
	Ipv4InterfaceContainer interfaces= address.Assign (devices);
	
	//Configure the Server application
	UdpServerHelper echoServer (9);
	
	//install server application on node 1
	ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
	
	//Configure start and stop time of server application
	serverApps.Start (Seconds (1.0));
	serverApps.Stop (Seconds (10.0));
	
	//Configure the client Application
	UdpClientHelper echoClient (interfaces.GetAddress (1),9);
	echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
	echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
	echoClient.SetAttribute ("PacketSize",UintegerValue (1024));
	
	//install client application on node 0
	ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
	
	//Configure start and stop time of client
	clientApps.Start (Seconds (2.0));
	clientApps.Stop (Seconds (10.0));
	
	AnimationInterface anim ("P2PUdp.xml");
        anim.SetConstantPosition (nodes.Get (0), 10.0, 10.0);
        anim.SetConstantPosition (nodes.Get (1), 30.0, 10.0);
	
	Simulator::Run ();
	Simulator::Destroy ();
	return 0;
}
