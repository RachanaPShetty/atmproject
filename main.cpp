#include <GL/glut.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cmath>
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==================== GLOBAL STATE ====================
int screen = 0;
/*
 0=Welcome  1=InsertCard  2=PIN  3=Menu  4=Balance
 5=Withdraw 6=Deposit     7=Receipt 8=Exit 9=MiniStmt
 10=FastCash 11=InvalidPIN
*/
string enteredPIN    = "";
string enteredAmount = "";
float  balance       = 5000.0f;
string message       = "";
int    windowWidth   = 900, windowHeight = 700;
const  string correctPIN = "1234";
vector<string> miniStatement;
bool blinkOn = true;

// ==================== TIMER ====================
void timerBlink(int) {
    blinkOn = !blinkOn;
    glutPostRedisplay();
    glutTimerFunc(500, timerBlink, 0);
}

// ==================== DRAW UTILS ====================

void drawFilledRect(float x1,float y1,float x2,float y2,float r,float g,float b){
    glColor3f(r,g,b);
    glBegin(GL_POLYGON);
    glVertex2f(x1,y1); glVertex2f(x2,y1);
    glVertex2f(x2,y2); glVertex2f(x1,y2);
    glEnd();
}

void drawGradientRect(float x1,float y1,float x2,float y2,
                      float r1,float g1,float b1,
                      float r2,float g2,float b2){
    glBegin(GL_QUADS);
    glColor3f(r1,g1,b1); glVertex2f(x1,y2); glVertex2f(x2,y2);
    glColor3f(r2,g2,b2); glVertex2f(x2,y1); glVertex2f(x1,y1);
    glEnd();
}

void drawRoundedRect(float x1,float y1,float x2,float y2,float rad,
                     float r,float g,float b){
    glColor3f(r,g,b);
    int segs = 10;
    glBegin(GL_POLYGON);
    for(int i=0;i<=segs;i++){
        float a = (float)M_PI + (float)i/segs*(float)(M_PI/2);
        glVertex2f(x1+rad+rad*cos(a), y1+rad+rad*sin(a));
    }
    for(int i=0;i<=segs;i++){
        float a = (float)(3*M_PI/2) + (float)i/segs*(float)(M_PI/2);
        glVertex2f(x2-rad+rad*cos(a), y1+rad+rad*sin(a));
    }
    for(int i=0;i<=segs;i++){
        float a = (float)i/segs*(float)(M_PI/2);
        glVertex2f(x2-rad+rad*cos(a), y2-rad+rad*sin(a));
    }
    for(int i=0;i<=segs;i++){
        float a = (float)(M_PI/2) + (float)i/segs*(float)(M_PI/2);
        glVertex2f(x1+rad+rad*cos(a), y2-rad+rad*sin(a));
    }
    glEnd();
}

void drawRoundedBorder(float x1,float y1,float x2,float y2,float rad,
                       float r,float g,float b){
    glColor3f(r,g,b);
    int segs=10;
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<=segs;i++){float a=(float)M_PI+(float)i/segs*(float)(M_PI/2);glVertex2f(x1+rad+rad*cos(a),y1+rad+rad*sin(a));}
    for(int i=0;i<=segs;i++){float a=(float)(3*M_PI/2)+(float)i/segs*(float)(M_PI/2);glVertex2f(x2-rad+rad*cos(a),y1+rad+rad*sin(a));}
    for(int i=0;i<=segs;i++){float a=(float)i/segs*(float)(M_PI/2);glVertex2f(x2-rad+rad*cos(a),y2-rad+rad*sin(a));}
    for(int i=0;i<=segs;i++){float a=(float)(M_PI/2)+(float)i/segs*(float)(M_PI/2);glVertex2f(x1+rad+rad*cos(a),y2-rad+rad*sin(a));}
    glEnd();
}

void drawCircle(float cx,float cy,float rad,float r,float g,float b){
    glColor3f(r,g,b);
    glBegin(GL_POLYGON);
    for(int i=0;i<48;i++){
        float a=2*M_PI*i/48;
        glVertex2f(cx+rad*cos(a), cy+rad*sin(a));
    }
    glEnd();
}

void drawCircleBorder(float cx,float cy,float rad,float r,float g,float b){
    glColor3f(r,g,b);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<48;i++){
        float a=2*M_PI*i/48;
        glVertex2f(cx+rad*cos(a), cy+rad*sin(a));
    }
    glEnd();
}

void drawLine(float x1,float y1,float x2,float y2,float r,float g,float b){
    glColor3f(r,g,b);
    glBegin(GL_LINES);
    glVertex2f(x1,y1); glVertex2f(x2,y2);
    glEnd();
}

void drawText(float x,float y,string s,void* font=GLUT_BITMAP_HELVETICA_18){
    glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(font,c);
}

// ==================== HELPERS ====================
void addTransaction(string t){
    miniStatement.push_back(t);
    if(miniStatement.size()>5) miniStatement.erase(miniStatement.begin());
}

void resetSession(){
    enteredPIN=""; enteredAmount=""; message=""; screen=0;
}

// ==================== SCREEN WIDGETS ====================

// Cyan-bordered header bar with title
void drawScreenHeader(string title){
    // Full screen glow border
    drawRoundedBorder(-0.56,-0.20,0.56,0.69,0.03, 0.0,0.60,0.78);
    // Header gradient strip
    drawGradientRect(-0.56,0.57,0.56,0.69, 0.0,0.30,0.42, 0.0,0.18,0.28);
    drawLine(-0.50,0.57, 0.50,0.57, 0.0,0.65,0.82);
    // Title centered (approx)
    float cw = 0.029f;
    float tw = title.length()*cw;
    glColor3f(0.0,0.90,1.0);
    drawText(-tw/2, 0.61, title, GLUT_BITMAP_TIMES_ROMAN_24);
    // Decorative dots
    drawCircle(-0.48,0.63,0.010, 1.0,0.78,0.15);
    drawCircle( 0.48,0.63,0.010, 1.0,0.78,0.15);
}

// Status/hint bar at bottom of screen
void drawStatusBar(string hint){
    drawGradientRect(-0.56,-0.20,0.56,-0.11, 0.04,0.06,0.12, 0.06,0.08,0.15);
    drawLine(-0.50,-0.11, 0.50,-0.11, 0.0,0.35,0.48);
    float cw=0.012f;
    float tw=hint.length()*cw;
    glColor3f(0.38,0.52,0.65);
    drawText(-tw/2,-0.175, hint, GLUT_BITMAP_HELVETICA_12);
}

// Input field — password shows filled/empty circles, text shows typed chars
void drawInputField(float x1,float y1,float x2,float y2,
                    string content, bool isPassword=false){
    drawRoundedRect(x1,y1,x2,y2,0.025, 0.04,0.06,0.12);
    drawRoundedBorder(x1,y1,x2,y2,0.025, 0.0,0.72,0.90);
    float cy=(y1+y2)/2;
    if(isPassword){
        // 4 dot positions centered
        float startX = (x1+x2)/2 - 0.075f;
        for(int i=0;i<4;i++){
            float cx = startX + i*0.050f;
            if(i<(int)content.length())
                drawCircle(cx,cy,0.020, 0.0,0.85,0.95);
            else{
                drawCircleBorder(cx,cy,0.020, 0.22,0.30,0.42);
                drawCircle(cx,cy,0.005, 0.15,0.22,0.32);
            }
        }
    } else {
        glColor3f(0.0,0.90,1.0);
        drawText(x1+0.06, cy-0.015, content);
        // blinking cursor
        if(blinkOn){
            float curX = x1+0.06 + content.length()*0.021f;
            drawLine(curX, y1+0.025, curX, y2-0.025, 0.0,0.90,1.0);
        }
    }
}

// Single menu row tile
void drawMenuTile(int num,string label,float y,
                  float r,float g,float b){
    float alpha=0.28f;
    drawRoundedRect(-0.46,y-0.046,0.46,y+0.046,0.022, r*alpha,g*alpha,b*alpha);
    drawRoundedBorder(-0.46,y-0.046,0.46,y+0.046,0.022, r,g,b);
    // Number badge circle
    drawCircle(-0.37,y,0.032, r*0.6f,g*0.6f,b*0.6f);
    drawCircleBorder(-0.37,y,0.032, r,g,b);
    glColor3f(0.05,0.05,0.08);
    string ns=to_string(num);
    drawText(-0.382,y-0.012, ns, GLUT_BITMAP_HELVETICA_12);
    // Label
    glColor3f(0.86,0.92,1.0);
    drawText(-0.28,y-0.015, label);
    // Arrow
    glColor3f(r,g,b);
    drawText(0.36,y-0.015, ">>");
}

// ==================== ATM BODY ====================
void drawATMBody(){
    // Outer body gradient (top lighter → bottom darker)
    drawGradientRect(-0.93,-0.97,0.93,0.97,
        0.16,0.18,0.24, 0.10,0.11,0.16);

    // Inner panel
    drawRoundedRect(-0.87,-0.92,0.87,0.93,0.05, 0.12,0.13,0.19);

    // ---- BANK HEADER ----
    drawGradientRect(-0.87,0.79,0.87,0.93,
        0.0,0.42,0.58, 0.0,0.25,0.38);
    drawRoundedBorder(-0.87,0.79,0.87,0.93,0.04, 0.0,0.60,0.80);
    glColor3f(1,1,1);
    drawText(-0.185,0.838, "NOVA BANK ATM", GLUT_BITMAP_TIMES_ROMAN_24);
    // Gold accent dots
    drawCircle(-0.72,0.860,0.013, 1.0,0.78,0.15);
    drawCircle(-0.68,0.860,0.013, 1.0,0.78,0.15);
    drawCircle( 0.68,0.860,0.013, 0.0,0.85,0.95);
    drawCircle( 0.72,0.860,0.013, 0.0,0.85,0.95);

    // ---- CARD SLOT ----
    drawRoundedRect(-0.26,0.71,0.26,0.78,0.02, 0.07,0.08,0.13);
    drawRoundedBorder(-0.26,0.71,0.26,0.78,0.02, 0.0,0.65,0.82);
    glColor3f(0.45,0.55,0.65);
    drawText(-0.095,0.733, "CARD SLOT", GLUT_BITMAP_HELVETICA_12);
    // Blinking LED
    if(blinkOn) drawCircle( 0.20,0.745,0.009, 0.0,0.90,0.50);
    else         drawCircle( 0.20,0.745,0.009, 0.0,0.30,0.20);
    drawCircleBorder(0.20,0.745,0.009, 0.0,0.60,0.40);

    // ---- SCREEN GLOW (3 layers) ----
    drawRoundedRect(-0.60,-0.24,0.60,0.71,0.04, 0.0,0.28,0.38);
    drawRoundedRect(-0.58,-0.22,0.58,0.70,0.04, 0.0,0.48,0.65);
    // Screen face (very dark)
    drawRoundedRect(-0.56,-0.20,0.56,0.69,0.03, 0.03,0.05,0.10);

    // ---- SIDE FUNCTION BUTTONS (4 per side) ----
    float btnY[4]={0.54,0.34,0.14,-0.06};
    for(int i=0;i<4;i++){
        // Left
        drawRoundedRect(-0.82,btnY[i]-0.052,-0.63,btnY[i]+0.052,0.016,
                        0.18,0.20,0.28);
        drawRoundedBorder(-0.82,btnY[i]-0.052,-0.63,btnY[i]+0.052,0.016,
                          0.28,0.32,0.44);
        // highlight top edge
        drawLine(-0.80,btnY[i]+0.040,-0.65,btnY[i]+0.040, 0.35,0.38,0.50);
        // connector line
        drawLine(-0.63,btnY[i],-0.56,btnY[i], 0.0,0.52,0.65);

        // Right
        drawRoundedRect(0.63,btnY[i]-0.052,0.82,btnY[i]+0.052,0.016,
                        0.18,0.20,0.28);
        drawRoundedBorder(0.63,btnY[i]-0.052,0.82,btnY[i]+0.052,0.016,
                          0.28,0.32,0.44);
        drawLine(0.65,btnY[i]+0.040,0.80,btnY[i]+0.040, 0.35,0.38,0.50);
        drawLine(0.56,btnY[i],0.63,btnY[i], 0.0,0.52,0.65);
    }

    // ---- CASH DISPENSER SLOT ----
    drawRoundedRect(-0.42,-0.50,0.42,-0.38,0.015, 0.07,0.08,0.12);
    drawRoundedBorder(-0.42,-0.50,0.42,-0.38,0.015, 0.0,0.52,0.65);
    glColor3f(0.38,0.48,0.58);
    drawText(-0.115,-0.460, "CASH DISPENSER", GLUT_BITMAP_HELVETICA_12);

    // ---- CORNER SCREWS ----
    float sx[]={-0.84, 0.84,-0.84, 0.84};
    float sy[]={ 0.89, 0.89,-0.89,-0.89};
    for(int i=0;i<4;i++){
        drawCircle(sx[i],sy[i],0.020, 0.18,0.20,0.28);
        drawCircleBorder(sx[i],sy[i],0.020, 0.28,0.32,0.42);
        drawLine(sx[i]-0.011f,sy[i],sx[i]+0.011f,sy[i], 0.12,0.14,0.20);
        drawLine(sx[i],sy[i]-0.011f,sx[i],sy[i]+0.011f, 0.12,0.14,0.20);
    }
}

// ==================== SCREEN CONTENTS ====================

void drawWelcomeScreen(){
    // Bank logo circle
    drawCircle(0,0.35,0.110, 0.0,0.22,0.32);
    drawCircleBorder(0,0.35,0.110, 0.0,0.70,0.88);
    glColor3f(1.0,0.78,0.15);
    drawText(-0.025,0.327, "N", GLUT_BITMAP_TIMES_ROMAN_24);

    // Bank name
    glColor3f(0.0,0.88,1.0);
    drawText(-0.128,0.165, "NOVA BANK", GLUT_BITMAP_TIMES_ROMAN_24);

    // Tagline
    glColor3f(0.45,0.58,0.72);
    drawText(-0.198,0.068, "Your Trusted Banking Partner");

    // Decorative separator
    drawLine(-0.32,0.020, 0.32,0.020, 0.0,0.48,0.62);
    drawCircle(0,0.020,0.008, 0.0,0.65,0.82);

    // Blinking call-to-action
    if(blinkOn){
        glColor3f(0.82,0.88,1.0);
        drawText(-0.188,-0.075, "Press ENTER to Begin");
    }

    drawStatusBar("ENTER = Start  |  ESC = Exit");
}

void drawInsertCardScreen(){
    drawScreenHeader("INSERT YOUR CARD");

    // Card graphic
    drawRoundedRect(-0.30,0.04,0.30,0.44,0.03, 0.06,0.18,0.50);
    drawRoundedBorder(-0.30,0.04,0.30,0.44,0.03, 0.0,0.65,0.90);
    // Chip
    drawRoundedRect(-0.20,0.26,-0.05,0.38,0.015, 0.80,0.70,0.18);
    drawRoundedBorder(-0.20,0.26,-0.05,0.38,0.015, 1.0,0.85,0.30);
    // Magnetic stripe
    drawFilledRect(-0.28,0.16, 0.28,0.22, 0.10,0.10,0.16);
    // Card number
    glColor3f(0.65,0.72,0.88);
    drawText(-0.24,0.082, "****  ****  ****  1234", GLUT_BITMAP_HELVETICA_12);
    // Bank label
    glColor3f(0.88,0.85,0.55);
    drawText(-0.22,0.40, "NOVA CARD", GLUT_BITMAP_HELVETICA_12);

    // Animated arrow
    if(blinkOn){
        glColor3f(0.0,0.88,1.0);
        drawText(0.32,0.22, ">>", GLUT_BITMAP_HELVETICA_18);
    }

    glColor3f(0.60,0.70,0.82);
    drawText(-0.232,-0.048, "Press ENTER to continue");

    drawStatusBar("BACKSPACE = Back  |  ENTER = Insert Card");
}

void drawPINScreen(){
    drawScreenHeader("ENTER YOUR PIN");

    // Lock icon
    drawCircle(0,0.43,0.065, 0.0,0.22,0.32);
    drawCircleBorder(0,0.43,0.065, 0.0,0.68,0.86);
    drawRoundedRect(-0.038,0.30,0.038,0.43,0.012, 0.0,0.50,0.68);
    drawCircle(0,0.30,0.015, 0.0,0.68,0.86);

    glColor3f(0.58,0.68,0.82);
    drawText(-0.25,0.215, "Enter your 4-digit PIN:");

    // PIN dots input field
    drawInputField(-0.32,0.03,0.32,0.16, enteredPIN, true);

    glColor3f(0.35,0.45,0.58);
    drawText(-0.22,-0.062, "Use keyboard number keys 0-9",
             GLUT_BITMAP_HELVETICA_12);

    drawStatusBar("Numbers = Type PIN  |  BACKSPACE = Delete  |  ENTER = OK");
}

void drawInvalidPINScreen(){
    // Error circle
    drawCircle(0,0.35,0.105, 0.42,0.05,0.05);
    drawCircleBorder(0,0.35,0.105, 0.82,0.12,0.12);
    glColor3f(1.0,0.35,0.35);
    drawText(-0.018,0.318, "!", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0.95,0.22,0.22);
    drawText(-0.162,0.155, "INVALID PIN!", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0.60,0.68,0.80);
    drawText(-0.240,0.020, "The PIN you entered is incorrect.");
    drawText(-0.202,-0.068, "Please check and try again.");

    // Retry button
    drawRoundedRect(-0.20,-0.220,0.20,-0.108,0.02, 0.0,0.28,0.38);
    drawRoundedBorder(-0.20,-0.220,0.20,-0.108,0.02, 0.0,0.68,0.85);
    glColor3f(0.0,0.88,1.0);
    drawText(-0.125,-0.178, "ENTER to Retry");

    drawStatusBar("Press ENTER to try again");
}

void drawMenuScreen(){
    drawScreenHeader("MAIN MENU");

    float  my[]={0.490f,0.370f,0.250f,0.130f,0.010f};
    float  mr[]={0.0, 0.0, 0.85,0.55,0.85};
    float  mg[]={0.78,0.55,0.55,0.22,0.22};
    float  mb[]={0.42,0.90,0.22,0.88,0.28};
    string ml[]={"Balance Inquiry","Withdraw Cash","Deposit Cash",
                 "Mini Statement","Exit"};

    for(int i=0;i<5;i++)
        drawMenuTile(i+1, ml[i], my[i], mr[i],mg[i],mb[i]);

    drawStatusBar("1-5 = Select  |  F = Fast Cash  |  CLICK options");
}

void drawBalanceScreen(){
    drawScreenHeader("BALANCE INQUIRY");

    // Balance card
    drawRoundedRect(-0.44,0.08,0.44,0.47,0.03, 0.0,0.16,0.26);
    drawRoundedBorder(-0.44,0.08,0.44,0.47,0.03, 0.0,0.65,0.85);

    glColor3f(0.45,0.58,0.72);
    drawText(-0.195,0.40, "Available Balance");
    drawLine(-0.38,0.358, 0.38,0.358, 0.0,0.35,0.50);

    stringstream ss;
    ss << "Rs. " << balance;
    glColor3f(0.0,0.90,1.0);
    drawText(-0.170,0.225, ss.str(), GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0.35,0.45,0.58);
    drawText(-0.145,0.108, "Savings Account", GLUT_BITMAP_HELVETICA_12);

    drawStatusBar("BACKSPACE = Return to Menu");
}

void drawWithdrawScreen(){
    drawScreenHeader("WITHDRAW CASH");

    glColor3f(0.52,0.64,0.80);
    drawText(-0.268,0.415, "Enter withdrawal amount:");

    drawInputField(-0.36,0.12,0.36,0.32, "Rs. "+enteredAmount);

    // Quick-amount buttons
    glColor3f(0.35,0.45,0.58);
    drawText(-0.400,0.065, "Quick:", GLUT_BITMAP_HELVETICA_12);
    const char* qa[]={"100","500","1000"};
    float qx[]={-0.38,-0.13,0.12};
    for(int i=0;i<3;i++){
        drawRoundedRect(qx[i],-0.060,qx[i]+0.225,0.038,0.014,
                        0.08,0.22,0.32);
        drawRoundedBorder(qx[i],-0.060,qx[i]+0.225,0.038,0.014,
                          0.0,0.52,0.68);
        glColor3f(0.55,0.72,0.85);
        string qs="Rs."; qs+=qa[i];
        drawText(qx[i]+0.030,-0.025, qs, GLUT_BITMAP_HELVETICA_12);
    }

    stringstream ss; ss<<"Balance: Rs. "<<balance;
    glColor3f(0.32,0.42,0.55);
    drawText(-0.240,-0.135, ss.str(), GLUT_BITMAP_HELVETICA_12);

    drawStatusBar("Numbers = Amount  |  ENTER = Confirm  |  BACK = Delete");
}

void drawDepositScreen(){
    drawScreenHeader("DEPOSIT CASH");

    glColor3f(0.52,0.64,0.80);
    drawText(-0.250,0.415, "Enter deposit amount:");

    drawInputField(-0.36,0.12,0.36,0.32, "Rs. "+enteredAmount);

    glColor3f(0.38,0.50,0.62);
    drawText(-0.295,0.060, "Insert cash in the dispenser slot");

    // Secure indicator
    drawRoundedRect(-0.140,-0.108,0.140,-0.018,0.018,
                    0.04,0.28,0.14);
    drawRoundedBorder(-0.140,-0.108,0.140,-0.018,0.018,
                      0.10,0.62,0.28);
    glColor3f(0.38,0.88,0.55);
    drawText(-0.082,-0.074, "SECURE", GLUT_BITMAP_HELVETICA_12);

    drawStatusBar("Numbers = Amount  |  ENTER = Confirm  |  BACK = Delete");
}

void drawFastCashScreen(){
    drawScreenHeader("FAST CASH");

    glColor3f(0.52,0.64,0.80);
    drawText(-0.268,0.455, "Select amount to withdraw:");

    struct Opt{ int amt; float r,g,b; float y; };
    Opt opts[]={
        {500,  0.0,0.78,0.42, 0.30},
        {1000, 0.0,0.55,0.92, 0.13},
        {2000, 0.88,0.55,0.18,-0.04}
    };
    for(int i=0;i<3;i++){
        float r=opts[i].r, g=opts[i].g, b=opts[i].b;
        drawRoundedRect(-0.42,opts[i].y-0.068,0.42,opts[i].y+0.068,0.026,
                        r*0.22f,g*0.22f,b*0.22f);
        drawRoundedBorder(-0.42,opts[i].y-0.068,0.42,opts[i].y+0.068,0.026,
                          r,g,b);
        drawCircle(-0.33,opts[i].y,0.034, r*0.55f,g*0.55f,b*0.55f);
        drawCircleBorder(-0.33,opts[i].y,0.034, r,g,b);
        glColor3f(0.05,0.05,0.08);
        drawText(-0.345,opts[i].y-0.014, to_string(i+1), GLUT_BITMAP_HELVETICA_12);
        glColor3f(0.88,0.94,1.0);
        drawText(-0.18,opts[i].y-0.015, "Rs. "+to_string(opts[i].amt));
        glColor3f(r*0.7f+0.3f, g*0.7f+0.3f, b*0.7f+0.3f);
        drawText(0.30,opts[i].y-0.015, ">>");
    }

    drawStatusBar("Press 1, 2, 3  |  Click Option  |  BACKSPACE = Back");
}

void drawMiniStatementScreen(){
    drawScreenHeader("MINI STATEMENT");

    if(miniStatement.empty()){
        drawCircleBorder(0,0.22,0.06, 0.28,0.38,0.52);
        glColor3f(0.38,0.48,0.60);
        drawText(-0.142,0.040, "No transactions yet");
    } else {
        glColor3f(0.32,0.42,0.55);
        drawText(-0.460,0.455, "RECENT TRANSACTIONS", GLUT_BITMAP_HELVETICA_12);
        drawLine(-0.50,0.425, 0.50,0.425, 0.12,0.20,0.30);

        float y=0.37f;
        for(int i=(int)miniStatement.size()-1;i>=0;i--){
            float bg=(i%2==0)?0.060f:0.082f;
            drawFilledRect(-0.50,y-0.042f,0.50,y+0.042f,
                           bg,bg*1.08f,bg*1.55f);
            // Color code
            bool isW = miniStatement[i].find("Withdraw")!=string::npos
                    || miniStatement[i].find("Fast Cash")!=string::npos;
            bool isO = miniStatement[i].find("Opening")!=string::npos;
            if(isO)       glColor3f(0.55,0.65,0.78);
            else if(isW)  glColor3f(0.95,0.42,0.42);
            else          glColor3f(0.35,0.90,0.55);
            drawText(-0.460,y-0.015f, miniStatement[i], GLUT_BITMAP_HELVETICA_12);
            y-=0.100f;
        }
    }
    drawStatusBar("BACKSPACE = Return to Menu");
}

void drawReceiptScreen(){
    drawScreenHeader("TRANSACTION COMPLETE");

    bool ok = message.find("Successful")!=string::npos;
    float ir=ok?0.0f:0.60f, ig=ok?0.82f:0.08f, ib=ok?0.42f:0.08f;

    drawCircle(0,0.365,0.108, ir*0.28f,ig*0.28f,ib*0.28f);
    drawCircleBorder(0,0.365,0.108, ir,ig,ib);

    // Checkmark or X symbol
    glColor3f(ir*0.5f+0.5f, ig*0.5f+0.5f, ib*0.5f+0.5f);
    drawText(-0.022f,0.337f, ok?"v":"x", GLUT_BITMAP_TIMES_ROMAN_24);

    // Message
    glColor3f(0.88,0.94,1.0);
    float tw=message.length()*0.025f;
    drawText(-tw/2, 0.140f, message, GLUT_BITMAP_TIMES_ROMAN_24);

    // Updated balance
    stringstream ss; ss<<"Current Balance:  Rs. "<<balance;
    glColor3f(0.40,0.52,0.65);
    drawText(-0.280,0.000f, ss.str(), GLUT_BITMAP_HELVETICA_12);

    drawLine(-0.40,-0.042f,0.40,-0.042f, 0.12,0.20,0.30);

    glColor3f(0.38,0.48,0.60);
    drawText(-0.215,-0.112f, "Thank you for banking");
    drawText(-0.182,-0.175f, "with Nova Bank!");

    drawStatusBar("ENTER = Return to Menu");
}

void drawExitScreen(){
    drawCircle(0,0.30,0.12, 0.0,0.20,0.30);
    drawCircleBorder(0,0.30,0.12, 0.0,0.65,0.82);
    glColor3f(1.0,0.78,0.15);
    drawText(-0.025f,0.270f, "N", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0.0,0.88,1.0);
    drawText(-0.148f,0.080f, "THANK YOU!", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0.52,0.62,0.76);
    drawText(-0.215f,-0.032f, "Please collect your card");
    drawText(-0.188f,-0.108f, "and receipt. Visit again!");

    drawLine(-0.30f,-0.152f,0.30f,-0.152f, 0.12,0.22,0.32);

    drawRoundedRect(-0.225,-0.268f,0.225,-0.168f,0.022, 0.0,0.25,0.35);
    drawRoundedBorder(-0.225,-0.268f,0.225,-0.168f,0.022, 0.0,0.62,0.80);
    glColor3f(0.0,0.88,1.0);
    drawText(-0.128f,-0.232f, "R = Restart ATM");

    drawStatusBar("R = Restart  |  ESC = Close Program");
}

// ==================== DISPLAY ====================
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);

    drawATMBody();

    switch(screen){
        case 0:  drawWelcomeScreen();    break;
        case 1:  drawInsertCardScreen(); break;
        case 2:  drawPINScreen();        break;
        case 3:  drawMenuScreen();       break;
        case 4:  drawBalanceScreen();    break;
        case 5:  drawWithdrawScreen();   break;
        case 6:  drawDepositScreen();    break;
        case 7:  drawReceiptScreen();    break;
        case 8:  drawExitScreen();       break;
        case 9:  drawMiniStatementScreen(); break;
        case 10: drawFastCashScreen();   break;
        case 11: drawInvalidPINScreen(); break;
    }

    glutSwapBuffers();
}

// ==================== KEYBOARD ====================
void keyboard(unsigned char key,int x,int y){
    if(key==27) exit(0);

    if(key=='r'||key=='R'){ if(screen==8) resetSession(); glutPostRedisplay(); return; }
    if(key=='f'||key=='F'){ if(screen==3) screen=10;     glutPostRedisplay(); return; }

    if(key==13){ // ENTER
        if(screen==0) screen=1;
        else if(screen==1) screen=2;
        else if(screen==2 && enteredPIN.length()==4){
            if(enteredPIN==correctPIN){screen=3;enteredPIN="";}
            else{screen=11;enteredPIN="";}
        }
        else if(screen==11) screen=2;
        else if(screen==5 && !enteredAmount.empty()){
            float amt=stof(enteredAmount);
            if(amt<=balance){balance-=amt;message="Withdrawal Successful!";addTransaction("Withdraw: Rs."+enteredAmount);}
            else message="Insufficient Balance!";
            enteredAmount=""; screen=7;
        }
        else if(screen==6 && !enteredAmount.empty()){
            float amt=stof(enteredAmount);
            balance+=amt;message="Deposit Successful!";
            addTransaction("Deposit: Rs."+enteredAmount);
            enteredAmount=""; screen=7;
        }
        else if(screen==7) screen=3;
        glutPostRedisplay(); return;
    }

    if(key==8){ // BACKSPACE
        if(screen==2  && !enteredPIN.empty())    enteredPIN.pop_back();
        else if((screen==5||screen==6) && !enteredAmount.empty()) enteredAmount.pop_back();
        else if(screen==1) screen=0;
        else if(screen==2) screen=1;
        else if(screen==4||screen==5||screen==6||screen==9||screen==10) screen=3;
        glutPostRedisplay(); return;
    }

    if(screen==2 && key>='0' && key<='9' && enteredPIN.length()<4) enteredPIN+=key;

    if(screen==3){
        if(key=='1') screen=4;
        else if(key=='2') screen=5;
        else if(key=='3') screen=6;
        else if(key=='4') screen=9;
        else if(key=='5') screen=8;
    }

    if(screen==10){
        int fa=0;
        if(key=='1') fa=500; else if(key=='2') fa=1000; else if(key=='3') fa=2000;
        if(fa>0){
            if(fa<=balance){balance-=fa;message="Fast Cash Successful!";addTransaction("Fast Cash: Rs."+to_string(fa));}
            else message="Insufficient Balance!";
            screen=7;
        }
    }

    if((screen==5||screen==6) && key>='0' && key<='9') enteredAmount+=key;

    glutPostRedisplay();
}

// ==================== MOUSE ====================
void mouse(int button,int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        float mx=(float)x/windowWidth*2.0f-1.0f;
        float my=1.0f-(float)y/windowHeight*2.0f;

        if(screen==3){
            float my_[]={0.490f,0.370f,0.250f,0.130f,0.010f};
            for(int i=0;i<5;i++){
                if(mx>=-0.46f&&mx<=0.46f&&my>=my_[i]-0.046f&&my<=my_[i]+0.046f){
                    int ns[]={4,5,6,9,8};
                    screen=ns[i];
                }
            }
        }

        if(screen==10){
            float oy[]={0.30f,0.13f,-0.04f};
            int   oa[]={500,1000,2000};
            for(int i=0;i<3;i++){
                if(mx>=-0.42f&&mx<=0.42f&&my>=oy[i]-0.068f&&my<=oy[i]+0.068f){
                    if(oa[i]<=balance){balance-=oa[i];message="Fast Cash Successful!";addTransaction("Fast Cash: Rs."+to_string(oa[i]));}
                    else message="Insufficient Balance!";
                    screen=7;
                }
            }
        }

        glutPostRedisplay();
    }
}

// ==================== INIT ====================
void init(){
    glClearColor(0.08,0.09,0.14,1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-1,1,-1,1);
}

// ==================== MAIN ====================
int main(int argc,char** argv){
    miniStatement.push_back("Opening Balance: Rs. 5000");

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth,windowHeight);
    glutInitWindowPosition(100,50);
    glutCreateWindow("Nova Bank ATM  |  OpenGL Simulation");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(500,timerBlink,0);
    glutMainLoop();
    return 0;
}
