
#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define MAX_BOLAS 10

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

// FUNCOES:
Color cor_random(void);

float modulo_das_velocidades(float vx, float vy);

void alterar_colisao(int *tipoColisao);
void alterar_infos(int *infos);
void altera_multiplicador(int *tipoForca, float *multiplicadorForca);

void info(int infos, int tipoColisao, float multiplicadorForca, float forca, Bola player, Bola bolas[], int quantidadeBolas, float velocidadePrevista, float anguloGraus);

void colisaoBORDASGeral(int screenWidth, int screenHeight, Bola *player, Bola bolas[], int quantidadeBolas);
void mover_bola(Bola *b);
void verificar_borda(Bola *b, int largura, int altura);

void desenhar_bola(Bola b);
void seta(int mouseX, int mouseY, float dx, float dy);
void desenharVetor(Bola b, Color cor);

void lancamento_player(Bola *player, float distancia, float aceleracao, float dx, float dy);
void update_o_mouse(float *dx, float *dy, float *mouseX, float *mouseY, Bola player);

// A MAIS IMPORTANTE DE TODAS AS FUNCOES!!!!!!!
bool verificarColisaoCirculo(Bola b1, Bola b2);
void resolverColisao(Bola *b1, Bola *b2, int tipoColisao);

int main(void)
{
    srand(time(NULL));

    const int screenWidth = 1000;
    const int screenHeight = 550;

    float forca = 0;
    int tipoForca = 1;
    float multiplicadorForca = 0.01f;

    float aceleracao = 0;
    float distancia = 0;
    float velocidadePrevista = 0;

    int tipoColisao = 1;
    int infoAtiva = 1;

    InitWindow(screenWidth, screenHeight, "FISICA TRABALHO JOAO VICTOR GUZATTI");

    Bola player = {100, screenHeight / 2.0f, 0, 0, 3.0f, 20, WHITE};

    int quantidadeBolas = 3;

    Bola bolas[MAX_BOLAS] =
    {
        {400, 80, 0, 0, 3.0f, 20, cor_random()},
        {600, 80, 0, 0, 4.0f, 25, cor_random()},
        {300, 300, 0, 0, 6.0f, 30, cor_random()}
    };

    float mouseX = GetMouseX();
    float mouseY = GetMouseY();

    float dx = mouseX - player.x;
    float dy = mouseY - player.y;

    distancia = sqrt(dx * dx + dy * dy);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_C))
            alterar_colisao(&tipoColisao);

        if (IsKeyPressed(KEY_F))
            altera_multiplicador(&tipoForca, &multiplicadorForca);

        if (IsKeyPressed(KEY_Z))
        {
            player.velX = 0.0f;
            player.velY = 0.0f;
        }

        if (IsKeyPressed(KEY_X))
        {
            player.velX = 0.0f;
            player.velY = 0.0f;

            for (int i = 0; i < quantidadeBolas; i++)
            {
                bolas[i].velX = 0.0f;
                bolas[i].velY = 0.0f;
            }
        }

        // Cria outra bola
        if (IsKeyPressed(KEY_B) && quantidadeBolas < MAX_BOLAS)
        {
            bolas[quantidadeBolas] = (Bola)
            {
                GetRandomValue(50, screenWidth - 50),
                GetRandomValue(50, screenHeight - 50),
                0,
                0,
                GetRandomValue(2, 6),
                GetRandomValue(15, 30),
                cor_random()
            };

            quantidadeBolas++;
        }

        if (IsKeyPressed(KEY_TAB))
            alterar_infos(&infoAtiva);

        update_o_mouse(&dx, &dy, &mouseX, &mouseY, player);

        distancia = sqrt(dx * dx + dy * dy);

        forca = distancia * multiplicadorForca;
        aceleracao = forca / player.massa;
        velocidadePrevista = aceleracao * 5;

        float angulo = atan2(-dy, dx);
        float anguloGraus = angulo * (180 / PI);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            lancamento_player(&player, distancia, aceleracao, dx, dy);

        // Verifica as colisoes
        for (int i = 0; i < quantidadeBolas; i++)
        {
            resolverColisao(&player, &bolas[i], tipoColisao);

            for (int j = i + 1; j < quantidadeBolas; j++)
            {
                resolverColisao(&bolas[i], &bolas[j], tipoColisao);
            }
        }

        colisaoBORDASGeral(screenWidth, screenHeight, &player, bolas, quantidadeBolas);

        BeginDrawing();

        ClearBackground(DARKGREEN);

        DrawLine(player.x, player.y, mouseX, mouseY, BLUE);
        seta(mouseX, mouseY, dx, dy);

        DrawLine(player.x, player.y, player.x + 65, player.y, GRAY);

        desenhar_bola(player);

        for (int i = 0; i < quantidadeBolas; i++)
        {
            desenhar_bola(bolas[i]);
        }

        desenharVetor(player, RED);

        for (int i = 0; i < quantidadeBolas; i++)
        {
            desenharVetor(bolas[i], BLUE);
        }

        info(infoAtiva, tipoColisao, multiplicadorForca, forca, player, bolas, quantidadeBolas, velocidadePrevista, anguloGraus);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// Sorteia uma cor
Color cor_random(void)
{
    int cor = (rand() % 4) + 1;

    if (cor == 1) return BLUE;
    else if (cor == 2) return GREEN;
    else if (cor == 3) return YELLOW;
    else return PURPLE;
}

float modulo_das_velocidades(float vx, float vy)
{
    return sqrt((vx * vx) + (vy * vy));
}

// Altera o tipo de colisao
void alterar_colisao(int *tipoColisao)
{
    *tipoColisao += 1;

    if (*tipoColisao > 4)
    {
        *tipoColisao = 1;
    }
}

// Alterna a exibicao das informacoes
void alterar_infos(int *infos)
{
    *infos = !*infos;
}

// Altera o multiplicador da forca
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

// Mostra as informacoes na tela
void info(int infos, int tipoColisao, float multiplicadorForca, float forca, Bola player, Bola bolas[], int quantidadeBolas, float velocidadePrevista, float anguloGraus)
{
    if (infos == 1)
    {
        DrawText("Colisor fisica 1 - Aperte C para mudar a colisao, F para o multiplicador de forca, Z para parar o player, X para todas as bolas,\nB para adicionar mais uma bola. TAB para esconder as informacoes.",
                 10, 10, 15, WHITE);

        if (tipoColisao == 1)
        {
            DrawText("Perfeitamente inelastica", 10, 45, 20, YELLOW);
        }
        else if (tipoColisao == 2)
        {
            DrawText("Parcialmente inelastica", 10, 45, 20, ORANGE);
        }
        else if (tipoColisao == 3)
        {
            DrawText("Elastica", 10, 45, 20, GREEN);
        }
        else
        {
            DrawText("SUPERELASTICA (EXPLOSIVA)", 10, 45, 20, RED);
        }

        DrawText(TextFormat("Multiplicador Forca: %.2fN", multiplicadorForca), 10, 70, 14, WHITE);
        DrawText(TextFormat("Forca: %.2f N", forca), 10, 90, 14, WHITE);
        DrawText(TextFormat("Vx: %.2f Vy: %.2f", player.velX, player.velY), 10, 110, 14, WHITE);
    }

    //mostra a velocidade previsaata
    DrawText(TextFormat("Velocidade prevista: %.2f m/s", velocidadePrevista), 10, 130, 14, SKYBLUE);

    // graus
    DrawText(TextFormat("%.1f graus", anguloGraus), player.x + 35, player.y - 35, 14, YELLOW);

    // mostra a massa e a velocidade do player
    DrawText(TextFormat("%.0f kg", player.massa), player.x - 17, player.y - 5, 14, BLACK);

    float tamanho = 15;
    float pontaX = player.x + player.velX * tamanho;
    float pontaY = player.y + player.velY * tamanho;

    DrawText(TextFormat("%.1f m/s", modulo_das_velocidades(player.velX, player.velY)), pontaX, pontaY + 5, 14, RED);

    // Mostra as massas e velocidades das outras bolas
    for (int i = 0; i < quantidadeBolas; i++)
    {
        DrawText(TextFormat("%.0f kg", bolas[i].massa), bolas[i].x - 17, bolas[i].y - 5, 14, BLACK);

        pontaX = bolas[i].x + bolas[i].velX * tamanho;
        pontaY = bolas[i].y + bolas[i].velY * tamanho;

        DrawText(TextFormat("%.1f m/s", modulo_das_velocidades(bolas[i].velX, bolas[i].velY)), pontaX, pontaY + 5, 14, RED);
    }
}

// Move e verifica as bordas de todas as bolas
void colisaoBORDASGeral(int screenWidth, int screenHeight, Bola *player, Bola bolas[], int quantidadeBolas)
{
    mover_bola(player);
    verificar_borda(player, screenWidth, screenHeight);

    for (int i = 0; i < quantidadeBolas; i++)
    {
        mover_bola(&bolas[i]);
        verificar_borda(&bolas[i], screenWidth, screenHeight);
    }
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

void seta(int mouseX, int mouseY, float dx, float dy)
{
    float x1 = mouseX - 12 * cos(atan2(dy, dx) - 0.5);
    float y1 = mouseY - 12 * sin(atan2(dy, dx) - 0.5);

    float x2 = mouseX - 12 * cos(atan2(dy, dx) + 0.5);
    float y2 = mouseY - 12 * sin(atan2(dy, dx) + 0.5);

    DrawLine(mouseX, mouseY, x1, y1, BLUE);
    DrawLine(mouseX, mouseY, x2, y2, BLUE);
}

void desenharVetor(Bola b, Color cor)
{
    float tamanho = 15;

    // Calcula o tamanho da seta
    float dx = b.velX * tamanho;
    float dy = b.velY * tamanho;

    float pontaX = b.x + dx;
    float pontaY = b.y + dy;

    DrawLine(b.x, b.y, pontaX, pontaY, RED);

    float angulo = atan2(dy, dx);

    float x1 = pontaX - 10 * cos(angulo - 0.5f);
    float y1 = pontaY - 10 * sin(angulo - 0.5f);

    float x2 = pontaX - 10 * cos(angulo + 0.5f);
    float y2 = pontaY - 10 * sin(angulo + 0.5f);

    DrawLine(pontaX, pontaY, x1, y1, RED);
    DrawLine(pontaX, pontaY, x2, y2, RED);
}

// Calcula o lancamento do player
void lancamento_player(Bola *player, float distancia, float aceleracao, float dx, float dy)
{
    if (distancia > 0)
    {
        player->velX = (dx / distancia) * aceleracao * 5;
        player->velY = (dy / distancia) * aceleracao * 5;
    }
}

void update_o_mouse(float *dx, float *dy, float *mouseX, float *mouseY, Bola player)
{
    *mouseX = GetMouseX();
    *mouseY = GetMouseY();

    *dx = *mouseX - player.x;
    *dy = *mouseY - player.y;
}

bool verificarColisaoCirculo(Bola b1, Bola b2)
{
    float dx = b1.x - b2.x;
    float dy = b1.y - b2.y;
    float distancia = sqrt((dx * dx) + (dy * dy));

    return distancia < (b1.raio + b2.raio);
}

// Resolve a colisao entre duas bolas
void resolverColisao(Bola *b1, Bola *b2, int tipoColisao)
{
    if (verificarColisaoCirculo(*b1, *b2))
    {
        float dx = b2->x - b1->x;
        float dy = b2->y - b1->y;

        float dist = sqrtf(dx * dx + dy * dy);

        if (dist == 0)
        {
            return;
        }

        // Calcula a normal da colisao
        float nx = dx / dist;
        float ny = dy / dist;

        // aqui calcula a velocidade relativa de cada vetor x e y
        float rvx = b2->velX - b1->velX;
        float rvy = b2->velY - b1->velY;

        float velNormal = rvx * nx + rvy * ny;

        if (velNormal < 0)
        {
            float e;

            if (tipoColisao == 1)
            {
                e = 0.0f;
            }
            else if (tipoColisao == 2)
            {
                e = 0.5f;
            }
            else if (tipoColisao == 3)
            {
                e = 1.0f;
            }
            else
            {
                e = 1.5f;
            }

            // Calcula o impulso
            float j = -(1.0f + e) * velNormal;
            j = j / ((1.0f / b1->massa) + (1.0f / b2->massa));

            b1->velX = b1->velX - (j / b1->massa) * nx;
            b1->velY = b1->velY - (j / b1->massa) * ny;

            b2->velX = b2->velX + (j / b2->massa) * nx;
            b2->velY = b2->velY + (j / b2->massa) * ny;
        }
    }
}
