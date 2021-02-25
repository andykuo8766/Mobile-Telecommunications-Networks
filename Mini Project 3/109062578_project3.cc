/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 TEI of Western Macedonia, Greece
 *
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
 *
 * Author: Dimitrios J. Vergados <djvergad@gmail.com>
 */

// Network topology
//
//       n0 ----------- n1       UE
//            500 Kbps
//             5 ms
//
#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/dash-module.h"


/////////////////////////////////////////////
#include "ns3/lte-module.h"
#include "ns3/config-store-module.h"
#include "ns3/mobility-module.h"
/////////////////////////////////////////////


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DashExample");

int
main (int argc, char *argv[])
{
  bool tracing = false;
  uint32_t maxBytes = 100;
  uint32_t users = 1;
  double target_dt = 35.0;
  double stopTime = 30.0;
  std::string linkRate = "500Kbps";
  std::string delay = "5ms";
  std::string algorithm = "ns3::DashClient";
  uint32_t bufferSpace = 30000000;
  std::string window = "10s";
  double enbTxPowerDbm = 46.0;


  /*  LogComponentEnable("MpegPlayer", LOG_LEVEL_ALL);*/
  /* LogComponentEnable ("DashServer", LOG_LEVEL_ALL);
  LogComponentEnable ("DashClient", LOG_LEVEL_ALL);*/

  //
  // Allow the user to override any of the defaults at
  // run-time, via command-line arguments
  //
  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
  cmd.AddValue ("maxBytes", "Total number of bytes for application to send", maxBytes);
  cmd.AddValue ("users", "The number of concurrent videos", users);
  cmd.AddValue ("targetDt", "The target time difference between receiving and playing a frame.",
                target_dt);
  cmd.AddValue ("stopTime", "The time when the clients will stop requesting segments", stopTime);
  cmd.AddValue ("linkRate",
                "The bitrate of the link connecting the clients to the server (e.g. 500kbps)",
                linkRate);
  cmd.AddValue ("delay", "The delay of the link connecting the clients to the server (e.g. 5ms)",
                delay);
  cmd.AddValue ("algorithms",
                "The adaptation algorithms used. It can be a comma seperated list of"
                "protocolos, such as 'ns3::FdashClient,ns3::OsmpClient'."
                "You may find the list of available algorithms in src/dash/model/algorithms",
                algorithm);
  cmd.AddValue ("bufferSpace", "The space in bytes that is used for buffering the video",
                bufferSpace);
  cmd.AddValue ("window", "The window for measuring the average throughput (Time).", window);
  cmd.Parse (argc, argv);

//////////////////////////////////////////////////////////////////////////////////////////////
/*
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();


  
  enbNodes.Create (numberOfEnbs);
  ueNodes.Create (numberOfUes);

  // Install Mobility Model in UE
  MobilityHelper ueMobility;
  ueMobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  ueMobility.Install (ueNodes);
  ueNodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 500.0, 0));
  ueNodes.Get (0)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (20, 0, 0));

  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numberOfEnbs; i++)
    {
      Vector enbPosition (50 * (i + 1), 50, 0);
      enbPositionAlloc->Add (enbPosition);
    }

  MobilityHelper enbMobility;
  enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  enbMobility.SetPositionAllocator (enbPositionAlloc);
  enbMobility.Install (enbNodes);

  // Install LTE Devices in eNB and UEs
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (enbTxPowerDbm));
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  InternetStackHelper internet;
  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIfaces;
  ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

  // Attach all UEs to the first eNodeB
  for (uint16_t i = 0; i < numberOfUes; i++)
    {
      lteHelper->Attach (ueLteDevs.Get (i), enbLteDevs.Get (0));
    }
*/


  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");

  lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
  lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
                                            UintegerValue (30));
  lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
                                            UintegerValue (1));

  Ptr<Node> pgw = epcHelper->GetPgwNode ();


  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);
  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);


  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
      ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"),1);


  uint32_t numberOfEnbs = 1;
  uint32_t numberOfUes = users;
  NodeContainer ueNodes;
  NodeContainer enbNodes;

  enbNodes.Create (numberOfEnbs);
  ueNodes.Create (numberOfUes);

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mobility.Install (ueNodes);
  mobility.Install (enbNodes);

  // Install LTE Devices in eNB and UEs
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (enbTxPowerDbm));
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIfaces;
  ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

  // Attach all UEs to the first eNodeB
  for (uint16_t i = 0; i < numberOfUes; i++)
    {
      lteHelper->Attach (ueLteDevs.Get (i), enbLteDevs.Get (0));
    }






/*
//////////////////////////////////////////////////////////////////////////////////////
  //
  // Explicitly create the nodes required by the topology (shown above).
  //
  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (2);

  NS_LOG_INFO ("Create channels.");

  //
  // Explicitly create the point-to-point link required by the topology (shown above).
  //
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (linkRate));
  pointToPoint.SetChannelAttribute ("Delay", StringValue (delay));
  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  //
  // Install the internet stack on the nodes
  //
  //InternetStackHelper internet;
  internet.Install (nodes);

  //
  // We've got the "hardware" in place.  Now we need to add IP addresses.
  //
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);
*/




  NS_LOG_INFO ("Create Applications.");

  std::vector<std::string> algorithms;
  std::stringstream ss (algorithm);
  std::string proto;
  uint32_t protoNum = 0; // The number of algorithms
  while (std::getline (ss, proto, ',') && protoNum++ < users)
    {
      algorithms.push_back (proto);
    }

  uint16_t port = 80; // well-known echo port number
  std::vector<DashClientHelper> clients;
  std::vector<ApplicationContainer> clientApps;

  for (uint32_t user = 0; user < users; user++)
    {
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting =ipv4RoutingHelper.GetStaticRouting (ueNodes.Get (user)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);      

      DashClientHelper client ("ns3::TcpSocketFactory", InetSocketAddress (remoteHostAddr, port),
                               algorithms[user % protoNum]);
      //client.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
      client.SetAttribute ("VideoId", UintegerValue (user + 1)); // VideoId should be positive
      client.SetAttribute ("TargetDt", TimeValue (Seconds (target_dt)));
      client.SetAttribute ("window", TimeValue (Time (window)));
      client.SetAttribute ("bufferSpace", UintegerValue (bufferSpace));

      //ApplicationContainer clientApp = client.Install (nodes.Get (0));
      ApplicationContainer clientApp = client.Install (ueNodes.Get (user));

      clientApp.Start (Seconds (0.25));
      clientApp.Stop (Seconds (stopTime));

      clients.push_back (client);
      clientApps.push_back (clientApp);
    }

  DashServerHelper server ("ns3::TcpSocketFactory",
                           InetSocketAddress (Ipv4Address::GetAny (), port));
  //ApplicationContainer serverApps = server.Install (nodes.Get (1));
  ApplicationContainer serverApps = server.Install (remoteHost);
  serverApps.Start (Seconds (0.0));
  serverApps.Stop (Seconds (stopTime + 5.0));

  //
  // Set up tracing if enabled
  //

  if (tracing)
    {
      AsciiTraceHelper ascii;
      p2ph.EnableAsciiAll (ascii.CreateFileStream ("dash-send.tr"));
      p2ph.EnablePcapAll ("dash-send", false);
    }

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(Seconds(30.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  uint32_t k;
  for (k = 0; k < users; k++)
    {
      Ptr<DashClient> app = DynamicCast<DashClient> (clientApps[k].Get (0));
      std::cout << algorithms[k % protoNum] << "-Node: " << k;
      app->GetStats ();
    }

  return 0;
}
