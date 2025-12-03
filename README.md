# Renderizador 3D via Software

Este projeto implementa um **renderizador 3D completo via software** (CPU) desenvolvido como trabalho de Computação Gráfica. Toda a rasterização é feita manualmente, sem uso de APIs gráficas 3D.

## Características Principais

- **Pipeline Gráfico Completo**: Implementação de todas as etapas do pipeline 3D, desde transformações até rasterização
- **Rasterização Manual**: Algoritmo de Bresenham para linhas e scanline fill para polígonos
- **Recorte**: Cohen-Sutherland para linhas e Sutherland-Hodgman para polígonos
- **Preenchimento de Polígonos**: Implementação de scanline com ET (Edge Table) e AET (Active Edge Table)
- **Z-Buffer**: Remoção de superfícies ocultas por profundidade
- **Três Modos de Shading**: Flat, Gouraud e Phong
- **Sistema de Iluminação**: Modelo de iluminação Phong com componentes ambiente, difusa e especular
- **Câmera Dual**: Modo orbital (rotação em torno de um ponto) e modo FPS (navegação livre)
- **Projeções**: Perspectiva e ortográfica

---

## Funcionalidades Implementadas

### 1. Sistema de Câmera
- **Modo Orbital**: Rotação da câmera em torno de um ponto focal usando o mouse (tecla `9` para ativar)
- **Modo FPS**: Movimentação livre no espaço 3D com controles WASD/QE (tecla `9` para alternar)
- **Projeção Perspectiva**: Visão realista com profundidade (tecla `7`)
- **Projeção Ortográfica**: Visão sem distorção de perspectiva (tecla `7` para alternar)
- **Gizmo Visual**: Eixos XYZ coloridos no ponto focal da câmera

### 2. Criação de Objetos

#### Primitivas Geométricas (Menu Forma - tecla `F`)
- **Cubo**: Sólido com 6 faces quadradas
- **Esfera**: Gerada por subdivisão (12 stacks × 24 slices)
- **Cilindro**: Com 16 subdivisões circulares
- **Pirâmide**: Base quadrada com 4 faces triangulares

Para criar: Entre no menu de formas (`F`), selecione a forma (teclas `1-4`) e clique com o botão esquerdo do mouse.

#### Sistema de Extrusão (Menu Extrusão - tecla `G`)
Sistema interativo para criar sólidos 3D customizados:

1. **Escolha do Plano de Desenho** (tecla `P`):
   - Plano XY (padrão)
   - Plano XZ
   - Plano YZ

2. **Modo Desenho** (tecla `O`):
   - Clique com botão esquerdo para adicionar vértices do polígono
   - Sistema de ray-casting para projetar cliques do mouse no plano 3D
   - Preview em tempo real do próximo vértice
   - Linhas amarelas conectam os vértices desenhados
   - Pressione `SPACE` após desenhar no mínimo 3 vértices para iniciar extrusão

3. **Modo Extrusão**:
   - Movimente o mouse para ajustar a altura da extrusão
   - Preview do sólido em tempo real (cor cyan)
   - Geração automática de faces laterais, base e topo
   - Cálculo automático de normais para iluminação correta
   - Pressione `SPACE` para confirmar e criar o sólido

### 3. Modo de Transformação (tecla `H`)

Sistema completo para manipular objetos na cena:

- **Seleção de Objetos** (tecla `TAB`):
  - Navega entre os objetos criados
  - Objeto selecionado é destacado em amarelo/laranja
  - Câmera move automaticamente o ponto focal para o centro do objeto
  - Contador de objetos no HUD

- **Translação** (tecla `T`):
  - Move objeto seguindo o ponto focal da câmera
  - Gizmo ampliado para melhor visualização
  - Confirmação com `SPACE`

- **Rotação** (tecla `R`):
  - Arraste o mouse para rotacionar o objeto em torno de seu centro
  - Rotação em X e Y baseada no movimento do mouse
  - Feedback visual imediato
  - Confirmação com `SPACE`

- **Escala** (teclas `Z/X`):
  - `Z`: Diminui escala (0.9x)
  - `X`: Aumenta escala (1.1x)
  - Escala proporcional em todos os eixos

- **Alteração de Material**:
  - **Cor** (tecla `C`): 10 cores predefinidas (preto, branco, vermelho, verde, azul, laranja, amarelo, índigo, ciano, rosa)
  - **Material** (tecla `M`): 4 materiais com propriedades diferentes
    - Borracha: ka=0.05, kd=0.5, ks=0.7, shininess=10
    - Plástico: ka=0.0, kd=0.5, ks=0.7, shininess=32
    - Metal: ka=0.2, kd=0.7, ks=0.6, shininess=51
    - Pedra: ka=0.25, kd=0.95, ks=0.3, shininess=11

- **Remoção** (tecla `DEL` ou `BACKSPACE`):
  - Deleta o objeto selecionado da cena
  - Seleção automática do próximo objeto disponível

### 4. Sistema de Renderização

#### Shading (teclas `1/2/3`)
- **Flat Shading** (tecla `1`): Uma cor por face, calculada no centro da face
- **Gouraud Shading** (tecla `2`): Interpolação de intensidade entre vértices
- **Phong Shading** (tecla `3`): Interpolação de normais para iluminação por pixel (mais realista)

#### Iluminação
- Modelo de Phong com três componentes:
  - **Ambiente (ka)**: Iluminação uniforme base
  - **Difusa (kd)**: Baseada no ângulo entre normal e luz
  - **Especular (ks)**: Reflexos brilhantes baseados no ângulo de visão
- Luz pontual configurável (posição padrão: 5, 5, 5)

### 5. Interface e HUD

- **Menu Principal** (sempre visível):
  - Tipo de projeção atual
  - Modo de shading ativo
  - Modo de câmera (Orbit/FPS)
  - Contador de FPS em tempo real
  - Atalhos de teclado contextuais

- **Menus Contextuais**:
  - Navegação por teclas de atalho
  - Indicadores visuais do item selecionado
  - Instruções específicas para cada modo
  - Tecla `ESC` sempre volta ao menu anterior

- **Gizmo de Orientação**:
  - Eixos X (vermelho), Y (verde), Z (azul)
  - Escala adaptativa no modo Transform
  - Visível apenas no modo Orbital

- **Reset de Gizmo** (tecla `B`):
  - Reposiciona o ponto focal da câmera para origem (0,0,0)

---

## Implementação Técnica

### Algoritmos Implementados

#### Rasterização
- **Bresenham**: Traçado de linhas pixel a pixel com precisão
- **Scanline Fill com ET/AET**: Preenchimento de polígonos usando Edge Table e Active Edge Table

#### Recorte (Clipping)
- **Cohen-Sutherland**: Recorte de linhas contra janela 2D
- **Sutherland-Hodgman**: Recorte de polígonos contra planos 3D

#### Visibilidade
- **Z-Buffer**: Buffer de profundidade para determinar pixels visíveis

#### Iluminação
- **Modelo de Phong**: Componentes ambiente, difusa e especular
- **Flat Shading**: Intensidade calculada uma vez por face (no centro)
- **Gouraud Shading**: Intensidade calculada nos vértices e interpolada
- **Phong Shading**: Normais interpoladas e intensidade calculada por pixel


### Dependências

- **GLFW**: Criação de janela e input (mouse/teclado)
- **GLM**: Biblioteca matemática para vetores e matrizes
- **GLAD**: Carregador de funções OpenGL
- **OpenGL**: Usado apenas para exibir o framebuffer como textura (a renderização 3D é toda em software)

### Pipeline de Renderização

1. **Transformação**: Vértices dos objetos (model space → world space)
2. **View Transform**: Posicionamento da câmera (world space → camera space)
3. **Clipping 3D**: Sutherland-Hodgman contra planos da câmera
4. **Projeção**: Perspectiva ou ortográfica (camera space → clip space)
5. **Viewport**: Transformação para coordenadas de tela
6. **Clipping 2D**: Cohen-Sutherland para linhas
7. **Rasterização**: Bresenham (linhas) e Scanline Fill (polígonos)
8. **Z-Buffer**: Teste de profundidade por pixel
9. **Shading**: Cálculo de cor final baseado no modo ativo

---

## Como Rodar o Programa

### Opção 1: Executáveis Pré-compilados

Na pasta `dist/` você encontrará executáveis pré-compilados para:
- **Linux**: `dist/app`
- **Windows**: `dist/app.exe`

Basta executar o arquivo correspondente ao seu sistema operacional.

### Opção 2: Compilar do Código Fonte

#### Requisitos
- CMake 3.10 ou superior
- Compilador C++ com suporte a C++11
- Sistema operacional Linux, Windows ou macOS
- Bibliotecas (baixadas automaticamente pelo CMake):
  - GLFW (para criação de janelas)
  - GLM (matemática vetorial)
  - GLAD (carregamento de funções OpenGL)
  - OpenGL (apenas para exibir o framebuffer como textura)

#### Compilação e Execução

```bash
# Configurar o projeto
cmake -S . -B build

# Compilar
cmake --build build -j

# Executar
./build/app
```

## Participação dos Membros

### Matheus Ponciano – 14598358

- Implementação do **sistema de câmera** (Orbit / FPS), incluindo:
  - movimentação com WASD/QE,
  - controle por mouse (rotação, zoom),
  - alternância entre modos e projeções.
- Desenvolvimento do **sistema de menus e HUD**:
  - menus de Câmera, Formas, Materiais, Extrusão e Transformação,
  - exibição de projeção, modo de shading, FPS e dicas de teclado contextuais.
- Implementação da **seleção e manipulação de objetos**:
  - navegação entre objetos (TAB),
  - modo de Transform (translate/rotate/scale),
  - highlight visual do objeto selecionado.
- Integração de **input de teclado e mouse** com o pipeline de interação.
- Ajustes de usabilidade geral:
  - mapeamento de teclas,
  - fluxo entre menus,
  - reset de câmera / gizmo.

### Pedro Porto – 14603436

- Implementação do **renderizador via software**:
  - z-buffer,
  - rasterização de polígonos com scanline + ET/AET,
  - traçado de linhas com Bresenham.
- Desenvolvimento do **pipeline de shading**:
  - Flat, Gouraud e Phong,
  - cálculo de iluminação Phong (ka, kd, ks, shininess),
  - integração com o sistema de materiais.
- Implementação dos **algoritmos de clipping**:
  - Cohen-Sutherland (linhas),
  - Sutherland-Hodgman (polígonos 3D/2D).
- Criação e discretização das **formas primitivas 3D**:
  - cubo, esfera, cilindro, pirâmide (e demais variações do trabalho).
- Implementação do sistema de **extrusão 3D**:
  - ray casting mouse → plano 3D,
  - construção do sólido (base, topo, laterais),
  - cálculo de normais por vértice/face.
- Organização da **arquitetura do projeto**:
  - estrutura de módulos (camera, shapes, renderer, menu, extrusion),
  - integração com CMake e bibliotecas externas (GLFW, GLM, GLAD, OpenGL para display).

### Ambos

- Definição da **arquitetura geral do sistema** e divisão de responsabilidades.
- Depuração e testes de:
  - artefatos de rasterização,
  - erros de profundidade e clipping,
  - consistência visual entre modos de shading.
- Ajustes de desempenho e correções de bugs ao longo do desenvolvimento.
- Elaboração da **documentação** e preparação do material de entrega do trabalho.
