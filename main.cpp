#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <cstdio>

struct Square {
    float x, y, size;
    float r, g, b;
};

Square red = {100, 100, 20, 1.0f, 0.0f, 0.0f};
Square blue = {150, 150, 20, 0.0f, 0.0f, 1.0f};
Square green = {250, 250, 20, 0.0f, 1.0f, 0.0f};

float speed = 30.0f;
int windowWidth = 1000, windowHeight = 800;

clock_t startClock;
float cooldown = 0.0f;
const float cooldownDuration = 2.0f;
float remainingTime = 30.0f;

bool gameEnded = false;
bool gameStarted = false;
char winnerMsg[50] = "";

void drawSquare(const Square& s) {
    glColor3f(s.r, s.g, s.b);
    glBegin(GL_QUADS);
        glVertex2f(s.x - s.size, s.y - s.size);
        glVertex2f(s.x + s.size, s.y - s.size);
        glVertex2f(s.x + s.size, s.y + s.size);
        glVertex2f(s.x - s.size, s.y + s.size);
    glEnd();
}

bool isColliding(const Square& a, const Square& b) {
    return !(a.x + a.size < b.x - b.size ||
             a.x - a.size > b.x + b.size ||
             a.y + a.size < b.y - b.size ||
             a.y - a.size > b.y + b.size);
}

void checkCollisionAndWin() {
    if (isColliding(red, green) || isColliding(blue, green)) {
        sprintf(winnerMsg, "Player A Wins!");
        gameEnded = true;
    }
}

void drawText(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) {
        glutBitmapCharacter(font, *c);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (!gameStarted) {
        glColor3f(0, 0, 0);
        drawText(400, 400, "Press SPACE to Start");
        glutSwapBuffers();
        return;
    }

    drawSquare(red);
    drawSquare(blue);
    drawSquare(green);

    if (cooldown > 0.0f) {
        glColor3f(0, 0, 0);
        char cooldownMsg[50];
        sprintf(cooldownMsg, "Teleport Cooldown: %.1f", cooldown);
        drawText(10, windowHeight - 20, cooldownMsg, GLUT_BITMAP_HELVETICA_12);
    }

    // Display countdown timer
    glColor3f(0, 0, 0);
    char timeMsg[50];
    sprintf(timeMsg, "Time Left: %.1f", remainingTime);
    drawText(windowWidth - 150, windowHeight - 20, timeMsg, GLUT_BITMAP_HELVETICA_12);

    if (gameEnded) {
        glColor3f(0, 0, 0);
        drawText(400, 400, winnerMsg);
        drawText(350, 360, "Press R to Restart");
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

void moveSquare(Square &s, float dx, float dy) {
    s.x += dx;
    s.y += dy;

    if (s.x < s.size) s.x = s.size;
    if (s.x > windowWidth - s.size) s.x = windowWidth - s.size;
    if (s.y < s.size) s.y = s.size;
    if (s.y > windowHeight - s.size) s.y = windowHeight - s.size;
}

void timer(int v) {
    if (gameEnded || !gameStarted) return;

    float elapsed = (float)(clock() - startClock) / CLOCKS_PER_SEC;
    remainingTime = 30.0f - elapsed;

    if (remainingTime <= 0.0f) {
        sprintf(winnerMsg, "Player B Wins!");
        gameEnded = true;
        remainingTime = 0.0f;
        glutPostRedisplay();
        return;
    }

    if (cooldown > 0.0f) {
        cooldown -= 0.05f;
        if (cooldown < 0.0f) cooldown = 0.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(50, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (!gameStarted) {
        if (key == ' ') {
            gameStarted = true;
            startClock = clock();
            remainingTime = 30.0f;
            glutTimerFunc(0, timer, 0);
            glutPostRedisplay();
        }
        return;
    }

    if (gameEnded) {
        if (key == 'r') {
            red = {100, 100, 20, 1.0f, 0.0f, 0.0f};
            blue = {150, 150, 20, 0.0f, 0.0f, 1.0f};
            green = {250, 250, 20, 0.0f, 1.0f, 0.0f};
            cooldown = 0.0f;
            remainingTime = 30.0f;
            gameEnded = false;
            winnerMsg[0] = '\0';
            startClock = clock();
            glutTimerFunc(0, timer, 0);
            glutPostRedisplay();
        }
        return;
    }

    switch (key) {
        case 'w': moveSquare(red, 0, speed); break;
        case 's': moveSquare(red, 0, -speed); break;
        case 'a': moveSquare(red, -speed, 0); break;
        case 'd': moveSquare(red, speed, 0); break;

        case 'i': moveSquare(blue, 0, speed); break;
        case 'k': moveSquare(blue, 0, -speed); break;
        case 'j': moveSquare(blue, -speed, 0); break;
        case 'l': moveSquare(blue, speed, 0); break;
    }

    checkCollisionAndWin();
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (!gameStarted || gameEnded || cooldown > 0) return;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        green.x = x;
        green.y = windowHeight - y;
        cooldown = cooldownDuration;
        checkCollisionAndWin();
        glutPostRedisplay();
    }
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Chua Chui Game  41220200226");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}
