//
// Obstacle.ned
//
// Represents static or dynamic obstacles in the simulation area
// Obstacles are circular regions that UAVs must avoid
//

package bat_algorithm;

simple Obstacle
{
    parameters:
        @display("i=block/circle,red,40");
        
        // Position
        double positionX @unit(m) = default(250m);
        double positionY @unit(m) = default(250m);
        double positionZ @unit(m) = default(50m);
        
        // Size
        double radius @unit(m) = default(20m);
        
        // Movement (for dynamic obstacles)
        bool isDynamic = default(false);
        double speed @unit(mps) = default(5mps);
        double updateInterval @unit(s) = default(1s);
        
        // Movement bounds (for dynamic obstacles)
        double minX @unit(m) = default(0m);
        double maxX @unit(m) = default(500m);
        double minY @unit(m) = default(0m);
        double maxY @unit(m) = default(500m);
        
        // Signals
        @signal[positionChanged](type=simtime_t);
        @statistic[position](source=positionChanged; record=vector);
}
