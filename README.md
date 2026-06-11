# 🗡️ Dungeon Crawler

> Um jogo de exploração e combate em **ASCII**, jogado inteiramente no console e desenvolvido na linguagem **C**.

**Disciplina:** Algoritmos e Codificação de Sistemas
**Curso:** Ciência da Computação — CESUPA
**Ano:** 2026

---

## 👥 Desenvolvedores

Projeto desenvolvido por **dois estudantes de Ciência da Computação**:

- Gabriel LIma
- Diego Viana

---

## 📖 História

Você é um aventureiro que chega a uma vila à beira de uma antiga masmorra. Antes de descer, é preciso conversar com o NPC da vila e escolher uma arma. A partir daí, o objetivo é atravessar os **três andares** da masmorra — cada um mais perigoso que o anterior — enfrentando armadilhas, monstros e, por fim, derrotando o **Boss Final** para salvar a vila.

---

## 🎮 Como Jogar

### Objetivo

Descer os 3 andares da masmorra, resolvendo os desafios de cada fase (chaves, portas, caixas, espinhos e monstros) até chegar ao confronto final com o Boss.

### Controles

| Tecla | Ação |
|:-----:|------|
| `W` | Move para cima (símbolo vira `^`) |
| `A` | Move para a esquerda (símbolo vira `<`) |
| `S` | Move para baixo (símbolo vira `v`) |
| `D` | Move para a direita (símbolo vira `>`) |
| `I` | Interage com o objeto à frente (NPC, porta, entrada) |
| `O` | Ataca na direção em que o jogador está olhando |
| `Q` | Sai da partida e volta ao menu |

> O jogador **não se move na diagonal**. O ataque sempre parte da posição do jogador, na direção para a qual ele está virado.

---

## 🗺️ Símbolos do Mapa

| Símbolo | Significado |
|:-------:|-------------|
| `^` `v` `<` `>` | Jogador (e a direção para a qual está olhando) |
| `*` | Parede — bloqueia a passagem |
| `#` | Espinho — o jogador perde 1 vida e a fase reinicia |
| `k` | Caixa — bloqueia a passagem, mas pode ser destruída com ataque |
| `O` | Botão — executa uma ação ao ser ativado |
| `D` | Porta fechada — precisa de chave para abrir |
| `@` | Chave — abre uma porta fechada |
| `=` | Porta aberta — pode ser atravessada |
| `L` | Escada — leva o jogador para a próxima fase |
| `N` | NPC — interaja para escolher sua arma |
| `E` | Entrada da masmorra |
| `X` | Monstro Tipo 1 |
| `Y` | Monstro Tipo 2 |
| `Z` | Boss Final |

---

## ⚔️ Armas

Ao interagir com o NPC da vila, o jogador escolhe **uma entre três armas**. A escolha vale para a partida inteira, e cada arma tem um padrão de ataque único (tecla `O`):

| Arma | Padrão de Ataque |
|------|------------------|
| ⚔️ **Espada** | Atinge uma área de **3×2 células** diretamente à frente do jogador |
| 🏹 **Arco e Flecha** | Atinge **4 células em linha reta** na direção em que o jogador olha |
| 🪄 **Cajado** | Atinge as **8 células ao redor** do jogador, independente da direção |

O ataque é espelhado corretamente para as quatro direções possíveis (cima, baixo, esquerda e direita).

---

## 🏛️ Estrutura do Jogo

O jogo começa pelo **Menu Principal**, com as opções *Jogar*, *Tutorial* e *Sair*.

| Fase | Tamanho | O que tem |
|------|:-------:|-----------|
| 🏡 **Vila** | 10×10 | NPC para escolha de arma + entrada da masmorra |
| 🔑 **Andar 1** | 10×10 | Caixas destrutíveis, chave, porta fechada e escada |
| ⚠️ **Andar 2** | 15×15 | Tudo do andar anterior + espinhos e Monstro Tipo 1 |
| 👑 **Andar 3** | 25×25 | Desafio final: 3 chaves, 3 portas, botão, Monstro Tipo 2 e o Boss |

---

## ❤️ Mecânicas Gerais

- O jogador começa com **3 vidas**.
- Ao pisar em um espinho ou ser atingido por um monstro, perde **1 vida** e a fase **reinicia do começo**.
- Ao perder todas as vidas, aparece a tela de **Game Over** e o jogo volta ao menu.
- **Chaves** abrem portas fechadas (cada chave abre uma porta e é consumida).
- **Caixas** bloqueiam o caminho, mas podem ser destruídas com ataque.
- **Escadas** levam o jogador para a próxima fase.

---

## 💻 Como Compilar e Executar

O código foi escrito para ser **multiplataforma**, usando diretivas de pré-processador (`#ifdef _WIN32`) para funcionar tanto em **Windows** quanto em **macOS/Linux**. A captura de teclas sem precisar apertar *Enter* é feita com `conio.h` no Windows e com `termios.h` em sistemas Unix.

### macOS / Linux

```bash
gcc dungeon_crawler.c -o dungeon_crawler
./dungeon_crawler
```

### Windows

```bash
gcc dungeon_crawler.c -o dungeon_crawler.exe
dungeon_crawler.exe
```

> Em IDEs como CLion ou VS Code, basta abrir o projeto e usar o botão de *Build & Run*. Substitua `dungeon_crawler.c` pelo nome real do arquivo, se for diferente.

---

## 🤖 Declaração sobre uso de IA Generativa

A IA generativa foi utilizada como **ferramenta de apoio ao aprendizado** em pontos específicos do desenvolvimento, sempre com o código sendo compreendido e revisado pela equipe. Em resumo, a IA ajudou em:

- **Agilizar o posicionamento de elementos repetitivos** nas fases (por exemplo, as linhas de espinhos do 2º andar), para acelerar a montagem dos mapas.
- **Apoiar a lógica da função de movimentação dos monstros** (`moverMonstros`).
- **Esclarecer conceitos da linguagem C**, como a compatibilidade entre Windows e Linux, a leitura de tecla sem *Enter* via `termios.h` e o uso de `srand`/`rand` para gerar aleatoriedade.

Todo o código entregue foi entendido pela equipe, que é capaz de explicar cada decisão de implementação.
