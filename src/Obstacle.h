//
// Obstacle.h
//
// Header file for Obstacle module
//

#ifndef __BAT_ALGORITHM_OBSTACLE_H
#define __BAT_ALGORITHM_OBSTACLE_H

#include <omnetpp.h>
#include "inet/common/geometry/common/Coord.h"

using namespace omnetpp;
using namespace inet;

class Obstacle : public cSimpleModule
{
  private:
    // Position and size
    Coord position;
    double radius;
    
    // Dynamic movement
    bool isDynamic;
    double speed;
    Coord velocity;
    double updateInterval;
    
    // Bounds
    double minX, maxX, minY, maxY;
    
    // Timer
    cMessage *moveTimer;
    
    // Signal
    simsignal_t positionChangedSignal;
    
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    
    void updatePosition();
    void bounceAtBoundaries();
    
  public:
    Obstacle();
    virtual ~Obstacle();
    
    // Getters
    Coord getPosition() const { return position; }
    double getRadius() const { return radius; }
    bool checkCollision(const Coord& point, double safetyMargin = 0.0) const;
    double getDistanceToEdge(const Coord& point) const;
};

#endif
