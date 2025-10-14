#!/usr/bin/env python3#!/usr/bin/env python3#!/usr/bin/env python3

"""

Análise simplificada de resultados - Bat Algorithm com Obstacle Avoidance""""""

Foco: convergência de fitness e desvio de obstáculos

"""Simple analysis script for Bat Algorithm simulation resultsBat Algorithm Results Analysis Script



import osFocuses on obstacle avoidance performance

import sys

import pandas as pd"""This script analyzes the simulation results from the Bat Algorithm UAV swarm.

import numpy as np

import matplotlib.pyplot as pltIt reads the .vec (vector) files and generates plots and statistics.

from pathlib import Path

import re

def load_scalar_results(results_dir, config_name):

    """Carrega resultados escalares dos arquivos .sca"""import sysRequirements:

    sca_file = results_dir / f"{config_name}-#0.sca"

    import matplotlib.pyplot as plt    pip install numpy matplotlib pandas

    if not sca_file.exists():

        print(f"Aviso: {sca_file} não encontrado")import numpy as np

        return None

    from pathlib import PathUsage:

    data = []

    with open(sca_file, 'r') as f:    python analyze_results.py [results_directory]

        for line in f:

            if line.startswith('scalar'):def parse_scalar_file(filepath):"""

                parts = line.split()

                if len(parts) >= 4:    """Parse OMNeT++ .sca file and extract scalar values"""

                    module = parts[1]

                    metric = parts[2]    scalars = {}import sys

                    value = float(parts[3])

                    data.append({'module': module, 'metric': metric, 'value': value})    import os

    

    return pd.DataFrame(data) if data else None    with open(filepath, 'r') as f:import numpy as np



def plot_fitness_convergence(results_dir, config_name):        current_module = Noneimport matplotlib.pyplot as plt

    """Plota convergência de fitness"""

    df = load_scalar_results(results_dir, config_name)        for line in f:from pathlib import Path

    if df is None:

        return            line = line.strip()

    

    fig, axes = plt.subplots(1, 2, figsize=(14, 5))            def parse_vec_file(filename):

    fig.suptitle(f'Análise de Convergência - {config_name}', fontsize=14, fontweight='bold')

                # Extract module name    """Parse OMNeT++ .vec file and extract data."""

    # 1. Distribuição de fitness final

    fitness_data = df[df['metric'] == 'finalFitness']            if line.startswith('module'):    vectors = {}

    if not fitness_data.empty:

        axes[0].hist(fitness_data['value'], bins=15, color='steelblue', edgecolor='black', alpha=0.7)                match = re.search(r'module\s+([\w\.[\]]+)', line)    current_vector = None

        axes[0].axvline(fitness_data['value'].mean(), color='red', linestyle='--', 

                       linewidth=2, label=f'Média: {fitness_data["value"].mean():.1f}m')                if match:    

        axes[0].set_xlabel('Fitness Final (distância ao alvo em metros)', fontsize=11)

        axes[0].set_ylabel('Frequência', fontsize=11)                    current_module = match.group(1)    with open(filename, 'r') as f:

        axes[0].set_title('Distribuição de Fitness Final', fontsize=12)

        axes[0].legend()                    for line in f:

        axes[0].grid(True, alpha=0.3)

                # Extract scalar values            line = line.strip()

    # 2. Comparação por UAV

    pbest_data = df[df['metric'] == 'personalBestFitness']            elif line.startswith('scalar') and current_module:            

    if not fitness_data.empty and not pbest_data.empty:

        uav_ids = range(len(fitness_data))                parts = line.split()            # Vector declaration

        axes[1].plot(uav_ids, fitness_data['value'].values, 

                    marker='o', label='Fitness Final', linewidth=2, markersize=6)                if len(parts) >= 3:            if line.startswith('vector'):

        axes[1].plot(uav_ids, pbest_data['value'].values, 

                    marker='s', label='Melhor Pessoal', linewidth=2, markersize=6, alpha=0.7)                    scalar_name = parts[1]                parts = line.split()

        axes[1].set_xlabel('ID do UAV', fontsize=11)

        axes[1].set_ylabel('Fitness (metros)', fontsize=11)                    try:                vec_id = int(parts[1])

        axes[1].set_title('Fitness por UAV', fontsize=12)

        axes[1].legend()                        scalar_value = float(parts[2])                module = parts[2]

        axes[1].grid(True, alpha=0.3)

                            key = f"{current_module}.{scalar_name}"                name = parts[3]

    plt.tight_layout()

    output_file = results_dir / 'analysis' / f'{config_name}_fitness.png'                        scalars[key] = scalar_value                vectors[vec_id] = {

    output_file.parent.mkdir(parents=True, exist_ok=True)

    plt.savefig(output_file, dpi=300, bbox_inches='tight')                    except ValueError:                    'module': module,

    print(f"✓ Salvo: {output_file}")

    plt.close()                        continue                    'name': name,



def plot_obstacle_avoidance(results_dir, config_name):                        'time': [],

    """Plota estatísticas de desvio de obstáculos"""

    df = load_scalar_results(results_dir, config_name)    return scalars                    'value': []

    if df is None:

        return                }

    

    obstacles_data = df[df['metric'] == 'obstaclesAvoided']def parse_vector_file(filepath):            

    if obstacles_data.empty:

        print(f"  Sem dados de obstáculos para {config_name}")    """Parse OMNeT++ .vec file and extract vector data"""            # Skip attribute lines

        return

        vectors = {}            elif line.startswith('attr'):

    fig, axes = plt.subplots(1, 2, figsize=(14, 5))

    fig.suptitle(f'Análise de Desvio de Obstáculos - {config_name}',     vector_defs = {}                continue

                 fontsize=14, fontweight='bold')

                    

    # 1. Distribuição de obstáculos evitados

    axes[0].hist(obstacles_data['value'], bins=range(int(obstacles_data['value'].max())+2),     with open(filepath, 'r') as f:            # Vector data

                color='coral', edgecolor='black', alpha=0.7)

    axes[0].axvline(obstacles_data['value'].mean(), color='darkred', linestyle='--',         for line in f:            elif line and not line.startswith('#'):

                   linewidth=2, label=f'Média: {obstacles_data["value"].mean():.1f}')

    axes[0].set_xlabel('Número de Obstáculos Evitados', fontsize=11)            line = line.strip()                parts = line.split()

    axes[0].set_ylabel('Frequência', fontsize=11)

    axes[0].set_title('Distribuição de Obstáculos Evitados', fontsize=12)                            if len(parts) >= 3:

    axes[0].legend()

    axes[0].grid(True, alpha=0.3)            # Vector definition line                    try:

    

    # 2. Obstáculos por UAV            if line.startswith('vector'):                        vec_id = int(parts[0])

    uav_ids = range(len(obstacles_data))

    colors = ['green' if v > 0 else 'gray' for v in obstacles_data['value'].values]                parts = line.split()                        time = float(parts[2])

    axes[1].bar(uav_ids, obstacles_data['value'].values, color=colors, alpha=0.7, edgecolor='black')

    axes[1].set_xlabel('ID do UAV', fontsize=11)                if len(parts) >= 4:                        value = float(parts[3]) if len(parts) > 3 else 0

    axes[1].set_ylabel('Obstáculos Evitados', fontsize=11)

    axes[1].set_title('Obstáculos Evitados por UAV', fontsize=12)                    vec_id = int(parts[1])                        

    axes[1].grid(True, alpha=0.3, axis='y')

                        module = parts[2]                        if vec_id in vectors:

    plt.tight_layout()

    output_file = results_dir / 'analysis' / f'{config_name}_obstacles.png'                    vector_name = parts[3]                            vectors[vec_id]['time'].append(time)

    plt.savefig(output_file, dpi=300, bbox_inches='tight')

    print(f"✓ Salvo: {output_file}")                    vector_defs[vec_id] = (module, vector_name)                            vectors[vec_id]['value'].append(value)

    plt.close()

                    vectors[vec_id] = {'time': [], 'value': []}                    except (ValueError, IndexError):

def print_summary(results_dir, config_name):

    """Imprime estatísticas resumidas"""                                    # Skip lines that don't match expected format

    df = load_scalar_results(results_dir, config_name)

    if df is None:            # Vector data line                        continue

        return

                elif not line.startswith('version') and not line.startswith('run') and line:    

    print(f"\n{'='*70}")

    print(f"  Configuração: {config_name}")                parts = line.split()    return vectors

    print(f"{'='*70}")

                    if len(parts) >= 3:

    fitness_data = df[df['metric'] == 'finalFitness']

    if not fitness_data.empty:                    try:def plot_fitness_convergence(vectors, output_dir):

        print(f"\n  📊 Estatísticas de Fitness (distância ao alvo):")

        print(f"     Média:    {fitness_data['value'].mean():>8.2f} m")                        vec_id = int(parts[0])    """Plot fitness convergence over time for all UAVs."""

        print(f"     Mediana:  {fitness_data['value'].median():>8.2f} m")

        print(f"     Desvio:   {fitness_data['value'].std():>8.2f} m")                        time = float(parts[1])    plt.figure(figsize=(12, 6))

        print(f"     Mínimo:   {fitness_data['value'].min():>8.2f} m  (melhor)")

        print(f"     Máximo:   {fitness_data['value'].max():>8.2f} m")                        value = float(parts[2])    

    

    obstacles_data = df[df['metric'] == 'obstaclesAvoided']                        if vec_id in vectors:    fitness_vectors = {k: v for k, v in vectors.items() 

    if not obstacles_data.empty:

        total_avoided = int(obstacles_data['value'].sum())                            vectors[vec_id]['time'].append(time)                      if 'fitness' in v['name'].lower()}

        avg_per_uav = obstacles_data['value'].mean()

        uavs_with_obstacles = (obstacles_data['value'] > 0).sum()                            vectors[vec_id]['value'].append(value)    

        print(f"\n  🛑 Estatísticas de Obstáculos:")

        print(f"     Total evitados:        {total_avoided}")                    except (ValueError, IndexError):    for vec_id, data in fitness_vectors.items():

        print(f"     Média por UAV:         {avg_per_uav:.1f}")

        print(f"     UAVs que encontraram:  {uavs_with_obstacles}/{len(obstacles_data)}")                        continue        if data['time']:

    

    print()                plt.plot(data['time'], data['value'], 



def main():    return vectors, vector_defs                    label=f"UAV {vec_id}", alpha=0.7)

    # Configuração de caminhos

    script_dir = Path(__file__).parent    

    results_dir = script_dir / 'simulations' / 'results'

    def analyze_results(results_dir='simulations/results'):    plt.xlabel('Time (s)')

    if not results_dir.exists():

        print(f"❌ Erro: Diretório de resultados não encontrado: {results_dir}")    """Analyze simulation results and generate plots"""    plt.ylabel('Fitness (distance to target, m)')

        sys.exit(1)

            plt.title('Bat Algorithm - Fitness Convergence')

    # Configurações disponíveis

    all_configs = ['General', 'QuickTest', 'SimpleObstacles', 'ManyObstacles',     results_path = Path(results_dir)    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')

                   'FastAvoidance', 'ObstacleCourse']

            plt.grid(True, alpha=0.3)

    # Detectar quais configurações têm resultados

    configs = [c for c in all_configs     # Find the latest results    plt.tight_layout()

              if (results_dir / f"{c}-#0.sca").exists()]

        sca_files = list(results_path.glob('*.sca'))    

    if not configs:

        print("❌ Nenhum resultado encontrado!")    vec_files = list(results_path.glob('*.vec'))    output_file = os.path.join(output_dir, 'fitness_convergence.png')

        print("   Execute uma simulação primeiro: ./run_sim.sh")

        sys.exit(1)        plt.savefig(output_file, dpi=300)

    

    print("\n" + "="*70)    if not sca_files or not vec_files:    print(f"Saved: {output_file}")

    print("  🦇 Bat Algorithm - Análise de Resultados")

    print("="*70)        print("No result files found!")    plt.close()

    

    # Gerar análises para cada configuração        return

    for config in configs:

        print(f"\n📈 Analisando {config}...")    def plot_average_fitness(vectors, output_dir):

        plot_fitness_convergence(results_dir, config)

        plot_obstacle_avoidance(results_dir, config)    # Use the latest files    """Plot average fitness of the swarm over time."""

        print_summary(results_dir, config)

        latest_sca = max(sca_files, key=lambda p: p.stat().st_mtime)    fitness_vectors = {k: v for k, v in vectors.items() 

    print("\n" + "="*70)

    print("  ✅ Análise completa!")    latest_vec = max(vec_files, key=lambda p: p.stat().st_mtime)                      if 'fitness' in v['name'].lower()}

    print(f"  📁 Gráficos salvos em: {results_dir / 'analysis'}")

    print("="*70 + "\n")        



if __name__ == '__main__':    print(f"Analyzing: {latest_sca.name}")    if not fitness_vectors:

    main()

    print(f"Vector data: {latest_vec.name}")        print("No fitness data found")

    print()        return

        

    # Parse data    # Find common time points

    scalars = parse_scalar_file(latest_sca)    all_times = set()

    vectors, vector_defs = parse_vector_file(latest_vec)    for data in fitness_vectors.values():

            all_times.update(data['time'])

    # Extract relevant metrics    

    fitness_data = {}    times = sorted(list(all_times))

    loudness_data = {}    avg_fitness = []

    pulserate_data = {}    

        for t in times:

    for vec_id, data in vectors.items():        values = []

        if vec_id in vector_defs:        for data in fitness_vectors.values():

            module, name = vector_defs[vec_id]            # Find closest time point

                        if data['time']:

            if 'fitness' in name:                idx = min(range(len(data['time'])), 

                fitness_data[module] = data                         key=lambda i: abs(data['time'][i] - t))

            elif 'loudness' in name:                values.append(data['value'][idx])

                loudness_data[module] = data        

            elif 'pulseRate' in name:        if values:

                pulserate_data[module] = data            avg_fitness.append(np.mean(values))

        

    # Print summary statistics    plt.figure(figsize=(12, 6))

    print("=" * 60)    plt.plot(times, avg_fitness, 'b-', linewidth=2, label='Average Fitness')

    print("SUMMARY STATISTICS")    plt.fill_between(times, 

    print("=" * 60)                     [f - np.std([v['value'][min(range(len(v['time'])), 

                                                      key=lambda i: abs(v['time'][i] - t))] 

    # Detect network type                                 for v in fitness_vectors.values()]) 

    network_name = None                      for t, f in zip(times, avg_fitness)],

    for key in scalars.keys():                     [f + np.std([v['value'][min(range(len(v['time'])), 

        if 'BatSwarmNetwork' in key:                                                  key=lambda i: abs(v['time'][i] - t))] 

            if 'WithObstacles' in key:                                 for v in fitness_vectors.values()]) 

                network_name = 'BatSwarmNetworkWithObstacles'                      for t, f in zip(times, avg_fitness)],

            else:                     alpha=0.3, label='Standard Deviation')

                network_name = 'BatSwarmNetworkSimple'    

            break    plt.xlabel('Time (s)')

        plt.ylabel('Average Fitness (m)')

    if not network_name:    plt.title('Bat Algorithm - Average Swarm Fitness')

        network_name = 'BatSwarmNetwork'    plt.legend()

        plt.grid(True, alpha=0.3)

    # Count UAVs    plt.tight_layout()

    num_uavs = len([k for k in scalars.keys() if 'uav[' in k and 'fitness' in k])    

    print(f"Network: {network_name}")    output_file = os.path.join(output_dir, 'average_fitness.png')

    print(f"Number of UAVs: {num_uavs}")    plt.savefig(output_file, dpi=300)

        print(f"Saved: {output_file}")

    # Fitness statistics    plt.close()

    final_fitness = []

    initial_fitness = []def plot_loudness_pulseRate(vectors, output_dir):

    obstacles_avoided = []    """Plot loudness and pulse rate evolution."""

        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))

    for i in range(num_uavs):    

        # Final fitness    # Loudness

        key = f"{network_name}.uav[{i}].batAlgorithm.fitness:last"    loudness_vectors = {k: v for k, v in vectors.items() 

        if key in scalars:                       if 'loudness' in v['name'].lower()}

            final_fitness.append(scalars[key])    for vec_id, data in loudness_vectors.items():

                if data['time']:

        # Obstacles avoided            ax1.plot(data['time'], data['value'], alpha=0.6)

        key = f"{network_name}.uav[{i}].batAlgorithm.obstaclesAvoided:last"    

        if key in scalars:    ax1.set_xlabel('Time (s)')

            obstacles_avoided.append(int(scalars[key]))    ax1.set_ylabel('Loudness')

            ax1.set_title('Bat Algorithm - Loudness Evolution')

        # Initial fitness (from vector data)    ax1.grid(True, alpha=0.3)

        module = f"{network_name}.uav[{i}].batAlgorithm"    

        if module in fitness_data and len(fitness_data[module]['value']) > 0:    # Pulse Rate

            initial_fitness.append(fitness_data[module]['value'][0])    pulse_vectors = {k: v for k, v in vectors.items() 

                        if 'pulse' in v['name'].lower()}

    if final_fitness:    for vec_id, data in pulse_vectors.items():

        print(f"\nFitness (distance to target):")        if data['time']:

        print(f"  Mean final fitness: {np.mean(final_fitness):.2f}m")            ax2.plot(data['time'], data['value'], alpha=0.6)

        print(f"  Best final fitness: {np.min(final_fitness):.2f}m")    

        print(f"  Worst final fitness: {np.max(final_fitness):.2f}m")    ax2.set_xlabel('Time (s)')

            ax2.set_ylabel('Pulse Rate')

        if initial_fitness:    ax2.set_title('Bat Algorithm - Pulse Rate Evolution')

            improvement = np.mean(initial_fitness) - np.mean(final_fitness)    ax2.grid(True, alpha=0.3)

            improvement_pct = (improvement / np.mean(initial_fitness)) * 100    

            print(f"  Average improvement: {improvement:.2f}m ({improvement_pct:.1f}%)")    plt.tight_layout()

        

    if obstacles_avoided and sum(obstacles_avoided) > 0:    output_file = os.path.join(output_dir, 'loudness_pulserate.png')

        print(f"\nObstacle Avoidance:")    plt.savefig(output_file, dpi=300)

        print(f"  Total obstacles avoided: {sum(obstacles_avoided)}")    print(f"Saved: {output_file}")

        print(f"  Average per UAV: {np.mean(obstacles_avoided):.1f}")    plt.close()

        print(f"  Max by single UAV: {max(obstacles_avoided)}")

    def print_statistics(vectors):

    # Create plots    """Print statistical summary of the results."""

    fig, axes = plt.subplots(2, 2, figsize=(14, 10))    fitness_vectors = {k: v for k, v in vectors.items() 

    fig.suptitle(f'Bat Algorithm Analysis - {latest_sca.stem}', fontsize=16)                      if 'fitness' in v['name'].lower()}

        

    # Plot 1: Fitness convergence    if not fitness_vectors:

    ax = axes[0, 0]        print("No fitness data found")

    for module, data in fitness_data.items():        return

        if len(data['time']) > 0:    

            ax.plot(data['time'], data['value'], alpha=0.6, linewidth=0.8)    print("\n" + "="*60)

    ax.set_xlabel('Time (s)')    print("SIMULATION STATISTICS")

    ax.set_ylabel('Fitness (distance to target, m)')    print("="*60)

    ax.set_title('Fitness Convergence Over Time')    

    ax.grid(True, alpha=0.3)    print(f"\nNumber of UAVs: {len(fitness_vectors)}")

        

    # Plot 2: Average fitness    # Initial and final fitness

    ax = axes[0, 1]    initial_fitness = []

    if fitness_data:    final_fitness = []

        # Calculate average fitness at each time point    

        all_times = set()    for data in fitness_vectors.values():

        for data in fitness_data.values():        if data['value']:

            all_times.update(data['time'])            initial_fitness.append(data['value'][0])

                    final_fitness.append(data['value'][-1])

        times = sorted(all_times)    

        avg_fitness = []    if not initial_fitness:

                print("\n⚠️  No fitness data found in vectors!")

        for t in times:        print("Make sure the simulation is recording fitness values.")

            values = []        return

            for data in fitness_data.values():    

                if t in data['time']:    print(f"\nInitial Fitness:")

                    idx = data['time'].index(t)    print(f"  Mean: {np.mean(initial_fitness):.2f} m")

                    values.append(data['value'][idx])    print(f"  Std:  {np.std(initial_fitness):.2f} m")

            if values:    print(f"  Min:  {np.min(initial_fitness):.2f} m")

                avg_fitness.append(np.mean(values))    print(f"  Max:  {np.max(initial_fitness):.2f} m")

            

        ax.plot(times[:len(avg_fitness)], avg_fitness, 'b-', linewidth=2)    print(f"\nFinal Fitness:")

        ax.fill_between(times[:len(avg_fitness)], avg_fitness, alpha=0.3)    print(f"  Mean: {np.mean(final_fitness):.2f} m")

        ax.set_xlabel('Time (s)')    print(f"  Std:  {np.std(final_fitness):.2f} m")

        ax.set_ylabel('Average Fitness (m)')    print(f"  Min:  {np.min(final_fitness):.2f} m")

        ax.set_title('Swarm Average Fitness')    print(f"  Max:  {np.max(final_fitness):.2f} m")

        ax.grid(True, alpha=0.3)    

        improvement = [(i - f) / i * 100 for i, f in zip(initial_fitness, final_fitness) if i > 0]

    # Plot 3: Loudness evolution    if improvement:

    ax = axes[1, 0]        print(f"\nImprovement:")

    for module, data in loudness_data.items():        print(f"  Mean: {np.mean(improvement):.2f}%")

        if len(data['time']) > 0:        print(f"  Best: {np.max(improvement):.2f}%")

            ax.plot(data['time'], data['value'], alpha=0.6, linewidth=0.8)        print(f"  Worst: {np.min(improvement):.2f}%")

    ax.set_xlabel('Time (s)')    

    ax.set_ylabel('Loudness')    print("\n" + "="*60)

    ax.set_title('Loudness Decay Over Time')

    ax.grid(True, alpha=0.3)def main():

    ax.set_ylim([0, 1])    """Main analysis function."""

        # Get results directory

    # Plot 4: Pulse rate evolution    if len(sys.argv) > 1:

    ax = axes[1, 1]        results_dir = sys.argv[1]

    for module, data in pulserate_data.items():    else:

        if len(data['time']) > 0:        results_dir = os.path.join(os.path.dirname(__file__), 

            ax.plot(data['time'], data['value'], alpha=0.6, linewidth=0.8)                                   'simulations', 'results')

    ax.set_xlabel('Time (s)')    

    ax.set_ylabel('Pulse Rate')    if not os.path.exists(results_dir):

    ax.set_title('Pulse Rate Increase Over Time')        print(f"Error: Results directory not found: {results_dir}")

    ax.grid(True, alpha=0.3)        print("Run the simulation first!")

    ax.set_ylim([0, 1])        return 1

        

    plt.tight_layout()    # Find .vec files

        vec_files = list(Path(results_dir).glob('*.vec'))

    # Save plot    

    output_file = results_path / 'analysis' / 'convergence_analysis.png'    if not vec_files:

    output_file.parent.mkdir(exist_ok=True)        print(f"Error: No .vec files found in {results_dir}")

    plt.savefig(output_file, dpi=150, bbox_inches='tight')        return 1

    print(f"\nPlot saved to: {output_file}")    

        print(f"Found {len(vec_files)} result file(s)")

    # Show plot    

    plt.show()    # Create output directory for plots

    output_dir = os.path.join(results_dir, 'analysis')

if __name__ == '__main__':    os.makedirs(output_dir, exist_ok=True)

    results_dir = sys.argv[1] if len(sys.argv) > 1 else 'simulations/results'    

    analyze_results(results_dir)    # Process each file

    for vec_file in vec_files:
        print(f"\nAnalyzing: {vec_file.name}")
        
        # Parse data
        vectors = parse_vec_file(vec_file)
        print(f"  Found {len(vectors)} data vectors")
        
        # Generate plots
        plot_fitness_convergence(vectors, output_dir)
        plot_average_fitness(vectors, output_dir)
        plot_loudness_pulseRate(vectors, output_dir)
        
        # Print statistics
        print_statistics(vectors)
    
    print(f"\nAnalysis complete! Plots saved in: {output_dir}")
    return 0

if __name__ == '__main__':
    try:
        import matplotlib
        matplotlib.use('Agg')  # Use non-interactive backend
    except ImportError:
        print("Error: matplotlib not installed")
        print("Install with: pip install matplotlib numpy")
        sys.exit(1)
    
    sys.exit(main())
