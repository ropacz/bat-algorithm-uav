# 🦇 Bat Algorithm for UAV Swarm - Obstacle Avoidance

[![OMNeT++](https://img.shields.io/badge/OMNeT%2B%2B-6.2.0-blue)](https://omnetpp.org/)
[![INET](https://img.shields.io/badge/INET-4.5.4-green)](https://inet.omnetpp.org/)
[![License](https://img.shields.io/badge/license-Academic-orange)](LICENSE)

UAV swarm simulation using Bat Algorithm (bio-inspired metaheuristic) for navigation with static obstacle avoidance in OMNeT++ with INET Framework.

## 📋 Overview

This project implements the **Bat Algorithm** (Yang, 2010) for coordinating multiple UAV drones to:
- ✈️ Navigate towards a target position
- 🚧 Avoid static obstacles
- 🤝 Maintain safe distance between drones
- 📊 Optimize paths using bio-inspired swarm intelligence

### Key Features

- **Bio-Inspired Navigation**: Mimics bat echolocation behavior
- **Collision Avoidance**: Both static obstacles and inter-drone collision prevention
- **Realistic Physics**: Velocity limits, smooth movements, boundary constraints
- **Multiple Scenarios**: Pre-configured test cases
- **Visualization**: Qtenv GUI support with event logging

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
# Using make script
./make.sh

# Or manually
make MODE=release
```

### 2. Run Simulation

**Command-line mode (Cmdenv):**
```bash
./run_sim.sh QuickTest
```

**GUI mode (Qtenv):**
```bash
./run_qtenv_fixed.sh
```

### 3. Analyze Results

```bash
python3 analyze_results.py
```

Results will be in `simulations/results/analysis/`

## 📊 Simulation Scenarios

### Available Configurations

| Config | UAVs | Obstacles | Duration | Description |
|--------|------|-----------|----------|-------------|
| `General` | 3 | 1 | 300s | Default configuration |
| `SimpleObstacles` | 3 | 1 | 300s | Simple obstacle avoidance |
| `QuickTest` | 3 | 1 | 60s | Fast testing scenario |
| `ManyObstacles` | 5 | 1 | 400s | More drones, longer simulation |

### Running Specific Scenarios

```bash
./run_sim.sh SimpleObstacles
./run_sim.sh ManyObstacles
```

## 🧮 Bat Algorithm Implementation

### Core Equations

**1. Frequency Update:**
```
f_i = f_min + (f_max - f_min) × β,  β ∈ [0,1]
```

**2. Velocity Update:**
```
v_i(t+1) = v_i(t) × w + (x_best - x_i(t)) × f_i
```
- `w = 0.5` (inertia factor)
- Prevents velocity accumulation

**3. Position Update:**
```
x_i(t+1) = x_i(t) + v_i(t) × Δt
```
- `Δt` ensures smooth continuous movement

**4. Loudness & Pulse Rate:**
```
A_i(t+1) = α × A_i(t)
r_i(t+1) = r_i(0) × [1 - exp(-γ × t)]
```

### Parameters

```ini
# Bat Algorithm Core
*.uav[*].batAlgorithm.frequencyMin = 0.0
*.uav[*].batAlgorithm.frequencyMax = 2.0
*.uav[*].batAlgorithm.loudness = 0.9
*.uav[*].batAlgorithm.pulseRate = 0.5
*.uav[*].batAlgorithm.alpha = 0.9       # Loudness reduction
*.uav[*].batAlgorithm.gamma = 0.9       # Pulse rate increase
*.uav[*].batAlgorithm.updateInterval = 1s

# Collision Avoidance
*.uav[*].batAlgorithm.safetyDistance = 20m       # Obstacle margin
*.uav[*].batAlgorithm.uavSafetyDistance = 50m    # Inter-drone distance (increased spacing)
*.uav[*].batAlgorithm.obstacleWeight = 3.0       # Penalty multiplier
```

## 📁 Project Structure

```
bat-algorithm/
├── src/                          # Source code
│   ├── BatAlgorithm.{cc,h,ned}  # Bat Algorithm implementation
│   ├── ArbitraryMobility.{cc,h,ned} # Custom mobility model
│   ├── Obstacle.{cc,h,ned}      # Obstacle module
│   ├── UAV.ned                  # UAV compound module
│   └── package.ned              # Package definition
├── simulations/                 # Simulation scenarios
│   ├── omnetpp.ini              # Configuration file
│   ├── BatSwarmNetworkWithObstacles.ned  # Network topology
│   └── results/                 # Simulation outputs
├── images/                      # Visual assets
├── BAT_ALGORITHM.md            # Algorithm documentation
├── analyze_results.py          # Results analysis script
├── run_sim.sh                  # Command-line runner
├── run_qtenv_fixed.sh          # GUI runner (macOS fixes)
└── README.md                   # This file
```

## 🔬 Key Implementation Details

### Bat Algorithm Module (`BatAlgorithm.cc`)

- **6-Phase Update Cycle**:
  1. State Assessment
  2. Velocity Update (with inertia)
  3. Position Calculation
  4. Solution Acceptance
  5. Local Search
  6. Communication & Scheduling

### Mobility Model (`ArbitraryMobility.cc`)

- Smooth 3D movement with realistic physics
- Boundary bounce with safety margins
- Velocity-based position updates

### Obstacle Module (`Obstacle.cc`)

- Static obstacles with configurable radius
- Collision detection with safety margins
- Distance-to-edge calculations

## 📈 Performance Metrics

The simulation tracks:

- **Fitness**: Distance to target + obstacle penalty
- **Loudness**: Exploration parameter (decreases over time)
- **Pulse Rate**: Exploitation parameter (increases over time)
- **Position Broadcasts**: Inter-drone communication count

Access via:
- Statistics in `.sca` files
- Vectors in `.vec` files
- Analysis plots in `results/analysis/`

## 🐛 Troubleshooting

### Common Issues

**1. "This OMNeT++ installation cannot be used outside an opp_env shell"**
```bash
# Always start with:
opp_env shell omnetpp-6.2.0 inet-4.5.4
```

**2. Qtenv crashes on macOS**
```bash
# Use the fixed script:
./run_qtenv_fixed.sh

# Or use command-line mode:
./run_sim.sh QuickTest
```

**3. Compilation errors**
```bash
# Clean and rebuild:
make clean
make MODE=release
```

**4. Missing INET libraries**
```bash
# Verify environment:
echo $INET_PROJ
# Should output: /path/to/inet-4.5.4
```

## 📚 Documentation

- **[BAT_ALGORITHM.md](BAT_ALGORITHM.md)** - Detailed algorithm documentation
- **[CONFIGURAR_IDE.md](CONFIGURAR_IDE.md)** - IDE setup guide (Portuguese)
- **OMNeT++ Manual**: https://doc.omnetpp.org/
- **INET Manual**: https://inet.omnetpp.org/docs/

## 🔍 Code Quality

✅ **Reviewed and validated:**
- No memory leaks (proper pointer management)
- No TODOs or FIXMEs
- Consistent coding style
- Comprehensive error handling
- Well-documented code and parameters

## 📄 License

Academic Public License - NOT FOR COMMERCIAL USE

See OMNeT++ and INET Framework licenses for details.

## 📖 References

**Primary Source:**
- Yang, X. S. (2010). "A new metaheuristic bat-inspired algorithm"

**Related Work:**
- Goerzen et al. (2015) - BA for UAV path planning
- Gandhi et al. (2020) - BA with inertia factor
- Zhang et al. (2018) - BA for multi-drone coordination

## 👤 Author

**Rodrigo Paz**
- GitHub: [@ropacz](https://github.com/ropacz)
- Repository: [bat-algorithm-uav](https://github.com/ropacz/bat-algorithm-uav)

## 🙏 Acknowledgments

- OMNeT++ Community
- INET Framework developers
- Bio-inspired algorithms research community

---

**Last Updated:** October 2025  
**Status:** ✅ Production Ready
