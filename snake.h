//definições das estruturas e assinaturas;

#ifndef SNAKE_H
#define SNAKE_H

//bibliotecas nescessarias;
#include <stdlib.h>
#include "raylib.h"
#include "lista.h" //lista para definir o corpo da cobra;

//definições por padrão;
#define LARGURA 660
#define ALTURA 660
#define STD_SIZE_X 40
#define STD_SIZE_Y 40
#define TEMPO 0.2
#define COOLDOWN 0.2
#define SNAKE_COLOR RED
#define FOOD_COLOR BLUE
#define MAX_OBS 20 //definido quantos obstaculos vao aparecer por mapa. Isso está na struct jogo também

typedef struct Bordas{
    Rectangle pos;
}Bordas;

typedef struct Body{ //controle remoto, ela le as tlecas do teclado (usada para orientar na posição da cobra).
    Rectangle pos;
    Color color;
    int direcao;
}Body;

typedef struct Food{
    Rectangle pos;
    Color color;
    Texture2D tex; //textura.
}Food;

typedef struct {
    Rectangle pos; //posição e tamanho
    Color cor; //cor
} Obstaculo; //struct obstaculo.

typedef struct Jogo{
    Body body;
    Food food;
    Bordas bordas[4];
    double tempo;
    double cooldown;
    Segmento *cobra;   // cabeça da cobra (aponta para o primeiro elemento)
    Segmento *cauda;   // cauda da cobra (aponta para o segundo elemento)
    Sound somComer; //adicionando som
    Obstaculo obs[MAX_OBS]; //vetor para a minha struct obstaculo de tamanho 20
    int qtdeObs; //contador de obstaculos
    Texture2D *fundoMapa; //somente um ponteiro (guarda o endereço da textura).
}Jogo;

void IniciaBody(Jogo *j);
void IniciaBordas(Jogo *j);
void IniciaFood(Jogo *j);
void IniciaJogo(Jogo *j);
void DesenhaBody(Jogo *j);
void DesenhaFood(Jogo *j);
void DesenhaBordas(Jogo *j);
void DesenhaJogo(Jogo *j);
void AtualizaDirecao(Jogo *j);
void AtualizaPosBody(Jogo *j);
void AtualizaRodada(Jogo *j);
void ColisaoFood(Jogo *j);
void AtualizaSeg(Jogo *j);
int VFoodCobra(Jogo *j, Rectangle posFood);
int ColisaoCorpo(Jogo *j);
void DrawTextCentralizado(const char *txt, int y, int tamFonte, Color cor); //função para o texto no centro;
void IniciaObstaculos(Jogo *j); //função para criar os obstaculos;
int VColisaoObstaculos(Jogo *j); //verifica a colisão com os obstaculos;
void DesenhaObstaculos(Jogo *j); //função para desenhar os obs na tela.
int VFoodObst(Jogo *j, Rectangle pos); //verifica se a fruta nasceu em cima do obstaculo;
void IniciaObstaculosMapa1(Jogo *j);
void IniciaObstaculosMapa2(Jogo *j);
void lerRankingDoArquivo();
void ordenarRanking();
void atualizarRanking();
#endif