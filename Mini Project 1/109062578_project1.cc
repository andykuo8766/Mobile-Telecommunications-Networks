/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h" 
// Default Network Topology
//
// Number of wifi or csma nodes can be increased up to 250
//                          |
//                 Rank 0   |   Rank 1
// -------------------------|----------------------------
//   Wifi 10.1.2.0                    Wifi 10.1.3.0
//                 AP                AP
//  *    *    *    *                 *    *    *    *
//  |    |    |    |    10.1.1.0     |    |    |    |
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  
//                                   
//                                    

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TestScriptExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nCsma = 3;
  uint32_t nWifi = 3;
  bool tracing = true;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // Check for valid number of csma or wifi nodes
  // 250 should be enough, otherwise IP addresses 
  // soon become an issue
  if (nWifi > 250 || nCsma > 250)
    {
      std::cout << "Too many wifi or csma nodes, no more than 250 each." << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

/************************************Topology************************************/
  //建立使用P2P連線的兩個網路節點
  NodeContainer p2pNodes;
  p2pNodes.Create (2);
  //設置datarate和delay
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  //安裝P2P網卡到P2P網路節點
  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);
  //建立STA節點
  NodeContainer wifiStaNodes1;
  wifiStaNodes1.Create (nWifi);
  //將node 0 設為AP
  NodeContainer wifiApNode1 = p2pNodes.Get (0);
  //建立STA節點
  NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (nWifi);
  //將node 1 設為AP
  NodeContainer wifiApNode2 = p2pNodes.Get (1);
  //將channel初始化
  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
  phy1.SetChannel (channel1.Create ());
//將channel初始化
  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
  //設置STA MAC
  WifiMacHelper mac1;
  Ssid ssid1 = Ssid ("ns-3-ssid");
  mac1.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid1),
               "ActiveProbing", BooleanValue (false));
  //設置STA MAC
  WifiMacHelper mac2;
  Ssid ssid2 = Ssid ("ns-3-ssid");
  mac2.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid2),
               "ActiveProbing", BooleanValue (false));
  //安裝網卡到連接AP的STA節點
  NetDeviceContainer staDevices1;
  staDevices1 = wifi.Install (phy1, mac1, wifiStaNodes1);
  //安裝網卡到連接AP的STA節點
  NetDeviceContainer staDevices2;
  staDevices2 = wifi.Install (phy2, mac2, wifiStaNodes2);
  //設置AP MAC
  mac1.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid1));
  //設置AP MAC
  mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2));

  //安裝網卡到AP
  NetDeviceContainer apDevices1;
  apDevices1 = wifi.Install (phy1, mac1, wifiApNode1);
  //安裝網卡到AP
  NetDeviceContainer apDevices2;
  apDevices2 = wifi.Install (phy2, mac2, wifiApNode2);
  //增加移動模型
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  //將STA AP安裝到模型上						 
  mobility.Install (wifiStaNodes1);
  mobility.Install (wifiStaNodes2);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode1);
  mobility.Install (wifiApNode2);

  //安裝網路協議
  InternetStackHelper stack;
  stack.Install (wifiApNode1);
  stack.Install (wifiStaNodes1);
  stack.Install (wifiApNode2);
  stack.Install (wifiStaNodes2);

  Ipv4AddressHelper address;
  //為P2P設置IP address
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);
  //為AP(n0)設置IP address
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer staInterfaces;
  staInterfaces = address.Assign (staDevices1);
  address.Assign (apDevices1);
  //為AP(n1)設置IP address
  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices2);
  address.Assign (apDevices2);


/********************************************************************************/

/***********************************Application**********************************/


  UdpEchoServerHelper echoServer (9);

  //將server安裝在WIFI的到數第3個節點上
  ApplicationContainer serverApps = echoServer.Install (wifiStaNodes1.Get (nWifi - 1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  
  UdpEchoClientHelper echoClient(staInterfaces.GetAddress(nWifi - 1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  //將client安裝在WIFI的到數第3個節點上
  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes2.Get (nWifi - 1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  Simulator::Stop (Seconds (10.0));
/********************************************************************************/

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("P2P");
      phy1.EnablePcap ("AP1", apDevices1.Get (0));
      phy2.EnablePcap ("AP2", apDevices2.Get (0));
    }
  AnimationInterface anim("109062578_project1.xml");  
  anim.SetConstantPosition(p2pNodes.Get(0), 0, 50);  
  anim.SetConstantPosition(p2pNodes.Get(1), 50, 50);
  Simulator::Run ();
  Simulator::Destroy ();


  return 0;
}