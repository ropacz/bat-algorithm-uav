//
// ArbitraryMobility.ned
// Random mobility with boundary reflection
//

package bat_algorithm;

import inet.mobility.contract.IMobility;

//
// Random mobility module with automatic movement
// UAVs move with random velocity and bounce at boundaries
//
simple ArbitraryMobility like IMobility
{
    parameters:
        @class(ArbitraryMobility);
        @signal[mobilityStateChanged](type=inet::MobilityBase);
        @display("i=block/cogwheel");

        // Base mobility parameters
        string subjectModule = default("^");
        string coordinateSystemModule = default("");
        string displayStringTextFormat @mutable = default("p: %p\nv: %v");
        bool updateDisplayString = default(true);
        bool faceForward = default(true);

        // Initial position (set in omnetpp.ini)
        double initialX @unit(m) = default(uniform(50m, 150m));
        double initialY @unit(m) = default(uniform(50m, 150m));
        double initialZ @unit(m) = default(uniform(70m, 120m));

        // Speed limits for random movement
        double minSpeed @unit(mps) = default(10mps);
        double maxSpeed @unit(mps) = default(20mps);

        // Boundary area
        double constraintAreaMinX @unit(m) = default(0m);
        double constraintAreaMinY @unit(m) = default(0m);
        double constraintAreaMinZ @unit(m) = default(50m);
        double constraintAreaMaxX @unit(m) = default(500m);
        double constraintAreaMaxY @unit(m) = default(500m);
        double constraintAreaMaxZ @unit(m) = default(200m);

        // Movement update interval
        double updateInterval @unit(s) = default(0.1s);
}
