
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include "ReadObj.h"

#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <random>
#include <time.h>
#include <windows.h>
#include <vector>
#include <gl/glew.h> 
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>

#define WIDTH 800
#define HEIGHT 600

#define MAXFPS 60

#define MAP_1 "C:\Users\sumin\Desktop\2020\컴퓨터 그래픽스\termproj_git\[컴퓨터 그래픽스] 텀 프로젝트\[컴퓨터 그래픽스] 텀 프로젝트\MAP_Veryeasy.txt"
#define MAP_2 "MAP_Easy.txt"
#define MAP_3 "MAP_Normal.txt"
#define MAP_4 "MAP_Hard.txt"
#define MAP_5 "MAP_Veryhard.txt"
#define INIT "MAP_Veryeasy.txt"

#define SIZE 22 // 맵 사이즈
using namespace std;

// 기본 함수 선언
char* filetobuf(char*);
void make_vertexShader();
void make_fragmentShader();
GLuint make_shaderProgram();
void InitBuffer();
void InitShader();
void InitTexture();
GLubyte* LoadDIBitmap(const char*, BITMAPINFO**);
GLvoid Reshape(int, int);
void drawScene();
void TimerFunction(int);
void Keyboard(unsigned char, int, int);
void Mouse(int, int, int, int);
void CalculateLight();

// 함수 선언
void throw_bomb();
int Loadfile();
void remain();
void board_maker();
void renderBitmapCharacter(string);
void get_time();

GLUquadricObj* qobj;

GLuint shaderID;
GLuint s_program[3];
GLuint s_LineProgram;
GLchar* vertexsource, * fragmentsource;
GLuint vertexshader, lineVertexShader, fragmentshader;

GLuint VAO[10], VBO[10];
GLuint VAOCube[7], VBOCube[7];

///////////////////////////////////////////////////////////////
// 맵 파일 입출력
GLubyte* LoadDIBitmap(const char* filename, BITMAPINFO** info);
GLubyte* pBytes; // 데이터를 가리킬 포인터
BITMAPINFO* info; // 비트맵 헤더 저장할 변수
GLuint textures[1];
void initTextures();

int mapcollect = 0;
int makeboard[SIZE][SIZE] = { 0 };
float colorbuffer[4][3] = { 0 };

unsigned int texture;

struct Shape {
    int index;
    float r;
    float g;
    float b;
    float scaleX;
    float scaleY;
    float scaleZ;
};

float objectSize = 1;
bool isChanged = false;

float r;
float g;
float b;
Shape shapeCrain[7];

bool isMoveX = false;
int directionX = 0;

bool isMoveZ = false;
int directionZ = 0;
float allAngle = 0.0f;

bool isSwing = true;
float rightAngle = 0.0f;
int rightAngleDirection = 1;

float leftAngle = 0.f;
int leftAngleDirection = 1;

float rootX = 0;
float rootInitY = -8;
float rootY = rootInitY;
float rootZ = 0;
float speed = 0.1f;

float camera_x = 0.0f;
float camera_y = 10.0f;
float camera_z = 40.0f;

float light_x = -15.0;
float light_y = 15.0;
float light_z = 0.0;

float light_r = 1.0;
float light_g = 1.0;
float light_b = 1.0;

random_device rd;
default_random_engine dre{ rd() };
uniform_real_distribution<> random_pos_urd{ -15.0, 15.0 };

float random_xpos = random_pos_urd(dre);
float random_zpos = random_pos_urd(dre);
float random_move_xpos = random_pos_urd(dre);
float random_move_zpos = random_pos_urd(dre);

float robot_xpos[4] = { 0, };
float robot_ypos[4] = { 0.0f };
float robot_zpos[4] = { 0, };

float player_xpos = 0.0f;
float player_ypos = 0.0f;
float player_zpos = -20.0f;

bool robot_default_move = true;

float Proj_degree = 50.0f;

bool camera_rotate_mode = false;
float rt_y = 0.0f;
float cameraRotateangle = 0.0f;
float cameraRotateangleY = 0.0f;
bool camerarotate = false;
bool camerarotateY = false;
int cameraAngleDirection = 1;
int cameraAngleDirectionY = 1;

bool threed_mode = true;    // 시점 변환
bool bomb_mode = false;     // 폭탄 던지기

// time
float delta_time = 0.0f;
float lastFrame = 0.0f;

// 마우스 시점 변환 관련
//double camera_angle_h = 0;
//double camera_angle_v = 0;
//int drag_x_origin;
//int drag_y_origin;
//int dragging = 0;
bool firstMouse = true;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

float Background[] = {
    -1.0,1.0,-1.0, 0.0,1.0,0.0, 1.0,1.0,
    -1.0,-1.0,-1.0, 0.0,1.0,0.0, 1.0,0.0,
    1.0,1.0,-1.0, 0.0,1.0,0.0, 0.0,1.0,

    -1.0,-1.0,-1.0, 0.0,1.0,0.0, 1.0,0.0,
    1.0,-1.0,-1.0, 0.0,1.0,0.0, 0.0,0.0,
    1.0,1.0,-1.0, 0.0,1.0,0.0, 0.0,1.0
};

std::vector< glm::vec3 > robot_vertices;
std::vector< glm::vec2 > robot_uvs;
std::vector< glm::vec3 > robot_normals;

std::vector< glm::vec3 > bottom_vertices;
std::vector< glm::vec2 > bottom_uvs;
std::vector< glm::vec3 > bottom_normals;

std::vector< glm::vec3 > cube_vertices;
std::vector< glm::vec2 > cube_uvs;
std::vector< glm::vec3 > cube_normals;

bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals
) {
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3>temp_vertices;
    std::vector<glm::vec2>temp_uvs;
    std::vector<glm::vec3>temp_normals;

    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        std::cout << "파일 열수 없음!" << std::endl;
        return false;
    }

    while (1) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;;

        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }

        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }

        else if (strcmp(lineHeader, "f") == 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                std::cout << "읽을 수 없습니다.\n" << std::endl;
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }

        for (unsigned int i = 0; i < vertexIndices.size(); i++) {
            unsigned int vertexIndex = vertexIndices[i];
            glm::vec3 vertex = temp_vertices[vertexIndex - 1];
            out_vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < uvIndices.size(); i++) {
            unsigned int uvIndex = uvIndices[i];
            glm::vec2 uv = temp_uvs[uvIndex - 1];
            out_uvs.push_back(uv);
        }

        for (unsigned int i = 0; i < normalIndices.size(); i++) {
            unsigned int normalIndex = normalIndices[i];
            glm::vec3 normal = temp_normals[normalIndex - 1];
            out_normals.push_back(normal);
        }

    }

}

bool res_robot = loadOBJ("robot.obj", robot_vertices, robot_uvs, robot_normals);
bool res_cube = loadOBJ("cube3.obj", cube_vertices, cube_uvs, cube_normals);

// 카메라 벡터 선언
glm::vec3 cameraPos = glm::vec3(player_xpos, player_ypos, player_zpos);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraPos3 = glm::vec3(0.0f, 20.0f, 30.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraPosM = glm::vec3(0.0f, 0.0f, 3.0f);
float radius = 5.0f;
float cameraX = sin(glutGet(GLUT_ELAPSED_TIME)) * radius;
float cameraZ = cos(glutGet(GLUT_ELAPSED_TIME)) * radius;

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

// 색 지정
glm::vec3 Red = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 Green = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 Blue = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 Gray = glm::vec3(0.7f, 0.7f, 0.7f);
glm::vec3 White = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Black = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 Yellow = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 Brown = glm::vec3(0.6f, 0.3f, 0.0f);

// 변환에 이용되는 매트릭스
glm::mat4 S = glm::mat4(1.0f);
glm::mat4 T = glm::mat4(1.0f);
glm::mat4 Tx = glm::mat4(1.0f);
glm::mat4 Rx = glm::mat4(1.0f);
glm::mat4 Ry = glm::mat4(1.0f);
glm::mat4 RxY = glm::mat4(1.0f);
glm::mat4 STR = glm::mat4(1.0f);
glm::mat4 Bomb_STR = glm::mat4(1.0f);
glm::mat4 Robot_STR = glm::mat4(1.0f);
glm::mat4 Player_STR = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

void main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Example18");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "GLEW Initialized\n";
    glewInit();


    InitBuffer();
    InitShader();
    InitTexture();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Loadfile();
    board_maker();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(50, TimerFunction, 1);
    glutMainLoop();
}

void drawScene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(s_program[0]);
    glUseProgram(s_program[1]);
    glUseProgram(s_program[2]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (!threed_mode) {
        view = glm::lookAt(cameraPosM, cameraPosM + cameraFront, cameraUp);
        GLuint viewlocation = glGetUniformLocation(s_program[0], "View");
        glUniformMatrix4fv(viewlocation, 1, GL_FALSE, value_ptr(view));
    }
    else {
        view = glm::lookAt(cameraPos3, cameraDirection, cameraUp);
        GLuint viewlocation = glGetUniformLocation(s_program[0], "View");
        glUniformMatrix4fv(viewlocation, 1, GL_FALSE, value_ptr(view));
    }

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    GLuint Projectionlocation = glGetUniformLocation(s_program[0], "Projection");
    glUniformMatrix4fv(Projectionlocation, 1, GL_FALSE, value_ptr(projection));

    S = glm::scale(glm::mat4(1.0f), glm::vec3(0.7, 0.7, 0.7));
    Ry = glm::rotate(glm::mat4(1.0f), float(glm::radians(180.0f)), glm::vec3(0.0, 1.0, 0.0));
    STR = Ry * S;

    T = glm::translate(glm::mat4(1.0f), glm::vec3((float)player_xpos, (float)player_ypos, (float)player_zpos));
    Player_STR = S * Ry * T;
    unsigned int Player = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Player, 1, GL_FALSE, glm::value_ptr(Player_STR));
    unsigned int Color_Player = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(Color_Player, Blue.r, Blue.g, Blue.b);

    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_TRIANGLES, 0, robot_vertices.size());

    if (bomb_mode) {
        throw_bomb();
        // throw_bomb = false;
        // 적 구현 이후에 timer함수에서 움직이는거 구현
    }

    throw_bomb();
    string msg = "WHY GGAM BBACK";
    renderBitmapCharacter(msg);

    get_time();

    CalculateLight();

    glutPostRedisplay();
    glutSwapBuffers();
}

int cameracount = 0;

bool y_rotate = false;
int y_roll = 0;

float cameraSpeed = 1.0f;

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case'w':
        if (!threed_mode) {
            cameraPos += cameraSpeed * cameraFront;
        }
        player_zpos += 1.0f;
        break;
    case 'a':
        if (!threed_mode) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        player_xpos += 1.0f;
        break;
    case 's':
        if (!threed_mode) {
            cameraPos -= cameraFront * cameraSpeed;
        }
        player_zpos -= 1.0f;
        break;
    case 'd':
        if (!threed_mode) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        player_xpos -= 1.0f;
        break;
    case 'c':
    case 'C':
        if (threed_mode == false)
            threed_mode = true;
        else
            threed_mode = false;
        break;
    case 'b':
        // 일단 보이는것만.. b로 구현함 ㅠㅠ
        bomb_mode = true;
        break;
    }
    glutPostRedisplay();
}

//void Mouse(int button, int state, int x, int y) {
//    if (button == GLUT_LEFT_BUTTON) {
//        if (state == GLUT_DOWN) {
//            dragging = 1;
//            drag_x_origin = x;
//            drag_y_origin = y;
//        }
//        else
//            dragging = 0;
//    }
//}
//
//void mouse_move(int x, int y) {
//    if (dragging) {
//        camera_angle_v += (y - drag_y_origin) * 0.3;
//        camera_angle_h += (x - drag_x_origin) * 0.3;
//        drag_x_origin = x;
//        drag_y_origin = y;
//    }
//}

void Mouse(int button, int state, int x, int y) {
    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    printf("lastX, lastY : (%f, %f)\n", lastX, lastY);

    float sensitivity = 1;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    if ((button == 3) || (button == 4)) {
        if (state == GLUT_UP)
            return;
        printf("Scroll %s At %d, %d\n", (button == 3) ? "Up" : "Down", x, y);

        if (fov >= 1.0f && fov <= 45.0f)
            fov -= yoffset;
        if (fov <= 1.0f)
            fov = 1.0f;
        if (fov >= 45.0f)
            fov = 45.0f;
    }
}

void TimerFunction(int value) {

    glutTimerFunc(10, TimerFunction, 1);
}

int HP = 10000;

const double fpsLimit = 1.0 / 60.0;
float lastUpdate = 0.0f;

void get_time() {
    float currentFrame = glutGet(GLUT_ELAPSED_TIME);
    delta_time = currentFrame - lastUpdate;


    if ((currentFrame - lastFrame) >= fpsLimit) {
        lastFrame = currentFrame;
    }
    lastUpdate = currentFrame;

    HP -= lastUpdate;
    printf("deltatime : %f\n", delta_time);
    if (HP % 100 == 0) {
        printf("HP : %d \n", HP);
    }
}

void throw_bomb() {
    qobj = gluNewQuadric();

    // Ry = glm::rotate(glm::mat4(1.0f), float(glm::radians(Mercury_y)), glm::vec3(0.0, 1.0, 0.0));
    S = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 1.5, 1.5));
    T = glm::translate(glm::mat4(1.0f), glm::vec3(5.0, 0.0, 0.0));
    //Planet_STR = Circle_STR * Ry * T;
    Bomb_STR = S * T;
    unsigned int Planet = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Planet, 1, GL_FALSE, glm::value_ptr(Bomb_STR));
    unsigned int Color_Bomb = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(Color_Bomb, Red.r, Red.g, Red.b);
    gluSphere(qobj, 0.5, 20, 20);
}


void renderBitmapCharacter(string s) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0.0, 1.0, 0.0);
    glRasterPos2i(WIDTH / 3, HEIGHT - 30);

    // string s = "Respect mah authoritah!";
    void* font = GLUT_BITMAP_TIMES_ROMAN_24;
    for (string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(font, c);
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

FILE* fp;
////////////////////////////////////////////////////////////////////////
// 맵을 읽어옵시다
int Loadfile()
{
    printf("fdksjalfnvsafjwelscdk,njfal.\n");

    switch (mapcollect)
    {
    case 0:
        printf("fdksjalfnvsafjwelscdk,njfal.\n");
        fp = fopen(MAP_1, "rt");
        break;
    case 1:
        fp = fopen(MAP_2, "rt");
        break;
    case 2:
        fp = fopen(MAP_3, "rt");
        break;
    case 3:
        fp = fopen(MAP_4, "rt");
        break;
    case 4:
        fp = fopen(MAP_5, "rt");
        break;
    case 5:
        fp = fopen(INIT, "rt");
        break;
    }

    if (fp == NULL)
    {
        printf("\n실패\n");
        printf("errno = %s\n", strerror(errno));
        return 1;
    }

    printf("\n완료\n");

    int cha;

    while (feof(fp) == 0)
    {
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                fscanf(fp, "%d", &cha);
                makeboard[i][j] = cha;
            }
        }
    }
    fclose(fp);

    return 1;
}

// 이젠 맵을 그릴거예요

float obj_rot = 0;
void draw_block()
{
    glPushMatrix();
    {
        glColor3f(colorbuffer[0][0], colorbuffer[0][1], colorbuffer[0][2]);
        glTranslatef(0, 1.1, 0);
        glutSolidCube(1);

        glColor3f(colorbuffer[1][0], colorbuffer[1][1], colorbuffer[1][2]);
        glTranslatef(0, 1.1, 0);
        glutSolidCube(1);

        glColor3f(colorbuffer[2][0], colorbuffer[2][1], colorbuffer[2][2]);
        glTranslatef(0, 1.1, 0);
        glutSolidCube(1);

        glColor3f(colorbuffer[3][0], colorbuffer[3][1], colorbuffer[3][2]);
        glTranslatef(0, 1.1, 0);
        glutSolidCube(1);
    }
    glPopMatrix();
}

void item()
{
    glPushMatrix();
    {
        glEnable(GL_BLEND);
        glPushMatrix();
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            //glColor3f(0.7, 0.2, 0.2);
            glTranslatef(0, 2, 0);
            glColor4f(0, 0.8, 0, 0.8);
            glRotatef(obj_rot, 0, 1, 0);
            glutSolidCube(0.3);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glColor4f(0.7, 0, 0, 0.5);
            glTranslatef(0, 2, 0);
            glRotatef(obj_rot, 0, 1, 0);
            glRotatef(90, -1, 0, -1);
            glutSolidCube(0.6);
        }
        glPopMatrix();

        glDisable(GL_BLEND);
    }
    glPopMatrix();
}

void Line_block()
{
    glColor3f(0.7, 0.7, 0.7);

    glPushMatrix();
    {
        glNormal3f(0, 1, 0);
        glTranslatef(0, 1.1, 0);
        glutSolidCube(1);

        glTranslatef(0, 1.1, 0);
        glutSolidCube(1);

        glTranslatef(0, 1.1, 0);
        glutSolidCube(1);

        glTranslatef(0, 1.1, 0);
        glutSolidCube(1);

        glTranslatef(0, 1.1, 0);
        glutSolidCube(1);
    }
    glPopMatrix();
}

void remain()
{
    glPushMatrix();
    {
        glEnable(GL_BLEND);
        glPushMatrix();
        {
            //glScaled(1.5,1,1.5);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            //glColor3f(0.7, 0.2, 0.2);
            glTranslatef(0, 0.6, 0);
            glColor4f(0, 0.5, 0.5, 0.8);
            glRotatef(obj_rot, 0, 1, 0);
            glutSolidSphere(0.1, 15, 15);
            //glutSolidCube(0.1);
        }
        glPopMatrix();

        glPushMatrix();
        {
            //glScaled(1.5, 1, 1.5);
            glColor4f(0.0, 0.0, 0.8, 0.5);
            glTranslatef(0, 0.6, 0);
            glRotatef(obj_rot + 5, 0, 1, 0);
            glRotatef(90, -1, 0, -1);
            //glutSolidSphere(0.15, 15, 15);
            glutSolidCube(0.25);
        }
        glPopMatrix();

        glDisable(GL_BLEND);
    }
    glPopMatrix();
}

void board_maker()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (makeboard[i][j] == 1)
            {
                glPushMatrix();
                {
                    glTranslatef(i * 1.1 - 15, 0, j * 1.1 - 15);
                    draw_block();
                }
                glPopMatrix(); // 블럭 표시

                glColor3f(1, 0, 0);
                glLineWidth(1);
                //glBegin(GL_LINE_LOOP);
                //{
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].min_z);
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].min_z);
                //}
                //glEnd();
                glLineWidth(1); // 블럭 위치
            }
            else if (makeboard[i][j] == 2)
            {
                glPushMatrix();
                {
                    glTranslatef(i * 1.1 - 15, 0, j * 1.1 - 15);
                    item();
                }
                glPopMatrix(); // 아이템 표시

                glColor3f(0, 1, 0);
                glLineWidth(1);
                //glBegin(GL_LINE_LOOP);
                //{
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].min_z);
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].min_z);
                //}
                //glEnd();
                glLineWidth(1); // 아이템 위치
            }
            else if (makeboard[i][j] == 3)
            {
                glPushMatrix();
                {
                    glTranslatef(i * 1.1 - 15, 1.5, j * 1.1 - 15);
                    glColor3f(0.0, 0.0, 0.0);

                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(-1.0f, 2.0f, 1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(-1.0f, -2.0f, 1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(1.0f, -2.0f, 1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(1.0f, 2.0f, 1.0f);
                    }
                    glEnd();
                    //오른쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(1.0f, 2.0f, 1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(1.0f, -2.0f, 1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(1.0f, -2.0f, -1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(1.0f, 2.0f, -1.0f);
                    }
                    glEnd();
                    //뒷쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(1.0f, 2.0f, -1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(1.0f, -2.0f, -1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(-1.0f, -2.0f, -1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(-1.0f, 2.0f, -1.0f);
                    }
                    glEnd();
                    ////왼쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(-1.0f, 2.0f, -1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(-1.0f, -2.0f, -1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(-1.0f, -2.0f, 1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(-1.0f, 2.0f, 1.0f);
                    }
                    glEnd();
                    ////아랫쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(-1.0f, -2.0f, 1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(-1.0f, -2.0f, -1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(1.0f, -2.0f, -1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(1.0f, -2.0f, 1.0f);
                    }
                    glEnd();
                    ////윗쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(-1.0f, 2.0f, 1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(-1.0f, 2.0f, -1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(1.0f, 2.0f, -1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(1.0f, 2.0f, 1.0f);
                    }
                    glEnd();
                }
                glPopMatrix(); // 도착지 표시

                glColor3f(0, 0, 1);
                glLineWidth(1);
                //glBegin(GL_LINE_LOOP);
                //{
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].min_z);
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].min_z);
                //}
                //glEnd();
                glLineWidth(1); // 도착지 위치
            }
            else if (makeboard[i][j] == 4)
            {
                glPushMatrix();
                {
                    glTranslatef(i * 1.1 - 15, 0, j * 1.1 - 15);
                    remain();
                }
                glPopMatrix();

                glColor3f(0, 1, 0);
                //glLineWidth(1);
                //glBegin(GL_LINE_LOOP);
                //{
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].min_z);
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].min_z);
                //}
                //glEnd();
                //glLineWidth(1); // 아이템 위치
            }
            if (makeboard[i][j] == 5)
            {
                glPushMatrix();
                {
                    glTranslatef(i * 1.1 - 15, 0, j * 1.1 - 15);
                    Line_block();
                }
                glPopMatrix(); // 블럭 표시

                glColor3f(1, 0, 0);
                glLineWidth(1);
                //glBegin(GL_LINE_LOOP);
                //{
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].max_z);
                //	glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].min_z);
                //	glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].min_z);
                //}
                //glEnd();
                glLineWidth(1); // 블럭 위치
            }
        }
    }
}
//void DrawHPTimer(int winPosX, int WinPosY, char* strMsg, void* font, double color[3]) {
//    DrawText(10, 10, "test", GLUT_BITMAP_8_BY_13, m_fFontColor);
//
//    // draw text on screen
//    double FontWidth = 0.02;
//    double GLPosX, GLPosY, GLPosZ;
//    WinPosToWorldPos(WinPosX, WinPosY, 0, &GLPosX, &GLPosY, &GLPosZ);
//
//    glColor3f(Color[R], Color[G], Color[B]);
//
//    int len = (int)strlen(strMsg);
//    glRasterPost3d(GLPosX, GLPosY, GLPosZ);
//    
//    for (int i = 0; i < len; i++) {
//        glutBitmapCharacter(font, strMsg[i]);
//    }
// }

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

char* filetobuf(const char* file) {
    FILE* fptr; long length; char* buf;
    fptr = fopen(file, "rb"); // Open file for reading
    if (!fptr) // Return NULL on failure
        return NULL;
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file
    length = ftell(fptr); // Find out how many bytes into the file we are
    buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
    fclose(fptr); // Close the fil
    buf[length] = 0; // Null terminator 
    return buf; // Return the buffer 
}

void make_vertexShader()
{
    vertexsource = filetobuf("vertex.glsl");
    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
    glCompileShader(vertexshader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexshader, 512, NULL, errorLog);
        std::cerr << "ERROR:vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

void make_fragmentShader()
{
    fragmentsource = filetobuf("fragment.glsl");
    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, NULL);
    glCompileShader(fragmentshader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentshader, 512, NULL, errorLog);
        std::cerr << "ERROR:vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

GLuint make_shaderProgram() {
    GLuint Shaders_program = glCreateProgram();
    glAttachShader(Shaders_program, vertexshader);
    glAttachShader(Shaders_program, fragmentshader);
    glLinkProgram(Shaders_program);
    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);

    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(Shaders_program, GL_LINK_STATUS, &result);
    if (!result) {
        cerr << "ERROR: shader program 연결실패\n" << errorLog << endl;
        return false;
    }
    glUseProgram(Shaders_program);
    return Shaders_program;
}

void InitBuffer()
{
    glGenVertexArrays(10, VAO);
    glGenBuffers(10, VBO);

    // 큐브

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, cube_vertices.size() * sizeof(glm::vec3), &cube_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, cube_normals.size() * sizeof(glm::vec3), &cube_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // 로봇
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, robot_vertices.size() * sizeof(glm::vec3), &robot_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, robot_normals.size() * sizeof(glm::vec3), &robot_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

void InitShader() {
    make_vertexShader();
    make_fragmentShader();

    for (int i = 0; i < 3; ++i)
    {
        s_program[i] = glCreateProgram();
        glAttachShader(s_program[i], vertexshader);
        glAttachShader(s_program[i], fragmentshader);
        glLinkProgram(s_program[i]);
        glDeleteShader(vertexshader);
        glDeleteShader(fragmentshader);

        glUseProgram(s_program[i]);
    }
}

void InitTexture()
{
    int imagewidth, imageheight, numofChannel;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bg = stbi_load("bg.jpg", &imagewidth, &imageheight, &numofChannel, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_RGB, GL_UNSIGNED_BYTE, bg);
    stbi_image_free(bg);

    glUseProgram(s_program[2]);
    int tLocation = glGetUniformLocation(s_program[2], "outTexture");
    glUniform1i(tLocation, 0);
}

void CalculateLight() {
    // 조명 계산
    unsigned int light_pos = glGetUniformLocation(s_program[2], "lightPos");
    glUniform3f(light_pos, light_x, light_y, light_z);

    unsigned int light_color = glGetUniformLocation(s_program[2], "lightColor");
    glUniform3f(light_color, light_r, light_g, light_b);

    unsigned int view_pos = glGetUniformLocation(s_program[2], "viewPos");
    glUniform3f(view_pos, camera_x, camera_y, camera_z);

    unsigned int ambientLight_on = glGetUniformLocation(s_program[2], "ambientLight_on_off");
    glUniform3f(ambientLight_on, 0.7, 0.7, 0.7);
}