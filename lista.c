//lista.h e lista.c é somente para a lista encadeada.

#include <stdio.h>
#include "lista.h"
#include <stdlib.h>

Segmento *criarSegmento(Rectangle pos, Color cor, int direcao) { //quando a cobra cresce ou é inicializada, é chamada essa função;
    Segmento *s = (struct Segmento*)malloc(sizeof(Segmento));

    //novos segmentos da cobra;
    s->pos = pos;
    s->cor = cor;
    s->direcao = direcao;
    s->prox = NULL; //proximo segmento nulo, ele nasce e não tem ninguém atrás dela.
    return s;
}

void liberarCobra(Segmento *cabeca) {
    //percorre a lista, desalocando toda area alocada;

    Segmento *atual = cabeca; //atual é o ponteiro "aux";
    while (atual != NULL) {
        Segmento *prox = atual->prox; //salvando o endereço do prox nó, antes de destruir o nó atual;
        free(atual); //destroi o anterior (o mesmo, atual);
        atual = prox; //move o ponteiro atual para o proximo;
    }
}