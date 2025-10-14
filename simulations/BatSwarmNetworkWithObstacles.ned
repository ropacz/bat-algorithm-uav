//
// BatSwarmNetworkWithObstacles.ned
//
// Simple UAV swarm network with static obstacles
//

package bat_algorithm.simulations;

import bat_algorithm.UAV;
import bat_algorithm.Obstacle;

network BatSwarmNetworkWithObstacles
{
    parameters:
        int numUAVs = default(10);
        int numObstacles = default(1);
        @display("bgb=1000,1000;bgg=100,1,grey95");
        
    submodules:
        uav[numUAVs]: UAV {
            @display("p=,,ring");
        }
        
        obstacle[numObstacles]: Obstacle {
            @display("i=block/circle,red,40");
            isDynamic = false;
            radius = 40m;
            positionX = 500m;
            positionY = 500m;
            positionZ = 100m;
        }
}
