/*
 * BatAlgorithm.h
 * 
 * Implementation of Bat Algorithm for UAV swarm optimization
 * Based on: Yang, X. S. (2010). A new metaheuristic bat-inspired algorithm.
 * 
 * Simplified version focused on:
 * - Target convergence
 * - Static obstacle avoidance
 * 
 * The Bat Algorithm simulates the echolocation behavior of bats:
 * - Frequency tuning: Bats adjust frequency to detect prey
 * - Loudness: Decreases as bat approaches prey
 * - Pulse rate: Increases as bat approaches prey
 */

#ifndef BATALGORITHM_H_
#define BATALGORITHM_H_

#include <omnetpp.h>
#include "inet/common/geometry/common/Coord.h"
#include "ArbitraryMobility.h"
#include "Obstacle.h"
#include <vector>

using namespace omnetpp;
using namespace inet;

class INET_API BatAlgorithm : public cSimpleModule {
private:
    // Bat Algorithm parameters
    double frequencyMin;      // Minimum frequency (Hz)
    double frequencyMax;      // Maximum frequency (Hz)
    double loudness;          // Loudness A (0-1)
    double pulseRate;         // Pulse emission rate r (0-1)
    double alpha;             // Loudness reduction factor
    double gamma;             // Pulse rate increase factor
    
    // Target parameters
    Coord targetPosition;     // Global best position (target)
    double targetRadius;      // Radius around target
    
    // State variables
    Coord currentPosition;
    Coord velocity;
    double frequency;
    double currentLoudness;
    double currentPulseRate;
    double fitness;           // Distance to target + obstacle penalty
    
    // Best positions
    Coord personalBest;
    double personalBestFitness;
    
    // Obstacle avoidance
    std::vector<Obstacle*> obstacles;
    double safetyDistance;
    double detectionRange;
    double obstacleWeight;    // Weight for obstacle penalty
    double uavSafetyDistance; // Minimum distance between UAVs
    
    // References
    ArbitraryMobility *mobility;
    cMessage *updateTimer;
    
    // Update interval
    double updateInterval;
    
    // Statistics
    int obstaclesAvoided;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    
    // Bat Algorithm methods
    virtual void updateVelocity();
    virtual double calculateFitness(const Coord& position);
    virtual void updateLoudnessAndPulseRate();
    virtual Coord getGlobalBest();
    virtual void broadcastPosition();
    
    // Obstacle avoidance methods
    void discoverObstacles();
    double calculateObstaclePenalty(const Coord& position);
    double calculateUAVProximityPenalty(const Coord& position);
    bool isPositionSafe(const Coord& position);

public:
    BatAlgorithm();
    virtual ~BatAlgorithm();
    
    // Public interface
    virtual Coord getPosition() const { return currentPosition; }
    virtual double getFitness() const { return fitness; }
    virtual Coord getPersonalBest() const { return personalBest; }
    virtual int getObstaclesAvoided() const { return obstaclesAvoided; }
};

#endif /* BATALGORITHM_H_ */
