//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
//
// ArbitraryMobility.ned
// Definição do módulo de mobilidade customizada
//

package bat_algorithm;

import inet.mobility.contract.IMobility;

//
// Custom mobility module for UAVs with realistic flight patterns
//
simple ArbitraryMobility like IMobility
{
    parameters:
        @class(ArbitraryMobility);
        @signal[mobilityStateChanged](type=inet::MobilityBase);
        @display("i=block/cogwheel");

        // Base mobility parameters (inherited from MobilityBase)
        string subjectModule = default("^");
        string coordinateSystemModule = default("");
        string displayStringTextFormat @mutable = default("p: %p\nv: %v");
        bool updateDisplayString = default(true);

        // MovingMobilityBase parameters
        bool faceForward = default(true);

        // Initial position parameters
        double initialX @unit(m) = default(uniform(50m, 150m));
        double initialY @unit(m) = default(uniform(50m, 150m));
        double initialZ @unit(m) = default(uniform(70m, 120m));

        // Parâmetros de velocidade
        double minSpeed @unit(mps) = default(15mps);
        double maxSpeed @unit(mps) = default(25mps);

        // Área de contenção
        double constraintAreaMinX @unit(m) = default(0m);
        double constraintAreaMinY @unit(m) = default(0m);
        double constraintAreaMinZ @unit(m) = default(50m);
        double constraintAreaMaxX @unit(m) = default(1000m);
        double constraintAreaMaxY @unit(m) = default(1000m);
        double constraintAreaMaxZ @unit(m) = default(200m);

        // Parâmetros padrão do IMobility
        double updateInterval @unit(s) = default(0.1s);
}
