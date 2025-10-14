/*
 * BatAlgorithm.cc
 * 
 * Implementation of Bat Algorithm for UAV swarm optimization
 * 
 * Based on: Yang, X. S. (2010). "A new metaheuristic bat-inspired algorithm"
 * 
 * Simplified version focused on:
 * - Target convergence
 * - Static obstacle avoidance
 */

#include "BatAlgorithm.h"
#include "Obstacle.h"
#include "inet/common/ModuleAccess.h"
#include <algorithm>
#include <cmath>

Define_Module(BatAlgorithm);

BatAlgorithm::BatAlgorithm()
{
    updateTimer = nullptr;
    mobility = nullptr;
    obstaclesAvoided = 0;
}

BatAlgorithm::~BatAlgorithm()
{
    cancelAndDelete(updateTimer);
}

void BatAlgorithm::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    
    if (stage == INITSTAGE_LOCAL) {
        // Read parameters
        frequencyMin = par("frequencyMin");
        frequencyMax = par("frequencyMax");
        loudness = par("loudness");
        pulseRate = par("pulseRate");
        alpha = par("alpha");
        gamma = par("gamma");
        updateInterval = par("updateInterval");
        
        // Obstacle avoidance parameters
        safetyDistance = par("safetyDistance");
        detectionRange = par("detectionRange");
        obstacleWeight = par("obstacleWeight");
        uavSafetyDistance = par("uavSafetyDistance");
        
        // Initialize target position
        targetPosition.x = par("targetX");
        targetPosition.y = par("targetY");
        targetPosition.z = par("targetZ");
        targetRadius = par("targetRadius");
        
        // Initialize state
        currentLoudness = loudness;
        currentPulseRate = pulseRate;
        frequency = frequencyMin;
        obstaclesAvoided = 0;
        
        updateTimer = new cMessage("batUpdate");
        
        EV << "BatAlgorithm initialized: target at (" << targetPosition.x 
           << ", " << targetPosition.y << ", " << targetPosition.z << ")" << endl;
    }
    else if (stage == INITSTAGE_PHYSICAL_ENVIRONMENT) {
        // Get reference to mobility module
        mobility = check_and_cast<ArbitraryMobility*>(
            getParentModule()->getSubmodule("mobility"));
        
        if (!mobility) {
            throw cRuntimeError("BatAlgorithm requires ArbitraryMobility module");
        }
        
        // Initialize position from mobility
        currentPosition = mobility->getCurrentPosition();
        velocity = mobility->getCurrentVelocity();
        
        // Initialize personal best
        personalBest = currentPosition;
        personalBestFitness = calculateFitness(currentPosition);
        fitness = personalBestFitness;
        
        // Discover obstacles in the network
        discoverObstacles();
        
        EV << "BatAlgorithm: Initial position (" << currentPosition.x 
           << ", " << currentPosition.y << ", " << currentPosition.z 
           << "), fitness: " << fitness << endl;
        EV << "Found " << obstacles.size() << " obstacles" << endl;
    }
    else if (stage == INITSTAGE_LAST) {
        // Schedule first update
        scheduleAt(simTime() + updateInterval, updateTimer);
    }
}

void BatAlgorithm::handleMessage(cMessage *msg)
{
    if (msg == updateTimer) {
        // ============================================================
        // PHASE 1: STATE ASSESSMENT
        // ============================================================
        
        // Get current position from mobility
        currentPosition = mobility->getCurrentPosition();
        
        // Calculate current fitness
        fitness = calculateFitness(currentPosition);
        
        // Update personal best
        if (fitness < personalBestFitness) {
            personalBest = currentPosition;
            personalBestFitness = fitness;
            EV << "BatAlgorithm: New personal best! Fitness: " << fitness << endl;
        }
        
        // Get global best position (from other bats)
        Coord globalBest = getGlobalBest();
        
        EV_INFO << "BAT[" << getParentModule()->getIndex() << "] t=" << simTime() 
                << " pos=(" << (int)currentPosition.x << "," << (int)currentPosition.y << "," << (int)currentPosition.z << ")"
                << " fitness=" << (int)fitness << " loud=" << currentLoudness << endl;
        
        // ============================================================
        // PHASE 2: VELOCITY UPDATE (Bat Algorithm Equation)
        // ============================================================
        
        // Update frequency (random within range)
        frequency = frequencyMin + (frequencyMax - frequencyMin) * uniform(0, 1);
        
        // Update velocity using bat algorithm equations WITH INERTIA
        // This prevents infinite velocity accumulation
        updateVelocity();
        
        // CRITICAL: Limit velocity BEFORE calculating position
        // This ensures maxSpeed constraint is respected
        double speed = velocity.length();
        double maxSpeed = mobility->getMaxSpeed();
        if (speed > maxSpeed) {
            velocity = velocity * (maxSpeed / speed);
        }
        
        // ============================================================
        // PHASE 3: POSITION CALCULATION
        // ============================================================
        
        // Generate new solution using CORRECT time integration
        // CRITICAL: Must multiply by updateInterval (Δt)
        // Without this, drones will "jump" instead of moving smoothly
        Coord newPosition = currentPosition + velocity * updateInterval;
        
        // Apply boundary constraints with safety margins
        // This prevents drones from leaving simulation area
        double minX = mobility->par("constraintAreaMinX");
        double maxX = mobility->par("constraintAreaMaxX");
        double minY = mobility->par("constraintAreaMinY");
        double maxY = mobility->par("constraintAreaMaxY");
        double minZ = mobility->par("constraintAreaMinZ");
        double maxZ = mobility->par("constraintAreaMaxZ");
        
        // Clamp position within bounds (5m safety margin)
        newPosition.x = std::max(minX + 5.0, std::min(maxX - 5.0, newPosition.x));
        newPosition.y = std::max(minY + 5.0, std::min(maxY - 5.0, newPosition.y));
        newPosition.z = std::max(minZ + 5.0, std::min(maxZ - 5.0, newPosition.z));
        
        // ============================================================
        // PHASE 4: SOLUTION ACCEPTANCE (Bat Algorithm Criteria)
        // ============================================================
        
        // Evaluate new solution
        double newFitness = calculateFitness(newPosition);
        
        // Accept new solution if it's better AND random < loudness
        // This is the main Bat Algorithm acceptance criterion
        if (newFitness < fitness && uniform(0, 1) < currentLoudness) {
            // CRITICAL: Recalculate velocity from actual movement
            // This ensures smooth transitions and correct physics
            velocity = (newPosition - currentPosition) / updateInterval;
            
            // Apply new position and velocity to mobility module
            mobility->setPositionVelocity(newPosition, velocity);
            
            // Update loudness and pulse rate (Bat Algorithm dynamics)
            updateLoudnessAndPulseRate();
            
            EV << "BatAlgorithm: Accepted new position, fitness: " << newFitness 
               << ", loudness: " << currentLoudness 
               << ", pulse rate: " << currentPulseRate << endl;
        }
        // ============================================================
        // PHASE 5: LOCAL SEARCH (if main solution rejected)
        // ============================================================
        // Local search: generate local solution around best solution
        // This is triggered when random > currentPulseRate
        else if (uniform(0, 1) > currentPulseRate) {
            // Small random walk around global best
            // Search radius scales with loudness (larger when exploring)
            double searchRadius = 5.0 * currentLoudness;
            Coord localPosition = globalBest + Coord(
                uniform(-searchRadius, searchRadius),
                uniform(-searchRadius, searchRadius),
                uniform(-searchRadius/2, searchRadius/2)  // Less variation in Z
            );
            
            // Apply same boundary constraints
            localPosition.x = std::max(minX + 5.0, std::min(maxX - 5.0, localPosition.x));
            localPosition.y = std::max(minY + 5.0, std::min(maxY - 5.0, localPosition.y));
            localPosition.z = std::max(minZ + 5.0, std::min(maxZ - 5.0, localPosition.z));
            
            double localFitness = calculateFitness(localPosition);
            
            // Accept local solution if better
            if (localFitness < fitness) {
                // CRITICAL: Calculate velocity for SMOOTH transition
                // Never set position directly without velocity update
                velocity = (localPosition - currentPosition) / updateInterval;
                
                // Limit velocity to respect maxSpeed
                speed = velocity.length();
                if (speed > maxSpeed) {
                    velocity = velocity * (maxSpeed / speed);
                }
                
                mobility->setPositionVelocity(localPosition, velocity);
                
                EV << "BatAlgorithm: Local search found better solution, fitness: " 
                   << localFitness << endl;
            }
        }
        
        // ============================================================
        // PHASE 6: COMMUNICATION AND SCHEDULING
        // ============================================================
        
        // Broadcast position (for global best computation)
        broadcastPosition();
        
        // Schedule next update
        scheduleAt(simTime() + updateInterval, updateTimer);
    }
}

void BatAlgorithm::updateVelocity()
{
    // Get global best
    Coord globalBest = getGlobalBest();
    
    // Bat algorithm velocity update equation:
    // v_i(t+1) = v_i(t) * w + (x_best - x_i(t)) * frequency
    // 
    // KEY IMPLEMENTATION POINTS:
    // 
    // 1. DIRECTION: (globalBest - currentPosition)
    //    - This makes the bat move TOWARDS the target
    //    - Common mistake: (currentPosition - globalBest) moves AWAY
    // 
    // 2. INERTIA FACTOR (w = 0.5):
    //    - Prevents infinite velocity accumulation
    //    - 0.5 = retains 50% of previous velocity
    //    - Provides damping for smooth convergence
    //    - Without this, velocity grows unbounded
    // 
    // 3. FREQUENCY (f_i):
    //    - Controls step size towards target
    //    - Updated randomly each iteration: f ∈ [f_min, f_max]
    //    - Higher frequency = larger steps (more exploration)
    //    - Lower frequency = smaller steps (more exploitation)
    
    double inertia = 0.5; // Inertia/damping factor
    
    velocity = velocity * inertia + (globalBest - currentPosition) * frequency;
}

double BatAlgorithm::calculateFitness(const Coord& position)
{
    // Simple fitness: distance to target + obstacle penalty
    double fitness = position.distance(targetPosition);
    
    // Add obstacle penalty (high cost for being near obstacles)
    double obstaclePenalty = calculateObstaclePenalty(position);
    fitness += obstaclePenalty * obstacleWeight;
    
    // Add UAV proximity penalty (avoid collisions between drones)
    double uavPenalty = calculateUAVProximityPenalty(position);
    fitness += uavPenalty;
    
    return fitness;
}

void BatAlgorithm::updateLoudnessAndPulseRate()
{
    // Loudness decreases as bat approaches prey
    // A_i(t+1) = alpha * A_i(t)
    currentLoudness = alpha * currentLoudness;
    
    // Pulse rate increases as bat approaches prey
    // r_i(t+1) = r_i(0) * [1 - exp(-gamma * t)]
    currentPulseRate = pulseRate * (1 - exp(-gamma * simTime().dbl()));
}

Coord BatAlgorithm::getGlobalBest()
{
    // In a real swarm, this would query other bats
    // For now, we use the target as the global best approximation
    // In a more complete implementation, bats would communicate via messages
    
    // Try to find other bat modules in the network
    cModule *network = getParentModule()->getParentModule();
    if (network) {
        double bestFitness = personalBestFitness;
        Coord bestPosition = personalBest;
        
        for (int i = 0; i < network->getSubmoduleVectorSize("uav"); i++) {
            cModule *uav = network->getSubmodule("uav", i);
            if (uav && uav != getParentModule()) {
                BatAlgorithm *otherBat = check_and_cast<BatAlgorithm*>(
                    uav->getSubmodule("batAlgorithm"));
                if (otherBat) {
                    double otherFitness = otherBat->getFitness();
                    if (otherFitness < bestFitness) {
                        bestFitness = otherFitness;
                        bestPosition = otherBat->getPersonalBest();
                    }
                }
            }
        }
        
        return bestPosition;
    }
    
    return personalBest;
}

void BatAlgorithm::broadcastPosition()
{
    // Emit signal for communication visualization
    emit(registerSignal("positionBroadcast"), simTime());
    
    // Log broadcast for visibility in simulator
    EV << "BAT[" << getParentModule()->getIndex() << "] Broadcasting: "
       << "pos=(" << (int)currentPosition.x << "," << (int)currentPosition.y << ","  << (int)currentPosition.z << ") "
       << "fitness=" << (int)fitness << endl;
}

void BatAlgorithm::finish()
{
    // Record final statistics
    recordScalar("finalFitness", fitness);
    recordScalar("finalDistanceToTarget", currentPosition.distance(targetPosition));
    recordScalar("personalBestFitness", personalBestFitness);
    recordScalar("obstaclesAvoided", obstaclesAvoided);
    
    EV << "BatAlgorithm finished:" << endl;
    EV << "  Final fitness = " << fitness << endl;
    EV << "  Personal best = " << personalBestFitness << endl;
    EV << "  Obstacles avoided = " << obstaclesAvoided << endl;
}

// ============================================================
// OBSTACLE AVOIDANCE METHODS
// ============================================================

void BatAlgorithm::discoverObstacles()
{
    obstacles.clear();
    
    cModule *network = getParentModule()->getParentModule();
    if (!network) return;
    
    // Find all Obstacle modules in the network (both "obstacle" and "dynamicObstacle" arrays)
    // Use try-catch to handle cases where arrays don't exist
    try {
        int numObstacles = network->getSubmoduleVectorSize("obstacle");
        for (int i = 0; i < numObstacles; i++) {
            cModule *obsMod = network->getSubmodule("obstacle", i);
            if (obsMod) {
                Obstacle *obs = check_and_cast<Obstacle*>(obsMod);
                obstacles.push_back(obs);
                EV << "Discovered obstacle at (" << obs->getPosition().x << ", " 
                   << obs->getPosition().y << "), radius: " << obs->getRadius() << "m" << endl;
            }
        }
    } catch (...) {
        // No obstacle array in this network
    }
    
    // Also check for dynamicObstacle array
    try {
        int numDynamic = network->getSubmoduleVectorSize("dynamicObstacle");
        for (int i = 0; i < numDynamic; i++) {
            cModule *obsMod = network->getSubmodule("dynamicObstacle", i);
            if (obsMod) {
                Obstacle *obs = check_and_cast<Obstacle*>(obsMod);
                obstacles.push_back(obs);
                EV << "Discovered dynamic obstacle at (" << obs->getPosition().x << ", " 
                   << obs->getPosition().y << "), radius: " << obs->getRadius() << "m" << endl;
            }
        }
    } catch (...) {
        // No dynamicObstacle array in this network
    }
}

double BatAlgorithm::calculateObstaclePenalty(const Coord& position)
{
    double penalty = 0.0;
    
    for (auto obs : obstacles) {
        double distToEdge = obs->getDistanceToEdge(position);
        
        if (distToEdge < 0) {
            // Inside obstacle - VERY high penalty
            penalty += 10000.0 - distToEdge * 100.0;
        } else if (distToEdge < safetyDistance) {
            // Too close - penalty increases as distance decreases
            penalty += (safetyDistance - distToEdge) * 100.0;
        }
    }
    
    return penalty;
}

double BatAlgorithm::calculateUAVProximityPenalty(const Coord& position)
{
    double penalty = 0.0;
    
    cModule *network = getParentModule()->getParentModule();
    if (!network) return penalty;
    
    // Check distance to other UAVs
    for (int i = 0; i < network->getSubmoduleVectorSize("uav"); i++) {
        cModule *uav = network->getSubmodule("uav", i);
        if (uav && uav != getParentModule()) {
            BatAlgorithm *otherBat = check_and_cast<BatAlgorithm*>(
                uav->getSubmodule("batAlgorithm"));
            if (otherBat) {
                double dist = position.distance(otherBat->getPosition());
                if (dist < uavSafetyDistance) {
                    // Penalty increases as drones get closer
                    penalty += (uavSafetyDistance - dist) * 50.0;
                }
            }
        }
    }
    
    return penalty;
}

bool BatAlgorithm::isPositionSafe(const Coord& position)
{
    for (auto obs : obstacles) {
        if (obs->checkCollision(position, safetyDistance)) {
            return false;
        }
    }
    return true;
}
