#include <stdio.h>
#include "entidade.h"

int main (void)
{
    InitWindow(800, 600, "MODULACAO");
    SetTargetFPS(60);

    Entidade *jogador = entidadeCriar(ENTIDADE_JOGADOR, (Vector2){ 400, 100 });
    Entidade *inimigo = entidadeCriar(ENTIDADE_INIMIGO, (Vector2){ 400, 300 });
    Entidade *item = entidadeCriar(ENTIDADE_ITEM, (Vector2){ 400, 500 });


    while (!WindowShouldClose())
    {



        BeginDrawing();


            DrawCircleV(jogador->pos, 15.0f, BLUE);
            DrawCircleV(inimigo->pos, 20.0f, RED);
            DrawCircleV(item->pos, 10.0f, YELLOW);
        EndDrawing();


    }
    




    return 0;
}