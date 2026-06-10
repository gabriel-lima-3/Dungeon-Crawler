#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// ============================================================
// CONSTANTES
// ============================================================

// tamanho maximo do mapa (o 3o andar e 25x25, entao reservo esse tamanho)
#define MAX_LINHAS 25
#define MAX_COLUNAS 25

// simbolos do mapa
#define VAZIO          ' '
#define PAREDE         '*'
#define CHAVE          '@'
#define CAIXA          'k'
#define PORTA_FECHADA  'D'
#define PORTA_ABERTA   '='
#define ESCADA         'L'
#define NPC            'N'
#define ENTRADA        'E'   // entrada da masmorra na vila
#define ESPINHO        '#'
#define BOTAO          'O'
#define INIMIGO_X      'X'
#define INIMIGO_Y      'Y'
#define BOSS           'Z'

// armas
#define ARMA_NENHUMA   0
#define ARMA_ESPADA    1
#define ARMA_ARCO      2
#define ARMA_CAJADO    3


// ============================================================
// VARIAVEIS GLOBAIS
// ============================================================

// mapa: usa o tamanho maximo, mas so trabalha com linhasAtual/colunasAtual
char mapa[MAX_LINHAS][MAX_COLUNAS];
int linhasAtual = 10;
int colunasAtual = 10;

// jogador
int jogadorY = 1;
int jogadorX = 1;
char direcaoJogador = 'v';

// estado do jogo
int vidas = 3;
int temChave = 0;
int armaEscolhida = ARMA_NENHUMA;
int faseAtual = 0;      // 0 = vila, 1 = andar 1, 2 = andar 2, 3 = andar 3
int faseConcluida = 0;  // 0 = jogando, 1 = passou de fase, -1 = morreu (reiniciar)


// ============================================================
// UTILITARIOS
// ============================================================

// le uma tecla sem precisar de Enter (substituto do _getch do Windows)
char meuGetch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void limparTela() {
    printf("\033[H\033[J");
}

// verifica se uma celula esta dentro dos limites do mapa atual
int dentroDoMapa(int y, int x) {
    return (y >= 0 && y < linhasAtual && x >= 0 && x < colunasAtual);
}


// ============================================================
// INICIALIZACAO DAS FASES
// ============================================================

// preenche o mapa com paredes nas bordas e vazio no meio
void montarBordas() {
    for (int i = 0; i < linhasAtual; i++) {
        for (int j = 0; j < colunasAtual; j++) {
            if (i == 0 || i == linhasAtual - 1 ||
                j == 0 || j == colunasAtual - 1)
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

    // NPC que da a arma
    mapa[3][5] = NPC;

    // entrada da masmorra
    mapa[8][8] = ENTRADA;

    // jogador comeca no canto
    jogadorX = 1;
    jogadorY = 1;
    direcaoJogador = 'v';
}

void inicializarAndar1() {
    linhasAtual = 10;
    colunasAtual = 10;
    montarBordas();

    // caixas que bloqueiam um caminho (tutorial de quebrar caixa)
    mapa[4][3] = CAIXA;
    mapa[4][4] = CAIXA;

    // chave
    mapa[2][7] = CHAVE;

    // porta fechada e escada logo apos (tutorial de chave + porta)
    mapa[8][1] = PORTA_FECHADA;
    mapa[8][2] = ESCADA;

    // reset do estado da fase
    jogadorX = 1;
    jogadorY = 1;
    direcaoJogador = 'v';
    temChave = 0;
}


// ============================================================
// IMPRESSAO
// ============================================================

void imprimirMapa() {
    limparTela();

    if (faseAtual == 0) printf("--- VILA ---\n");
    else                printf("--- ANDAR %d ---\n", faseAtual);

    for (int i = 0; i < linhasAtual; i++) {
        for (int j = 0; j < colunasAtual; j++) {
            if (i == jogadorY && j == jogadorX)
                printf("%c ", direcaoJogador);
            else
                printf("%c ", mapa[i][j]);
        }
        printf("\n");
    }

    // mostra nome da arma escolhida
    char nomeArma[20] = "Nenhuma";
    if      (armaEscolhida == ARMA_ESPADA) strcpy(nomeArma, "Espada");
    else if (armaEscolhida == ARMA_ARCO)   strcpy(nomeArma, "Arco");
    else if (armaEscolhida == ARMA_CAJADO) strcpy(nomeArma, "Cajado");

    printf("Vidas: %d | Chave: %s | Arma: %s\n",
           vidas, temChave ? "Sim" : "Nao", nomeArma);
    printf("WASD = mover | I = interagir | O = atacar | Q = sair\n");
}


// ============================================================
// ATAQUE
// ============================================================

// destroi caixa ou inimigo numa celula (boss tem logica propria depois)
void atacarCelula(int y, int x) {
    if (!dentroDoMapa(y, x)) return;

    if (mapa[y][x] == CAIXA ||
        mapa[y][x] == INIMIGO_X ||
        mapa[y][x] == INIMIGO_Y) {
        mapa[y][x] = VAZIO;
    }
}

void atacar() {
    if (armaEscolhida == ARMA_NENHUMA) {
        printf("\nVoce nao tem arma! Volte e fale com o NPC.\n");
        meuGetch();
        return;
    }

    // direcao para frente (depende de onde o jogador esta olhando)
    int dyF = 0, dxF = 0;
    if      (direcaoJogador == '^') dyF = -1;
    else if (direcaoJogador == 'v') dyF =  1;
    else if (direcaoJogador == '<') dxF = -1;
    else if (direcaoJogador == '>') dxF =  1;

    if (armaEscolhida == ARMA_ESPADA) {
        // 3x2 a frente: 3 de largura, 2 de profundidade
        // perpendicular para criar a "largura":
        //   se olha para cima/baixo, a largura e horizontal (dx)
        //   se olha para esquerda/direita, a largura e vertical (dy)
        int dyP = 0, dxP = 0;
        if (direcaoJogador == '^' || direcaoJogador == 'v') dxP = 1;
        else                                                 dyP = 1;

        for (int prof = 1; prof <= 2; prof++) {
            int cy = jogadorY + dyF * prof;
            int cx = jogadorX + dxF * prof;
            for (int larg = -1; larg <= 1; larg++) {
                atacarCelula(cy + dyP * larg, cx + dxP * larg);
            }
        }
    }
    else if (armaEscolhida == ARMA_ARCO) {
        // 4 celulas em linha reta a frente
        for (int p = 1; p <= 4; p++) {
            atacarCelula(jogadorY + dyF * p, jogadorX + dxF * p);
        }
    }
    else if (armaEscolhida == ARMA_CAJADO) {
        // todas as 8 celulas adjacentes (independente da direcao)
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
    printf("2. Arco   - atinge 4 celulas em linha reta\n");
    printf("3. Cajado - atinge as 8 celulas ao redor\n\n");
    printf("Pressione 1, 2 ou 3: ");

    char c = meuGetch();
    if      (c == '1') armaEscolhida = ARMA_ESPADA;
    else if (c == '2') armaEscolhida = ARMA_ARCO;
    else if (c == '3') armaEscolhida = ARMA_CAJADO;
    else {
        printf("\nEscolha invalida.\n");
        meuGetch();
    }
}

void interagir() {
    // celula a frente do jogador
    int dyF = 0, dxF = 0;
    if      (direcaoJogador == '^') dyF = -1;
    else if (direcaoJogador == 'v') dyF =  1;
    else if (direcaoJogador == '<') dxF = -1;
    else if (direcaoJogador == '>') dxF =  1;

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
    }
    else {
        // nada interagivel a frente
    }
}


// ============================================================
// MOVIMENTO
// ============================================================

void moverJogador(char comando) {
    int dx = 0, dy = 0;

    switch (comando) {
        case 'w': dy = -1; direcaoJogador = '^'; break;
        case 's': dy =  1; direcaoJogador = 'v'; break;
        case 'a': dx = -1; direcaoJogador = '<'; break;
        case 'd': dx =  1; direcaoJogador = '>'; break;
        default: return;
    }

    int novoX = jogadorX + dx;
    int novoY = jogadorY + dy;

    if (!dentroDoMapa(novoY, novoX)) return;

    char alvo = mapa[novoY][novoX];

    // bloqueia movimento atraves de coisas solidas
    if (alvo == PAREDE || alvo == CAIXA || alvo == PORTA_FECHADA ||
        alvo == NPC    || alvo == ENTRADA)
        return;

    // movimento permitido
    jogadorX = novoX;
    jogadorY = novoY;

    // pegou chave
    if (mapa[jogadorY][jogadorX] == CHAVE) {
        temChave = 1;
        mapa[jogadorY][jogadorX] = VAZIO;
        printf("\nVoce pegou a chave!\n");
        meuGetch();
    }
    // pisou na escada -> avanca de fase
    else if (mapa[jogadorY][jogadorX] == ESCADA) {
        faseConcluida = 1;
    }
    // pisou em espinho -> perde vida e reinicia a fase
    else if (mapa[jogadorY][jogadorX] == ESPINHO) {
        vidas--;
        printf("\nVoce pisou em um espinho! Vidas restantes: %d\n", vidas);
        meuGetch();
        faseConcluida = -1;
    }
}


// ============================================================
// LOOP DE UMA FASE
// retornos: 0 = saiu (Q), 1 = concluida, -1 = morreu (reiniciar)
// ============================================================

int loopFase() {
    faseConcluida = 0;

    while (1) {
        imprimirMapa();
        fflush(stdout);

        char comando = meuGetch();

        if      (comando == 'q') return 0;
        else if (comando == 'i') interagir();
        else if (comando == 'o') atacar();
        else                     moverJogador(comando);

        if (faseConcluida == 1)  return 1;
        if (faseConcluida == -1) return -1;
        if (vidas <= 0)          return -1;
    }
}


// ============================================================
// TELAS DE FIM
// ============================================================

void telaGameOver() {
    limparTela();
    printf("\n");
    printf("  *************************\n");
    printf("  *    G A M E  O V E R    *\n");
    printf("  *************************\n\n");
    printf("Voce perdeu todas as suas vidas.\n\n");
    printf("Pressione qualquer tecla para voltar ao menu...\n");
    meuGetch();
}

void telaVitoria() {
    limparTela();
    printf("\n");
    printf("  *************************\n");
    printf("  *      V I T O R I A      *\n");
    printf("  *************************\n\n");
    printf("Voce derrotou o Boss e salvou a vila!\n\n");
    printf("Pressione qualquer tecla para voltar ao menu...\n");
    meuGetch();
}


// ============================================================
// FLUXO PRINCIPAL DO JOGO
// ============================================================

void jogar() {
    // reset geral da partida
    vidas = 3;
    armaEscolhida = ARMA_NENHUMA;
    temChave = 0;

    // ===== VILA =====
    faseAtual = 0;
    while (1) {
        inicializarVila();
        int res = loopFase();
        if (res == 0)  return;                          // saiu pro menu
        if (res == -1) {                                // morreu
            if (vidas <= 0) { telaGameOver(); return; }
            continue;                                   // reinicia a vila
        }
        if (res == 1)  break;                           // passou de fase
    }

    // ===== ANDAR 1 =====
    faseAtual = 1;
    while (1) {
        inicializarAndar1();
        int res = loopFase();
        if (res == 0)  return;
        if (res == -1) {
            if (vidas <= 0) { telaGameOver(); return; }
            continue;
        }
        if (res == 1)  break;
    }

    // ===== ANDAR 2 e ANDAR 3 entram aqui nas proximas etapas =====

    // mensagem temporaria enquanto os outros andares nao estao prontos
    limparTela();
    printf("\nVoce completou tudo que esta implementado ate aqui!\n");
    printf("(Andares 2 e 3 serao adicionados nas proximas etapas)\n\n");
    printf("Pressione qualquer tecla para voltar ao menu...\n");
    meuGetch();
}


// ============================================================
// TUTORIAL E SAIR
// ============================================================

void tutorial() {
    limparTela();
    printf("=== TUTORIAL ===\n\n");

    printf("HISTORIA:\n");
    printf("Voce e um aventureiro que chegou em uma vila a beira de uma masmorra.\n");
    printf("Escolha sua arma com o NPC e desca os 3 andares para derrotar o Boss!\n\n");

    printf("CONTROLES:\n");
    printf("  W A S D = mover\n");
    printf("  I       = interagir com o que esta a frente\n");
    printf("  O       = atacar\n");
    printf("  Q       = sair para o menu\n\n");

    printf("SIMBOLOS:\n");
    printf("  ^ v < > voce (a direcao que esta olhando)\n");
    printf("  *       parede\n");
    printf("  k       caixa (destrua com ataque)\n");
    printf("  @       chave\n");
    printf("  D       porta fechada\n");
    printf("  =       porta aberta\n");
    printf("  L       escada (avanca de fase)\n");
    printf("  N       NPC (interaja para escolher arma)\n");
    printf("  E       entrada da masmorra\n");
    printf("  #       espinho (perde 1 vida)\n");
    printf("  O       botao\n");
    printf("  X       monstro tipo 1 (movimento aleatorio)\n");
    printf("  Y       monstro tipo 2 (persegue o jogador)\n");
    printf("  Z       boss final\n\n");

    printf("Pressione qualquer tecla para voltar...\n");
    meuGetch();
}

void sair() {
    limparTela();
    printf("=== CREDITOS ===\n\n");
    printf("Desenvolvido por: [SEU NOME AQUI]\n");
    printf("CESUPA - 2026\n\n");
    printf("Obrigado por jogar!\n\n");
}


// ============================================================
// MAIN
// ============================================================

int main(void) {
    int opcao;

    do {
        limparTela();
        printf("\n=== DUNGEON CRAWLER ===\n\n");
        printf("1. Jogar\n");
        printf("2. Tutorial\n");
        printf("3. Sair\n\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            opcao = 0;
        }
        // consome o \n que sobra do scanf (senao bagunca o meuGetch depois)
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

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