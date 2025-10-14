//
// BatAlgorithm.ned
// Bat Algorithm module for UAV swarm optimization
//

package bat_algorithm;

//
// Bat Algorithm for UAV navigation and optimization
// 
// This module implements the Bat Algorithm metaheuristic for optimizing
// UAV positions to converge towards a target location while avoiding obstacles.
// The algorithm simulates the echolocation behavior of microbats.
//
// Key parameters:
// - frequencyMin/Max: Range for frequency tuning
// - loudness: Initial loudness (decreases over time)
// - pulseRate: Initial pulse emission rate (increases over time)
// - alpha: Loudness reduction factor (0.8-0.99)
// - gamma: Pulse rate increase factor (0.8-0.99)
//
simple BatAlgorithm
{
    parameters:
        @class(BatAlgorithm);
        @display("i=block/network2");
        
        // Bat Algorithm parameters
        double frequencyMin = default(0.0);        // Minimum frequency
        double frequencyMax = default(2.0);        // Maximum frequency
        double loudness = default(0.9);            // Initial loudness A (0-1)
        double pulseRate = default(0.5);           // Initial pulse rate r (0-1)
        double alpha = default(0.9);               // Loudness reduction factor
        double gamma = default(0.9);               // Pulse rate increase factor
        
        // Obstacle avoidance parameters
        double safetyDistance @unit(m) = default(15m);   // Minimum distance from obstacles
        double detectionRange @unit(m) = default(50m);   // Range to detect obstacles
        double obstacleWeight = default(2.0);            // Weight for obstacle penalty
        double uavSafetyDistance @unit(m) = default(30m); // Minimum distance between UAVs
        
        // Target parameters
        double targetX @unit(m) = default(1000m);  // Target X coordinate
        double targetY @unit(m) = default(1000m);  // Target Y coordinate
        double targetZ @unit(m) = default(150m);   // Target Z coordinate (altitude)
        double targetRadius @unit(m) = default(50m); // Acceptance radius around target
        
        // Update interval
        double updateInterval @unit(s) = default(1s); // How often to update bat position
        
        // Statistics
        @signal[fitness](type=double);
        @statistic[fitness](title="Fitness (distance to target)"; record=vector,stats; interpolationmode=sample-hold);
        @signal[loudness](type=double);
        @statistic[loudness](title="Current Loudness"; record=vector,stats; interpolationmode=sample-hold);
        @signal[pulseRate](type=double);
        @statistic[pulseRate](title="Current Pulse Rate"; record=vector,stats; interpolationmode=sample-hold);
        @signal[positionBroadcast](type=simtime_t);
        @statistic[positionBroadcast](title="Position Broadcasts"; record=count);
        
    gates:
        input in @directIn;  // For receiving position updates from other UAVs
}
