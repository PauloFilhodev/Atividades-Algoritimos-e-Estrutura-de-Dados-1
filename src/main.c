/*
 * Ponteiros + Alocação Dinâmica (vetor e matriz) com raylib
 * ---------------------------------------------------------------
 * Este programa desenha:
 *   1) Uma matriz dinâmica (grade de células coloridas) alocada com malloc,
 *      onde cada linha é um ponteiro para um vetor de inteiros.
 *   2) Um um vetor dinâmico de bolinhas (struct Bola) que se movem na tela,
 *      manipuladas via ponteiros.
 *
 * Conceitos praticados:
 *   - malloc / free
 *   - ponteiro para ponteiro (int **) para representar matriz
 *   - vetor de structs alocado dinamicamente
 *   - passagem de ponteiros para funções (evita cópias, permite alterar
 *     o dado original)
 *   - aritmética de ponteiros ( *(p + i) é equivalente a p[i] )
 *
 * Compilar (Linux, com raylib instalada):
 *   gcc exemplo_ponteiros_raylib.c -o exemplo -lraylib -lm -lpthread -ldl -lrt -lX11
 *
 * Compilar (Windows, MinGW):
 *   gcc exemplo_ponteiros_raylib.c -o exemplo.exe -lraylib -lgdi32 -lwinmm
 */

#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define TAM_CELULA     40   // tamanho de cada célula da grade (matriz)


typedef struct {
    Vector2 pos;
    Vector2 vel;
    float   raio;
    Color   cor;
} Bola;

/* ---------------------------------------------------------------
 * cria uma MATRIZ dinâmica de inteiros (linhas x colunas)
 * Retorna um ponteiro para ponteiro (int **): cada posição do
 * vetor externo aponta para um vetor de inteiros (uma linha).
 * --------------------------------------------------------------- */
int **criarMatriz(int linhas, int colunas) {

    // aloca o vetor de ponteiros (um ponteiro por linha)
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    if (matriz == NULL) return NULL;

    for (int i = 0; i < linhas; i++) {
        // aloca cada linha como um vetor de inteiros
        matriz[i] = (int *)malloc(colunas * sizeof(int));
        for (int j = 0; j < colunas; j++) {
            // preenche com 0 ou 1 aleatoriamente (dois "tipos" de célula)
            matriz[i][j] = 0;
        }
    }
    return matriz;
}

/* libera a memória da matriz: primeiro cada linha, depois o vetor de linhas */
void liberarMatriz(int **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);   // libera cada linha
    }
    free(matriz);           // libera o vetor de ponteiros
}

/* desenha a matriz na tela, célula por célula */
void desenharMatriz(int **matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            Color cor = (matriz[i][j] == 1) ? (Color){20, 40, 70, 255}
                                             : (Color){15, 30, 55, 100};
            DrawRectangle(j * TAM_CELULA, i * TAM_CELULA,
                           TAM_CELULA - 2, TAM_CELULA - 2, cor);
        }
    }
}

/* ---------------------------------------------------------------
 * cria o vetor dinâmico de bolas
 * --------------------------------------------------------------- */
Bola *criarBolas(int quantidade) {
    Bola *bolas = (Bola *)malloc(quantidade * sizeof(Bola));
    if (bolas == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        // usar (bolas + i) é o mesmo que &bolas[i]: aqui acessamos
        // o campo via ponteiro para deixar explícito o conceito.
        Bola *b = (bolas + i);
        b->pos = (Vector2){ GetRandomValue(50, LARGURA_JANELA - 50),
                             GetRandomValue(50, ALTURA_JANELA - 50) };
        b->vel = (Vector2){ (float)GetRandomValue(-4, 4),
                             (float)GetRandomValue(-4, 4) };
        b->raio = (float)GetRandomValue(10, 25);
        b->cor  = (Color){ GetRandomValue(100,255), GetRandomValue(100,255),
                            GetRandomValue(100,255), 255 };
    }
    return bolas;
}

/* atualiza a posição de UMA bola: recebe um PONTEIRO para a struct,
 * então as alterações afetam diretamente o vetor original (sem cópia) */
void atualizarBola(Bola *b) {
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;

    // rebate nas bordas
    if (b->pos.x - b->raio < 0 || b->pos.x + b->raio > LARGURA_JANELA)
        b->vel.x *= -1;
    if (b->pos.y - b->raio < 0 || b->pos.y + b->raio > ALTURA_JANELA)
        b->vel.y *= -1;
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

void marcarVisitaGrade(int ** grade, int linha, int coluna, const Bola *bola, int *contador)
{
    int linha_atual = (int) (bola->pos.y / TAM_CELULA);
    int coluna_atual = (int) (bola->pos.x / TAM_CELULA);
    if (linha_atual >= 0 && linha_atual < linha && coluna_atual >= 0 && coluna_atual < coluna)
    {
        if (grade[linha_atual][coluna_atual] == 0)
        {
            grade[linha_atual][coluna_atual] = 1;
            (*contador)++;
        }
    }
}

// void contarCelulasVisitadas(int ** grade, int linhas, int colunas, int *contador)
// {
//     // int contador;
//     for (int i = 0; i < linhas; i++)
//     {
//         for (int j = 0; j < colunas; j++)
//         {
//             // if (grade[i][j] == 0)
//             // {
//             //     grade[i][j] = 1;
//             //     (*contador)++;
//             // }
//             if (grade[i][j] == 1)
//             {
//                 (*contador)++;
//             }
//         }
//     }
//     // return contador;
// }

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA,
               "Ponteiros e Alocacao Dinamica - raylib");
    SetTargetFPS(60);

    int linhas   = ALTURA_JANELA / TAM_CELULA;
    int colunas  = LARGURA_JANELA / TAM_CELULA;
    int **grade  = criarMatriz(linhas, colunas);   // matriz dinâmica

    int quantidadeBolas = 1;
    Bola *bolas = criarBolas(quantidadeBolas);      // vetor dinâmico
    int contador_celulas_visitadas = 0;
    int *p_contador = &contador_celulas_visitadas;
    int *p_quantiaBolas = &quantidadeBolas;

    while (!WindowShouldClose()) {
        // percorre o vetor usando aritmética de ponteiros:
        // (bolas + i) aponta para o i-ésimo elemento do vetor
        for (int i = 0; i < quantidadeBolas; i++) {
            atualizarBola(bolas + i);
            marcarVisitaGrade(grade, linhas, colunas, &bolas[i], p_contador);
        }

        // int totalVisitadas = contarCelulasVisitadas(grade, linhas, colunas);
        // contarCelulasVisitadas(grade, linhas, colunas, p_contador);

        gerenciarBolas(&bolas, &quantidadeBolas);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            desenharMatriz(grade, linhas, colunas);

            for (int i = 0; i < quantidadeBolas; i++) {
                DrawCircleV(bolas[i].pos, bolas[i].raio, bolas[i].cor);
            }

            DrawText("Matriz (int**) e vetor de structs (Bola*) alocados com malloc",
                     10, 10, 18, WHITE);
            DrawText(TextFormat("Celulas visitadas %d / %d", contador_celulas_visitadas, linhas * colunas), 10, 35, 20, YELLOW);
            DrawText(TextFormat("Quantidade de bolas na cena: %d", *p_quantiaBolas), 10, 65, 20, RED);
            DrawText("Pressione ESC para sair", 10, ALTURA_JANELA - 25, 16, WHITE);

        EndDrawing();
    }

    // libera TODA a memória alocada dinamicamente antes de encerrar
    free(bolas);
    liberarMatriz(grade, linhas);

    CloseWindow();
    return 0;
}