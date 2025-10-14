/*
 * ArbitraryMobility.h
 * Custom mobility model for FANET UAVs
 * Compatible with INET 4.5.4 and OMNeT++ 6.2.0
 */

#ifndef ARBITRARYMOBILITY_H_
#define ARBITRARYMOBILITY_H_

#include "inet/mobility/base/MovingMobilityBase.h"

using namespace omnetpp;
using namespace inet;

class INET_API ArbitraryMobility : public MovingMobilityBase {
private:
    // State variables
    simtime_t lastUpdate;

    // Boundary parameters
    double constraintAreaMinX;
    double constraintAreaMaxX;
    double constraintAreaMinY;
    double constraintAreaMaxY;
    double minAltitude;
    double maxAltitude;

    // Timer for periodic updates
    cMessage *moveTimer;

protected:
    virtual void initialize(int stage) override;
    virtual void setInitialPosition() override;
    virtual void move() override;
    virtual void orient() override;
    virtual void finish() override;
    virtual void handleSelfMessage(cMessage *message) override;

public:
    ArbitraryMobility();
    virtual ~ArbitraryMobility();

    // External interface for position/velocity updates
    virtual void setPositionVelocity(const Coord& position, const Coord& velocity);
    virtual double getMaxSpeed() const override;
};

#endif /* ARBITRARYMOBILITY_H_ */
