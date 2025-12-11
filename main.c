#include "raylib.h"
#include <stdio.h>
#include "snake.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "lista.h"
#define MAX_RANK 20


int ranking[20];
int gameOver = 1;
int pont = 0; //pontuação para a contagem de frutas comidas.
//tela inicial(0), jogando(1), ranking(2) e seleção de mapas(3), começa sempre no 0 (menu).
int tela = 0;
//variavél para saber qual é a opção selecionada:
int OpcaoMenu = 0; //0 = jogar, 1 = ranking.
int OpcaoMapa = 0; //0 = map 1, 1 = map 2.
int mapaSelecionado = 0; //qual mapa selecionado 1 ou 2.

int main(){
    srand(time(NULL));
    InitWindow(LARGURA, ALTURA, "Snake Game"); //inicia
    InitAudioDevice(); //chamando a biblioteca do som.
    Texture2D fundoMapa1 = LoadTexture("assets/grama.png");//carrega as texturas
    Texture2D fundoMapa2 = LoadTexture("assets/mapa_neve.png");
    Texture2D frutaMapa1 = LoadTexture("assets/moeda-icon.png");
    Texture2D frutaMapa2 = LoadTexture("assets/panetone2.png");

    Jogo jogo; //variavel para a struct criada.
    jogo.somComer = LoadSound("assets/som-mordida.wav");
    Sound somGameOver = LoadSound("assets/som-gameOver.wav");
    Sound selecaoMapas = LoadSound("assets/selecionar-jogo.mp3");

    Music musica = LoadMusicStream("assets/som-jogo.wav");
    PlayMusicStream(musica);

    SetTargetFPS(120);
    srand(time(NULL));
    
    jogo.fundoMapa = &fundoMapa1; //recebendo o endereço do mapa1. (isso só garante que o jogo tenha algum fundo, para não ficar dando tela preta).
    SetSoundVolume(jogo.somComer, 1.0f);
    SetMasterVolume(1.0f);

    lerRankingDoArquivo();
    ordenarRanking();

    //while = gameLoop. O while é inifito, e só acaba quando a janela é fechada.
    while (!WindowShouldClose()){ //main fica rodando infinitamente para saber o que o usuario quer. Ela fica rodando 120 vezes por segundo e faz a secagem em EndDrawing.
        BeginDrawing();
        ClearBackground(BLACK);

        //Tela inicial (tela 0)
        if (tela == 0){ //menu
            DrawTextCentralizado("SNAKE GAME", ALTURA/2 - 100, 60, GREEN); //desenha o titulo.
            DrawTextCentralizado("Pressione ENTER para iniciar", ALTURA/2, 30, WHITE);

            int baseY = ALTURA - 140; //posição fixa das opções.

            //cor das opções.
            Color corJogar   = (OpcaoMenu == 0 ? YELLOW : WHITE); //se o cursor está em jogar fica amarelo. (verifica)
            Color corRanking = (OpcaoMenu == 1 ? YELLOW : WHITE); //se o ranking está em jogar fica amarelo.

            // Desenhar as opções na tela.
            DrawTextCentralizado("JOGAR", baseY, 40, corJogar);
            DrawTextCentralizado("RANKING", baseY + 60, 40, corRanking);

            // Navegação pelas setas
            if (IsKeyPressed(KEY_UP)) { //seta para cima, OpçãoMenu vai decrementando;
                OpcaoMenu--; //se apertado seta para cima, ele vai para o ranking (da a "volta").
            if (OpcaoMenu < 0) OpcaoMenu = 1; //verifica. E da a impressão de "anel".
            }
                if (IsKeyPressed(KEY_DOWN)) {//seta para baixo, OpçãoMenu aumenta.
                OpcaoMenu++; //se apertado seta para baixo, ele vai pra o ranking, e se apertado novamente para baixo, ele irá dar a "volta" em jogar.
            if (OpcaoMenu > 1) OpcaoMenu = 0;
            }
            //seleção com enter e funções;
            if (IsKeyPressed(KEY_ENTER)){ //verifica a tecla digitada.
                PlaySound(selecaoMapas);
                if(OpcaoMenu == 0){
                    tela = 3; //vai para a seleção de mapas.
                } else if (OpcaoMenu == 1) tela = 2; //vai para o ranking.
                
            } 
        }

        //Tela jogo
        else if (tela == 1){ //jogando.
            UpdateMusicStream(musica);
            if (gameOver){ //verifica se a variavél (gameOver) é verdadeira.
                DesenhaJogo(&jogo);
                char textoPont[20]; //vetor de caracteres, para guardar a frase e o numero de frutas. Ex "Frutas: 123"
                sprintf(textoPont, "Score: %d", pont); //escrita na tela.
                DrawText(textoPont, LARGURA - 150, 20, 25, RED); //frase construida no canto superior direito.
                AtualizaRodada(&jogo);
                ColisaoFood(&jogo); //detecta se a cobra comeu a fruta.

                if (ColisaoCorpo(&jogo) || VColisaoObstaculos(&jogo)){ //verifica a condição. E se bater no corpo, borda, ou obstaculo, morre.
                    StopMusicStream(musica);
                    PlaySound(somGameOver);
                    gameOver = 0;
                }
            }
            else { //se gameOver for falsa (0) entra nesse else aqui.
                DrawTextCentralizado("Game Over!", ALTURA/2 - 60, 40, RED);
                DrawTextCentralizado("Pressione ENTER para jogar novamente!", ALTURA/2 + 10, 30, GREEN);
                DrawTextCentralizado("Pressione M para voltar ao menu.", ALTURA/2 + 50, 25, WHITE);

                if (IsKeyPressed(KEY_ENTER)){ //apertar enter para jogar novamente.
                    atualizarRanking(pont);
                    liberarCobra(jogo.cobra);
                    IniciaJogo(&jogo);
                    StopSound(somGameOver);
                    PlayMusicStream(musica);
                    gameOver = 1;
                    pont = 0; //total de frutas comidas volta ao normal (0).
                }

                if (IsKeyPressed(KEY_M)){
                    atualizarRanking(pont);
                    StopSound(somGameOver);
                    tela = 0; // volta ao menu
                }
            }
        }

        //seleção de mapas.
        else if (tela == 3){
            DrawTextCentralizado("Selecione o mapa:", 120, 45, GREEN);

            Color corMapa1 = (OpcaoMapa == 0 ? YELLOW : WHITE);
            Color corMapa2 = (OpcaoMapa == 1 ? YELLOW : WHITE);

            DrawTextCentralizado("MAPA 1", 250, 35, corMapa1);
            DrawTextCentralizado("MAPA 2", 320, 35, corMapa2);

            DrawTextCentralizado("Setas para escolher, ENTER para confirmar", 450, 25, LIGHTGRAY);

                //navegação
                if (IsKeyPressed(KEY_UP)){ //mesma logica usada tem tela = 0;
                    OpcaoMapa--;
                if (OpcaoMapa < 0) OpcaoMapa = 1;
                }

                if (IsKeyPressed(KEY_DOWN)){
                    OpcaoMapa++;
                if (OpcaoMapa > 1) OpcaoMapa = 0;
                }

                //confirmar mapa
                if (IsKeyPressed(KEY_ENTER)){
                    PlaySound(selecaoMapas);
                    mapaSelecionado = OpcaoMapa; // salva qual mapa o usuário escolheu

                //reinicia/inicia o game.
                IniciaJogo(&jogo);

                //verifica o mapa e, carrega o mapa correto.
                if (mapaSelecionado == 0){
                    jogo.fundoMapa = &fundoMapa1; //atualiza o fundo conforme a escolha do jogador. (ponteiro que aponta para a textura do mapa).
                    IniciaObstaculosMapa1(&jogo);
                    jogo.food.tex = frutaMapa1; //textura da fruta.
                }
                else if (mapaSelecionado == 1){
                    jogo.fundoMapa = &fundoMapa2; //atualiza o fundo conforme a escolha do jogador.
                    IniciaObstaculosMapa2(&jogo);   
                    jogo.food.tex = frutaMapa2; //textura da fruta.            
                }

                //inicialização para começar o jogo.
                pont = 0; //reinicia a contagem das frutas;
                gameOver = 1; //vivo.
                PlayMusicStream(musica);
                tela = 1; // vai para a tela 1, onde começa o jogo.
                }
        }

        //tela ranking
        else if (tela == 2){//ranking
            DrawTextCentralizado("RANKING (20 melhores)", 50, 40, YELLOW); // título no topo
            int baseY = 100; // começa mais abaixo
            int espacamento = 28; // 30 pixels entre cada linha
            for(int i=0;i<MAX_RANK;i++){
                char txt[30];
                sprintf(txt, "%dº - %d", i+1, ranking[i]);
                DrawText(txt, LARGURA/2 - 50, baseY + i * espacamento, 20, WHITE);
            }

            if (IsKeyPressed(KEY_ENTER)){
                tela = 0;
            }
        }

        EndDrawing(); //frames
    }

    UnloadSound(jogo.somComer);
    UnloadSound(somGameOver);
    UnloadSound(selecaoMapas);
    UnloadMusicStream(musica);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}