//
// BatRouting.cc
// Implementation of Bat Algorithm-based routing for FANETs
//

#include "BatRouting.h"
#include "inet/common/ModuleAccess.h"
#include <algorithm>

Define_Module(BatRouting);

BatRouting::BatRouting()
{
    routeUpdateTimer = nullptr;
    myNodeId = -1;
}

BatRouting::~BatRouting()
{
    cancelAndDelete(routeUpdateTimer);
}

void BatRouting::initialize()
{
    // Get node ID from parent module
    cModule *parent = getParentModule();
    if (!parent) {
        EV_ERROR << "BatRouting: Cannot find parent module" << endl;
        return;
    }
    myNodeId = parent->getIndex();
    
    // Initialize Bat Algorithm parameters
    frequencyMin = par("frequencyMin");
    frequencyMax = par("frequencyMax");
    initialLoudness = par("loudness");
    initialPulseRate = par("pulseRate");
    currentLoudness = initialLoudness;
    currentPulseRate = initialPulseRate;
    alpha = par("alpha");
    gamma = par("gamma");
    
    // Initialize routing parameters
    routingUpdateInterval = par("routingUpdateInterval");
    hopCountWeight = par("hopCountWeight");
    linkQualityWeight = par("linkQualityWeight");
    energyWeight = par("energyWeight");
    mobilityWeight = par("mobilityWeight");
    maxRoutesPerDestination = par("maxRoutesPerDestination");
    routeTimeout = par("routeTimeout");
    
    // Register signals
    routeDiscoveredSignal = registerSignal("routeDiscovered");
    packetRoutedSignal = registerSignal("packetRouted");
    
    // Schedule first route discovery (delayed to allow other modules to initialize)
    routeUpdateTimer = new cMessage("routeUpdate");
    scheduleAt(simTime() + uniform(2, 3), routeUpdateTimer);
    
    EV << "BatRouting: Node " << myNodeId << " initialized" << endl;
}

void BatRouting::handleMessage(cMessage *msg)
{
    if (msg == routeUpdateTimer) {
        // Periodic route discovery and optimization
        discoverRoutes();
        optimizeRouteTable();
        cleanupExpiredRoutes();
        
        // Schedule next update
        scheduleAt(simTime() + routingUpdateInterval, routeUpdateTimer);
    }
    else if (auto routePkt = dynamic_cast<RouteDiscoveryPacket*>(msg)) {
        // Process route discovery packet
        processRouteDiscovery(routePkt);
    }
    else if (auto dataPkt = dynamic_cast<DataPacket*>(msg)) {
        // Route data packet
        routeDataPacket(dataPkt);
    }
    else {
        // Unknown message
        delete msg;
    }
}

void BatRouting::discoverRoutes()
{
    // Get all nodes in network
    cModule *network = getParentModule()->getParentModule();
    if (!network) return;
    
    int numNodes = network->getSubmoduleVectorSize("uav");
    
    // Discover routes to all other nodes using Bat Algorithm approach
    for (int destId = 0; destId < numNodes; destId++) {
        if (destId == myNodeId) continue;
        
        // Use Bat Algorithm frequency to decide if we explore new route
        double frequency = frequencyMin + (frequencyMax - frequencyMin) * uniform(0, 1);
        
        // With probability based on pulse rate, try route discovery
        if (uniform(0, 1) < currentPulseRate) {
            broadcastRouteDiscovery(destId);
        }
    }
    
    // Update Bat Algorithm parameters
    updateBatParameters();
}

void BatRouting::broadcastRouteDiscovery(int destId)
{
    RouteDiscoveryPacket *pkt = new RouteDiscoveryPacket("RouteDiscovery");
    pkt->sourceId = myNodeId;
    pkt->destId = destId;
    pkt->visitedNodes.push_back(myNodeId);
    pkt->accumulatedFitness = 0.0;
    
    // Add display properties for animation
    char msgName[32];
    sprintf(msgName, "RREQ %d->%d", myNodeId, destId);
    pkt->setName(msgName);
    
    // Broadcast to all neighbors (simulated)
    // In real implementation, would send via lowerLayerOut
    cModule *parent = getParentModule();
    if (!parent) {
        delete pkt;
        return;
    }
    
    cModule *network = parent->getParentModule();
    if (!network) {
        delete pkt;
        return;
    }
    
    int numNodes = network->getSubmoduleVectorSize("uav");
    for (int i = 0; i < numNodes; i++) {
        if (i == myNodeId) continue;
        
        cModule *otherUav = network->getSubmodule("uav", i);
        if (!otherUav) continue;
        
        cModule *myMobModule = parent->getSubmodule("mobility");
        cModule *otherMobModule = otherUav->getSubmodule("mobility");
        
        if (!myMobModule || !otherMobModule) continue;
        
        ArbitraryMobility *myMob = check_and_cast<ArbitraryMobility*>(myMobModule);
        ArbitraryMobility *otherMob = check_and_cast<ArbitraryMobility*>(otherMobModule);
        
        try {
            double dist = myMob->getCurrentPosition().distance(otherMob->getCurrentPosition());
            if (dist < 300.0) { // Communication range 300m
                RouteDiscoveryPacket *copy = dynamic_cast<RouteDiscoveryPacket*>(pkt->dup());
                if (copy) {
                    // Send to other UAV's radioIn gate (visible in animation)
                    sendDirect(copy, otherUav, "radioIn");
                }
            }
        } catch (const std::exception &e) {
            EV_WARN << "BatRouting: Error checking distance to node " << i << ": " << e.what() << endl;
        }
    }
    
    delete pkt;
}

void BatRouting::processRouteDiscovery(RouteDiscoveryPacket *pkt)
{
    // Check if already visited this node (avoid loops)
    for (int nodeId : pkt->visitedNodes) {
        if (nodeId == myNodeId) {
            delete pkt;
            return;
        }
    }
    
    // Add this node to path
    pkt->visitedNodes.push_back(myNodeId);
    
    // Update accumulated fitness
    if (pkt->visitedNodes.size() > 1) {
        int prevNode = pkt->visitedNodes[pkt->visitedNodes.size() - 2];
        double linkQuality = calculateLinkQuality(prevNode, myNodeId);
        pkt->accumulatedFitness += (1.0 / (linkQuality + 0.1)) * hopCountWeight;
    }
    
    // If we reached destination
    if (pkt->destId == myNodeId) {
        // Create route info
        RouteInfo route;
        route.path = pkt->visitedNodes;
        route.hopCount = pkt->visitedNodes.size() - 1;
        route.fitness = pkt->accumulatedFitness;
        route.lastUpdate = simTime();
        
        // Send route back to source (in real impl, would use reverse path)
        cModule *parent = getParentModule();
        if (parent) {
            cModule *network = parent->getParentModule();
            if (network) {
                cModule *sourceUav = network->getSubmodule("uav", pkt->sourceId);
                if (sourceUav) {
                    cModule *sourceRoutingModule = sourceUav->getSubmodule("batRouting");
                    if (sourceRoutingModule) {
                        try {
                            BatRouting *sourceRouting = check_and_cast<BatRouting*>(sourceRoutingModule);
                            sourceRouting->updateRouteTable(myNodeId, route);
                        } catch (const std::exception &e) {
                            EV_WARN << "BatRouting: Error updating route table: " << e.what() << endl;
                        }
                    }
                }
            }
        }
        
        emit(routeDiscoveredSignal, 1);
        EV << "BatRouting: Node " << myNodeId << " - Route discovered from " 
           << pkt->sourceId << " with " << route.hopCount << " hops" << endl;
        
        delete pkt;
        return;
    }
    
    // Continue forwarding if TTL allows
    if (pkt->visitedNodes.size() < 10) { // Max 10 hops
        // Forward to neighbors with probability based on loudness
        if (uniform(0, 1) < currentLoudness) {
            cModule *parent = getParentModule();
            if (!parent) {
                delete pkt;
                return;
            }
            
            cModule *network = parent->getParentModule();
            if (network) {
                int numNodes = network->getSubmoduleVectorSize("uav");
                for (int i = 0; i < numNodes; i++) {
                    if (i == myNodeId) continue;
                    
                    // Check if already in path
                    bool alreadyVisited = false;
                    for (int nodeId : pkt->visitedNodes) {
                        if (nodeId == i) {
                            alreadyVisited = true;
                            break;
                        }
                    }
                    if (alreadyVisited) continue;
                    
                    cModule *otherUav = network->getSubmodule("uav", i);
                    if (!otherUav) continue;
                    
                    cModule *myMobModule = parent->getSubmodule("mobility");
                    cModule *otherMobModule = otherUav->getSubmodule("mobility");
                    
                    if (!myMobModule || !otherMobModule) continue;
                    
                    try {
                        ArbitraryMobility *myMob = check_and_cast<ArbitraryMobility*>(myMobModule);
                        ArbitraryMobility *otherMob = check_and_cast<ArbitraryMobility*>(otherMobModule);
                        
                        double dist = myMob->getCurrentPosition().distance(otherMob->getCurrentPosition());
                        if (dist < 300.0) {
                            RouteDiscoveryPacket *copy = dynamic_cast<RouteDiscoveryPacket*>(pkt->dup());
                            if (copy) {
                                // Send to other UAV's radioIn gate (visible in animation)
                                sendDirect(copy, otherUav, "radioIn");
                            }
                        }
                    } catch (const std::exception &e) {
                        EV_WARN << "BatRouting: Error forwarding to node " << i << ": " << e.what() << endl;
                    }
                }
            }
        }
    }
    
    delete pkt;
}

void BatRouting::updateRouteTable(int dest, const RouteInfo &route)
{
    auto &routes = routeTable[dest];
    
    // Add new route
    routes.push_back(route);
    
    // Sort by fitness (lower is better)
    std::sort(routes.begin(), routes.end(), 
        [](const RouteInfo &a, const RouteInfo &b) {
            return a.fitness < b.fitness;
        });
    
    // Keep only top-N routes
    if (routes.size() > (size_t)maxRoutesPerDestination) {
        routes.resize(maxRoutesPerDestination);
    }
    
    EV << "BatRouting: Node " << myNodeId << " - Updated route to " << dest 
       << " (fitness: " << route.fitness << ")" << endl;
}

RouteInfo* BatRouting::selectBestRoute(int dest)
{
    auto it = routeTable.find(dest);
    if (it == routeTable.end() || it->second.empty()) {
        return nullptr;
    }
    
    // Return route with best fitness
    return &(it->second[0]);
}

void BatRouting::routeDataPacket(DataPacket *pkt)
{
    // This would be called when receiving data to route
    emit(packetRoutedSignal, 1);
    
    RouteInfo *route = selectBestRoute(pkt->destId);
    if (route) {
        pkt->routePath = route->path;
        pkt->currentHop = 0;
        
        // Forward to next hop (implementation would send via lowerLayerOut)
        EV << "BatRouting: Node " << myNodeId << " - Routing packet to " 
           << pkt->destId << " via route with " << route->hopCount << " hops" << endl;
    }
    
    delete pkt;
}

double BatRouting::calculateRouteFitness(const RouteInfo &route)
{
    double fitness = 0.0;
    
    // Hop count penalty
    fitness += route.hopCount * hopCountWeight;
    
    // Link quality (inverse)
    fitness += (1.0 / (route.linkQuality + 0.1)) * linkQualityWeight;
    
    // Energy cost
    fitness += route.energyCost * energyWeight;
    
    // Mobility penalty (more mobile = less stable route)
    for (size_t i = 0; i < route.path.size(); i++) {
        double mobility = calculateNodeMobility(route.path[i]);
        fitness += mobility * mobilityWeight;
    }
    
    return fitness;
}

void BatRouting::optimizeRouteTable()
{
    // Use Bat Algorithm to refine route selection
    for (auto &entry : routeTable) {
        auto &routes = entry.second;
        
        // Recalculate fitness for all routes
        for (auto &route : routes) {
            route.fitness = calculateRouteFitness(route);
        }
        
        // Re-sort by fitness
        std::sort(routes.begin(), routes.end(),
            [](const RouteInfo &a, const RouteInfo &b) {
                return a.fitness < b.fitness;
            });
    }
}

void BatRouting::updateBatParameters()
{
    // Update loudness (decreases over time)
    currentLoudness = alpha * currentLoudness;
    
    // Update pulse rate (increases over time)
    currentPulseRate = initialPulseRate * (1.0 - exp(-gamma * simTime().dbl()));
    
    // Prevent values from going to extremes
    if (currentLoudness < 0.1) currentLoudness = 0.1;
    if (currentPulseRate > 0.95) currentPulseRate = 0.95;
}

double BatRouting::calculateLinkQuality(int nodeA, int nodeB)
{
    cModule *parent = getParentModule();
    if (!parent) return 0.0;
    
    cModule *network = parent->getParentModule();
    if (!network) return 0.0;
    
    cModule *uavA = network->getSubmodule("uav", nodeA);
    cModule *uavB = network->getSubmodule("uav", nodeB);
    if (!uavA || !uavB) return 0.0;
    
    cModule *mobA = uavA->getSubmodule("mobility");
    cModule *mobB = uavB->getSubmodule("mobility");
    if (!mobA || !mobB) return 0.0;
    
    try {
        ArbitraryMobility *mA = check_and_cast<ArbitraryMobility*>(mobA);
        ArbitraryMobility *mB = check_and_cast<ArbitraryMobility*>(mobB);
        
        double dist = mA->getCurrentPosition().distance(mB->getCurrentPosition());
        
        // Link quality decreases with distance (1.0 at 0m, 0.0 at 300m)
        double quality = std::max(0.0, 1.0 - (dist / 300.0));
        return quality;
    } catch (const std::exception &e) {
        EV_WARN << "BatRouting: Error calculating link quality: " << e.what() << endl;
        return 0.0;
    }
}

double BatRouting::calculateNodeMobility(int nodeId)
{
    // Return constant low mobility value
    return 0.1;
}

void BatRouting::cleanupExpiredRoutes()
{
    for (auto it = routeTable.begin(); it != routeTable.end(); ) {
        auto &routes = it->second;
        
        // Remove expired routes
        routes.erase(
            std::remove_if(routes.begin(), routes.end(),
                [this](const RouteInfo &route) {
                    return (simTime() - route.lastUpdate) > routeTimeout;
                }),
            routes.end()
        );
        
        // Remove entry if no routes left
        if (routes.empty()) {
            it = routeTable.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<int> BatRouting::getNeighborIds()
{
    std::vector<int> neighbors;
    
    cModule *parent = getParentModule();
    if (!parent) return neighbors;
    
    cModule *network = parent->getParentModule();
    if (!network) return neighbors;
    
    cModule *myMobModule = parent->getSubmodule("mobility");
    if (!myMobModule) return neighbors;
    
    ArbitraryMobility *myMob = check_and_cast<ArbitraryMobility*>(myMobModule);
    
    int numNodes = network->getSubmoduleVectorSize("uav");
    for (int i = 0; i < numNodes; i++) {
        if (i == myNodeId) continue;
        
        cModule *otherUav = network->getSubmodule("uav", i);
        if (otherUav) {
            cModule *otherMobModule = otherUav->getSubmodule("mobility");
            if (otherMobModule) {
                ArbitraryMobility *otherMob = check_and_cast<ArbitraryMobility*>(otherMobModule);
                
                double dist = myMob->getCurrentPosition().distance(otherMob->getCurrentPosition());
                if (dist < 300.0) { // Communication range
                    neighbors.push_back(i);
                }
            }
        }
    }
    
    return neighbors;
}

void BatRouting::finish()
{
    // Statistics
    EV << "BatRouting: Node " << myNodeId << " - Routes in table: " 
       << routeTable.size() << endl;
    
    for (const auto &entry : routeTable) {
        EV << "  Destination " << entry.first << ": " 
           << entry.second.size() << " routes" << endl;
    }
}
