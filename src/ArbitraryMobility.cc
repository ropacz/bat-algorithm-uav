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
        // Start periodic movement updates
        double updateInterval = par("updateInterval");
        scheduleAt(simTime() + updateInterval, moveTimer);
        EV << "ArbitraryMobility: Movement timer scheduled with interval " << updateInterval << "s" << endl;
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
    double elapsedTime = (now - lastUpdate).dbl();
    
    if (elapsedTime > 0) {
        // Update position based on current velocity
        Coord newPosition = lastPosition;
        newPosition.x += lastVelocity.x * elapsedTime;
        newPosition.y += lastVelocity.y * elapsedTime;
        newPosition.z += lastVelocity.z * elapsedTime;
        
        // Check boundaries and reflect if needed
        bool bounced = false;
        
        if (newPosition.x < constraintAreaMinX) {
            newPosition.x = constraintAreaMinX;
            lastVelocity.x = -lastVelocity.x;
            bounced = true;
        } else if (newPosition.x > constraintAreaMaxX) {
            newPosition.x = constraintAreaMaxX;
            lastVelocity.x = -lastVelocity.x;
            bounced = true;
        }
        
        if (newPosition.y < constraintAreaMinY) {
            newPosition.y = constraintAreaMinY;
            lastVelocity.y = -lastVelocity.y;
            bounced = true;
        } else if (newPosition.y > constraintAreaMaxY) {
            newPosition.y = constraintAreaMaxY;
            lastVelocity.y = -lastVelocity.y;
            bounced = true;
        }
        
        if (newPosition.z < minAltitude) {
            newPosition.z = minAltitude;
            lastVelocity.z = -lastVelocity.z;
            bounced = true;
        } else if (newPosition.z > maxAltitude) {
            newPosition.z = maxAltitude;
            lastVelocity.z = -lastVelocity.z;
            bounced = true;
        }
        
        lastPosition = newPosition;
        
        // Occasionally change direction randomly (every ~5-10 seconds on average)
        if (!bounced && uniform(0, 1) < 0.05) { // 5% chance per update
            double currentSpeed = sqrt(lastVelocity.x * lastVelocity.x + 
                                      lastVelocity.y * lastVelocity.y + 
                                      lastVelocity.z * lastVelocity.z);
            
            // Random new direction
            double angleXY = uniform(0, 2 * M_PI);
            double angleZ = uniform(-M_PI/6, M_PI/6); // ±30 degrees vertical
            
            lastVelocity.x = currentSpeed * cos(angleXY) * cos(angleZ);
            lastVelocity.y = currentSpeed * sin(angleXY) * cos(angleZ);
            lastVelocity.z = currentSpeed * sin(angleZ);
            
            EV << "ArbitraryMobility: Random direction change at t=" << now << endl;
        }
        
        emitMobilityStateChangedSignal();
    }
    
    lastUpdate = now;
}

void ArbitraryMobility::orient()
{
    // Orientation not needed for this simulation
}

void ArbitraryMobility::handleSelfMessage(cMessage *message)
{
    if (message == moveTimer) {
        // Update movement
        move();
        
        // Schedule next update
        double updateInterval = par("updateInterval");
        scheduleAt(simTime() + updateInterval, moveTimer);
    } else {
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
