/*
 * ArbitraryMobility.cc
 * 
 * Custom mobility model for FANET UAVs
 * Updated for compatibility with INET 4.5.4 and OMNeT++ 6.2.0
 */

#include "ArbitraryMobility.h"
#include "inet/common/ModuleAccess.h"
#include <cmath>

using namespace omnetpp;
using namespace inet;

Define_Module(ArbitraryMobility);

ArbitraryMobility::ArbitraryMobility()
{
    lastUpdate = 0;
    moveTimer = nullptr;
}

ArbitraryMobility::~ArbitraryMobility()
{
    cancelAndDelete(moveTimer);
}

void ArbitraryMobility::initialize(int stage)
{
    MovingMobilityBase::initialize(stage);
    
    if (stage == INITSTAGE_LOCAL) {
        // Read boundary parameters
        constraintAreaMinX = par("constraintAreaMinX");
        constraintAreaMaxX = par("constraintAreaMaxX");
        constraintAreaMinY = par("constraintAreaMinY");
        constraintAreaMaxY = par("constraintAreaMaxY");
        minAltitude = par("constraintAreaMinZ");
        maxAltitude = par("constraintAreaMaxZ");
        
        // Create movement timer
        moveTimer = new cMessage("moveTimer");
        
        EV << "ArbitraryMobility: Boundary area: X[" << constraintAreaMinX << "," << constraintAreaMaxX 
           << "] Y[" << constraintAreaMinY << "," << constraintAreaMaxY 
           << "] Z[" << minAltitude << "," << maxAltitude << "]" << endl;
    }
    else if (stage == INITSTAGE_LAST) {
        // Schedule first movement update
        double updateInterval = par("updateInterval");
        scheduleAt(simTime() + updateInterval, moveTimer);
    }
}

void ArbitraryMobility::setInitialPosition()
{
    // Read initial position from parameters
    double x = par("initialX");
    double y = par("initialY");
    double z = par("initialZ");
    
    // If Z is not set (0), use random altitude within bounds
    if (z == 0) {
        z = uniform(minAltitude, maxAltitude, 0);
    }
    
    lastPosition = Coord(x, y, z);
    
    // Set initial velocity with random direction
    double speed = uniform(par("minSpeed").doubleValue(), par("maxSpeed").doubleValue());
    double angleXY = uniform(0, 2 * M_PI, 0);
    double angleZ = uniform(-M_PI/6, M_PI/6, 0); // ±30 degrees vertical
    
    lastVelocity.x = speed * cos(angleXY) * cos(angleZ);
    lastVelocity.y = speed * sin(angleXY) * cos(angleZ);
    lastVelocity.z = speed * sin(angleZ);
    
    EV << "ArbitraryMobility: setInitialPosition called with (" << x << ", " << y << ", " << z << ")" << endl;
}

void ArbitraryMobility::move()
{
    simtime_t now = simTime();
    
    if (now > lastUpdate) {
        simtime_t timeDelta = now - lastUpdate;
        
        // Update position: new_position = current_position + velocity * time
        Coord newPosition = lastPosition + lastVelocity * timeDelta.dbl();
        
        // Apply boundary constraints with bounce
        bool bounced = false;
        const double MARGIN = 5.0; // Margem maior para evitar oscilação
        
        if (newPosition.x <= constraintAreaMinX) {
            newPosition.x = constraintAreaMinX + MARGIN;
            lastVelocity.x = abs(lastVelocity.x); // Força direção positiva
            bounced = true;
        } else if (newPosition.x >= constraintAreaMaxX) {
            newPosition.x = constraintAreaMaxX - MARGIN;
            lastVelocity.x = -abs(lastVelocity.x); // Força direção negativa
            bounced = true;
        }
        
        if (newPosition.y <= constraintAreaMinY) {
            newPosition.y = constraintAreaMinY + MARGIN;
            lastVelocity.y = abs(lastVelocity.y);
            bounced = true;
        } else if (newPosition.y >= constraintAreaMaxY) {
            newPosition.y = constraintAreaMaxY - MARGIN;
            lastVelocity.y = -abs(lastVelocity.y);
            bounced = true;
        }
        
        if (newPosition.z <= minAltitude) {
            newPosition.z = minAltitude + MARGIN;
            lastVelocity.z = abs(lastVelocity.z);
            bounced = true;
        } else if (newPosition.z >= maxAltitude) {
            newPosition.z = maxAltitude - MARGIN;
            lastVelocity.z = -abs(lastVelocity.z);
            bounced = true;
        }
        
        // Add some randomness to direction (menos frequente se acabou de rebater)
        if (!bounced && uniform(0, 1, 0) < 0.1) { // 10% chance of direction change
            double angleXY = uniform(0, 2 * M_PI, 0);
            double angleZ = uniform(-M_PI/6, M_PI/6, 0); // ±30 degrees vertical
            double speed = lastVelocity.length();
            lastVelocity.x = speed * cos(angleXY) * cos(angleZ);
            lastVelocity.y = speed * sin(angleXY) * cos(angleZ);
            lastVelocity.z = speed * sin(angleZ);
        }
        
        if (bounced) {
            EV << "ArbitraryMobility: UAV bounced at position (" << newPosition.x 
               << ", " << newPosition.y << ", " << newPosition.z << ")" << endl;
        }
        
        lastPosition = newPosition;
    }
    
    lastUpdate = now;
    emitMobilityStateChangedSignal();
}

void ArbitraryMobility::orient()
{
    // Basic orientation - can be enhanced later
}

void ArbitraryMobility::handleSelfMessage(cMessage *message)
{
    if (message == moveTimer) {
        // Update position
        move();
        
        // Schedule next update
        double updateInterval = par("updateInterval");
        scheduleAt(simTime() + updateInterval, moveTimer);
    }
    else {
        MovingMobilityBase::handleSelfMessage(message);
    }
}

void ArbitraryMobility::finish()
{
    MovingMobilityBase::finish();
}

void ArbitraryMobility::setPositionVelocity(const Coord& position, const Coord& velocity)
{
    lastPosition = position;
    lastVelocity = velocity;
    emitMobilityStateChangedSignal();
}

double ArbitraryMobility::getMaxSpeed() const
{
    return par("maxSpeed").doubleValue();
}
