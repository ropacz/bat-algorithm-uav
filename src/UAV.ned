//
// UAV.ned
// Unmanned Aerial Vehicle module with mobility and routing
//

package bat_algorithm;

import bat_algorithm.ArbitraryMobility;
import bat_algorithm.BatRouting;

//
// UAV (Unmanned Aerial Vehicle) module with routing protocol
//
module UAV
{
    parameters:
        @networkNode;
        @display("i=misc/drone");
        
    gates:
        input radioIn @directIn;
        
    submodules:
        mobility: ArbitraryMobility {
            @display("p=100,100");
        }
        
        batRouting: BatRouting {
            @display("p=100,200");
        }
        
    connections:
        radioIn --> batRouting.radioIn;
}
