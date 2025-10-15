//
// BatRouting.ned
// Routing protocol using Bat Algorithm for route optimization in FANETs
//

package bat_algorithm;

simple BatRouting
{
    parameters:
        @class(BatRouting);
        @display("i=block/routing");
        
        // Bat Algorithm parameters for routing
        double routingUpdateInterval @unit(s) = default(5s);  // Route discovery interval
        double frequencyMin = default(0.0);
        double frequencyMax = default(2.0);
        double loudness = default(0.9);
        double pulseRate = default(0.5);
        double alpha = default(0.9);
        double gamma = default(0.9);
        
        // Routing metrics weights
        double hopCountWeight = default(1.0);      // Weight for number of hops
        double linkQualityWeight = default(1.5);   // Weight for link quality
        double energyWeight = default(1.0);        // Weight for energy consumption
        double mobilityWeight = default(0.8);      // Weight for node mobility
        
        // Route table parameters
        int maxRoutesPerDestination = default(3);  // Keep top-N routes
        double routeTimeout @unit(s) = default(30s);
        
        @signal[routeDiscovered](type=long);
        @statistic[routeDiscovered](title="Routes Discovered"; record=count,vector);
        @signal[packetRouted](type=long);
        @statistic[packetRouted](title="Packets Routed"; record=count,vector);
        
    gates:
        input radioIn @directIn;
}
