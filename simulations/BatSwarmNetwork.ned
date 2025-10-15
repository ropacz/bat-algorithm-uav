//
// BatSwarmNetwork.ned
//
// UAV swarm network with Bat Algorithm routing
//

package bat_algorithm.simulations;

import bat_algorithm.UAV;

network BatSwarmNetwork
{
    parameters:
        int numUAVs = default(10);
        @display("bgb=1000,1000;bgg=100,1,grey95");
        
    submodules:
        uav[numUAVs]: UAV {
            @display("p=,,ring");
        }
}
