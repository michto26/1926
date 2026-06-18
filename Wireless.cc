/* -- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -- */ 
#include "ns3/core-module.h" 
#include "ns3/network-module.h" 
#include "ns3/internet-module.h" 
#include "ns3/point-to-point-module.h" 
#include "ns3/applications-module.h" 
#include "ns3/wifi-module.h" 
#include "ns3/mobility-module.h" 
#include "ns3/ipv4-global-routing-helper.h" 
#include "ns3/netanim-module.h" 
#include "ns3/flow-monitor-module.h" 
using namespace ns3; 
NS_LOG_COMPONENT_DEFINE("ImprovedWirelessTopology"); 
int main(int argc, char *argv[]) 
{ 
uint32_t nWifi = 5; 
CommandLine cmd(_FILE_); 
cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi); 
cmd.Parse(argc, argv); 
Time::SetResolution(Time::NS); 
LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO); 
LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO); 
// Create P2P Nodes 
NodeContainer p2pNodes; 
p2pNodes.Create(2); 
PointToPointHelper pointToPoint; 
pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps")); 
pointToPoint.SetChannelAttribute("Delay", StringValue("2ms")); 
NetDeviceContainer p2pDevices; 
p2pDevices = pointToPoint.Install(p2pNodes); 
// Create WiFi Nodes 
NodeContainer wifiStaNodes; 
wifiStaNodes.Create(nWifi); 
NodeContainer wifiApNode; 
wifiApNode.Add(p2pNodes.Get(1)); 
// Configure WiFi 
WifiHelper wifi; 
 
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default(); 
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default(); 
 
    phy.SetChannel(channel.Create()); 
 
    WifiMacHelper mac; 
 
    Ssid ssid = Ssid("NS3-WIFI-NETWORK"); 
 
    // Station Configuration 
    mac.SetType("ns3::StaWifiMac", 
                "Ssid", SsidValue(ssid), 
                "ActiveProbing", BooleanValue(false)); 
 
    NetDeviceContainer staDevices; 
    staDevices = wifi.Install(phy, mac, wifiStaNodes); 
 
    // AP Configuration 
    mac.SetType("ns3::ApWifiMac", 
                "Ssid", SsidValue(ssid)); 
 
    NetDeviceContainer apDevices; 
    apDevices = wifi.Install(phy, mac, wifiApNode); 
 
    // Mobility Configuration 
 
    MobilityHelper mobility; 
 
    mobility.SetPositionAllocator( 
        "ns3::GridPositionAllocator", 
        "MinX", DoubleValue(0.0), 
        "MinY", DoubleValue(0.0), 
        "DeltaX", DoubleValue(10.0), 
        "DeltaY", DoubleValue(10.0), 
        "GridWidth", UintegerValue(3), 
        "LayoutType", StringValue("RowFirst")); 
 
    // Mobile WiFi stations 
    mobility.SetMobilityModel( 
        "ns3::RandomWalk2dMobilityModel", 
        "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50))); 
 
    mobility.Install(wifiStaNodes); 
 
    // Fixed AP node 
    MobilityHelper mobilityAp; 
    mobilityAp.SetMobilityModel("ns3::ConstantPositionMobilityModel"); 
    mobilityAp.Install(wifiApNode); 
 
    // Internet Stack 
 
    InternetStackHelper stack; 
    stack.Install(p2pNodes.Get(0)); 
    stack.Install(wifiApNode); 
    stack.Install(wifiStaNodes); 
 
    // Assign IP Addresses 
 
    Ipv4AddressHelper address; 
 
    // P2P Network 
    address.SetBase("10.1.1.0", "255.255.255.0"); 
    Ipv4InterfaceContainer p2pInterfaces; 
    p2pInterfaces = address.Assign(p2pDevices); 
 
    // WiFi Network 
    address.SetBase("192.168.1.0", "255.255.255.0"); 
 
    Ipv4InterfaceContainer staInterfaces; 
    staInterfaces = address.Assign(staDevices); 
 
    Ipv4InterfaceContainer apInterface; 
    apInterface = address.Assign(apDevices); 
 
    // Routing 
 
    Ipv4GlobalRoutingHelper::PopulateRoutingTables(); 
 
    // UDP Echo Server 
 
    UdpEchoServerHelper echoServer(9); 
 
    ApplicationContainer serverApps = 
        echoServer.Install(wifiStaNodes.Get(nWifi - 1)); 
 
    serverApps.Start(Seconds(1.0)); 
    serverApps.Stop(Seconds(20.0)); 
 
    // UDP Echo Client 
 
    UdpEchoClientHelper echoClient( 
        staInterfaces.GetAddress(nWifi - 1), 9); 
 
    echoClient.SetAttribute("MaxPackets", UintegerValue(20)); 
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0))); 
    echoClient.SetAttribute("PacketSize", UintegerValue(1024)); 
 
    ApplicationContainer clientApps = 
        echoClient.Install(p2pNodes.Get(0)); 
 
    clientApps.Start(Seconds(2.0)); 
    clientApps.Stop(Seconds(20.0)); 
 
    // Flow Monitor 
 
    FlowMonitorHelper flowmon; 
    Ptr<FlowMonitor> monitor = flowmon.InstallAll(); 
 
    // NetAnim Configuration 
 
    AnimationInterface anim("WirelessTopology.xml"); 
 
    // P2P Node Positions 
    anim.SetConstantPosition(p2pNodes.Get(0), 0.0, 30.0); 
    anim.SetConstantPosition(p2pNodes.Get(1), 40.0, 30.0); 
 
    // WiFi Node Positions 
    anim.SetConstantPosition(wifiStaNodes.Get(0), 70.0, 10.0); 
    anim.SetConstantPosition(wifiStaNodes.Get(1), 70.0, 25.0); 
    anim.SetConstantPosition(wifiStaNodes.Get(2), 70.0, 40.0); 
    anim.SetConstantPosition(wifiStaNodes.Get(3), 70.0, 55.0); 
    anim.SetConstantPosition(wifiStaNodes.Get(4), 70.0, 70.0); 
 
    // Node Descriptions 
    anim.UpdateNodeDescription(p2pNodes.Get(0), "CLIENT NODE"); 
    anim.UpdateNodeDescription(p2pNodes.Get(1), "ACCESS POINT"); 
 
    anim.UpdateNodeDescription(wifiStaNodes.Get(0), "STA-1"); 
    anim.UpdateNodeDescription(wifiStaNodes.Get(1), "STA-2"); 
    anim.UpdateNodeDescription(wifiStaNodes.Get(2), "STA-3"); 
    anim.UpdateNodeDescription(wifiStaNodes.Get(3), "STA-4"); 
    anim.UpdateNodeDescription(wifiStaNodes.Get(4), "SERVER NODE"); 
 
    // Node Colors 
    anim.UpdateNodeColor(p2pNodes.Get(0), 255, 0, 0); 
    anim.UpdateNodeColor(p2pNodes.Get(1), 0, 255, 0); 
 
    anim.UpdateNodeColor(wifiStaNodes.Get(0), 0, 0, 255); 
    anim.UpdateNodeColor(wifiStaNodes.Get(1), 0, 0, 255); 
    anim.UpdateNodeColor(wifiStaNodes.Get(2), 0, 0, 255); 
    anim.UpdateNodeColor(wifiStaNodes.Get(3), 0, 0, 255); 
    anim.UpdateNodeColor(wifiStaNodes.Get(4), 255, 165, 0); 
 
    // Packet Metadata 
    anim.EnablePacketMetadata(true); 
 
    // Run Simulation 
 
    Simulator::Stop(Seconds(20.0)); 
 
    Simulator::Run(); 
 
    // Flow Monitor Statistics 
 
    monitor->CheckForLostPackets(); 
 
    Ptr<Ipv4FlowClassifier> classifier = 
        DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier()); 
 
    std::map<FlowId, FlowMonitor::FlowStats> stats = 
        monitor->GetFlowStats(); 
 
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = 
             stats.begin(); 
         i != stats.end(); 
         ++i) 
    { 
        Ipv4FlowClassifier::FiveTuple t = 
            classifier->FindFlow(i->first); 
 
std::cout << "Flow ID: " << i->first << std::endl; 
std::cout << "Source IP: " << t.sourceAddress 
<< " -> Destination IP: " 
<< t.destinationAddress << std::endl; 
std::cout << "Tx Packets = " 
<< i->second.txPackets << std::endl; 
std::cout << "Rx Packets = " 
<< i->second.rxPackets << std::endl; 
std::cout << "Packet Loss = " 
<< (i->second.txPackets - i->second.rxPackets) 
<< std::endl; 
std::cout << "Throughput = " 
<< i->second.rxBytes * 8.0 / 
(i->second.timeLastRxPacket.GetSeconds() - 
i->second.timeFirstTxPacket.GetSeconds()) / 
1024 
<< " Kbps" 
<< std::endl; 
} 
Simulator::Destroy(); 
return 0; 
}
