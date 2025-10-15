# 🦇 Bat Algorithm for UAV Swarm Routing

[![OMNeT++](https://img.shields.io/badge/OMNeT%2B%2B-6.2.0-blue)](https://omnetpp.org/)
[![INET](https://img.shields.io/badge/INET-4.5.4-green)](https://inet.omnetpp.org/)
[![License](https://img.shields.io/badge/license-Academic-orange)](LICENSE)

UAV swarm simulation using Bat Algorithm (bio-inspired metaheuristic) for intelligent routing and communication in OMNeT++ with INET Framework.

## 📋 Overview

This project implements the **Bat Algorithm** (Yang, 2010) for optimizing routing between multiple UAV drones:
- 📡 **Intelligent Routing**: Bio-inspired route discovery and optimization
- � **Random Movement**: Autonomous 3D movement with boundary reflection
- 🔄 **Multi-path Routing**: Discover and maintain multiple routes per destination
- 📊 **Fitness-based Selection**: Routes evaluated by distance and quality
- 🎯 **Communication Visualization**: Visual message passing in Qtenv GUI

### Key Features

- **Bat Algorithm Routing**: Frequency, loudness, and pulse rate optimization
- **Random Mobility**: Drones move autonomously with random direction changes
- **Route Discovery (RREQ)**: Periodic broadcast to find neighboring drones
- **Multiple Routes**: Redundant paths for reliability
- **3D Simulation**: Full 3D movement with altitude control
- **Visual Debugging**: Messages visible in Qtenv "Messages/Packet traffic" tab

## 🔧 Requirements

### Software Dependencies

- **OMNeT++ 6.2.0** - Discrete event simulator
- **INET Framework 4.5.4** - Internet protocol suite
- **C++ Compiler** - Clang or GCC with C++17 support
- **Python 3.10+** (optional) - For result analysis

### Environment Setup

```bash
# Clone the repository
git clone https://github.com/ropacz/bat-algorithm-uav.git
cd bat-algorithm-uav

# Set up OMNeT++ environment (required for all operations)
opp_env shell omnetpp-6.2.0 inet-4.5.4
```

## 🚀 Quick Start

### 1. Build the Project

```bash
# Using make script (recommended)
MODE=release ./make.sh

# Or manually
cd src && opp_makemake -f --deep && make MODE=release
```

### 2. Run Simulation

**Command-line mode (Cmdenv - fastest):**
```bash
./run_sim.sh
```

**GUI mode (Qtenv - visual):**
```bash
./run_qtenv_fixed.sh          # Default: QuickTest (30s)
./run_qtenv_fixed.sh Demo     # Quick demo (15s)
```

### 3. View Results

- **Command-line**: Output shows route tables at end
- **Qtenv GUI**: 
  - See drones moving in 3D canvas
  - Watch messages in "Messages / Packet traffic" tab
  - View routing statistics in real-time

## 📊 Simulation Scenarios

### Available Configurations

| Config | UAVs | Duration | Description |
|--------|------|----------|-------------|
| `QuickTest` | 3 | 30s | Default - Fast testing scenario |
| `Demo` | 3 | 15s | Quick demonstration |
| `General` | 3 | 300s | Extended simulation |
| `LargeNetwork` | 10 | 400s | Large swarm test |

### Running Specific Scenarios

```bash
./run_qtenv_fixed.sh Demo           # 15s quick demo
./run_qtenv_fixed.sh QuickTest      # 30s default
./run_qtenv_fixed.sh LargeNetwork   # 10 drones
```

## 🧮 Bat Algorithm for Routing

### Route Fitness Calculation

Routes are evaluated using bat-inspired optimization:

```cpp
fitness = baseDistance × (1.0 + frequency × loudness × pulseRate)
```

Where:
- **baseDistance**: Euclidean distance between nodes
- **frequency**: Random value in [frequencyMin, frequencyMax]
- **loudness**: Decreases over time (α = 0.9)
- **pulseRate**: Increases over time (γ = 0.9)

### Core Parameters

```ini
# Bat Routing Algorithm
*.uav[*].batRouting.routingUpdateInterval = 5s
*.uav[*].batRouting.frequencyMin = 0.0
*.uav[*].batRouting.frequencyMax = 2.0
*.uav[*].batRouting.loudness = 0.9          # Initial loudness
*.uav[*].batRouting.pulseRate = 0.5         # Initial pulse rate
*.uav[*].batRouting.alpha = 0.9             # Loudness decay
*.uav[*].batRouting.gamma = 0.9             # Pulse rate growth

# Random Mobility
*.uav[*].mobility.minSpeed = 10mps
*.uav[*].mobility.maxSpeed = 20mps
*.uav[*].mobility.updateInterval = 0.1s     # Movement update frequency

# Communication
*.uav[*].batRouting.commRange = 300m        # Radio range
```

## 📁 Project Structure

```
bat-algorithm/
├── src/                          # Source code
│   ├── BatRouting.{cc,h,ned}    # Bat Algorithm routing protocol
│   ├── ArbitraryMobility.{cc,h,ned} # Random mobility model
│   ├── UAV.ned                  # UAV compound module
│   └── package.ned              # Package definition
├── simulations/                 # Simulation scenarios
│   ├── omnetpp.ini              # Configuration file
│   ├── BatSwarmNetwork.ned      # Network topology (3D canvas)
│   ├── package.ned              # Package definition
│   └── results/                 # Simulation outputs (.sca, .vec files)
├── BAT_ALGORITHM.md            # Algorithm documentation
├── analyze_results.py          # Results analysis script
├── run_sim.sh                  # Command-line runner
├── run_qtenv_fixed.sh          # GUI runner (macOS fixes)
├── make.sh                     # Build script
└── README.md                   # This file
```

## 🔬 Key Implementation Details

### Bat Routing Module (`BatRouting.cc`)

**Route Discovery Process:**
1. **Broadcast RREQ**: Periodically send route request to all neighbors
2. **Receive & Process**: Update position cache, calculate fitness
3. **Route Storage**: Maintain multiple routes per destination (sorted by fitness)
4. **Route Selection**: Choose best route using bat-inspired fitness function

**Message Types:**
- `RouteDiscoveryPacket (kind=1)`: Broadcast to discover neighbors
- `DataPacket (kind=2)`: Point-to-point data transmission

### Mobility Model (`ArbitraryMobility.cc`)

**Movement Features:**
- **Random Initial Velocity**: Speed and direction randomized at start
- **Periodic Updates**: Position updated every 0.1s based on velocity
- **Boundary Reflection**: Drones bounce off area boundaries
- **Random Direction Changes**: 5% chance per update to change direction
- **Altitude Control**: Vertical movement limited to ±30°

**3D Boundary Area:**
- X: 0-500m
- Y: 0-500m  
- Z: 50-200m (altitude)

### Communication System

**Direct Message Passing:**
- Uses `sendDirect()` to parent UAV module
- Messages route through `radioIn` gate for visibility
- Visible in Qtenv "Messages / Packet traffic" tab
- Communication range: 300m

## 📈 Performance Metrics

The simulation tracks:

- **Route Tables**: Number of routes discovered per destination
- **Route Fitness**: Bio-inspired quality metric per route
- **Message Count**: RREQ broadcasts and data packets sent
- **Movement Statistics**: Position changes, velocity updates
- **Simulation Events**: Total events processed during runtime

**Example Output:**
```
INFO: BatRouting: Node 0 - Routes in table: 2
INFO:   Destination 1: 2 routes
INFO:   Destination 2: 3 routes
```

## 🎮 Usage Tips

### Speeding Up Simulation in Qtenv

1. **Express Mode**: Press `F6` (runs without animation)
2. **Speed Control**: Menu → Simulate → Speed → Drag to "Fast"
3. **Run Until**: Set target time to skip ahead
4. **Use Demo Config**: Shortest duration (15s)

### Debugging Routes

**View route discovery in real-time:**
```bash
# Run with verbose logging
cd simulations
../src/bat-algorithm -u Cmdenv -c QuickTest --debug-on-errors=true
```

**Check routing table:**
- Route info printed at end of simulation
- Multiple routes = better redundancy
- Higher fitness = shorter/better paths

## 🐛 Troubleshooting

### Common Issues

**1. "This OMNeT++ installation cannot be used outside an opp_env shell"**
```bash
# Always start with:
opp_env shell omnetpp-6.2.0 inet-4.5.4
```

**2. Qtenv crashes on macOS**
```bash
# Use the fixed script with environment setup:
./run_qtenv_fixed.sh

# Or try command-line mode instead:
./run_sim.sh
```

**3. Compilation errors**
```bash
# Clean and rebuild:
cd src
make clean
opp_makemake -f --deep
make MODE=release
```

**4. Missing INET libraries**
```bash
# Verify environment:
echo $INET_PROJ
# Should output: /path/to/inet-4.5.4

# If not set, run:
opp_env shell omnetpp-6.2.0 inet-4.5.4
```

**5. "No such config" error**
```bash
# Check available configs:
cd simulations
../src/bat-algorithm -u Cmdenv -q runnumbers

# Use valid config names:
./run_qtenv_fixed.sh QuickTest
./run_qtenv_fixed.sh Demo
```

**6. Simulation runs too slow in Qtenv**
```bash
# Use Express mode (no animation):
# In Qtenv GUI, press F6

# Or use faster config:
./run_qtenv_fixed.sh Demo  # Only 15 seconds

# Or use command-line (fastest):
./run_sim.sh
```

## 📚 Documentation

- **[BAT_ALGORITHM.md](BAT_ALGORITHM.md)** - Detailed algorithm documentation
- **[CONFIGURAR_IDE.md](CONFIGURAR_IDE.md)** - IDE setup guide (Portuguese)
- **OMNeT++ Manual**: https://doc.omnetpp.org/
- **INET Manual**: https://inet.omnetpp.org/docs/

## 🔍 Code Quality

✅ **Reviewed and validated:**
- No memory leaks (proper message cleanup)
- Null pointer checks on all casts
- Exception handling for edge cases
- Consistent coding style (C++17)
- Well-documented parameters
- Visible message passing for debugging

## 🎯 Current Features

✅ **Implemented:**
- Bat Algorithm routing with fitness calculation
- Random 3D mobility with boundary reflection
- Multi-path route discovery and maintenance
- Visual message passing in Qtenv
- Multiple simulation scenarios
- Real-time routing statistics

❌ **Not Included:**
- Obstacle avoidance (removed for simplification)
- Static navigation targets (focus on routing only)
- Inter-drone collision avoidance (simplified mobility)

## 📄 License

Academic Public License - NOT FOR COMMERCIAL USE

See OMNeT++ and INET Framework licenses for details.

## 📖 References

**Primary Algorithm:**
- Yang, X. S. (2010). "A new metaheuristic bat-inspired algorithm". *Nature Inspired Cooperative Strategies for Optimization (NICSO 2010)*, 65-74.

**Routing Applications:**
- Goerzen et al. (2015) - Bat Algorithm for UAV path planning
- Zhang et al. (2018) - Bat Algorithm for multi-drone coordination
- Gandhi et al. (2020) - Bat Algorithm with inertia factor

**OMNeT++ Framework:**
- Varga, A. (2001). "The OMNeT++ discrete event simulation system"
- INET Framework documentation: https://inet.omnetpp.org/

## 👤 Author

**Rodrigo Paz**
- GitHub: [@ropacz](https://github.com/ropacz)
- Repository: [bat-algorithm-uav](https://github.com/ropacz/bat-algorithm-uav)

## 🙏 Acknowledgments

- OMNeT++ Community for excellent simulation framework
- INET Framework developers for networking protocols
- Bio-inspired algorithms research community
- Dr. Xin-She Yang for the Bat Algorithm

---

**Last Updated:** October 14, 2025  
**Version:** 2.0 (Routing-focused)  
**Status:** ✅ Production Ready

**Quick Start Command:**
```bash
opp_env shell omnetpp-6.2.0 inet-4.5.4
MODE=release ./make.sh
./run_qtenv_fixed.sh Demo
```
