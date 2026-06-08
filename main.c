#include <stdio.h>

void jogar(){}

void tutorial(){}

void sair(){}
int main(void) {


    int opcao;

    do {

        printf("\n MENU \n");
        printf("Jogar\n");
        printf("Tutorial\n");
        printf("Sair\n");

        switch (opcao) {
            case 1: jogar();
                break;
            case 2: tutorial();
                break;
            case 3: sair();
                break;
            default:
                printf("\nOpcao invalida!\n");
        }
    }
    while (opcao != 3);
    return 0;

}