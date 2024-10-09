#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// Estruturas para jogador, inimigos e tiros
struct Player {
    float x, y;           // Posição
    float size = 0.05f;   // Tamanho do jogador
};

struct Enemy {
    float x, y;           // Posição
    float size = 0.05f;   // Tamanho do inimigo
    float speed = 0.005f; // Velocidade inicial
};

struct Bullet {
    float x, y;           // Posição
    float size = 0.02f;   // Tamanho do tiro
    float speed = 0.02f;  // Velocidade do tiro
    float dirX, dirY;     // Direção do tiro
    bool active = true;   // Se o tiro está ativo ou não
};

// Variáveis globais
Player player;
vector<Bullet> bullets;
vector<Enemy> enemies;
int score = 0;
bool gameOver = false;
string playerName;
int playerAge;

// Função para inicializar o jogo
void initGame() {
    player.x = 0.0f;
    player.y = 0.0f;
    enemies.clear();
    bullets.clear();
    score = 0;
    gameOver = false;
}

// Função para gerar inimigos fora da tela
void spawnEnemy() {
    Enemy enemy;
    int side = rand() % 4;
    switch (side) {
        case 0: enemy.x = -1.1f; enemy.y = (rand() % 200 - 100) / 100.0f; break; // Esquerda
        case 1: enemy.x = 1.1f; enemy.y = (rand() % 200 - 100) / 100.0f; break;  // Direita
        case 2: enemy.x = (rand() % 200 - 100) / 100.0f; enemy.y = 1.1f; break;  // Cima
        case 3: enemy.x = (rand() % 200 - 100) / 100.0f; enemy.y = -1.1f; break; // Baixo
    }
    enemy.speed = 0.005f + (score * 0.001f); // Aumenta a velocidade conforme o score
    enemies.push_back(enemy);
}

// Função para checar colisão
bool checkCollision(float x1, float y1, float size1, float x2, float y2, float size2) {
    return fabs(x1 - x2) < (size1 + size2) && fabs(y1 - y2) < (size1 + size2);
}

// Desenho do jogador
void drawPlayer() {
    glColor3f(1.0f, 0.0f, 0.0f);  // Vermelho
    glBegin(GL_QUADS);
        glVertex2f(player.x - player.size, player.y - player.size);
        glVertex2f(player.x + player.size, player.y - player.size);
        glVertex2f(player.x + player.size, player.y + player.size);
        glVertex2f(player.x - player.size, player.y + player.size);
    glEnd();
}

// Desenho dos inimigos
void drawEnemies() {
    glColor3f(0.0f, 1.0f, 0.0f);  // Verde
    for (const auto& enemy : enemies) {
        glBegin(GL_TRIANGLES);
            glVertex2f(enemy.x, enemy.y + enemy.size);
            glVertex2f(enemy.x - enemy.size, enemy.y - enemy.size);
            glVertex2f(enemy.x + enemy.size, enemy.y - enemy.size);
        glEnd();
    }
}

// Desenho dos tiros
void drawBullets() {
    glColor3f(1.0f, 1.0f, 0.0f);  // Amarelo
    for (auto& bullet : bullets) {
        if (bullet.active) {
            glBegin(GL_QUADS);
                glVertex2f(bullet.x - bullet.size, bullet.y - bullet.size);
                glVertex2f(bullet.x + bullet.size, bullet.y - bullet.size);
                glVertex2f(bullet.x + bullet.size, bullet.y + bullet.size);
                glVertex2f(bullet.x - bullet.size, bullet.y + bullet.size);
            glEnd();
        }
    }
}

// Função de movimentação e lógica do jogo
void updateGame() {
    if (gameOver) return;

    // Movimento dos inimigos em direção ao jogador
    for (auto& enemy : enemies) {
        float dx = player.x - enemy.x;
        float dy = player.y - enemy.y;
        float length = sqrt(dx * dx + dy * dy);
        enemy.x += (dx / length) * enemy.speed;
        enemy.y += (dy / length) * enemy.speed;

        // Verifica colisão com o jogador
        if (checkCollision(player.x, player.y, player.size, enemy.x, enemy.y, enemy.size)) {
            gameOver = true;
            cout << "Game Over! Score final: " << score << endl;
            initGame();
        }
    }

    // Movimento dos tiros
    for (auto& bullet : bullets) {
        if (bullet.active) {
            bullet.x += bullet.dirX * bullet.speed; // Move na direção correta
            bullet.y += bullet.dirY * bullet.speed; // Move na direção correta
            if (bullet.y > 1.0f || bullet.y < -1.0f || bullet.x > 1.0f || bullet.x < -1.0f) {
                bullet.active = false; // Desativa o tiro se sair da tela
            }
        }
    }

    // Colisão dos tiros com inimigos
    for (auto& bullet : bullets) {
        if (!bullet.active) continue;
        for (auto& enemy : enemies) {
            if (checkCollision(bullet.x, bullet.y, bullet.size, enemy.x, enemy.y, enemy.size)) {
                bullet.active = false;
                enemy.x = 100;  // Move o inimigo para fora da tela (como se fosse destruído)
                score++;
                if (score % 5 == 0) {
                    spawnEnemy();  // Aumenta a dificuldade com mais inimigos
                }
            }
        }
    }

    // Adiciona inimigos conforme o tempo
    if (rand() % 100 < 1) {
        spawnEnemy();
    }
}

// Função para exibir texto (pontuação)
void drawText(float x, float y, string text) {
    glRasterPos2f(x, y);
    for (char& c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Função de renderização
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawPlayer();
    drawEnemies();
    drawBullets();

    // Exibe a pontuação
    glColor3f(1.0f, 1.0f, 1.0f);  // Branco
    drawText(-0.95f, 0.9f, "Score: " + to_string(score));

    glutSwapBuffers();
}

// Função de teclado para movimentar o jogador
void handleKeys(unsigned char key, int x, int y) {
    if (gameOver) return;

    switch (key) {
        case 'w': player.y += 0.05f; break; // Cima
        case 's': player.y -= 0.05f; break; // Baixo
        case 'a': player.x -= 0.05f; break; // Esquerda
        case 'd': player.x += 0.05f; break; // Direita
    }
    glutPostRedisplay();
}

// Função para controlar o tiro
void handleMouse(int button, int state, int x, int y) {
    if (gameOver) return;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        Bullet bullet;
        bullet.x = player.x; // Posição inicial do tiro
        bullet.y = player.y; // Posição inicial do tiro

        // Calcula a direção do tiro com base na posição do mouse
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
        float mouseX = (float)(x) / (windowHeight / 2) - 1.0f; // Normaliza para a faixa [-1, 1]
        float mouseY = -((float)(y) / (windowHeight / 2) - 1.0f); // Normaliza para a faixa [-1, 1]

        float dx = mouseX - bullet.x;
        float dy = mouseY - bullet.y;
        float length = sqrt(dx * dx + dy * dy);
        bullet.dirX = dx / length; // Normaliza
        bullet.dirY = dy / length; // Normaliza

        bullet.active = true;
        bullets.push_back(bullet);
    }
}

// Função de atualização do tempo
void timer(int value) {
    updateGame();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // Chama a função novamente após 16ms (~60fps)
}

// Função principal
int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(0))); // Semente para números aleatórios

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Game");

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Cor de fundo
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Define a projeção ortográfica

    glutDisplayFunc(renderScene);
    glutKeyboardFunc(handleKeys);
    glutMouseFunc(handleMouse);
    glutTimerFunc(0, timer, 0); // Inicia o timer

    initGame(); // Inicializa o jogo
    glutMainLoop(); // Inicia o loop principal
    return 0;
}
