/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */ 
 
#include "ns3/core-module.h" 
#include "ns3/network-module.h" 
#include "ns3/internet-module.h" 
#include "ns3/applications-module.h" 
#include "ns3/netanim-module.h" 
#include "ns3/wifi-module.h" 
#include "ns3/mobility-module.h" 
#include "ns3/log.h" 
 
using namespace ns3; 
 
NS_LOG_COMPONENT_DEFINE("wifi_Topology"); 
 
int 
main(int argc, char *argv[]) 
{ 
    CommandLine cmd(__FILE__); 
    cmd.Parse(argc, argv); 
 
    Time::SetResolution(Time::NS); 
 
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_ALL); 
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_ALL); 
 
    NodeContainer wifiStaNodes; 
    wifiStaNodes.Create(3); 
 
    NodeContainer wifiApNode; 
    wifiApNode.Create(1); 
 
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default(); 
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default(); 
    phy.SetChannel(channel.Create()); 
 
    WifiHelper wifi; 
    wifi.SetRemoteStationManager("ns3::AarfWifiManager"); 
 WifiMacHelper mac; 
    Ssid ssid = Ssid("wifi-network"); 
 
    mac.SetType("ns3::StaWifiMac", 
                "Ssid", SsidValue(ssid), 
                "ActiveProbing", BooleanValue(false)); 
 
    NetDeviceContainer staDevices; 
    staDevices = wifi.Install(phy, mac, wifiStaNodes); 
 
    mac.SetType("ns3::ApWifiMac", 
                "Ssid", SsidValue(ssid)); 
 
    NetDeviceContainer apDevice; 
    apDevice = wifi.Install(phy, mac, wifiApNode); 
 
    MobilityHelper mobility; 
 
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel"); 
 
    mobility.Install(wifiStaNodes); 
    mobility.Install(wifiApNode); 
 
    InternetStackHelper stack; 
    stack.Install(wifiApNode); 
    stack.Install(wifiStaNodes); 
 
    Ipv4AddressHelper address; 
 
    address.SetBase("192.168.1.0", "255.255.255.0"); 
 
    NetDeviceContainer allDevices; 
    allDevices.Add(apDevice); 
    allDevices.Add(staDevices); 
 
    Ipv4InterfaceContainer interfaces; 
    interfaces = address.Assign(allDevices); 
 
    UdpEchoServerHelper server(9); 
  ApplicationContainer serverApps = server.Install(wifiApNode.Get(0)); 
    serverApps.Start(Seconds(1.0)); 
    serverApps.Stop(Seconds(10.0)); 
 
    UdpEchoClientHelper client(interfaces.GetAddress(0), 9); 
 
    client.SetAttribute("MaxPackets", UintegerValue(1)); 
    client.SetAttribute("Interval", TimeValue(Seconds(1.0))); 
    client.SetAttribute("PacketSize", UintegerValue(1024)); 
 
    ApplicationContainer clientApps = 
        client.Install(wifiStaNodes.Get(0)); 
 
    clientApps.Start(Seconds(2.0)); 
    clientApps.Stop(Seconds(10.0)); 
 
    Ipv4GlobalRoutingHelper::PopulateRoutingTables(); 
 
    AnimationInterface anim("wifi_animation.xml"); 
 
    anim.SetMaxPktsPerTraceFile(99999999); 
 
    anim.SetConstantPosition(wifiApNode.Get(0), 50.0, 50.0); 
 
    anim.SetConstantPosition(wifiStaNodes.Get(0), 20.0, 20.0); 
    anim.SetConstantPosition(wifiStaNodes.Get(1), 80.0, 20.0); 
    anim.SetConstantPosition(wifiStaNodes.Get(2), 50.0, 80.0); 
 
    Simulator::Run(); 
    Simulator::Destroy(); 
 
    return 0; 
}
