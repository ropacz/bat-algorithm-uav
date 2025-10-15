//
// BatRouting.h
// Bat Algorithm-based routing protocol for FANETs
//

#ifndef __BAT_ALGORITHM_BATROUTING_H_
#define __BAT_ALGORITHM_BATROUTING_H_

#include <omnetpp.h>
#include <vector>
#include <map>
#include "inet/common/geometry/common/Coord.h"
#include "ArbitraryMobility.h"

using namespace omnetpp;
using namespace inet;

// Route information structure
struct RouteInfo {
    std::vector<int> path;        // Node IDs in path
    double fitness;               // Route quality metric
    double hopCount;              // Number of hops
    double linkQuality;           // Average link quality
    double energyCost;            // Estimated energy consumption
    simtime_t lastUpdate;         // Last update time
    
    RouteInfo() : fitness(1e9), hopCount(0), linkQuality(0), energyCost(0) {}
};

// Packet for route discovery
class RouteDiscoveryPacket : public cMessage {
  public:
    std::vector<int> visitedNodes;
    int sourceId;
    int destId;
    double accumulatedFitness;
    
    RouteDiscoveryPacket(const char *name=nullptr) : cMessage(name) {
        sourceId = -1;
        destId = -1;
        accumulatedFitness = 0.0;
        // Set display properties for animation
        setKind(1); // Route discovery type
    }
    
    virtual RouteDiscoveryPacket *dup() const override {
        return new RouteDiscoveryPacket(*this);
    }
};

// Data packet with routing info
class DataPacket : public cMessage {
  public:
    int sourceId;
    int destId;
    int currentHop;
    std::vector<int> routePath;
    
    DataPacket(const char *name=nullptr) : cMessage(name) {
        sourceId = -1;
        destId = -1;
        currentHop = 0;
        setKind(2); // Data packet type
    }
    
    virtual DataPacket *dup() const override {
        return new DataPacket(*this);
    }
};

class BatRouting : public cSimpleModule
{
  private:
    // Bat Algorithm parameters
    double frequencyMin, frequencyMax;
    double currentLoudness, currentPulseRate;
    double initialLoudness, initialPulseRate;
    double alpha, gamma;
    
    // Routing parameters
    double routingUpdateInterval;
    double hopCountWeight, linkQualityWeight;
    double energyWeight, mobilityWeight;
    int maxRoutesPerDestination;
    double routeTimeout;
    
    // Route table: destination -> list of routes
    std::map<int, std::vector<RouteInfo>> routeTable;
    
    // Neighbor information
    std::map<int, Coord> neighborPositions;
    std::map<int, simtime_t> neighborLastSeen;
    
    // Statistics
    simsignal_t routeDiscoveredSignal;
    simsignal_t packetRoutedSignal;
    
    // Messages
    cMessage *routeUpdateTimer;
    
    // My node ID
    int myNodeId;
    
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    
    // Routing functions
    void discoverRoutes();
    void processRouteDiscovery(RouteDiscoveryPacket *pkt);
    void updateRouteTable(int dest, const RouteInfo &route);
    RouteInfo* selectBestRoute(int dest);
    void routeDataPacket(DataPacket *pkt);
    
    // Bat Algorithm for route optimization
    double calculateRouteFitness(const RouteInfo &route);
    void optimizeRouteTable();
    void updateBatParameters();
    
    // Helper functions
    double calculateLinkQuality(int nodeA, int nodeB);
    double calculateNodeMobility(int nodeId);
    void broadcastRouteDiscovery(int destId);
    void cleanupExpiredRoutes();
    
  public:
    BatRouting();
    virtual ~BatRouting();
    
    // Public interface for other modules
    int getMyNodeId() const { return myNodeId; }
    std::vector<int> getNeighborIds();
};

#endif
