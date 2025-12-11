#include "raylib.h"
#include <stdio.h>
#include "snake.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "lista.h"

extern int gameOver;
extern int pont;
extern int tela;
extern int OpcaoMenu;
extern int OpcaoMapa;
extern int mapaSelecionado;
extern int ranking[20];
#define MAX_RANK 20

void DrawTextCentralizado(const char *txt, int y, int tamFonte, Color cor) { //função para centralizar o texto...
    int largura = MeasureText(txt, tamFonte);
    int x = (LARGURA - largura) / 2;
    DrawText(txt, x, y, tamFonte, cor);
}

void IniciaBody(Jogo *j){ //função que determina como a cobra irá onde/nascer.
    j->body.pos = (Rectangle) {LARGURA/2 - STD_SIZE_X, ALTURA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
    j->body.direcao = 0; //direção que a cobra nasce (olhando para cima).
    j->body.color = SNAKE_COLOR;
}

void IniciaBordas(Jogo *j){
    //Borda de cima
    j->bordas[0].pos = (Rectangle) {0, 0, LARGURA, 10};
    //Borda da direita
    j->bordas[1].pos = (Rectangle) {LARGURA - 10, 0, 10, ALTURA};
    //Borda de baixo
    j->bordas[2].pos = (Rectangle) {0, ALTURA - 10, LARGURA, 10};
    //Borda da esquerda
    j->bordas[3].pos = (Rectangle) {0, 0, 10, ALTURA};
}

//função que faz a fruta nascer/alterar a posição.
void IniciaFood(Jogo *j){
    Rectangle novaPos; //variavel que guarda a nova posição da fruta.

    do { //faz um sorteio para ela cair em um lugar aleatorio.
        novaPos = (Rectangle){
            (float)(rand() % ((ALTURA - 20) / STD_SIZE_Y) * STD_SIZE_Y + 10),
            (float)(rand() % ((ALTURA - 20) / STD_SIZE_Y) * STD_SIZE_Y + 10),
            STD_SIZE_X,
            STD_SIZE_Y
        };
    } while(VFoodCobra(j, novaPos) || VFoodObst(j, novaPos)); //só sai do loop quando a fruta não nasce no corpo e obstaculo.

    //depois de achar uma posição valida, essa posição é gravada na posição oficial do jogo.
    j->food.pos = novaPos;
    j->food.color = FOOD_COLOR;
}

void IniciaJogo(Jogo *j){ //função que deixa tudo pronto para o jogo começar a roda.
    IniciaBordas(j);
    IniciaBody(j);

    //adiciona de fato na cobra.
    j->cobra = criarSegmento(j->body.pos, SNAKE_COLOR, j->body.direcao);
    j->cauda = j->cobra; //cabeça = cauda.
    
    //cria os obs.
    IniciaObstaculos(j);

    //cria a comida.
    IniciaFood(j);

    j->tempo = GetTime();
    j->cooldown = 0;
}

void DesenhaBody(Jogo *j){ //desenha corpo da cobra.
    Segmento *seg = j->cobra;

    //desenha o corpo começando pela cabeça até a cauda.
    while(seg != NULL){
        DrawRectangleRec(seg->pos, seg->cor);
        seg = seg->prox;
    }
}

void DesenhaFood(Jogo *j){ //essa função somente desenha o quadrado da fruta.
    DrawTexturePro(
        j->food.tex, //imagem da fruta.
        (Rectangle){0, 0, j->food.tex.width, j->food.tex.height}, //configuração para desenhar a imagem da fruta.
        j->food.pos, //posição e tamanho que a fruta irá nascer no mapa.
        (Vector2){0, 0}, 
        0,
        WHITE //não alterar a imagem.
    );
}

void DesenhaBordas(Jogo *j){
    //Desenha as barreiras nas bordas
    for (int i = 0; i < 4; i++){
        DrawRectangleRec(j->bordas[i].pos, LIGHTGRAY);
    }
}

void DesenhaJogo(Jogo *j){ //essa função desenha o jogo, e as bordas, corpo da cobra, fruta e obstaculos. (é chamada 120 frames por seg).
    DrawTexture(*(j->fundoMapa), 0, 0, WHITE); //desenha o mapa pra qual o ponteiro está apontando (na main, dentro dos ifs).
    DesenhaBody(j);
    DesenhaFood(j);
    DesenhaObstaculos(j);
}

//função que verifica o tecla digitada, para a cobra andar.
void AtualizaDirecao(Jogo *j){ //essa função troca o j->body.direcao, e detecta qual tleca foi pressionada.
    //Atualiza para qual direção a cobra vai  
    if(IsKeyDown(KEY_UP) && j->body.direcao != 2 && GetTime() - j->cooldown > COOLDOWN){
        j->body.direcao = 0;
        j->cooldown = GetTime();
    }
    if(IsKeyDown(KEY_RIGHT) && j->body.direcao != 3 && GetTime() - j->cooldown > COOLDOWN){
        j->body.direcao = 1;
        j->cooldown = GetTime();
    }
    if(IsKeyDown(KEY_DOWN) && j->body.direcao != 0 && GetTime() - j->cooldown > COOLDOWN){
        j->body.direcao = 2;
        j->cooldown = GetTime();
    }
    if(IsKeyDown(KEY_LEFT) && j->body.direcao != 1 && GetTime() - j->cooldown > COOLDOWN){
        j->body.direcao = 3;
        j->cooldown = GetTime();
    }
}

//função que faz a cobra mover.
void AtualizaPosBody(Jogo *j){
    //função da cabeça, a cabeça avança.
    //aqui a cabeça simplesmente muda de posição conforme a direção. Ela pode ir para cima, baixo, esq ou dir. Ela só da um "passo".
    if (j->body.direcao == 0){ //sobe
        j->body.pos.y -= STD_SIZE_Y;
    }
    if (j->body.direcao == 1){ //direita
        j->body.pos.x += STD_SIZE_X;
    }
    if (j->body.direcao == 2){ //desce
        j->body.pos.y += STD_SIZE_Y;
    }
    if (j->body.direcao == 3){ //esquerda
        j->body.pos.x -= STD_SIZE_X;
    }

    //teleporte;
    if (j->body.pos.x >= LARGURA)       j->body.pos.x = 0;                         // saiu pela direita
    if (j->body.pos.x < 0)              j->body.pos.x = LARGURA - STD_SIZE_X;      // saiu pela esquerda
    if (j->body.pos.y >= ALTURA)        j->body.pos.y = 0;                         // saiu embaixo
    if (j->body.pos.y < 0)              j->body.pos.y = ALTURA - STD_SIZE_Y;       // saiu em cima

    j->cobra->pos = j->body.pos;
    j->cobra->direcao = j->body.direcao; //precisa ser salvo para a proxima posição do corpo nascer apontada para a direção certa (a direção da cabeça, e os segmento atrás).
}

void AtualizaRodada(Jogo *j){ //função principal que coordena a cobra para ela mover.
    AtualizaDirecao(j); //função para verificar o teclado (tecla pressionada).
    if (GetTime() - j->tempo > TEMPO){ //verifica a hora de andar. (ve o tempo atual - o tempo que a cobra andou da ultima vez e verifica se esse tempo > TEMPO)
        AtualizaSeg(j); //move o corpo primeiro, todos os segmentos, menos a cabeça. Cada segmento copia a posição anterior.
        AtualizaPosBody(j); //move a cabeça para a nova direção escolhida pelo jogador.
        j->tempo = GetTime();
        j->cooldown = COOLDOWN; //atualiza o tempo (reseta).
    }
}

//função para fazer a cobra crescer, e onde a fruta muda de posição.
void ColisaoFood(Jogo *j){
    if (CheckCollisionRecs(j->body.pos, j->food.pos)){ //detecta a colisão da cabeça com a fruta.
        pont++; //aumenta em 1 o numero de frutas comidas.
        PlaySound(j->somComer); //som acionado quando a cobra come a fruta.
        Rectangle posNovo = j->cauda->pos; //pega a posição atual da cauda (posição e tamanho).

        //Descobre a posição da cauda. Para o novo quadrado nascer na posição certa (uma posição atrás da cauda).
        if (j->cauda->direcao == 0) posNovo.y += STD_SIZE_Y; //cima - nasce para baixo.
        if (j->cauda->direcao == 1) posNovo.x -= STD_SIZE_X; //direita - nasce para esq.
        if (j->cauda->direcao == 2) posNovo.y -= STD_SIZE_Y; //baixo - nasce para cima.
        if (j->cauda->direcao == 3) posNovo.x += STD_SIZE_X; //esquerda - nasce para dir.

        Segmento *novo = criarSegmento(posNovo, j->cauda->cor, j->cauda->direcao);//a partir disso, ela irá criar um novo segmento.
        //ligando o novo pedaço (segmento) no final da cauda.
        j->cauda->prox = novo; //o NULL da cauda aponta para o novo segmento (nova cauda).
        j->cauda = novo;  //antiga cauda aponta para esse novo segmento
        IniciaFood(j); //gera uma nova fruta, em um lugar aleatorio.
    }
}

//função que faz cada segmento (exceto a cabeça) copiar a posição do segmento da frente.
void AtualizaSeg(Jogo *j){
    Segmento *atual = j->cobra; //aponta para o nó cabeça.
    //guarda a posição e direção antiga da cabeça. (essas posições serão copiadas para o segundo segmento)
    Rectangle posAnterior = j->cobra->pos;
    int dirAnterior =  j->cobra->direcao;

    Rectangle posTemp;
    int dirTemp;

    atual = atual->prox; //vai para o segundo segmento (pula a cabeça)

    while(atual != NULL){ //percorre todos os segmentos até a cauda.
        posTemp = atual->pos; //guarda a posição atual do segmento (variavel acima).
        dirTemp = atual->direcao; //guarda a direção.
        atual->pos = posAnterior; //o segmento vai para a posição que o da frente estava.
        atual->direcao = dirAnterior; //o segmento vai para a direção que o da frente estava.
        posAnterior = posTemp; //atualiza a posição anterior para o proximo da fila ir para essa posição.
        dirAnterior = dirTemp; //guarda a direção antiga do segmento atual, para que o proximo segmento vá para essa direção.
        atual = atual->prox; //vai para o proximo segmento e, continua na lista.
    }
}

//verifica se a fruta nasce em algum segmento da cobra.
int VFoodCobra(Jogo *j, Rectangle posFood){
    //a função verifica se a posição sorteada (posfood) colide com qualquer segmento da cobra.
    Segmento *seg = j->cobra; //é pegado o primeiro segmento da cobra (cabeça), e armazena em seg (isso para percorrer a lista).

    while(seg != NULL){ //percorre toda a cobra.
        if (CheckCollisionRecs(seg->pos, posFood)){ //e para cada segmento percorrido, é verificado se se tem alguma colisão com a fruta.
            return 1;  // fruta em cima da cobra
        }
        seg = seg->prox; //atualiza e vai para o proximo segmento
    }
    return 0; // sem colisões.
}

//verifica se a cobra colidiu com o propio corpo
int ColisaoCorpo(Jogo *j){
    //é chamada na main.
    Segmento *seg = j->cobra->prox;  //começa no SEGUNDO segmento. Não se pode comparar cabeça com cabeça, se não é game over direto. Pois a cabeça está colidindo com ela mesma.

    while(seg != NULL){  //percorre toda a lista encadeada.
        if (CheckCollisionRecs(j->cobra->pos, seg->pos)){ //e verifica se a posição dele colide com a cabeça. Verifica se a posição da cabeça está ocupando a mesma posição de qualquer segmento do corpo. seg->pos = posição de um segmento do corpo.
            return 1; // bateu no corpo
        }
        seg = seg->prox; //atualiza para o proximo segmento da cobra.
    }

    return 0; // não colidiu
}

void IniciaObstaculos(Jogo *j){ //função que inicia o OBS.
    j->qtdeObs = MAX_OBS; //incia o contador com 20 (que é o numero de obstaculos por mapa).

    for (int i = 0; i < j->qtdeObs; i++){
        Rectangle novo; //variavél temporaria (cria um obstaculo).
        do { //gera uma posição aleatoria para o obstaculo nascer no mapa.
            novo = (Rectangle){
                (float)(rand() % ((LARGURA - 20) / STD_SIZE_X) * STD_SIZE_X + 10),
                (float)(rand() % ((ALTURA  - 20) / STD_SIZE_Y) * STD_SIZE_Y + 10),
                STD_SIZE_X,
                STD_SIZE_Y
            };
        } while ( //enquanto o obstaculo nascer em algum local proibido, será gerado outro. Esse while faz a verificação da posição dos obstaculos. Só sai quando a condição for falsa.
            CheckCollisionRecs(novo, j->body.pos)); // evitar nascer na cabeça

        //depois de encontrar um lugar valido, essa variavél (obstaculo) é salva no vetor oficial.
        j->obs[i].pos = novo;
        j->obs[i].cor = GRAY;
    }
}

int VColisaoObstaculos(Jogo *j){ //verifica se a cobra colidiu com algum OBS.
    for (int i = 0; i < j->qtdeObs; i++){ //percorre todo os OBS e verifica se a cobra bateu em algum deles.
        if (CheckCollisionRecs(j->cobra->pos, j->obs[i].pos)){
            return 1;
        }
    }
    return 0;
}

void DesenhaObstaculos(Jogo *j){ //função que desenha os OBS.
    for (int i = 0; i < j->qtdeObs; i++){
        DrawRectangleRec(j->obs[i].pos, j->obs[i].cor); //desenhando um obs na tela.
    }
}

int VFoodObst(Jogo *j, Rectangle pos){ //verifica se a fruta nasceu no OBS.
    for (int i = 0; i < j->qtdeObs; i++){ //percorre todos OBS e verifica uma colisão.
        if (CheckCollisionRecs(j->obs[i].pos, pos)){
            return 1; // fruta nasceu em um obstáculo
        }
    }
    return 0; //não nasceu
}

void IniciaObstaculosMapa1(Jogo *j){ //mesma logica para os obstaculos.
    IniciaObstaculos(j);
}

void IniciaObstaculosMapa2(Jogo *j){ //mesma logica para os obstaculos.
    IniciaObstaculos(j);
}

//funções ranking (é passado o resultado dos 20 numeros(do ranking) para o vetor, se tiver um numero maior que algum numero no vetor, esse numero é adicionado e o vetor é ordenado, e é passado para o arquivo os numeros ordenados).

void lerRankingDoArquivo(){
    FILE *arq = fopen("ranking.txt", "r"); //arquivo aberto no modo leitura.
    for(int i=0;i<MAX_RANK;i++){ //percorre todas as posições do vetor.
        if(fscanf(arq, "%d", &ranking[i]) != 1) //le um numero inteiro do arquivo e armazena no vetor;
            ranking[i] = 0; //coloca 0 se falhar na leitura.
    }   
    fclose(arq);
}

void ordenarRanking() { //metodo Insertion.
    for(int i=1;i<MAX_RANK;i++){
        int chave = ranking[i];
        int j = i-1;
        while(j>=0 && ranking[j]<chave){ // maior pontuação vem primeiro
            ranking[j+1] = ranking[j];
            j--;
        }
        ranking[j+1] = chave;
    }
}

void atualizarRanking(int pontuacao) {
    if(pontuacao > ranking[MAX_RANK-1]) { // se a pontuação é maior que a menor do ranking.
        ranking[MAX_RANK-1] = pontuacao; //nova pontuação na ultima colocação.
        ordenarRanking();
        // salva no arquivo
        FILE *arq = fopen("ranking.txt", "w"); //abre o arq para sobrescrever.
        for(int i=0;i<MAX_RANK;i++)
            fprintf(arq,"%d\n", ranking[i]);
        fclose(arq);
    }
}