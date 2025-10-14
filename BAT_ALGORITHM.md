# Bat Algorithm - Algoritmo do Morcego

## 📚 Fundamentos Teóricos

O **Bat Algorithm** (BA) é um algoritmo metaheurístico bio-inspirado desenvolvido por Xin-She Yang em 2010. Ele simula o comportamento de ecolocalização dos micromorcegos para encontrar presas.

### Comportamento Natural dos Morcegos

1. **Ecolocalização**: Morcegos emitem pulsos sonoros e escutam os ecos
2. **Frequência Variável**: Ajustam a frequência para detectar presas
3. **Volume (Loudness)**: Diminui conforme se aproximam da presa
4. **Taxa de Pulso**: Aumenta conforme se aproximam da presa

## 🧮 Equações do Algoritmo

### 1. Atualização de Frequência
```
f_i = f_min + (f_max - f_min) * β
```
onde β ∈ [0,1] é um número aleatório uniforme

### 2. Atualização de Velocidade
```
v_i^t = v_i^(t-1) * w + (x_best - x_i^t) * f_i
```
onde:
- v_i^t: velocidade do morcego i no tempo t
- x_i^t: posição do morcego i no tempo t
- x_best: melhor solução global atual
- f_i: frequência do morcego i
- w: fator de inércia (0.5 recomendado) para prevenir acumulação infinita

**Nota Importante**: A direção correta é (x_best - x_i^t), não (x_i^t - x_best), para que o morcego se mova EM DIREÇÃO ao melhor alvo.

### 3. Atualização de Posição
```
x_i^t = x_i^(t-1) + v_i^t * Δt
```
onde Δt é o intervalo de tempo (updateInterval)

**Nota Importante**: A multiplicação por Δt é ESSENCIAL para movimento contínuo e suave, sem "saltos" ou "teleportes".

### 4. Busca Local
Se rand > r_i (taxa de pulso), gera solução local:
```
x_new = x_best + ε * A_avg
```
onde:
- ε ∈ [-1,1]: número aleatório
- A_avg: volume médio de todos os morcegos

### 5. Atualização de Volume e Taxa de Pulso
Se a nova solução é aceita:
```
A_i^(t+1) = α * A_i^t
r_i^(t+1) = r_i^0 * [1 - exp(-γ * t)]
```
onde:
- α ∈ [0,1]: fator de redução do volume (típico: 0.9)
- γ > 0: fator de aumento da taxa de pulso (típico: 0.9)

## 🎯 Aplicação em Enxames de UAVs

### Mapeamento para Drones

| Conceito Bat Algorithm | Implementação UAV |
|------------------------|-------------------|
| Morcego | Drone/UAV |
| Presa/Alimento | Posição alvo |
| Fitness | Distância ao alvo |
| Frequência | Controle de exploração |
| Volume | Convergência (alto = exploração) |
| Taxa de pulso | Convergência (alto = exploração) |

### Parâmetros Recomendados

#### Para Convergência Rápida
```ini
*.uav[*].batFrequencyMin = 0.0
*.uav[*].batFrequencyMax = 2.0
*.uav[*].batLoudness = 0.95
*.uav[*].batPulseRate = 0.5
*.uav[*].batAlpha = 0.95      # Redução rápida
*.uav[*].batGamma = 0.95      # Aumento rápido
*.uav[*].batUpdateInterval = 0.5s
```

#### Para Exploração Ampla
```ini
*.uav[*].batLoudness = 0.9
*.uav[*].batAlpha = 0.85      # Redução lenta
*.uav[*].batGamma = 0.85      # Aumento lento
*.uav[*].batUpdateInterval = 2s
```

#### Balanceado (Recomendado)
```ini
*.uav[*].batLoudness = 0.9
*.uav[*].batAlpha = 0.9
*.uav[*].batGamma = 0.9
*.uav[*].batUpdateInterval = 1s
```

## 📊 Comportamento do Algoritmo

### Fase Inicial (Exploração)
- **Volume alto** (A ≈ 0.9): exploração ampla do espaço
- **Taxa de pulso baixa** (r ≈ 0.5): aceita soluções variadas
- **Velocidades aleatórias**: busca global

### Fase Intermediária (Transição)
- **Volume decrescente**: foco em regiões promissoras
- **Taxa de pulso crescente**: refinamento de soluções
- **Convergência gradual**: aproximação do alvo

### Fase Final (Exploitação)
- **Volume baixo** (A → 0): busca local intensa
- **Taxa de pulso alta** (r → 1): refinamento fino
- **Convergência**: todos os UAVs próximos ao alvo

## 🔬 Características do Algoritmo

### Vantagens
✅ **Simples de implementar**: poucas equações
✅ **Poucos parâmetros**: apenas f_min, f_max, α, γ
✅ **Convergência rápida**: especialmente para problemas contínuos
✅ **Equilíbrio automático**: exploração vs. exploitação via A e r
✅ **Busca local e global**: combinadas naturalmente
✅ **Movimento contínuo**: física realista com velocidade e inércia

### Desvantagens
❌ **Sensível a parâmetros**: α e γ afetam convergência
❌ **Pode convergir prematuramente**: se α muito alto
❌ **Comunicação**: requer conhecimento da melhor solução global
❌ **Requer ajuste fino**: velocidade máxima e limites de área

## 🐛 Problemas Comuns e Soluções

### Problema 1: Drones "Saltando" ou Teleportando
**Causa**: Atualização de posição sem considerar tempo (Δt)
**Solução**: 
```cpp
newPosition = currentPosition + velocity * updateInterval;
```

### Problema 2: Drones Saindo da Área de Simulação
**Causa**: Falta de limitação de posição (clamping)
**Solução**: 
```cpp
// Aplicar margens de segurança (5m)
newPosition.x = std::max(minX + 5.0, std::min(maxX - 5.0, newPosition.x));
newPosition.y = std::max(minY + 5.0, std::min(maxY - 5.0, newPosition.y));
newPosition.z = std::max(minZ + 5.0, std::min(maxZ - 5.0, newPosition.z));
```

### Problema 3: Velocidade Infinita
**Causa**: Acumulação de velocidade sem fator de damping
**Solução**: Adicionar fator de inércia (w = 0.5)
```cpp
velocity = velocity * inertia + (globalBest - currentPosition) * frequency;
```

### Problema 4: Direção Errada do Movimento
**Causa**: Equação invertida (x_i - x_best) em vez de (x_best - x_i)
**Solução**: Usar direção correta
```cpp
// CORRETO: move PARA o alvo
velocity = velocity * w + (globalBest - currentPosition) * frequency;
// ERRADO: move PARA LONGE do alvo
// velocity = velocity * w + (currentPosition - globalBest) * frequency;
```

### Problema 5: Limiting de Velocidade Incorreto
**Causa**: Limitar velocidade DEPOIS de calcular posição
**Solução**: Limitar ANTES
```cpp
// 1. Atualizar velocidade
updateVelocity();

// 2. LIMITAR velocidade IMEDIATAMENTE
double speed = velocity.length();
if (speed > maxSpeed) {
    velocity = velocity * (maxSpeed / speed);
}

// 3. Calcular nova posição com velocidade limitada
newPosition = currentPosition + velocity * updateInterval;
```

## ⚙️ Implementação - Detalhes Técnicos

### Fluxo de Atualização Correto

```cpp
void handleMessage(cMessage *msg) {
    // 1. Obter posição atual
    currentPosition = mobility->getCurrentPosition();
    
    // 2. Calcular fitness
    fitness = calculateFitness(currentPosition);
    
    // 3. Atualizar melhor pessoal
    if (fitness < personalBestFitness) {
        personalBest = currentPosition;
        personalBestFitness = fitness;
    }
    
    // 4. Obter melhor global
    Coord globalBest = getGlobalBest();
    
    // 5. Atualizar frequência (aleatória)
    frequency = frequencyMin + (frequencyMax - frequencyMin) * uniform(0, 1);
    
    // 6. Atualizar velocidade com INÉRCIA
    updateVelocity();  // v = v * 0.5 + (best - pos) * freq
    
    // 7. LIMITAR velocidade ANTES de calcular posição
    double speed = velocity.length();
    if (speed > maxSpeed) {
        velocity = velocity * (maxSpeed / speed);
    }
    
    // 8. Calcular nova posição com TEMPO
    Coord newPosition = currentPosition + velocity * updateInterval;
    
    // 9. CLAMP posição nos limites (com margens)
    newPosition.x = std::max(minX + 5.0, std::min(maxX - 5.0, newPosition.x));
    newPosition.y = std::max(minY + 5.0, std::min(maxY - 5.0, newPosition.y));
    newPosition.z = std::max(minZ + 5.0, std::min(maxZ - 5.0, newPosition.z));
    
    // 10. Avaliar nova solução
    double newFitness = calculateFitness(newPosition);
    
    // 11. Aceitar se melhor E rand < loudness
    if (newFitness < fitness && uniform(0, 1) < currentLoudness) {
        // Recalcular velocidade baseada no movimento REAL
        velocity = (newPosition - currentPosition) / updateInterval;
        
        // Aplicar nova posição
        mobility->setPositionVelocity(newPosition, velocity);
        
        // Atualizar loudness e pulse rate
        updateLoudnessAndPulseRate();
    }
    // 12. Busca local se rand > pulseRate
    else if (uniform(0, 1) > currentPulseRate) {
        // Busca local: pequeno random walk
        double searchRadius = 5.0 * currentLoudness;
        Coord localPosition = globalBest + Coord(
            uniform(-searchRadius, searchRadius),
            uniform(-searchRadius, searchRadius),
            uniform(-searchRadius/2, searchRadius/2)
        );
        
        // Clamp
        localPosition.x = std::max(minX + 5.0, std::min(maxX - 5.0, localPosition.x));
        localPosition.y = std::max(minY + 5.0, std::min(maxY - 5.0, localPosition.y));
        localPosition.z = std::max(minZ + 5.0, std::min(maxZ - 5.0, localPosition.z));
        
        double localFitness = calculateFitness(localPosition);
        
        if (localFitness < fitness) {
            // Calcular velocidade para movimento SUAVE
            velocity = (localPosition - currentPosition) / updateInterval;
            
            // Limitar velocidade
            speed = velocity.length();
            if (speed > maxSpeed) {
                velocity = velocity * (maxSpeed / speed);
            }
            
            mobility->setPositionVelocity(localPosition, velocity);
        }
    }
    
    // 13. Broadcast posição
    broadcastPosition();
    
    // 14. Agendar próximo update
    scheduleAt(simTime() + updateInterval, updateTimer);
}
```

### Função updateVelocity() Correta

```cpp
void updateVelocity() {
    Coord globalBest = getGlobalBest();
    
    // Fator de inércia para prevenir acumulação infinita
    double inertia = 0.5;
    
    // Direção CORRETA: (globalBest - currentPosition)
    // Isso faz o drone se mover PARA o alvo
    velocity = velocity * inertia + (globalBest - currentPosition) * frequency;
}
```

### Margens de Segurança

Para evitar que drones encoste nas bordas da área de simulação, use margens de segurança:

```cpp
// Margem de 5 metros
double margin = 5.0;

newPosition.x = std::max(minX + margin, std::min(maxX - margin, newPosition.x));
newPosition.y = std::max(minY + margin, std::min(maxY - margin, newPosition.y));
newPosition.z = std::max(minZ + margin, std::min(maxZ - margin, newPosition.z));
```

Isso garante que os drones fiquem pelo menos 5m distante das bordas.

## 📈 Métricas de Avaliação

### Fitness (Aptidão)
```
fitness = distância_euclidiana(posição_UAV, posição_alvo)
```
Menor é melhor!

### Taxa de Convergência
```
taxa = (fitness_inicial - fitness_final) / fitness_inicial
```

### Distância Média do Enxame
```
dist_média = média(distâncias_de_todos_UAVs_ao_alvo)
```

### Tempo de Convergência
Tempo até que todos os UAVs estejam dentro de um raio do alvo.

## 🎓 Conceitos Avançados

### Melhoria 1: Busca Local Adaptativa
Ajustar o raio de busca local baseado no fitness:
```cpp
double searchRadius = baseLoudness * (1 + fitness/maxDistance);
```

### Melhoria 2: Frequência Adaptativa
Ajustar faixa de frequência baseada na convergência:
```cpp
if (convergenceRate < threshold) {
    frequencyMax *= 1.1;  // Aumentar exploração
}
```

### Melhoria 3: Comunicação Limitada
UAVs só conhecem vizinhos dentro de raio de comunicação:
```cpp
Coord getLocalBest(double communicationRadius) {
    // Buscar apenas UAVs vizinhos
}
```

## 📖 Referências

1. **Yang, X. S. (2010)**. "A new metaheuristic bat-inspired algorithm". 
   In: Nature Inspired Cooperative Strategies for Optimization (NICSO 2010). 
   Springer, pp. 65-74.

2. **Yang, X. S. (2011)**. "Bat algorithm for multi-objective optimisation". 
   International Journal of Bio-Inspired Computation, 3(5), 267-274.

3. **Yang, X. S., & Gandomi, A. H. (2012)**. "Bat algorithm: a novel approach 
   for global engineering optimization". 
   Engineering Computations, 29(5), 464-483.

4. **Mirjalili, S., et al. (2014)**. "Novel approaches to autonomous 
   control of UAV formations". 
   International Journal of Innovative Computing, Information and Control.

## 🔗 Links Úteis

- **Artigo Original**: [Nature Inspired Cooperative Strategies for Optimization](https://link.springer.com/chapter/10.1007/978-3-642-12538-6_6)
- **Xin-She Yang's Website**: http://www.cs.mdx.ac.uk/staffpages/XSYang/
- **OMNeT++ Documentation**: https://omnetpp.org/documentation
- **INET Framework**: https://inet.omnetpp.org

## 💡 Dicas de Uso

1. **Comece com configuração padrão**: teste `SmallSwarm` primeiro
2. **Verifique movimento suave**: Se drones "saltam", revise multiplicação por Δt
3. **Ajuste margens de segurança**: Use pelo menos 5m para evitar colisões com bordas
4. **Monitore velocidades**: Se drones param, aumente frequencyMax
5. **Monitore convergência**: Se muito lenta, aumente alpha e gamma
6. **Use inércia**: Fator de 0.5 previne acumulação infinita de velocidade
7. **Limite velocidade ANTES de posição**: Ordem crítica para movimento realista
8. **Visualize no Qtenv**: Use `./run_qtenv_fixed.sh` para ver movimento em tempo real
9. **Analise resultados**: Execute `python analyze_results.py simulations/results`
10. **Verifique direção**: Equação deve ser (best - current), não (current - best)

## 🔍 Checklist de Debugging

Se os drones não se comportam corretamente, verifique:

- [ ] Velocidade multiplicada por updateInterval na posição?
- [ ] Direção da equação de velocidade correta (best - current)?
- [ ] Fator de inércia aplicado (0.5 recomendado)?
- [ ] Velocity limiting ANTES do cálculo de posição?
- [ ] Position clamping com margens de segurança (5m)?
- [ ] Busca local usando movimento suave (não teleporte)?
- [ ] maxSpeed configurado adequadamente (recomendado: 10-20 m/s)?
- [ ] updateInterval não muito grande (recomendado: 0.5-2s)?

## 📝 Resumo das Correções Aplicadas (Out/2025)

### Correções Críticas de Física de Movimento

1. **Equação de Velocidade Corrigida**
   - ❌ Antes: `velocity + (currentPosition - globalBest) * frequency`
   - ✅ Depois: `velocity * inertia + (globalBest - currentPosition) * frequency`
   - **Impacto**: Drones agora se movem NA DIREÇÃO do alvo

2. **Atualização de Posição com Tempo**
   - ❌ Antes: `currentPosition + velocity`
   - ✅ Depois: `currentPosition + velocity * updateInterval`
   - **Impacto**: Movimento contínuo e suave, sem "saltos"

3. **Ordem de Limiting de Velocidade**
   - ❌ Antes: Limitar DEPOIS de calcular posição
   - ✅ Depois: Limitar ANTES de calcular posição
   - **Impacto**: Respeita velocidade máxima corretamente

4. **Remoção de Função localSearch() Separada**
   - ❌ Antes: Função separada que chamava `setPositionVelocity()` diretamente
   - ✅ Depois: Busca local integrada no handleMessage() com transições suaves
   - **Impacto**: Eliminou "teleportes" durante busca local

5. **Fator de Inércia Adicionado**
   - ❌ Antes: Acumulação infinita de velocidade
   - ✅ Depois: `velocity * 0.5` previne crescimento ilimitado
   - **Impacto**: Velocidades realistas e controláveis

6. **Margens de Segurança Aumentadas**
   - ❌ Antes: 1m de margem (insuficiente)
   - ✅ Depois: 5m de margem
   - **Impacto**: Drones não encostam nas bordas da área

### Resultado Final
✅ Movimento físico realista e contínuo
✅ Convergência suave para o alvo
✅ Sem "saltos" ou "teleportes"
✅ Respeito aos limites da área de simulação
✅ Velocidades controladas e limitadas
2. **Ajuste α e γ juntos**: valores similares (0.85-0.95)
3. **Aumente updateInterval**: para swarms grandes (reduz carga computacional)
4. **Visualize trails**: para entender comportamento
5. **Compare configurações**: use diferentes configs e compare resultados

## 🧪 Experimentos Sugeridos

### Experimento 1: Impacto de α
Teste α = [0.8, 0.85, 0.9, 0.95, 0.99] e compare tempo de convergência.

### Experimento 2: Tamanho do Enxame
Teste com 5, 10, 20, 30, 50 UAVs e analise escalabilidade.

### Experimento 3: Posição do Alvo
Mude targetX, targetY, targetZ e observe adaptação.

### Experimento 4: Restrições de Área
Reduza o espaço de busca e observe impacto.

### Experimento 5: Alvos Múltiplos
Modifique para ter vários alvos e divisão do enxame.

---

**Autor**: Implementação para OMNeT++/INET  
**Data**: Outubro 2025  
**Versão**: 1.0
