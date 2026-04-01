#include <GL/glut.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
using namespace std;

// ---------------- GLOBAL VARIABLES ----------------
int screen = 0;
/*
0 = Welcome
1 = Insert Card
2 = PIN Entry
3 = Main Menu
4 = Balance
5 = Withdraw
6 = Deposit
7 = Receipt
8 = Exit
9 = Mini Statement
10 = Fast Cash
11 = Invalid PIN
*/

string enteredPIN = "";
string enteredAmount = "";
float balance = 5000.0;
string message = "";
int windowWidth = 900, windowHeight = 700;
const string correctPIN = "1234";
vector<string> miniStatement;

// ---------------- TEXT FUNCTION ----------------
void drawText(float x, float y, string text, void *font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(font, c);
}

// ---------------- RECTANGLE FUNCTION ----------------
void drawFilledRect(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}

// ---------------- BORDER RECTANGLE ----------------
void drawBorderRect(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}

// ---------------- ADD MINI STATEMENT ----------------
void addTransaction(string txn) {
    miniStatement.push_back(txn);
    if (miniStatement.size() > 5)
        miniStatement.erase(miniStatement.begin());
}

// ---------------- RESET ATM SESSION ----------------
void resetSession() {
    enteredPIN = "";
    enteredAmount = "";
    message = "";
    screen = 0;
}

// ---------------- ATM BODY ----------------
void drawATMBody() {
    drawFilledRect(-0.95, -0.95, 0.95, 0.95, 0.25, 0.25, 0.28);
    drawFilledRect(-0.85, -0.85, 0.85, 0.85, 0.35, 0.35, 0.4);

    drawFilledRect(-0.65, -0.55, 0.65, 0.65, 0.88, 0.95, 1.0);
    drawBorderRect(-0.65, -0.55, 0.65, 0.65, 0.1, 0.1, 0.1);

    drawFilledRect(-0.65, 0.68, 0.65, 0.80, 0.0, 0.2, 0.5);
    glColor3f(1, 1, 1);
    drawText(-0.18, 0.73, "ATM MACHINE", GLUT_BITMAP_TIMES_ROMAN_24);

    drawFilledRect(-0.18, 0.53, 0.18, 0.60, 0.1, 0.1, 0.1);
    glColor3f(1, 1, 1);
    drawText(-0.09, 0.56, "CARD SLOT", GLUT_BITMAP_HELVETICA_12);

    drawFilledRect(0.42, -0.82, 0.78, -0.15, 0.18, 0.18, 0.2);
    drawBorderRect(0.42, -0.82, 0.78, -0.15, 1, 1, 1);

    for (int i = 0; i < 4; i++) {
        drawFilledRect(-0.80, 0.40 - i * 0.20, -0.70, 0.47 - i * 0.20, 0.6, 0.6, 0.65);
        drawFilledRect(0.70, 0.40 - i * 0.20, 0.80, 0.47 - i * 0.20, 0.6, 0.6, 0.65);
    }

    float startX = 0.47, startY = -0.25;
    int num = 1;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            float x1 = startX + col * 0.10;
            float y1 = startY - row * 0.15;
            drawFilledRect(x1, y1, x1 + 0.08, y1 - 0.10, 0.85, 0.85, 0.88);
            glColor3f(0, 0, 0);
            drawText(x1 + 0.03, y1 - 0.06, to_string(num));
            num++;
        }
    }

    drawFilledRect(0.57, -0.70, 0.65, -0.80, 0.85, 0.85, 0.88);
    glColor3f(0, 0, 0);
    drawText(0.60, -0.76, "0");
}

// ---------------- SCREENS ----------------
void drawWelcomeScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.23, 0.25, "WELCOME TO ATM", GLUT_BITMAP_TIMES_ROMAN_24);
    drawText(-0.28, 0.05, "Interactive OpenGL ATM Simulation");
    drawText(-0.20, -0.15, "Press ENTER to Start");
    drawText(-0.15, -0.35, "Press ESC to Exit");
}

void drawInsertCardScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.18, 0.25, "INSERT YOUR CARD", GLUT_BITMAP_TIMES_ROMAN_24);

    drawFilledRect(-0.20, 0.00, 0.20, 0.15, 0.1, 0.3, 0.8);
    drawFilledRect(-0.16, 0.07, -0.03, 0.12, 0.9, 0.8, 0.2);
    glColor3f(1, 1, 1);
    drawText(-0.08, 0.03, "ATM CARD");

    glColor3f(0, 0, 0);
    drawText(-0.22, -0.18, "Press ENTER to Insert Card");
    drawText(-0.13, -0.35, "Backspace = Back");
}

void drawPINScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.14, 0.25, "ENTER YOUR PIN", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0, 0, 0);
    drawText(-0.20, 0.05, "Use keyboard number keys");

    string hiddenPIN = "";
    for (int i = 0; i < enteredPIN.length(); i++) hiddenPIN += "*";

    drawFilledRect(-0.22, -0.10, 0.22, 0.03, 1, 1, 1);
    drawBorderRect(-0.22, -0.10, 0.22, 0.03, 0, 0, 0);
    glColor3f(0, 0, 0);
    drawText(-0.03, -0.04, hiddenPIN);

    drawText(-0.24, -0.25, "Enter 4 digits and press ENTER");
    drawText(-0.18, -0.40, "Backspace = Delete");
}

void drawInvalidPINScreen() {
    glColor3f(0.6, 0, 0);
    drawText(-0.14, 0.20, "INVALID PIN!", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0, 0, 0);
    drawText(-0.20, 0.00, "Incorrect PIN entered");
    drawText(-0.28, -0.20, "Press ENTER to try again");
}

void drawMenuScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.12, 0.36, "ATM MAIN MENU", GLUT_BITMAP_TIMES_ROMAN_24);

    drawFilledRect(-0.35, 0.20, 0.35, 0.30, 0.2, 0.6, 0.2);
    drawFilledRect(-0.35, 0.06, 0.35, 0.16, 0.2, 0.4, 0.8);
    drawFilledRect(-0.35, -0.08, 0.35, 0.02, 0.8, 0.5, 0.2);
    drawFilledRect(-0.35, -0.22, 0.35, -0.12, 0.5, 0.2, 0.7);
    drawFilledRect(-0.35, -0.36, 0.35, -0.26, 0.8, 0.2, 0.2);

    glColor3f(1, 1, 1);
    drawText(-0.15, 0.23, "1. Balance Inquiry");
    drawText(-0.14, 0.09, "2. Withdraw Cash");
    drawText(-0.12, -0.05, "3. Deposit Cash");
    drawText(-0.13, -0.19, "4. Mini Statement");
    drawText(-0.06, -0.33, "5. Exit");

    glColor3f(0, 0, 0);
    drawText(-0.15, -0.48, "Press F for Fast Cash");
}

void drawBalanceScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.16, 0.25, "ACCOUNT BALANCE", GLUT_BITMAP_TIMES_ROMAN_24);

    stringstream ss;
    ss << "Available Balance: Rs. " << balance;

    glColor3f(0, 0, 0);
    drawText(-0.28, 0.02, ss.str());
    drawText(-0.24, -0.22, "Press BACKSPACE to return");
}

void drawWithdrawScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.13, 0.25, "WITHDRAW CASH", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0, 0, 0);
    drawText(-0.28, 0.07, "Enter amount using keyboard:");

    drawFilledRect(-0.22, -0.10, 0.22, 0.03, 1, 1, 1);
    drawBorderRect(-0.22, -0.10, 0.22, 0.03, 0, 0, 0);
    glColor3f(0, 0, 0);
    drawText(-0.10, -0.04, enteredAmount);

    drawText(-0.20, -0.25, "Press ENTER to confirm");
    drawText(-0.20, -0.40, "Backspace = Delete");
}

void drawDepositScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.12, 0.25, "DEPOSIT CASH", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0, 0, 0);
    drawText(-0.28, 0.07, "Enter amount using keyboard:");

    drawFilledRect(-0.22, -0.10, 0.22, 0.03, 1, 1, 1);
    drawBorderRect(-0.22, -0.10, 0.22, 0.03, 0, 0, 0);
    glColor3f(0, 0, 0);
    drawText(-0.10, -0.04, enteredAmount);

    drawText(-0.20, -0.25, "Press ENTER to confirm");
    drawText(-0.20, -0.40, "Backspace = Delete");
}

void drawFastCashScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.10, 0.30, "FAST CASH", GLUT_BITMAP_TIMES_ROMAN_24);

    drawFilledRect(-0.30, 0.10, 0.30, 0.20, 0.2, 0.6, 0.2);
    drawFilledRect(-0.30, -0.05, 0.30, 0.05, 0.2, 0.4, 0.8);
    drawFilledRect(-0.30, -0.20, 0.30, -0.10, 0.8, 0.5, 0.2);

    glColor3f(1, 1, 1);
    drawText(-0.07, 0.13, "1. Rs. 500");
    drawText(-0.08, -0.02, "2. Rs. 1000");
    drawText(-0.08, -0.17, "3. Rs. 2000");

    glColor3f(0, 0, 0);
    drawText(-0.20, -0.38, "Backspace = Return to Menu");
}

void drawMiniStatementScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.14, 0.28, "MINI STATEMENT", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0, 0, 0);
    if (miniStatement.empty()) {
        drawText(-0.14, 0.05, "No Transactions Yet");
    } else {
        float y = 0.12;
        for (int i = miniStatement.size() - 1; i >= 0; i--) {
            drawText(-0.35, y, miniStatement[i]);
            y -= 0.12;
        }
    }

    drawText(-0.24, -0.42, "Press BACKSPACE to return");
}

void drawReceiptScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.10, 0.25, "TRANSACTION", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0, 0, 0);
    drawText(-0.20, 0.02, message);
    drawText(-0.28, -0.20, "Press ENTER to return to menu");
}

void drawExitScreen() {
    glColor3f(0, 0, 0.3);
    drawText(-0.08, 0.20, "THANK YOU!", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0, 0, 0);
    drawText(-0.12, 0.00, "Visit Again");
    drawText(-0.25, -0.18, "Press R to Restart ATM");
    drawText(-0.22, -0.32, "Press ESC to close program");
}

// ---------------- DISPLAY ----------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawATMBody();

    switch (screen) {
        case 0: drawWelcomeScreen(); break;
        case 1: drawInsertCardScreen(); break;
        case 2: drawPINScreen(); break;
        case 3: drawMenuScreen(); break;
        case 4: drawBalanceScreen(); break;
        case 5: drawWithdrawScreen(); break;
        case 6: drawDepositScreen(); break;
        case 7: drawReceiptScreen(); break;
        case 8: drawExitScreen(); break;
        case 9: drawMiniStatementScreen(); break;
        case 10: drawFastCashScreen(); break;
        case 11: drawInvalidPINScreen(); break;
    }

    glFlush();
}

// ---------------- KEYBOARD ----------------
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC

    if (key == 'r' || key == 'R') {
        if (screen == 8) resetSession();
        glutPostRedisplay();
        return;
    }

    if (key == 'f' || key == 'F') {
        if (screen == 3) screen = 10;
        glutPostRedisplay();
        return;
    }

    if (key == 13) { // ENTER
        if (screen == 0) screen = 1;
        else if (screen == 1) screen = 2;
        else if (screen == 2 && enteredPIN.length() == 4) {
            if (enteredPIN == correctPIN) {
                screen = 3;
                enteredPIN = "";
            } else {
                screen = 11;
                enteredPIN = "";
            }
        }
        else if (screen == 11) screen = 2;
        else if (screen == 5 && !enteredAmount.empty()) {
            float amt = stof(enteredAmount);
            if (amt <= balance) {
                balance -= amt;
                message = "Withdrawal Successful!";
                addTransaction("Withdraw: Rs. " + enteredAmount);
            } else {
                message = "Insufficient Balance!";
            }
            enteredAmount = "";
            screen = 7;
        }
        else if (screen == 6 && !enteredAmount.empty()) {
            float amt = stof(enteredAmount);
            balance += amt;
            message = "Deposit Successful!";
            addTransaction("Deposit: Rs. " + enteredAmount);
            enteredAmount = "";
            screen = 7;
        }
        else if (screen == 7) screen = 3;

        glutPostRedisplay();
        return;
    }

    if (key == 8) { // BACKSPACE
        if (screen == 2 && !enteredPIN.empty()) enteredPIN.pop_back();
        else if ((screen == 5 || screen == 6) && !enteredAmount.empty()) enteredAmount.pop_back();
        else if (screen == 1) screen = 0;
        else if (screen == 2) screen = 1;
        else if (screen == 4 || screen == 5 || screen == 6 || screen == 9 || screen == 10) screen = 3;
        glutPostRedisplay();
        return;
    }

    if (screen == 2 && key >= '0' && key <= '9' && enteredPIN.length() < 4)
        enteredPIN += key;

    if (screen == 3) {
        if (key == '1') screen = 4;
        else if (key == '2') screen = 5;
        else if (key == '3') screen = 6;
        else if (key == '4') screen = 9;
        else if (key == '5') screen = 8;
    }

    if (screen == 10) {
        int fastAmt = 0;
        if (key == '1') fastAmt = 500;
        else if (key == '2') fastAmt = 1000;
        else if (key == '3') fastAmt = 2000;

        if (fastAmt > 0) {
            if (fastAmt <= balance) {
                balance -= fastAmt;
                message = "Fast Cash Successful!";
                addTransaction("Fast Cash: Rs. " + to_string(fastAmt));
            } else {
                message = "Insufficient Balance!";
            }
            screen = 7;
        }
    }

    if ((screen == 5 || screen == 6) && key >= '0' && key <= '9')
        enteredAmount += key;

    glutPostRedisplay();
}

// ---------------- MOUSE ----------------
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float mx = (float)x / windowWidth * 2.0 - 1.0;
        float my = 1.0 - (float)y / windowHeight * 2.0;

        if (screen == 3) {
            if (mx >= -0.35 && mx <= 0.35 && my >= 0.20 && my <= 0.30) screen = 4;
            else if (mx >= -0.35 && mx <= 0.35 && my >= 0.06 && my <= 0.16) screen = 5;
            else if (mx >= -0.35 && mx <= 0.35 && my >= -0.08 && my <= 0.02) screen = 6;
            else if (mx >= -0.35 && mx <= 0.35 && my >= -0.22 && my <= -0.12) screen = 9;
            else if (mx >= -0.35 && mx <= 0.35 && my >= -0.36 && my <= -0.26) screen = 8;
        }

        if (screen == 10) {
            int fastAmt = 0;
            if (mx >= -0.30 && mx <= 0.30 && my >= 0.10 && my <= 0.20) fastAmt = 500;
            else if (mx >= -0.30 && mx <= 0.30 && my >= -0.05 && my <= 0.05) fastAmt = 1000;
            else if (mx >= -0.30 && mx <= 0.30 && my >= -0.20 && my <= -0.10) fastAmt = 2000;

            if (fastAmt > 0) {
                if (fastAmt <= balance) {
                    balance -= fastAmt;
                    message = "Fast Cash Successful!";
                    addTransaction("Fast Cash: Rs. " + to_string(fastAmt));
                } else {
                    message = "Insufficient Balance!";
                }
                screen = 7;
            }
        }

        glutPostRedisplay();
    }
}

// ---------------- INIT ----------------
void init() {
    glClearColor(0.75, 0.82, 0.95, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-1, 1, -1, 1);
}

// ---------------- MAIN ----------------
int main(int argc, char** argv) {
    miniStatement.push_back("Opening Balance: Rs. 5000");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("ATM Workflow and User Interface - OpenGL Simulation");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMainLoop();

    return 0;
}