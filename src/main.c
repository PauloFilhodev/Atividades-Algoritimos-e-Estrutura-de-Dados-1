/*
 * Ponteiros para Struct + Vetor de Struct com raylib
 * ---------------------------------------------------------------
 * Evolução da atividade3: agora o foco é em como PONTEIROS PARA
 * STRUCT permitem localizar e alterar um elemento específico dentro
 * de um vetor de struct, sem copiar a struct inteira.
 *
 * O vetor de inimigos é um VETOR DE STRUCT (bloco contíguo, alocado
 * uma única vez com malloc). As funções recebem e retornam
 * "Inimigo *" (ponteiro para struct) para localizar e modificar um
 * elemento específico desse vetor.
 *
 * Conceitos praticados:
 *   - vetor de struct (bloco contíguo de memória)
 *   - ponteiro para struct como retorno de função (localizar um
 *     elemento dentro do vetor e devolver o endereço dele)
 *   - ponteiro para struct como parâmetro de função (alterar o
 *     elemento apontado diretamente, sem cópia)
 *   - enum para representar o estado de cada inimigo
 *   - malloc / free do vetor
 *
 * Compilar (Linux, com raylib instalada):
 *   gcc atividade4.c -o atividade4 -lraylib -lm -lpthread -ldl -lrt -lX11
 */

#include "raylib.h"
#include <stdlib.h> 
#include <time.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define RAIO_JOGADOR   20.0f
#define TOTAL_INIMIGOS 8
#define DANO_TIRO      20
#define MAX_VIDA 60

typedef enum {
    INIMIGO_VIVO,
    INIMIGO_MORTO
} EstadoInimigo;

typedef struct {
    Vector2       pos;
    float         raio;
    int           vida;
    EstadoInimigo estado;
} Inimigo;

/* preenche o vetor de struct (recebido por ponteiro) com valores iniciais */
void inicializarInimigos(Inimigo *vetor, int n) {
    for (int i = 0; i < n; i++) {
        Inimigo *ini = (vetor + i); // ponteiro para o i-ésimo elemento
        ini->pos    = (Vector2){ GetRandomValue(30, LARGURA_JANELA - 30),
                                  GetRandomValue(30, ALTURA_JANELA - 30) };
        ini->raio   = 15.0f;
        ini->vida   = GetRandomValue(20, MAX_VIDA);
        ini->estado = INIMIGO_VIVO;
    }
}

/* recebe um ponteiro para UM inimigo específico do vetor e altera
 * a vida/estado diretamente na memória original (sem cópia) */
void atingirInimigo(Inimigo *inimigo, int dano) {
    if (inimigo == NULL || inimigo->estado == INIMIGO_MORTO) return;

    inimigo->vida -= dano;
    if (inimigo->vida <= 0) {
        inimigo->vida = 0;
        inimigo->estado = INIMIGO_MORTO;
    }
}

/* percorre o vetor de struct e RETORNA UM PONTEIRO para o inimigo
 * vivo mais próximo da posição informada (ou NULL se não houver) */
Inimigo *encontrarInimigoMaisProximo(Inimigo *vetor, int n, Vector2 posJogador) {
    Inimigo *maisProximo = NULL;
    float menorDistancia = 0.0f;

    for (int i = 0; i < n; i++) {
        Inimigo *ini = (vetor + i);
        if (ini->estado == INIMIGO_MORTO) continue;

        float dx = ini->pos.x - posJogador.x;
        float dy = ini->pos.y - posJogador.y;
        float distancia = sqrtf(dx * dx + dy * dy);

        if (maisProximo == NULL || distancia < menorDistancia) {
            maisProximo = ini;
            menorDistancia = distancia;
        }
    }
    return maisProximo;
}

void curarTodos(Inimigo *vetor, int n, int cura)
{
    for (int i = 0; i < n; i++)
    {
        Inimigo *atual = (vetor + i);

        if (atual->estado != INIMIGO_MORTO)
        {
            atual->vida += cura;
        }
    }
}

Inimigo *encontrarInimigoMaisFraco(Inimigo *vetor_inimigos, int max_inimigos)
{
    int menor_vida = vetor_inimigos->vida;
    Inimigo *i_mais_fraco;
    for (int i = 0; i < max_inimigos; i++)
    {
        Inimigo *i_atual = (vetor_inimigos + i);
        if (i_atual->estado == INIMIGO_MORTO) {
            continue;
        }
        
        if (i_atual->vida < menor_vida)
        {
            i_mais_fraco = i_atual;
            menor_vida = i_mais_fraco->vida;
        } else if (i_atual->vida == menor_vida)
        {
            continue;
        }
    }

    if (!i_mais_fraco) return NULL;
    return i_mais_fraco;
}

void desenharInimigo(Inimigo *ini) {
    if (ini->estado == INIMIGO_MORTO) return;
    if (ini->vida > MAX_VIDA) ini->vida = MAX_VIDA;
    Color cor = (ini->vida > 30) ? MAROON : ORANGE;
    
    DrawCircleV(ini->pos, ini->raio, cor);
    DrawText(TextFormat("%d", ini->vida), ini->pos.x - 8, ini->pos.y - 26, 14, BLACK);
}

void gerenciarBolas(Bola **bolas, int *quantidadeBolas)
{
    if (IsKeyPressed(KEY_SPACE)) // cria a bola nova
    {
        (*quantidadeBolas)++;

        Bola *temp = realloc(*bolas, (*quantidadeBolas) * sizeof(Bola));
        if (temp != NULL)
        {
            *bolas = temp;

            Bola *newB = &((*bolas)[*quantidadeBolas - 1]);
            newB->pos = (Vector2){ GetRandomValue(50, LARGURA_JANELA - 50),
                                    GetRandomValue(50, ALTURA_JANELA - 50)};
            newB->vel = (Vector2){ (float)GetRandomValue(-4, 4),
                                    (float)GetRandomValue(-4, 4)};
            newB->raio = (float)GetRandomValue(10, 25);
            newB->cor  = (Color){ GetRandomValue(100,255), GetRandomValue(100,255),
                            GetRandomValue(100,255), 255 };
        }
    } else if (IsKeyPressed(KEY_BACKSPACE)) // deleta a ultima bola
    {
        if ((*quantidadeBolas) > 0)
        {
            (*quantidadeBolas)--;

            if (*quantidadeBolas > 0)
            {
                Bola *temp = realloc(*bolas, (*quantidadeBolas) * sizeof(Bola));
                if (temp != NULL) *bolas = temp;
            } else {
                free(*bolas);
                *bolas = NULL;
            }
        } 
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 4 - Ponteiros para Struct + Vetor de Struct");
    SetTargetFPS(60);

    Vector2 jogador = { LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f };

    // vetor de struct: um único bloco contíguo de memória com TOTAL_INIMIGOS structs
    Inimigo *inimigos = (Inimigo *)malloc(TOTAL_INIMIGOS * sizeof(Inimigo));
    inicializarInimigos(inimigos, TOTAL_INIMIGOS);

    while (!WindowShouldClose()) {

        float vel = 250.0f * GetFrameTime();
        if (IsKeyDown(KEY_RIGHT)) jogador.x += vel;
        if (IsKeyDown(KEY_LEFT))  jogador.x -= vel;
        if (IsKeyDown(KEY_UP))    jogador.y -= vel;
        if (IsKeyDown(KEY_DOWN))  jogador.y += vel;
        if (IsKeyPressed(KEY_C)) {
            curarTodos(inimigos, TOTAL_INIMIGOS, GetRandomValue(10, 30)); 
        }

        if (IsKeyPressed(KEY_SPACE)) {
            // ponteiro para o inimigo vivo mais próximo (ou NULL)
            // Inimigo *alvo = encontrarInimigoMaisProximo(inimigos, TOTAL_INIMIGOS, jogador);
            Inimigo *alvo = encontrarInimigoMaisFraco(inimigos, TOTAL_INIMIGOS);
            atingirInimigo(alvo, DANO_TIRO);
        }

        gerenciarBolas(&bolas, &quantidadeBolas);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int i = 0; i < TOTAL_INIMIGOS; i++) {
                desenharInimigo(inimigos + i);
            }

            DrawCircleV(jogador, RAIO_JOGADOR, BLUE);

            DrawText("ESPACO atira no inimigo vivo mais proximo", 10, 10, 20, DARKGRAY);
            DrawText("Setas movem o jogador | ESC sai", 10, ALTURA_JANELA - 25, 16, GRAY);

        EndDrawing();
    }

    free(inimigos); // libera o vetor de struct

    CloseWindow();
    return 0;
}