#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================================================
// COMPATIBILIDADE WINDOWS / LINUX
// ============================================================
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    char meuGetch() {
        return _getch();
    }
    void limparTela() {
        system("cls");
    }
#else
    #include <termios.h>
    #include <unistd.h>
char meuGetch() {
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        // limpa lixo residual do buffer
        while (read(STDIN_FILENO, &ch, 1) == 1) {
            struct timeval tv = {0, 0};
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);
            if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) <= 0) break;
        }

        read(STDIN_FILENO, &ch, 1);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }

    void limparTela() {
        printf("\033[H\033[J");
    }
#endif

// ============================================================
// CONSTANTES
// ============================================================

// tamanho maximo do mapa
#define MAX_LINHAS 25
#define MAX_COLUNAS 25

// simbolos do mapa
#define VAZIO ' '
#define PAREDE '*'
#define CHAVE '@'
#define CAIXA 'k'
#define PORTA_FECHADA 'D'
#define PORTA_ABERTA '='
#define ESCADA 'L'
#define NPC 'N'
#define ENTRADA 'E'
#define ESPINHO '#'
#define BOTAO 'O'
#define INIMIGO_X 'X'
#define INIMIGO_Y 'Y'
#define BOSS 'Z'

// armas
#define ARMA_NENHUMA 0
#define ARMA_ESPADA 1
#define ARMA_ARCO 2
#define ARMA_CAJADO 3

// ============================================================
// VARIAVEIS GLOBAIS
// ============================================================

char mapa[MAX_LINHAS][MAX_COLUNAS];
int linhasAtual = 10;
int colunasAtual = 10;

int jogadorY = 1;
int jogadorX = 1;
char direcaoJogador = 'v';

int vidas = 3;
int temChave = 0;
int armaEscolhida = ARMA_NENHUMA;
int faseAtual = 0;
int faseConcluida = 0;
int skipFase = 0;

// ============================================================
// UTILITARIOS
// ============================================================

int dentroDoMapa(int y, int x) {
    return (y >= 0 && y < linhasAtual && x >= 0 && x < colunasAtual);
}

// ============================================================
// INICIALIZACAO DAS FASES
// ============================================================

void montarBordas() {
    for (int i = 0; i < linhasAtual; i++) {
        for (int j = 0; j < colunasAtual; j++) {
            if (i == 0 || i == linhasAtual - 1 || j == 0 || j == colunasAtual - 1)
                mapa[i][j] = PAREDE;
            else
                mapa[i][j] = VAZIO;
        }
    }
}

void inicializarVila() {
    linhasAtual = 10;
    colunasAtual = 10;
    montarBordas();

    mapa[3][5] = NPC;
    mapa[8][8] = ENTRADA;

    jogadorX = 1;
    jogadorY = 1;
    direcaoJogador = 'v';
}

void inicializarAndar1() {
    linhasAtual = 10;
    colunasAtual = 10;
    montarBordas();

    mapa[4][3] = CAIXA;
    mapa[4][4] = CAIXA;
    mapa[2][7] = CHAVE;
    mapa[8][1] = PORTA_FECHADA;
    mapa[8][2] = ESCADA;

    jogadorX = 1;
    jogadorY = 1;
    direcaoJogador = 'v';
    temChave = 0;
}

//IA agilizou
void inicializarAndar2(){

linhasAtual = 15;
    colunasAtual = 15;
    montarBordas();

    // Caixas
mapa[1][13] = CAIXA;
mapa[4][4] = CAIXA;
mapa[7][5] = CAIXA;
mapa[10][5] = CAIXA;
mapa[13][11] = CAIXA;

// Chaves
mapa[3][3] = CHAVE;
mapa[13][12] = CHAVE;

// Portas fechadas
mapa[6][6] = PORTA_FECHADA;
mapa[6][8] = PORTA_FECHADA;

//Botão
mapa[13][7] = BOTAO;

// Escada
mapa[6][13] = ESCADA;

// Espinhos (todos-uso de IA para acelerar)
mapa[4][13] = ESPINHO;
mapa[4][12] = ESPINHO;
mapa[4][11] = ESPINHO;
mapa[4][10] = ESPINHO;
mapa[4][9] = ESPINHO;
mapa[4][8] = ESPINHO;
mapa[4][7] = ESPINHO;
mapa[4][6] = ESPINHO;

mapa[8][13] = ESPINHO;
mapa[8][12] = ESPINHO;
mapa[8][11] = ESPINHO;
mapa[8][10] = ESPINHO;
mapa[8][9] = ESPINHO;
mapa[8][8] = ESPINHO;
mapa[8][7] = ESPINHO;
mapa[8][6] = ESPINHO;

mapa[5][6] = ESPINHO;
mapa[5][7] = ESPINHO;
mapa[5][8] = ESPINHO;

mapa[7][6] = ESPINHO;
mapa[7][7] = ESPINHO;
mapa[7][8] = ESPINHO;

//Inimigo 1- IA agilizar
mapa[10][7] = INIMIGO_X;

    jogadorX = 1;
    jogadorY = 1;
    direcaoJogador = 'v';
    temChave = 0;

}

void inicializarAndar3() {
    linhasAtual = 25;
    colunasAtual = 25;
    montarBordas();   // preenche bordas com '*' e interior com ' '

    // ===== 3 CHAVES =====
    mapa[2][2] = CHAVE;
    mapa[12][12] = CHAVE;
    mapa[22][22] = CHAVE;

    // ===== 3 PORTAS FECHADAS =====
    mapa[3][22] = PORTA_FECHADA;
    mapa[12][20] = PORTA_FECHADA;
    mapa[22][3] = PORTA_FECHADA;

    // ===== CAIXAS (obstáculos) =====
    mapa[5][5] = CAIXA;
    mapa[5][6] = CAIXA;
    mapa[10][10] = CAIXA;
    mapa[10][11] = CAIXA;
    mapa[15][15] = CAIXA;
    mapa[18][18] = CAIXA;

    // ===== ESPINHOS =====
    mapa[8][8] = ESPINHO;
    mapa[8][9] = ESPINHO;
    mapa[9][8] = ESPINHO;
    mapa[9][9] = ESPINHO;
    mapa[16][16] = ESPINHO;
    mapa[16][17] = ESPINHO;
    mapa[17][16] = ESPINHO;

    // ===== BOTÃO =====
    mapa[20][20] = BOTAO;

    // ===== MONSTRO TIPO 2 (persegue jogador) =====
    mapa[6][18] = INIMIGO_Y;

    // ===== BOSS FINAL =====
    mapa[23][23] = BOSS;

    // ===== ESCADA (saída – atrás do boss) =====
    mapa[12][23] = ESCADA;

    // ===== POSIÇÃO INICIAL DO JOGADOR =====
    jogadorX = 1;
    jogadorY = 1;
    direcaoJogador = 'v';
    temChave = 0;   // começa sem chave neste andar
}
// ============================================================
// IMPRESSAO
// ============================================================

void imprimirMapa() {
    limparTela();

    if (faseAtual == 0) printf("--- VILA ---\n");
    else printf("--- ANDAR %d ---\n", faseAtual);

    for (int i = 0; i < linhasAtual; i++) {
        for (int j = 0; j < colunasAtual; j++) {
            if (i == jogadorY && j == jogadorX)
                printf("%c ", direcaoJogador);
            else
                printf("%c ", mapa[i][j]);
        }
        printf("\n");
    }

    char nomeArma[20] = "Nenhuma";
    if (armaEscolhida == ARMA_ESPADA) strcpy(nomeArma, "Espada");
    else if (armaEscolhida == ARMA_ARCO) strcpy(nomeArma, "Arco");
    else if (armaEscolhida == ARMA_CAJADO) strcpy(nomeArma, "Cajado");

    printf("Vidas: %d | Chave: %s | Arma: %s\n", vidas, temChave ? "Sim" : "Nao", nomeArma);
    printf("WASD = mover | I = interagir | O = atacar | Q = sair\n");
}

// ============================================================
// ATAQUE
// ============================================================

void atacarCelula(int y, int x) {
    if (!dentroDoMapa(y, x)) return;

    if (mapa[y][x] == CAIXA || mapa[y][x] == INIMIGO_X || mapa[y][x] == INIMIGO_Y) {
        mapa[y][x] = VAZIO;
    }

    //IA
    if (mapa[y][x] == CAIXA || mapa[y][x] == INIMIGO_X || mapa[y][x] == INIMIGO_Y || mapa[y][x] == BOSS) {
    mapa[y][x] = VAZIO;
}

}

void atacar() {
    if (armaEscolhida == ARMA_NENHUMA) {
        printf("\nVoce nao tem arma! Volte e fale com o NPC.\n");
        meuGetch();
        return;
    }

    int dyF = 0, dxF = 0;
    if (direcaoJogador == '^') dyF = -1;
    else if (direcaoJogador == 'v') dyF = 1;
    else if (direcaoJogador == '<') dxF = -1;
    else if (direcaoJogador == '>') dxF = 1;

    if (armaEscolhida == ARMA_ESPADA) {
        int dyP = 0, dxP = 0;
        if (direcaoJogador == '^' || direcaoJogador == 'v') dxP = 1;
        else dyP = 1;

        for (int prof = 1; prof <= 2; prof++) {
            int cy = jogadorY + dyF * prof;
            int cx = jogadorX + dxF * prof;
            for (int larg = -1; larg <= 1; larg++) {
                atacarCelula(cy + dyP * larg, cx + dxP * larg);
            }
        }
    }
    else if (armaEscolhida == ARMA_ARCO) {
        for (int p = 1; p <= 4; p++) {
            atacarCelula(jogadorY + dyF * p, jogadorX + dxF * p);
        }
    }
    else if (armaEscolhida == ARMA_CAJADO) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dy == 0 && dx == 0) continue;
                atacarCelula(jogadorY + dy, jogadorX + dx);
            }
        }
    }
}







// ============================================================
// INTERACAO (tecla I)
// ============================================================

void escolherArma() {
    limparTela();
    printf("--- NPC ---\n\n");
    printf("Bem-vindo, viajante. Escolha sua arma:\n\n");
    printf("1. Espada - ataca area 3x2 a sua frente\n");
    printf("2. Arco - atinge 4 celulas em linha reta\n");
    printf("3. Cajado - atinge as 8 celulas ao redor\n\n");
    printf("Pressione 1, 2 ou 3: ");

    char c = meuGetch();
    if (c == '1') armaEscolhida = ARMA_ESPADA;
    else if (c == '2') armaEscolhida = ARMA_ARCO;
    else if (c == '3') armaEscolhida = ARMA_CAJADO;
    else {
        printf("\nEscolha invalida.\n");
        meuGetch();
    }
}

void acionarBotao(int y, int x) {
    // Percorre as 8 direções ao redor do botão (inclui diagonais)
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue; // ignora a posição do botão
            int ny = y + dy;
            int nx = x + dx;
            if (dentroDoMapa(ny, nx) && mapa[ny][nx] == VAZIO) {
                mapa[ny][nx] = CAIXA;   // coloca uma caixa
            }
        }
    }
}

void interagir() {
    int dyF = 0, dxF = 0;
    if (direcaoJogador == '^') dyF = -1;
    else if (direcaoJogador == 'v') dyF = 1;
    else if (direcaoJogador == '<') dxF = -1;
    else if (direcaoJogador == '>') dxF = 1;

    int fy = jogadorY + dyF;
    int fx = jogadorX + dxF;

    if (!dentroDoMapa(fy, fx)) return;

    char alvo = mapa[fy][fx];

    if (alvo == NPC) {
        escolherArma();
    }
    else if (alvo == ENTRADA) {
        if (armaEscolhida == ARMA_NENHUMA) {
            printf("\nVoce precisa escolher uma arma com o NPC antes de entrar!\n");
            meuGetch();
        } else {
            faseConcluida = 1;
        }
    }
    else if (alvo == PORTA_FECHADA) {
        if (temChave) {
            mapa[fy][fx] = PORTA_ABERTA;
            temChave = 0;
            printf("\nVoce abriu a porta!\n");
            meuGetch();
        } else {
            printf("\nA porta esta trancada. Procure uma chave.\n");
            meuGetch();
        }
    }else if (alvo == BOTAO) {
        acionarBotao(fy, fx);
        mapa[fy][fx] = VAZIO;          // botão desaparece (opcional)
        printf("\n🔘 Botão ativado! Caixas surgiram ao redor!\n");
        meuGetch();
    }
}

// ============================================================
// MOVIMENTO
// ============================================================

void moverJogador(char comando) {
    int dx = 0, dy = 0;

    switch (comando) {
        case 'w': dy = -1; direcaoJogador = '^'; break;
        case 's': dy = 1; direcaoJogador = 'v'; break;
        case 'a': dx = -1; direcaoJogador = '<'; break;
        case 'd': dx = 1; direcaoJogador = '>'; break;
        default: return;
    }

    int novoX = jogadorX + dx;
    int novoY = jogadorY + dy;

    if (!dentroDoMapa(novoY, novoX)) return;

    char alvo = mapa[novoY][novoX];

    if (alvo == PAREDE || alvo == CAIXA || alvo == PORTA_FECHADA || alvo == NPC || alvo == ENTRADA)
        return;

    jogadorX = novoX;
    jogadorY = novoY;

    if (mapa[jogadorY][jogadorX] == CHAVE) {
        temChave = 1;
        mapa[jogadorY][jogadorX] = VAZIO;
        printf("\nVoce pegou a chave!\n");
        meuGetch();
    }
    else if (mapa[jogadorY][jogadorX] == ESCADA) {
        faseConcluida = 1;
    }
    else if (mapa[jogadorY][jogadorX] == ESPINHO) {
        vidas--;
        printf("\nVoce pisou em um espinho! Vidas restantes: %d\n", vidas);
        meuGetch();
        faseConcluida = -1;
    }
}

//IA-idealizou e criou
void moverMonstros() {
    for (int i = 0; i < linhasAtual; i++) {
        for (int j = 0; j < colunasAtual; j++) {
            // ===== MONSTRO X (movimento aleatório) =====
            if (mapa[i][j] == INIMIGO_X) {
                int dir = rand() % 4;
                int ny = i, nx = j;
                if (dir == 0) ny--;
                else if (dir == 1) ny++;
                else if (dir == 2) nx--;
                else nx++;
                if (ny >= 0 && ny < linhasAtual && nx >= 0 && nx < colunasAtual) {
                    if (ny == jogadorY && nx == jogadorX) {
                        vidas--;
                        faseConcluida = -1;
                        return;
                    }
                    if (mapa[ny][nx] == VAZIO) {
                        mapa[ny][nx] = INIMIGO_X;
                        mapa[i][j] = VAZIO;
                    }
                }
            }
            // ===== MONSTRO Y (persegue jogador) =====
            else if (mapa[i][j] == INIMIGO_Y) {
                int dy = 0, dx = 0;
                // Prioriza movimento vertical ou horizontal que reduza a distância
                if (i < jogadorY) dy = 1;      // jogador está abaixo
                else if (i > jogadorY) dy = -1; // jogador está acima
                else if (j < jogadorX) dx = 1;  // jogador está à direita
                else if (j > jogadorX) dx = -1; // jogador está à esquerda
                int ny = i + dy;
                int nx = j + dx;
                if (ny >= 0 && ny < linhasAtual && nx >= 0 && nx < colunasAtual) {
                    if (ny == jogadorY && nx == jogadorX) {
                        vidas--;
                        faseConcluida = -1;
                        return;
                    }
                    if (mapa[ny][nx] == VAZIO) {
                        mapa[ny][nx] = INIMIGO_Y;
                        mapa[i][j] = VAZIO;
                    }
                }
            }
            // ===== BOSS Z (mesmo comportamento do Y, ou personalizado) =====
            else if (mapa[i][j] == BOSS) {
                int dy = 0, dx = 0;
                if (i < jogadorY) dy = 1;
                else if (i > jogadorY) dy = -1;
                else if (j < jogadorX) dx = 1;
                else if (j > jogadorX) dx = -1;
                int ny = i + dy;
                int nx = j + dx;
                if (ny >= 0 && ny < linhasAtual && nx >= 0 && nx < colunasAtual) {
                    if (ny == jogadorY && nx == jogadorX) {
                        vidas--;
                        faseConcluida = -1;
                        return;
                    }
                    if (mapa[ny][nx] == VAZIO) {
                        mapa[ny][nx] = BOSS;
                        mapa[i][j] = VAZIO;
                    }
                }
            }
        }
    }
}

// ============================================================
// LOOP DE UMA FASE
// ============================================================

int loopFase() {
    faseConcluida = 0;
    while (1) {
        imprimirMapa();
        fflush(stdout);

        char comando = meuGetch();

        if (comando == 'q') return 0;
        if (comando == 'n') { skipFase = 1; return 1; }  // ← return aqui
        else if (comando == 'i') interagir();
        else if (comando == 'o') atacar();
        else moverJogador(comando);

        moverMonstros();//IA

        if (faseConcluida == 1) return 1;
        if (faseConcluida == -1) return -1;
        if (vidas <= 0) return -1;
    }
}

// ============================================================
// TELAS DE FIM
// ============================================================

void telaGameOver() {
    limparTela();
    printf("\n");
    printf(" *************************\n");
    printf(" * G A M E O V E R *\n");
    printf(" *************************\n\n");
    printf("Voce perdeu todas as suas vidas.\n\n");
    printf("Pressione qualquer tecla para voltar ao menu...\n");
    meuGetch();
}

void telaVitoria() {
    limparTela();
    printf("\n");
    printf(" *************************\n");
    printf(" * V I T O R I A *\n");
    printf(" *************************\n\n");
    printf("Voce derrotou o Boss e salvou a vila!\n\n");
    printf("Pressione qualquer tecla para voltar ao menu...\n");
    meuGetch();
}

// ============================================================
// FLUXO PRINCIPAL DO JOGO
// ============================================================

void jogar() {
    vidas = 3;
    armaEscolhida = ARMA_NENHUMA;
    temChave = 0;

    faseAtual = 0;
    while (1) {
        inicializarVila();
        int res = loopFase();
        if (res == 0) return;
        if (res == -1) {
            if (vidas <= 0) { telaGameOver(); return; }
            continue;
        }
        if (res == 1 || skipFase) { skipFase = 0; break; }
    }

    faseAtual = 1;
    while (1) {
        inicializarAndar1();
        int res = loopFase();
        if (res == 0) return;
        if (res == -1) {
            if (vidas <= 0) { telaGameOver(); return; }
            continue;
        }
        if (res == 1 || skipFase) { skipFase = 0; break; }
    }

    faseAtual = 2;
    while (1) {
        inicializarAndar2();
        int res = loopFase();
        if (res == 0) return;
        if (res == -1) {
            if (vidas <= 0) { telaGameOver(); return; }
            continue;
        }
        if (res == 1 || skipFase) { skipFase = 0; break; }
    }

    faseAtual = 3;
    while (1) {
        inicializarAndar3();
        int res = loopFase();
        if (res == 0) return;
        if (res == -1) {
            if (vidas <= 0) { telaGameOver(); return; }
            continue;
        }
        if (res == 1 || skipFase) { skipFase = 0; break; }
    }


    limparTela();
    telaVitoria();
    printf("Pressione qualquer tecla para voltar ao menu...\n");
    meuGetch();
}

// ============================================================
// TUTORIAL E SAIR
// ============================================================

void tutorial() {
    limparTela();
    printf("=== TUTORIAL ===\n\n");
    printf("HISTORIA:\nVoce e um aventureiro que chegou em uma vila a beira de uma masmorra.\n");
    printf("Escolha sua arma com o NPC e desca os 3 andares para derrotar o Boss!\n\n");
    printf("CONTROLES:\n W A S D = mover\n I = interagir\n O = atacar\n Q = sair\n\n");
    printf("Pressione qualquer tecla para voltar...\n");
    meuGetch();
}

void sair() {
    limparTela();
    printf("=== CREDITOS ===\n\nDesenvolvido por: Gabriel Lima, Diego Viana \nCESUPA - 2026\nObrigado por jogar!\n\n");
}

// ============================================================
// MAIN
// ============================================================

int main(void) {

     int opcao;
     srand(time(NULL));//IA
    do {
        limparTela();
        printf("\n=== DUNGEON CRAWLER ===\n\n1. Jogar\n2. Tutorial\n3. Sair\n\nEscolha uma opcao: ");

        char c = meuGetch();  // ← troca scanf por isso
        opcao = c - '0';      // converte char pra int

        switch (opcao) {
            case 1: jogar(); break;
            case 2: tutorial(); break;
            case 3: sair(); break;
            default:
                printf("\nOpcao invalida! Pressione qualquer tecla...\n");
                meuGetch();
        }
    } while (opcao != 3);
    return 0;
}
