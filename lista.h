//lista.h e lista.c é somente para a lista encadeada.

#ifndef LISTA_H
#define LISTA_H

#include "raylib.h"

//a cobra cresce adicionando um segmento atrás. Quando a cobra cresce, os segmentos seguem a cabeça indo para a posição que a cabeça estava.

//nó da cobra, um pedaço do corpo da cobra (segmento).
typedef struct Segmento { //segmento = um pedaço da cobra;
    Rectangle pos; //posição da cobra (com x,y);
    Color cor; //cor da cobra;
    int direcao; //direção do segmento, essencial para quando um novo segmento nascer ele saiba como alinha corretamente com a cabeça;
    struct Segmento *prox; //aponta para o proximo pedaço da cobra, primeiro a cabeça, e assim por diante...;

}Segmento;

Segmento *criarSegmento(Rectangle pos, Color cor, int direcao);

void liberarCobra(Segmento *cabeca);

#endif
