# Preenchimento de Polígonos com ET e AET

Este projeto implementa o **algoritmo de preenchimento de polígonos baseado em coerência de arestas**, utilizando as estruturas **ET (Edge Table)** e **AET (Active Edge Table)**.  
A aplicação permite a criação interativa de polígonos 2D e o preenchimento automático de seu interior.

---

## Como utilizar o programa

### Compilação e execução
```bash
cmake -S . -B build
cmake --build build -j
./build/app
````

### Interação

* **Criar polígonos:** clique com o **botão esquerdo do mouse** para adicionar os vértices.
* **Fechar o polígono:** pressione **Espaço** para conectar o último vértice ao primeiro e preencher o interior com a cor selecionada.
* **Cores de preenchimento:**

  * `r` → vermelho
  * `g` → verde
  * `b` → azul
  * `w` → branco
* **Espessura da borda:**

  * `x` → aumenta
  * `z` → diminui
* **Sair do programa:** `Esc` ou botão de fechar da janela.

---

## Estrutura do algoritmo

1. **Tabela de Lados (ET):** armazena todas as arestas organizadas por linha de varredura (scanline).
2. **Tabela de Lados Ativos (AET):** mantém as arestas que cruzam a scanline atual.
3. **Processamento:**

   * Para cada linha de varredura, atualiza a AET.
   * Ordena os cruzamentos de arestas pelo `x`.
   * Preenche pares de interseções (regra par-ímpar).
   * Incrementa `x` usando o inverso da inclinação (1/m), explorando a coerência de arestas.

---

## Testes

O sistema foi validado em:

* **Polígonos simples** (convexos e côncavos).
* **Polígonos complexos** com interseções de múltiplas arestas.
  Em todos os casos, o preenchimento foi realizado corretamente, confirmando a validade da implementação.

---

## Autoria e contribuições

Matheus Ponciano - **14598358**
Pedro Porto - **14603436**


* **Matheus**

  * Implementação da lógica de desenho, armazenamento de polígonos e linhas.
  * Desenvolvimento das keybinds (atalhos de teclado), mudanças de cor e espessura.

* **Pedro e Matheus**

  * Implementação do algoritmo de ponto médio para rasterização de linhas (baseado no trabalho anterior).

* **Pedro**

  * Configuração do ambiente (OpenGL + CMake).
  * Implementação do algoritmo de preenchimento de polígonos (ET/AET).

---

## Observações

* Projeto desenvolvido em **C++ e OpenGL**.

