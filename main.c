#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

typedef struct
{
    float x;
    float y;
    float velX;
    float velY;
    float massa;
    float raio;
    Color cor;
} Bola;

Color cor_random()
{
    int cor = (rand() % 4) + 1;

    if (cor == 1) return BLUE;
    else if (cor == 2) return GREEN;
    else if (cor == 3) return YELLOW;
    else return RED;
}

void mover_bola(Bola *b)
{
    b->x = b->x + b->velX;
    b->y = b->y + b->velY;
}

void verificar_borda(Bola *b, int largura, int altura)
{
    if (b->x > largura - b->raio)
    {
        b->x = largura - b->raio;
        b->velX = -b->velX;
    }
    else if (b->x < b->raio)
    {
        b->x = b->raio;
        b->velX = -b->velX;
    }

    if (b->y > altura - b->raio)
    {
        b->y = altura - b->raio;
        b->velY = -b->velY;
    }
    else if (b->y < b->raio)
    {
        b->y = b->raio;
        b->velY = -b->velY;
    }
}

void desenhar_bola(Bola b)
{
    DrawCircle(b.x, b.y, b.raio, b.cor);
}

void alterar_colisao(int *tipoColisao)
{
    *tipoColisao += 1;
    if (*tipoColisao > 2)
    {
        *tipoColisao = 1;
    }
}

void altera_multiplicador(int *tipoForca, float *multiplicadorForca)
{
    if (*tipoForca == 1)
    {
        *multiplicadorForca = 0.05f;
        *tipoForca = 2;
    }
    else
    {
        *multiplicadorForca = 0.01f;
        *tipoForca = 1;
    }
}

void seta(int mouseX, int mouseY, float dx, float dy)
{       
    float x1 = mouseX - 12 * cos(atan2(dy, dx) - 0.5);
    float y1 = mouseY - 12 * sin(atan2(dy, dx) - 0.5);     
                                                
    float x2 = mouseX - 12 * cos(atan2(dy, dx) + 0.5);   
    float y2 = mouseY - 12 * sin(atan2(dy, dx) + 0.5);

    DrawLine(mouseX, mouseY, x1, y1, RED);
    DrawLine(mouseX, mouseY, x2, y2, RED);
}

void lancamento_player(Bola *player, float distancia, float aceleracao, float dx, float dy)
{
    if (distancia > 0)
    {
        player->velX = (dx / distancia) * aceleracao * 5;
        player->velY = (dy / distancia) * aceleracao * 5;
    }
}

void update_mouse(float *dx, float *dy, float *mouseX, float *mouseY, Bola player)
{
    *mouseX = GetMouseX();
    *mouseY = GetMouseY();

    *dx = *mouseX - player.x;
    *dy = *mouseY - player.y;
}

void colisaoBORDASGeral(int screenWidth, int screenHeight, Bola *player, Bola *bola1, Bola *bola2, Bola *bola3) 
{
    mover_bola(player);
    mover_bola(bola1);
    mover_bola(bola2);
    mover_bola(bola3);

    verificar_borda(bola1, screenWidth, screenHeight);
    verificar_borda(bola2, screenWidth, screenHeight);
    verificar_borda(bola3, screenWidth, screenHeight);
    verificar_borda(player, screenWidth, screenHeight);
}

bool verificarColisaoCirculo(Bola b1, Bola b2)
{
    float dx = b1.x - b2.x;
    float dy = b1.y - b2.y;
    float distancia = sqrt((dx * dx) + (dy * dy));

    return distancia < (b1.raio + b2.raio);
}


void resolverColisao(Bola *b1, Bola *b2, int tipoColisao)
{
    if (verificarColisaoCirculo(*b1, *b2))
    {
        float dx = b2->x - b1->x;
        float dy = b2->y - b1->y;

        float dist = sqrt(dx * dx + dy * dy);

        if (dist == 0)
        {
            dist = 1;
        }

        float nx = dx / dist;
        float ny = dy / dist;

        float vel1 = b1->velX * nx + b1->velY * ny;
        float vel2 = b2->velX * nx + b2->velY * ny;

        float auxX = b1->velX;
        float auxY = b1->velY;

        if (vel1 > vel2)
        {
            if (tipoColisao == 1)
            {
                b1->velX = b2->velX;
                b1->velY = b2->velY;

                b2->velX = auxX;
                b2->velY = auxY;
            }
            else
            {
                b1->velX = (b1->velX + b2->velX) / 2;
                b1->velY = (b1->velY + b2->velY) / 2;

                b2->velX = b1->velX;
                b2->velY = b1->velY;
            }
        }
    }
}


int main(void)
{
    srand(time(NULL));

    const int screenWidth = 800;
    const int screenHeight = 450;

    float forca = 0;
    int tipoForca = 1;
    float multiplicadorForca = 0.01f;

    float aceleracao = 0;
    float distancia = 0;

    int tipoColisao = 1;

    InitWindow(screenWidth, screenHeight, "FISICA TRABALHO JOAO VICTOR GUZATTI");

    Bola player = {100, screenHeight / 2.0f, 0, 0, 3.0f, 20, WHITE};

    Bola bola1 = {400, 80, 0, 0, 3.0f, 20, cor_random()};
    Bola bola2 = {600, 80, 0, 0, 4.0f, 25, cor_random()};
    Bola bola3 = {300, 300, 0, 0, 6.0f, 30, cor_random()};

    float mouseX = GetMouseX();
    float mouseY = GetMouseY();

    float dx = mouseX - player.x;
    float dy = mouseY - player.y;

    /* git config --global user.email "joaoguzatti@alunos.utfpr.edu.br"
  git config --global user.name "Your Name"*/


    distancia = sqrt(dx * dx + dy * dy);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_C))
            alterar_colisao(&tipoColisao);

        if (IsKeyPressed(KEY_F))
            altera_multiplicador(&tipoForca, &multiplicadorForca);

        update_mouse(&dx, &dy, &mouseX, &mouseY, player);

        distancia = sqrt(dx * dx + dy * dy);

        forca = distancia * multiplicadorForca;  
        aceleracao = forca / player.massa;

        float angulo = atan2(-dy, dx);
        float anguloGraus = angulo * (180 / PI);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            lancamento_player(&player, distancia, aceleracao, dx, dy);

        resolverColisao(&player, &bola1, tipoColisao);
        resolverColisao(&player, &bola2, tipoColisao);
        resolverColisao(&player, &bola3, tipoColisao);
        resolverColisao(&bola1, &bola2, tipoColisao);
        resolverColisao(&bola1, &bola3, tipoColisao);
        resolverColisao(&bola2, &bola3, tipoColisao);

        colisaoBORDASGeral(screenWidth, screenHeight, &player, &bola1, &bola2, &bola3);
        
        BeginDrawing();

        ClearBackground(DARKGREEN);

        DrawLine(player.x, player.y, mouseX, mouseY, RED);

        seta(mouseX, mouseY, dx, dy); 
        
        DrawLine(player.x, player.y, player.x + 65, player.y, GRAY);

        DrawText(TextFormat("%.1f graus", anguloGraus), player.x + 35, player.y - 35, 14, YELLOW);

        DrawText("Colisor fisica 1 - Aperte C para mudar a colisao; F para o multiplicador de força", 10, 10, 18, WHITE);

        if (tipoColisao == 1) DrawText("Colisao elastica", 10, 35, 20, BLACK);

        if (tipoColisao == 2) DrawText("Colisao inelastica", 10, 35, 20, BLACK);

        DrawText(TextFormat("Multiplicador Forca: %.2fN", multiplicadorForca), 10, 70, 14, WHITE);
        DrawText(TextFormat("Forca: %.2f N", forca), 10, 90, 14, WHITE);
        DrawText(TextFormat("Aceleracao: %.2f m/s2", aceleracao), 10, 110, 14, WHITE);
        DrawText(TextFormat("Vx: %.2f Vy: %.2f", player.velX, player.velY), 10, 130, 14, WHITE);

        desenhar_bola(player);
        desenhar_bola(bola1);
        desenhar_bola(bola2);
        desenhar_bola(bola3);

        DrawText(TextFormat("%.0f kg", player.massa), player.x - 17, player.y - 5, 14, BLACK);
        DrawText(TextFormat("%.0f kg", bola1.massa), bola1.x - 17, bola1.y - 5, 14, BLACK);
        DrawText(TextFormat("%.0f kg", bola2.massa), bola2.x - 17, bola2.y - 5, 14, BLACK);
        DrawText(TextFormat("%.0f kg", bola3.massa), bola3.x - 17, bola3.y - 5, 14, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
