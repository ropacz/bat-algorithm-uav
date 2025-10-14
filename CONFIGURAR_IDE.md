# Configurar INET no IDE do OMNeT++

## 🎯 Passos para Configurar o Projeto no IDE

### Passo 1: Abrir o IDE
```bash
cd ~/omnetpp-workspace/bat-algorithm
opp_env shell omnetpp-6.2.0 inet-4.5.4
omnetpp
```

### Passo 2: Importar/Atualizar Projeto
1. **File → Import → General → Existing Projects into Workspace**
2. Selecione: `/Users/rodrigo/omnetpp-workspace/bat-algorithm`
3. Marque: ✅ "Copy projects into workspace" (DESMARQUE isso se já estiver importado)
4. **Finish**

### Passo 3: Configurar Dependências do INET
1. **Clique direito no projeto `bat-algorithm`**
2. **Properties**
3. **Project References**
4. ✅ Marque: **inet-4.5.4**
5. **Apply and Close**

### Passo 4: Configurar Build Options
1. **Clique direito no projeto `bat-algorithm`**
2. **Properties**
3. **OMNeT++ → Makemake**
4. Selecione a pasta **src**
5. **Options...**
6. Na aba **Target**:
   - Target type: **Executable**
7. Na aba **Compile**:
   - Em **Include path** adicione: 
     ```
     $(INET_4_5_4_PROJ)/src
     ```
   - Marque: ✅ **INET_IMPORT**
8. Na aba **Link**:
   - Em **Libraries** adicione:
     ```
     INET$(D)
     ```
   - Em **Library path** adicione:
     ```
     $(INET_4_5_4_PROJ)/src
     ```
9. Na aba **Custom**:
   - Em **Makefile variables (-K)** adicione:
     ```
     INET_4_5_4_PROJ=/Users/rodrigo/omnetpp-workspace/inet-4.5.4
     ```
10. **OK** → **Apply and Close**

### Passo 5: Regenerar Makefile
1. **Clique direito no projeto `bat-algorithm`**
2. **Build Project**
3. Ou: **Project → Clean → Clean all projects**

---

## 🚀 Método Alternativo (Mais Rápido via Terminal)

Se o IDE ainda não funcionar, use o terminal dentro do `opp_env`:

```bash
# Entrar no ambiente
opp_env shell omnetpp-6.2.0 inet-4.5.4

# Ir para o projeto
cd ~/omnetpp-workspace/bat-algorithm

# Limpar tudo
make cleanall

# Regenerar Makefiles com configurações corretas
cd src
opp_makemake -f --deep -O out \
  -KINET_4_5_4_PROJ=/Users/rodrigo/omnetpp-workspace/inet-4.5.4 \
  -DINET_IMPORT \
  -I'$(INET_4_5_4_PROJ)/src' \
  -L'$(INET_4_5_4_PROJ)/src' \
  -lINET'$(D)'

# Voltar e compilar
cd ..
make MODE=debug
make MODE=release
```

---

## ✅ Verificar se Funcionou

No IDE:
1. **Project → Clean**
2. **Project → Build Project**
3. Deve compilar sem erros! ✅

---

## 📝 Notas Importantes

- ⚠️ **SEMPRE** abra o IDE via `opp_env shell` ou usando `omnetpp` dentro do ambiente
- ⚠️ O arquivo `.oppbuildspec` já foi atualizado automaticamente
- ⚠️ O arquivo `.nedfolders` deve conter apenas `src` e `simulations` (NÃO inclua `inet`)
- ⚠️ O arquivo `.project` NÃO deve ter `linkedResources` para INET (causa erro de package)
- ⚠️ Se ainda tiver problemas, delete o projeto do IDE e reimporte

---

## 🐛 Se Ver Erro "Declared package 'inet.examples...' does not match"

Isso significa que o IDE está tentando carregar TODOS os exemplos do INET. Solução:

1. **Verifique `.nedfolders`** - deve conter APENAS:
   ```
   src
   simulations
   ```

2. **Remova linked resources** do `.project`:
   - Abra `.project`
   - Delete toda a seção `<linkedResources>...</linkedResources>`
   - Salve

3. **Refresh no IDE**:
   - **Project → Clean**
   - **File → Refresh** (F5)

---

## 🐛 Se Ainda Não Funcionar

Delete os arquivos de configuração e deixe o IDE regenerar:

```bash
cd ~/omnetpp-workspace/bat-algorithm
rm .cproject
rm src/Makefile
```

Depois, no IDE:
1. **Project → Clean**
2. Siga os passos de configuração acima novamente
