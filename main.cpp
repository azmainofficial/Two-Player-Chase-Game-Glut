#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <cstdio>

struct Square {
    float x, y, size;
    float r, g, b; // color
};

Square red = {100, 100, 20, 1.0f, 0.0f, 0.0f};
Square blue = {150, 150, 20, 0.0f, 0.0f, 1.0f};
Square green = {250, 250, 20, 0.0f, 1.0f, 0.0f};

float speed = 2.0f;

int windowWidth = 500, windowHeight = 500;

time_t startTime;
float cooldown = 0.0f;
const float cooldownDuration = 2.0f;
bool gameEnded = false;
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
    return fabs(a.x - b.x) < (a.size + b.size) && fabs(a.y - b.y) < (a.size + b.size);
}

void checkCollisionAndWin() {
    if (isColliding(red, green)) {
        sprintf(winnerMsg, "Player A Wins!");
        gameEnded = true;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawSquare(red);
    drawSquare(green);

    if (gameEnded) {
        glColor3f(1.0, 1.0, 1.0);
        glRasterPos2f(200, 250);
        for (char* c = winnerMsg; *c; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
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

void keyboard(unsigned char key, int x, int y) {
    if (gameEnded) return;

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
    if (gameEnded || cooldown > 0) return;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        green.x = x;
        green.y = windowHeight - y;
        cooldown = cooldownDuration;
        checkCollisionAndWin();
        glutPostRedisplay();
    }
}

void timer(int v) {
    if (gameEnded) return;

    float elapsed = difftime(time(NULL), startTime);
    if (elapsed >= 30.0f) {
        sprintf(winnerMsg, "Player B Wins!");
        gameEnded = true;
        glutPostRedisplay();
        return;
    }

    if (cooldown > 0.0f) {
        cooldown -= 0.05f;
        if (cooldown < 0.0f) cooldown = 0.0f;
    }

    glutTimerFunc(50, timer, 0);
}

void init() {
    glClearColor(0, 0, 0, 1);
    startTime = time(NULL);
    glutTimerFunc(0, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Two Player Chase Game");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}
