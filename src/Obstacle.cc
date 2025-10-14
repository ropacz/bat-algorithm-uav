//
// Obstacle.cc
//
// Implementation of Obstacle module
//

#include "Obstacle.h"
#include <algorithm>

Define_Module(Obstacle);

Obstacle::Obstacle()
{
    moveTimer = nullptr;
}

Obstacle::~Obstacle()
{
    cancelAndDelete(moveTimer);
}

void Obstacle::initialize()
{
    // Read parameters
    position.x = par("positionX");
    position.y = par("positionY");
    position.z = par("positionZ");
    radius = par("radius");
    
    isDynamic = par("isDynamic");
    speed = par("speed");
    updateInterval = par("updateInterval");
    
    minX = par("minX");
    maxX = par("maxX");
    minY = par("minY");
    maxY = par("maxY");
    
    // Initialize signal
    positionChangedSignal = registerSignal("positionChanged");
    
    // Initialize velocity for dynamic obstacles
    if (isDynamic) {
        // Random initial direction
        double angle = uniform(0, 2 * M_PI);
        velocity = Coord(cos(angle) * speed, sin(angle) * speed, 0);
        
        moveTimer = new cMessage("obstacleMove");
        scheduleAt(simTime() + updateInterval, moveTimer);
        
        EV << "Dynamic Obstacle initialized at (" << position.x << ", " 
           << position.y << "), radius: " << radius << "m, speed: " << speed << "m/s" << endl;
    } else {
        EV << "Static Obstacle initialized at (" << position.x << ", " 
           << position.y << "), radius: " << radius << "m" << endl;
    }
}

void Obstacle::handleMessage(cMessage *msg)
{
    if (msg == moveTimer && isDynamic) {
        updatePosition();
        emit(positionChangedSignal, simTime());
        scheduleAt(simTime() + updateInterval, moveTimer);
    }
}

void Obstacle::updatePosition()
{
    // Update position based on velocity
    position += velocity * updateInterval;
    
    // Check boundaries and bounce
    bounceAtBoundaries();
}

void Obstacle::bounceAtBoundaries()
{
    bool bounced = false;
    
    // X boundaries
    if (position.x - radius < minX) {
        position.x = minX + radius;
        velocity.x = fabs(velocity.x);
        bounced = true;
    } else if (position.x + radius > maxX) {
        position.x = maxX - radius;
        velocity.x = -fabs(velocity.x);
        bounced = true;
    }
    
    // Y boundaries
    if (position.y - radius < minY) {
        position.y = minY + radius;
        velocity.y = fabs(velocity.y);
        bounced = true;
    } else if (position.y + radius > maxY) {
        position.y = maxY - radius;
        velocity.y = -fabs(velocity.y);
        bounced = true;
    }
    
    if (bounced) {
        EV << "Obstacle bounced at (" << position.x << ", " << position.y << ")" << endl;
    }
}

bool Obstacle::checkCollision(const Coord& point, double safetyMargin) const
{
    double distance = position.distance(point);
    return distance < (radius + safetyMargin);
}

double Obstacle::getDistanceToEdge(const Coord& point) const
{
    double distance = position.distance(point);
    return distance - radius;
}

void Obstacle::finish()
{
    EV << "Obstacle finished at position (" << position.x << ", " 
       << position.y << ")" << endl;
}
