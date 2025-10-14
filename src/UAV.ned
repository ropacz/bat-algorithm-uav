//
// UAV.ned
// Unmanned Aerial Vehicle module combining mobility and bat algorithm
//

package bat_algorithm;

import bat_algorithm.ArbitraryMobility;
import bat_algorithm.BatAlgorithm;
import inet.mobility.contract.IMobility;

//
// UAV (Unmanned Aerial Vehicle) module
//
// This compound module represents a single drone/UAV with:
// - ArbitraryMobility: Handles physical movement and constraints
// - BatAlgorithm: Implements the optimization algorithm
//
// The BatAlgorithm controls the UAV's movement by updating
// the mobility module's position and velocity.
//
module UAV
{
    parameters:
        @networkNode;
        @display("i=misc/drone");
        
        // Target position
        double targetX @unit(m) = default(800m);
        double targetY @unit(m) = default(500m);
        double targetZ @unit(m) = default(100m);
        
    submodules:
        mobility: ArbitraryMobility {
            @display("p=100,100");
        }
        
        batAlgorithm: BatAlgorithm {
            @display("p=100,200");
            targetX = parent.targetX;
            targetY = parent.targetY;
            targetZ = parent.targetZ;
        }
}
