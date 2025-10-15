# 🦇 Bat Algorithm for FANET Routing

## 📡 Real Purpose Implementation

Este projeto agora implementa a **aplicação real** do Bat Algorithm em FANETs (Flying Ad-Hoc Networks): **Roteamento Adaptativo de Pacotes**.

## 🎯 O Que Foi Implementado

### **Antes (Navegação Apenas)**
- ❌ Bat Algorithm usado apenas para movimentação física
- ❌ Sem comunicação real entre drones
- ❌ Sem roteamento de dados

### **Agora (Roteamento Completo)**
- ✅ **BatRouting**: Protocolo de roteamento usando Bat Algorithm
- ✅ **Descoberta de Rotas**: Exploração inteligente de caminhos
- ✅ **Otimização Multi-Objetivo**: Fitness baseado em múltiplas métricas
- ✅ **Tabela de Rotas Dinâmica**: Adaptação às mudanças da rede

## 🧮 Como Funciona o Bat Routing

### **1. Descoberta de Rotas (Exploração)**
```
- Usa pulse rate (r) para decidir quando buscar novas rotas
- Frequency (f) controla agressividade da busca
- Loudness (A) define probabilidade de forwarding
```

### **2. Fitness de Rota**
```cpp
fitness = hop_count × w₁ + 
          (1 / link_quality) × w₂ + 
          energy_cost × w₃ + 
          mobility × w₄
```

**Métricas:**
- **Hop Count**: Número de saltos (menos é melhor)
- **Link Quality**: Qualidade do sinal (0-1, baseado em distância)
- **Energy Cost**: Consumo energético estimado
- **Mobility**: Velocidade dos nós (rotas mais estáveis)

### **3. Seleção de Rota**
- Mantém top-3 melhores rotas por destino
- Ordena por fitness (menor = melhor)
- Atualiza periodicamente (5s default)

### **4. Parâmetros Bat Algorithm**

| Parâmetro | Valor | Função |
|-----------|-------|--------|
| `frequencyMin/Max` | 0.0 - 2.0 | Controla exploração vs exploitation |
| `loudness` | 0.9 → decai | Probabilidade de forward (alta no início) |
| `pulseRate` | 0.5 → cresce | Frequência de descoberta (aumenta com tempo) |
| `alpha` | 0.9 | Taxa de decaimento do loudness |
| `gamma` | 0.9 | Taxa de crescimento do pulse rate |

## 🔧 Arquitetura

```
UAV Module
├── mobility (ArbitraryMobility)
│   └── Posição física 3D
├── batAlgorithm (BatAlgorithm) 
│   └── Navegação e movimento
└── batRouting (BatRouting) ⭐ NOVO
    └── Roteamento de pacotes
```

## 📊 Métricas Coletadas

### **Estatísticas do BatRouting**
- `routeDiscovered`: Número de rotas descobertas
- `packetRouted`: Número de pacotes roteados
- Tamanho da tabela de rotas
- Fitness médio das rotas

## 🚀 Como Usar

### **1. Compilar**
```bash
MODE=release ./make.sh
```

### **2. Executar Simulação**
```bash
./run_qtenv_fixed.sh
```

### **3. Observar Roteamento**
No Qtenv, você verá:
- Descoberta de rotas entre drones
- Atualização de tabelas de roteamento
- Logs de fitness de rotas
- Mensagens de roteamento

## 📈 Exemplo de Saída

```
BatRouting: Node 0 initialized
BatRouting: Node 0 - Route discovered from 2 with 2 hops
BatRouting: Node 0 - Updated route to 2 (fitness: 3.45)
BatRouting: Node 0 - Routing packet to 2 via route with 2 hops
```

## 🔬 Diferencial do Bat Algorithm

### **vs. AODV/DSR (Tradicionais)**
- ✅ Multi-objetiv

o (não só hop count)
- ✅ Adaptação contínua (pulse rate/loudness)
- ✅ Exploração inteligente (frequency)

### **vs. ACO (Ant Colony)**
- ✅ Menos overhead (não precisa feromônio em cada link)
- ✅ Convergência mais rápida
- ✅ Melhor para redes altamente dinâmicas

## 📚 Referências Acadêmicas

1. **Yang, X.-S. (2010)** - "A New Metaheuristic Bat-Inspired Algorithm"
2. **Bitam et al. (2014)** - "Bee Life-based Multi Constraints Multicast Routing Optimization for Vehicular Ad Hoc Networks"
3. **Gupta et al. (2018)** - "Bat algorithm based QoS aware routing protocol for wireless sensor networks"

## 🎓 Aplicações Reais

- 🚁 Roteamento em enxames de drones
- 📡 Redes de sensores sem fio
- 🚗 VANETs (Vehicular Ad-Hoc Networks)
- 🌐 IoT em larga escala
- 🛰️ Redes de satélites

## ⚙️ Configuração

### **omnetpp.ini**
```ini
# Bat Routing parameters
*.uav[*].batRouting.routingUpdateInterval = 5s
*.uav[*].batRouting.hopCountWeight = 1.0
*.uav[*].batRouting.linkQualityWeight = 1.5
*.uav[*].batRouting.energyWeight = 1.0
*.uav[*].batRouting.mobilityWeight = 0.8
*.uav[*].batRouting.maxRoutesPerDestination = 3
```

## 🎯 Próximos Passos Possíveis

1. ✨ Adicionar módulo de aplicação (gerador de tráfego)
2. ✨ Implementar QoS (Quality of Service)
3. ✨ Adicionar segurança (rotas autenticadas)
4. ✨ Implementar agregação de dados
5. ✨ Comparar com AODV/OLSR

---

**Agora sim, o Bat Algorithm está sendo usado para sua proposta REAL em FANETs! 🚁📡**
