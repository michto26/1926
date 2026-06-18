/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-apps-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DHCPExample");

int main(int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  LogComponentEnable ("DhcpServer", LOG_LEVEL_ALL);
  LogComponentEnable ("DhcpClient", LOG_LEVEL_ALL);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);

  NodeContainer dhcpNodes;
  NodeContainer router;
  dhcpNodes.Create(3);
  router.Create(2);

  NodeContainer busnodes(dhcpNodes, router);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("5Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue("2ms"));
  csma.SetDeviceAttribute ("Mtu", UintegerValue (1500));

  NetDeviceContainer busDevices = csma.Install (busnodes);

  NodeContainer p2pNodes;
  p2pNodes.Add (busnodes.Get (4));
  p2pNodes.Create(1);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  InternetStackHelper stack;
  stack.Install(busnodes);
  stack.Install(p2pNodes.Get(1));

  Ipv4AddressHelper address;
  address.SetBase ("20.0.0.0","255.0.0.0");

  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign(p2pDevices);
 
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Declare DHCP helper
  DhcpHelper dhcpHelper;

  Ipv4InterfaceContainer fixedInterface =
    dhcpHelper.InstallFixedAddress(busDevices.Get(4),
    Ipv4Address ("10.0.0.17"),
    Ipv4Mask("/8"));  

  ApplicationContainer dhcpServerApp =
    dhcpHelper.InstallDhcpServer(
      busDevices.Get (3),
      Ipv4Address ("10.0.0.12"),
      Ipv4Address ("10.0.0.0"),
      Ipv4Mask ("255.0.0.0"),  
      Ipv4Address ("10.0.0.10"),
      Ipv4Address ("10.0.0.15"),
      Ipv4Address("10.0.0.17"));

  dhcpServerApp.Start (Seconds (0.0));
  dhcpServerApp.Stop (Seconds (20.0));

  NetDeviceContainer dhcpClientNetDevs;
  dhcpClientNetDevs.Add (busDevices.Get (0));
  dhcpClientNetDevs.Add (busDevices.Get (1));
  dhcpClientNetDevs.Add (busDevices.Get (2));

  ApplicationContainer dhcpClients =
  dhcpHelper.InstallDhcpClient(dhcpClientNetDevs);

  dhcpClients.Start (Seconds (1.0));
  dhcpClients.Stop (Seconds (20.0));

  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps =
    echoServer.Install (p2pNodes.Get (1));

  serverApps.Start (Seconds (0.0));
  serverApps.Stop (Seconds (20.0));

  UdpEchoClientHelper echoClient (p2pInterfaces.GetAddress (1), 9);

  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps =
  echoClient.Install (busnodes.Get (1));

  clientApps.Start (Seconds (0.0));
  clientApps.Stop (Seconds (20.0));

 

  Simulator::Stop (Seconds (30.0));

  Simulator::Run();
  Simulator::Destroy ();

  return 0;
}

