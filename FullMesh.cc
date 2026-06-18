/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"

#include "ns3/mobility-module.h"

using namespace ns3;

// Define Log Component
NS_LOG_COMPONENT_DEFINE ("FullMeshTopology");

int
main (int argc, char *argv[])
{
    CommandLine cmd;
    cmd.Parse (argc, argv);

    // ================= ENABLE LOGGING =================

    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    LogComponentEnable("FullMeshTopology", LOG_LEVEL_INFO);

    // ================= CREATE NODES =================

    NodeContainer nodes;
    nodes.Create (5);

    NS_LOG_INFO("Created 5 Nodes");

    // ================= POINT TO POINT =================

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

    // ================= INSTALL INTERNET STACK =================

    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;

    // ================= FULL MESH CONNECTIONS =================

    // Node0 <-> Node1
    NodeContainer n0n1(nodes.Get(0), nodes.Get(1));
    NetDeviceContainer d0d1 = p2p.Install(n0n1);
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i1 = address.Assign(d0d1);

    // Node0 <-> Node2
    NodeContainer n0n2(nodes.Get(0), nodes.Get(2));
    NetDeviceContainer d0d2 = p2p.Install(n0n2);
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i2 = address.Assign(d0d2);

    // Node0 <-> Node3
    NodeContainer n0n3(nodes.Get(0), nodes.Get(3));
    NetDeviceContainer d0d3 = p2p.Install(n0n3);
    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i3 = address.Assign(d0d3);

    // Node0 <-> Node4
    NodeContainer n0n4(nodes.Get(0), nodes.Get(4));
    NetDeviceContainer d0d4 = p2p.Install(n0n4);
    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i4 = address.Assign(d0d4);

    // Node1 <-> Node2
    NodeContainer n1n2(nodes.Get(1), nodes.Get(2));
    NetDeviceContainer d1d2 = p2p.Install(n1n2);
    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i2 = address.Assign(d1d2);

    // Node1 <-> Node3
    NodeContainer n1n3(nodes.Get(1), nodes.Get(3));
    NetDeviceContainer d1d3 = p2p.Install(n1n3);
    address.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i3 = address.Assign(d1d3);

    // Node1 <-> Node4
    NodeContainer n1n4(nodes.Get(1), nodes.Get(4));
    NetDeviceContainer d1d4 = p2p.Install(n1n4);
    address.SetBase("10.1.7.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i4 = address.Assign(d1d4);

    // Node2 <-> Node3
    NodeContainer n2n3(nodes.Get(2), nodes.Get(3));
    NetDeviceContainer d2d3 = p2p.Install(n2n3);
    address.SetBase("10.1.8.0", "255.255.255.0");
    Ipv4InterfaceContainer i2i3 = address.Assign(d2d3);

    // Node2 <-> Node4
    NodeContainer n2n4(nodes.Get(2), nodes.Get(4));
    NetDeviceContainer d2d4 = p2p.Install(n2n4);
    address.SetBase("10.1.9.0", "255.255.255.0");
    Ipv4InterfaceContainer i2i4 = address.Assign(d2d4);

    // Node3 <-> Node4
    NodeContainer n3n4(nodes.Get(3), nodes.Get(4));
    NetDeviceContainer d3d4 = p2p.Install(n3n4);
    address.SetBase("10.1.10.0", "255.255.255.0");
    Ipv4InterfaceContainer i3i4 = address.Assign(d3d4);

    NS_LOG_INFO("Full Mesh Connections Created");

    // ================= ROUTING =================

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // ================= MOBILITY =================

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    // ================= SERVERS =================

    UdpEchoServerHelper server1(9);
    UdpEchoServerHelper server2(10);
    UdpEchoServerHelper server3(11);
    UdpEchoServerHelper server4(12);

    ApplicationContainer s1 = server1.Install(nodes.Get(1));
    ApplicationContainer s2 = server2.Install(nodes.Get(2));
    ApplicationContainer s3 = server3.Install(nodes.Get(3));
    ApplicationContainer s4 = server4.Install(nodes.Get(4));

    s1.Start(Seconds(1.0));
    s2.Start(Seconds(1.0));
    s3.Start(Seconds(1.0));
    s4.Start(Seconds(1.0));

    s1.Stop(Seconds(20.0));
    s2.Stop(Seconds(20.0));
    s3.Stop(Seconds(20.0));
    s4.Stop(Seconds(20.0));

    NS_LOG_INFO("Servers Started");

    // ================= CLIENTS =================

    // Client to Node1
    UdpEchoClientHelper client1(i0i1.GetAddress(1), 9);
    client1.SetAttribute("MaxPackets", UintegerValue(10));
    client1.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    client1.SetAttribute("PacketSize", UintegerValue(1024));

    // Client to Node2
    UdpEchoClientHelper client2(i0i2.GetAddress(1), 10);
    client2.SetAttribute("MaxPackets", UintegerValue(10));
    client2.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    client2.SetAttribute("PacketSize", UintegerValue(1024));

    // Client to Node3
    UdpEchoClientHelper client3(i0i3.GetAddress(1), 11);
    client3.SetAttribute("MaxPackets", UintegerValue(10));
    client3.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    client3.SetAttribute("PacketSize", UintegerValue(1024));

    // Client to Node4
    UdpEchoClientHelper client4(i0i4.GetAddress(1), 12);
    client4.SetAttribute("MaxPackets", UintegerValue(10));
    client4.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    client4.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer c1 = client1.Install(nodes.Get(0));
    ApplicationContainer c2 = client2.Install(nodes.Get(0));
    ApplicationContainer c3 = client3.Install(nodes.Get(0));
    ApplicationContainer c4 = client4.Install(nodes.Get(0));

    c1.Start(Seconds(2.0));
    c2.Start(Seconds(3.0));
    c3.Start(Seconds(4.0));
    c4.Start(Seconds(5.0));

    c1.Stop(Seconds(20.0));
    c2.Stop(Seconds(20.0));
    c3.Stop(Seconds(20.0));
    c4.Stop(Seconds(20.0));

    NS_LOG_INFO("Clients Started Sending Packets");

    // ================= NETANIM =================

    AnimationInterface anim("fullmesh.xml");

    // Node Positions
    anim.SetConstantPosition(nodes.Get(0), 30, 10);
    anim.SetConstantPosition(nodes.Get(1), 10, 25);
    anim.SetConstantPosition(nodes.Get(2), 50, 25);
    anim.SetConstantPosition(nodes.Get(3), 20, 45);
    anim.SetConstantPosition(nodes.Get(4), 40, 45);

    // Labels
    anim.UpdateNodeDescription(nodes.Get(0), "Node0");
    anim.UpdateNodeDescription(nodes.Get(1), "Node1");
    anim.UpdateNodeDescription(nodes.Get(2), "Node2");
    anim.UpdateNodeDescription(nodes.Get(3), "Node3");
    anim.UpdateNodeDescription(nodes.Get(4), "Node4");

    // Colors
    anim.UpdateNodeColor(nodes.Get(0), 255, 0, 0);
    anim.UpdateNodeColor(nodes.Get(1), 0, 255, 0);
    anim.UpdateNodeColor(nodes.Get(2), 0, 0, 255);
    anim.UpdateNodeColor(nodes.Get(3), 255, 255, 0);
    anim.UpdateNodeColor(nodes.Get(4), 255, 0, 255);

    NS_LOG_INFO("Starting Simulation");

    Simulator::Stop(Seconds(20.0));

    Simulator::Run();

    Simulator::Destroy();

    NS_LOG_INFO("Simulation Finished");

    return 0;
}
