#include "ReadObj.h"

#pragma warning(disable:4996)
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <time.h>
#include <windows.h>
#include <vector>
//#include <fstream>
//#include <windows.h>
//#include <mmsystem.h>
#include <gl/glew.h> 
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>

#define WIDTH 800
#define HEIGHT 600

#define MAP_1 "MAP_1.txt"
#define MAP_2 "MAP_2.txt"
#define MAP_3 "MAP_3.txt"
#define MAP_4 "MAP_4.txt"
#define MAP_5 "MAP_5.txt"
#define INIT "MAP_1.txt"

#define SHAPE_SIZE 0.5f // Enemy size

#define SIZE 31 // 맵 사이즈
#define MONSTER_SIZE 20
using namespace std;

normal_distribution <float>uid_mColor{ 0.0,1.0 };
normal_distribution <float>uid_mDir{ -1.0 ,1.0 };
normal_distribution <float>uid_mPos{ -20, 20 };

default_random_engine dre((size_t)time(NULL));

const float BOMB_TIME = 3.0;

struct Vector3 {
    float x;
    float y;
    float z;

    Vector3() {}

    Vector3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

struct Vector4 {
    float minx;
    float minz;
    float maxx;
    float maxz;

    Vector4() {}

    Vector4(float minX, float minZ, float maxX, float maxZ) {
        this->minx = minX;
        this->minz = minZ;
        this->maxx = maxX;
        this->maxz = maxZ;
    }
};


enum BOARD_TYPE {
    NONE = 0,
    WALL = 1,
    ITEM = 2,
    FIXED_WALL = 5,
};

struct Shape {
    BOARD_TYPE type;
    Vector3 color;
    Vector3 scale;
    Vector3 pos;
    Vector3 dir;
    float radius;

    Vector4 GetBB() {
        return Vector4(pos.x - radius, pos.z - radius, pos.x + radius, pos.z + radius);
    }

    Shape() {
        return;
    }
};

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
void CalculateLight(float, float, float, float);
int Loadfile(int mapCollect);
void DrawBoard();
void CameraSetting(GLuint s_program, Vector3 cameraPosition, Vector3 cameraDir, float yPos);

// 함수 선언
void throw_bomb();
void DrawPlayer();
void DrawCube();
void Draw2ndCube();
void DrawEnemy();
void DrawKey();
void InitShape();
float get_time();
float currentTime();
bool CollisionCheck(Shape, Shape);
bool radius_collision(Shape, Shape);

void SpecialKeyboard(int key, int x, int y); //키보드 조종
void Keyboard(unsigned char Key, int x, int y); // 키보드 조종2
void releaseKey(int key, int x, int y);

GLUquadricObj* qobj;

GLuint shaderID;
GLuint s_program[3];
GLuint s_LineProgram;
GLchar* vertexsource, * fragmentsource;
GLuint vertexshader, lineVertexShader, fragmentshader;

GLuint VAO[10], VBO[10];
GLuint VAOCube[7], VBOCube[7];

unsigned int texture[10];

///////////////////////////////////////////////////////////////
// 맵 파일 입출력
GLubyte* LoadDIBitmap(const char* filename, BITMAPINFO** info);
GLubyte* pBytes; // 데이터를 가리킬 포인터
BITMAPINFO* info; // 비트맵 헤더 저장할 변수
GLuint textures[1];
void initTextures();

int mapCollect = 0;
float colorbuffer[4][3] = { 0 };

Shape boardShape[SIZE][SIZE];
Shape monster[MONSTER_SIZE];

float objectSize = 1;
bool isChanged = false;

float r;
float g;
float b;

int monster_num = 0;

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
//default_random_engine dre{ rd() };
uniform_real_distribution<> random_pos_urd{ -15.0, 15.0 };

float random_xpos = random_pos_urd(dre);
float random_zpos = random_pos_urd(dre);
float random_move_xpos = random_pos_urd(dre);
float random_move_zpos = random_pos_urd(dre);

float robot_xpos[4] = { 0, };
float robot_ypos[4] = { 0.0f };
float robot_zpos[4] = { 0, };

Vector3 cameraDir = Vector3();

Shape bombShape = Shape();
Shape player = Shape();

float enemy_xpos;
float enemy_ypos = 0.0f;
float enemy_zpos = -30.0f;

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
bool enemy_valid = false;    // enemy 살아있는지

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

// 충돌체크를 위한 radius 선언
float player_radius = 1.0f;
float enemy_radius = 0.75f;
float bomb_radius = 0.3f;


////////////////////////////////////////////
// Camera

// 각
float angle = 0.0f;

//벡터
float lx = 0.0f, lz = -1.0f, ly = 0.0f;

// 이동 변수
float deltaAngle = 0.0f;
float deltaMove = 0;
int xOrigin = -1;

bool isFPS = false;

float Background[] = {
    -1.0,1.0,-1.0, 0.0,1.0,0.0, 1.0,1.0,
    -1.0,-1.0,-1.0, 0.0,1.0,0.0, 1.0,0.0,
    1.0,1.0,-1.0, 0.0,1.0,0.0, 0.0,1.0,

    -1.0,-1.0,-1.0, 0.0,1.0,0.0, 1.0,0.0,
    1.0,-1.0,-1.0, 0.0,1.0,0.0, 0.0,0.0,
    1.0,1.0,-1.0, 0.0,1.0,0.0, 0.0,1.0
};

std::vector< glm::vec3 > cube_vertices;
std::vector< glm::vec2 > cube_uvs;
std::vector< glm::vec3 > cube_normals;

std::vector< glm::vec3 > robot_vertices;
std::vector< glm::vec2 > robot_uvs;
std::vector< glm::vec3 > robot_normals;

std::vector< glm::vec3 > pyramid_vertices;
std::vector< glm::vec2 > pyramid_uvs;
std::vector< glm::vec3 > pyramid_normals;

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
bool res_pyramid = loadOBJ("pyramid.obj", pyramid_vertices, pyramid_uvs, pyramid_normals);

float radius = 5.0f;
float cameraX = sin(glutGet(GLUT_ELAPSED_TIME)) * radius;
float cameraZ = cos(glutGet(GLUT_ELAPSED_TIME)) * radius;

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
glm::mat4 cubeSTR = glm::mat4(1.0f);
glm::mat4 seccubeSTR = glm::mat4(1.0f);
glm::mat4 EnemySTR = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

void main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(500, 100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("GAME NAME");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "GLEW Initialized\n";
    glewInit();

    Loadfile(1);
    InitShape();
    InitBuffer();
    InitShader();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKeyboard);
    glutSpecialUpFunc(releaseKey);
    glutTimerFunc(50, TimerFunction, 1);
    glutMainLoop();
}

// 플레이어 그리기 함수
void DrawPlayer(Shape player)
{
    glm::mat4 STR = glm::mat4(1.0f);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.7, 0.7, 0.7));
    glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), float(glm::radians(180 - (angle + deltaAngle) * 180.0 / 3.14)), glm::vec3(0.0, 1.0, 0.0));
    STR = S * Ry;

    //glRotatef(180 - (angle + deltaAngle) * 180.0 / 3.14, 0.0, 1.0, 0.0);

    glm::mat4 cubeSTR = glm::mat4(1.0f);
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3((float)player.pos.x, (float)player.pos.y, (float)player.pos.z));
    cubeSTR = T * Ry * S;
    unsigned int Player = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Player, 1, GL_FALSE, glm::value_ptr(cubeSTR));
    unsigned int Color_Player = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(Color_Player, Blue.r, Blue.g, Blue.b);

    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_TRIANGLES, 0, robot_vertices.size());
}

void get_bb(Shape shape) {
    glPushMatrix();
    {
        glTranslatef(shape.pos.x, 1, shape.pos.z);
        glLineWidth(7);
        glBegin(GL_LINE_LOOP);
        {
            glVertex3f(-shape.radius, -2, shape.radius);
            glVertex3f(-shape.radius, -2, -shape.radius);
            glVertex3f(shape.radius, -2, -shape.radius);
            glVertex3f(shape.radius, -2, shape.radius);
        }
        glEnd();
        glLineWidth(1);
    }
    glPopMatrix();
}

// 큐브 그리기 함수 -> 맵 그릴 때 사용
void DrawCube(Shape shape)
{
    // get_bb(shape.GetBB());
    glm::mat4 S = glm::mat4(1.0f);
    glm::mat4 T = glm::mat4(1.0f);
    S = glm::scale(glm::mat4(1.0f), glm::vec3(shape.scale.x, shape.scale.y, shape.scale.z));
    T = glm::translate(glm::mat4(1.0f), glm::vec3(shape.pos.x, shape.pos.y, shape.pos.z));
    cubeSTR = S * T;
    unsigned int transformCube = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transformCube, 1, GL_FALSE, glm::value_ptr(cubeSTR));
    unsigned int colorCube = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorCube, shape.color.x, shape.color.y, shape.color.z);

    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size());
}

void Draw2ndCube(Shape shape) {
    glm::mat4 S = glm::mat4(1.0f);
    glm::mat4 T = glm::mat4(1.0f);

    S = glm::scale(glm::mat4(1.0f), glm::vec3(shape.scale.x, shape.scale.y, shape.scale.z));
    T = glm::translate(glm::mat4(1.0f), glm::vec3(shape.pos.x, shape.pos.y + 2 * shape.radius, shape.pos.z));
    seccubeSTR = S * T;
    unsigned int transform2ndCube = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transform2ndCube, 1, GL_FALSE, glm::value_ptr(seccubeSTR));
    unsigned int colorCube = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorCube, shape.color.x, shape.color.y, shape.color.z);

    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size());
}

void PrintUI()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    string text = "TIME : " + to_string(currentTime()).substr(0, 4);
    const char* string = text.data();
    glRasterPos2f(-0.9, 0.9);  // 문자 출력할 위치 설정  
    int len = (int)strlen(string);
    for (int i = 0; i < len; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
}

void DrawEnemy() {
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 1.5, 1.5));
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(enemy_xpos, enemy_ypos, enemy_zpos));
    EnemySTR = S * T;

    unsigned int transformEnemy = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transformEnemy, 1, GL_FALSE, glm::value_ptr(EnemySTR));
    unsigned int colorEnemy = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorEnemy, Green.r, Green.g, Green.b);

    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size());
}

void DrawKey(Shape shape) {
    glm::mat4 S = glm::mat4(1.0f);
    glm::mat4 T = glm::mat4(1.0f);

    S = glm::scale(glm::mat4(1.0f), glm::vec3(shape.scale.x, shape.scale.y, shape.scale.z));
    T = glm::translate(glm::mat4(1.0f), glm::vec3(shape.pos.x, shape.pos.y, shape.pos.z));
    cubeSTR = S * T;
    unsigned int transformCube = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transformCube, 1, GL_FALSE, glm::value_ptr(cubeSTR));
    unsigned int colorCube = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorCube, shape.color.x, shape.color.y, shape.color.z);

    glBindVertexArray(VAO[2]);
    glDrawArrays(GL_TRIANGLES, 0, pyramid_vertices.size());

    CalculateLight(shape.pos.x, shape.pos.y, shape.pos.z, 1.0);
}

void DrawBoard()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            switch (boardShape[i][j].type) {
            case BOARD_TYPE::NONE:
                break;
            case BOARD_TYPE::WALL:
            case BOARD_TYPE::FIXED_WALL:
                DrawCube(boardShape[i][j]);
                Draw2ndCube(boardShape[i][j]);
                break;
            case BOARD_TYPE::ITEM:
                DrawKey(boardShape[i][j]);
                break;
            }
        }
    }
}

void computePos(float deltaMove)
{
    player.pos.x += deltaMove * lx * 0.01f;
    player.pos.z += deltaMove * lz * 0.01f;
}
void computeDir(float deltaAngle) {
    angle += deltaAngle;
    lx = sin(angle);
    lz = -cos(angle);
}

void DrawMonster(Shape monster) {
    glm::mat4 monsterSTR = glm::mat4(1.0f);
    qobj = gluNewQuadric();
    glm::vec3 monsterPos = glm::vec3(monster.pos.x, monster.pos.y, monster.pos.z);
    S = glm::scale(glm::mat4(1.0f), glm::vec3(4, 4, 4));
    T = glm::translate(glm::mat4(1.0f), monsterPos);
    monsterSTR = S * T;
    unsigned int Planet = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Planet, 1, GL_FALSE, glm::value_ptr(monsterSTR));

    unsigned int Color_Bomb = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(Color_Bomb, monster.color.x, monster.color.y, monster.color.z);

    CalculateLight(monsterPos.x, monsterPos.y, monsterPos.z, 1.0);

    gluSphere(qobj, 1.0, 20, 20);
}

void InitShape() {
    for (int i = 0; i < MONSTER_SIZE; i++) {
        monster[i].pos = Vector3(uid_mPos(dre), 1.0f, uid_mPos(dre));
        monster[i].color = Vector3(uid_mColor(dre), uid_mColor(dre), uid_mColor(dre));
        monster[i].dir = Vector3(uid_mDir(dre), 0.0f, uid_mDir(dre));
        monster[i].radius = 1.0f;
    }

    player.radius = 0.3f;
}

void drawScene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    PrintUI();

    glUseProgram(s_program[0]);
    glUseProgram(s_program[1]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    CalculateLight(light_x, light_y, light_z, 0.5);

    if (deltaAngle)
        computeDir(deltaAngle);

    if (deltaMove)
        computePos(deltaMove);

    Vector3 dir = Vector3();
    float yPos = 5.0f;
    if (!isFPS)
        dir = Vector3(player.pos.x + lx * 100, player.pos.y + ly, player.pos.z + lz * 100);
    else {
        dir = Vector3(player.pos.x + lx, player.pos.y + ly, player.pos.z + lz);
        yPos *= 20;
    }
    CameraSetting(s_program[0], player.pos, dir, yPos);
    DrawBoard();
    DrawPlayer(player);
    // cout << "player pos : " << player.pos.x << ", " << player.pos.y << ", " << player.pos.z << endl;

    // 폭탄
    if (bomb_mode) {
        throw_bomb();
        // throw_bomb = false;
        // 적 구현 이후에 timer함수에서 움직이는거 구현
    }
        
    for (int i = 0; i < MONSTER_SIZE; i++) {
        DrawMonster(monster[i]);
        // cout << i << " : " << monster[i].pos.x << ", " << monster[i].pos.y << ", " << monster[i].pos.z << endl;
    }

    glutPostRedisplay();
    glutSwapBuffers();
}

int cameracount = 0;

bool y_rotate = false;
int y_roll = 0;

float cameraSpeed = 1.0f;

void releaseKey(int key, int x, int y) {

    switch (key)
    {
    case GLUT_KEY_LEFT:
    case GLUT_KEY_RIGHT: deltaAngle = 0; break;

    case GLUT_KEY_UP:
    case GLUT_KEY_DOWN: deltaMove = 0; break;
    }
}

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        isFPS = false;
        break;
    case '3':
        isFPS = true;
        break;
    case 'b':
        // 일단 보이는것만.. b로 구현함 ㅠㅠ
        if (!bomb_mode) {
            bombShape.pos = player.pos;
            bombShape.dir = Vector3(lx, ly, lz);
            bomb_mode = true;
        }
        break;
    case 'e':
        random_xpos = random_pos_urd(dre);
        enemy_xpos = random_xpos;
        enemy_valid = true;
        break;
    }
}

void SpecialKeyboard(int key, int xx, int yy)
{
    if (key == GLUT_KEY_LEFT)
    {
        deltaAngle = -0.025f;
    }
    if (key == GLUT_KEY_RIGHT)
    {
        deltaAngle = 0.025f;
    }
    if (key == GLUT_KEY_UP)
    {
        deltaMove = 15.0;
    }
    if (key == GLUT_KEY_DOWN)
    {
        deltaMove = -15.0;
    }
    glutPostRedisplay();
}

float elapsedTime;
void TimerFunction(int value) {
    if (bomb_mode) {
        if (elapsedTime > 0) {
            elapsedTime -= 0.1f;
            bombShape.pos.x += bombShape.dir.x;
            bombShape.pos.z += bombShape.dir.z;
            // Monster Collision
            for (int i = 0; i < MONSTER_SIZE; i++) {
                if (CollisionCheck(monster[i], bombShape)) {
                    printf("Collision MONSTER to BOMB\n");
                }
                if (radius_collision(monster[i], bombShape)) {
                    printf("RADIUS_COLLISION : [%d]MONSTER to BOMB\n", i);
                }
            }
            // TODO : 충돌체크 여기다 해주면 됨
        }
        else {
            elapsedTime = BOMB_TIME;
            bomb_mode = false;
        }
    }
    for (int i = 0; i < MONSTER_SIZE; ++i) {
        monster[i].pos.x += 0.01 * monster[i].dir.x;
        monster[i].pos.z += 0.01 * monster[i].dir.z;
    }

    // Player to Board Collision
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            switch (boardShape[i][j].type) {
            case BOARD_TYPE::NONE:
                break;
            case BOARD_TYPE::WALL:
            case BOARD_TYPE::FIXED_WALL:
                if (CollisionCheck(boardShape[i][j], player))
                    printf("Collision PLAYER\n");
                if (radius_collision(boardShape[i][j], player))
                    printf("RADIUS_COLLISION : [%d][%d]WALL to PLAYER\n", i, j);
                break;
            case BOARD_TYPE::ITEM:
                if (CollisionCheck(boardShape[i][j], player))
                    printf("Collision ITEM\n");
                if (radius_collision(boardShape[i][j], player))
                    printf("RADIUS_COLLISION : [%d][%d]ITEM to PLAYER\n", i, j);
                break;
            }
        }
    }

    // Player to Monster Collision
    for (int i = 0; i < MONSTER_SIZE; i++) {
        if (radius_collision(monster[i], player))
            printf("RADIUS_COLLISION : [%d]MONSTER to PLAYER\n", i);
    }

    glutTimerFunc(10, TimerFunction, 1);
}

// 잔여 HP 표현하는 delta_time 얻는 함수
float get_time()
{
    float currentFrame = glutGet(GLUT_ELAPSED_TIME);
    delta_time = currentFrame - lastFrame;
    lastFrame = currentFrame;

    return lastFrame;
}

float currentTime() {
    return round(get_time() / 100) / 10;
}

// 폭탄
void throw_bomb() {
    qobj = gluNewQuadric();
    glm::vec3 bomb_pos = glm::vec3(bombShape.pos.x, bombShape.pos.y, bombShape.pos.z);
    S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0));
    T = glm::translate(glm::mat4(1.0f), bomb_pos);
    Bomb_STR = S * T;
    unsigned int Bomb = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Bomb, 1, GL_FALSE, glm::value_ptr(Bomb_STR));

    unsigned int Color_Bomb = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(Color_Bomb, Red.r, Red.g, Red.b);

    CalculateLight(bomb_pos.x, bomb_pos.y, bomb_pos.z, 1.0);

    gluSphere(qobj, 0.5, 20, 20);
}

////////////////////////////////////////////////////////////////////////
// 맵을 txt 파일에서 읽어온다
int Loadfile(int mapCollect)
{
    FILE* fp = new FILE();
    switch (mapCollect)
    {
    case 0:
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
        printf("\n board gen fail\n");
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

                boardShape[i][j].type = (BOARD_TYPE)cha;

                boardShape[i][j].pos = Vector3((i * 7.5f - 105), 0, (j * 7.5f - 105));

                if (boardShape[i][j].type == ITEM) {
                    boardShape[i][j].color = Vector3(Yellow.r, Yellow.g, Yellow.b);
                    boardShape[i][j].scale = Vector3(1.0, 1.0, 1.0);
                    boardShape[i][j].radius = 3.0f;
                }
                else {
                    boardShape[i][j].radius = 1.0f;
                    boardShape[i][j].scale = Vector3(3.0, 3.0, 3.0);

                    if (boardShape[i][j].type == FIXED_WALL) {
                        boardShape[i][j].color = Vector3(0.7, 0.7, 0.7);
                    }
                    else {
                        boardShape[i][j].color = Vector3(0.3, 0.3, 0.3);
                    }
                }
            }
        }
    }

    fclose(fp);

    return 1;
}

float obj_rot = 0;

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 이후는 초기화함수
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
        std::cerr << "ERROR:fragment shader 컴파일 실패\n" << errorLog << std::endl;
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
        cerr << "ERROR:shader program 연결실패\n" << errorLog << endl;
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
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, robot_vertices.size() * sizeof(glm::vec3), &robot_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, robot_normals.size() * sizeof(glm::vec3), &robot_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // 로봇
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, pyramid_vertices.size() * sizeof(glm::vec3), &pyramid_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, pyramid_normals.size() * sizeof(glm::vec3), &pyramid_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

void InitShader() {
    make_vertexShader();
    make_fragmentShader();

    for (int i = 0; i < 2; ++i)
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

// 조명 계산
void CalculateLight(float lgt_x, float lgt_y, float lgt_z, float amb) {
    unsigned int light_pos = glGetUniformLocation(s_program[0], "lightPos");
    glUniform3f(light_pos, lgt_x, lgt_y, lgt_z);

    unsigned int light_color = glGetUniformLocation(s_program[0], "lightColor");
    glUniform3f(light_color, light_r, light_g, light_b);

    unsigned int view_pos = glGetUniformLocation(s_program[0], "viewPos");
    glUniform3f(view_pos, camera_x, camera_y, camera_z);

    unsigned int ambientLight_on = glGetUniformLocation(s_program[0], "ambientLight_on_off");
    glUniform3f(ambientLight_on, amb, amb, amb);
}

void CameraSetting(GLuint s_program, Vector3 cameraPosition, Vector3 cameraDir, float yPos)
{
    int projLoc = glGetUniformLocation(s_program, "Projection");
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians((float)45.0f), 800.f / 600.f, 0.1f, 100.0f);
    glm::vec3 cameraPos = glm::vec3(cameraPosition.x, cameraPosition.y + yPos, cameraPosition.z);
    glm::vec3 cameraDirection = glm::vec3(cameraDir.x, cameraDir.y, cameraDir.z);
    glm::vec3 cameraUp = glm::vec3(0, 1, 0);
    glm::mat4 view = glm::mat4(1.0f);

    int viewLoc = glGetUniformLocation(s_program, "View");
    view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
}


bool CollisionCheck(Shape shape, Shape shape2) { // shpae2 : player
    Vector4 bbox = shape.GetBB();
    Vector4 bbox2 = shape2.GetBB();
    bool xcollision = false;
    bool zcollision = false;

    if ((bbox.minx < bbox2.minx && bbox.maxx > bbox2.minx) ||
        ((bbox.minx < bbox2.maxx && bbox.maxx > bbox2.maxx))) {
        xcollision = true;
    }

    if ((bbox.minz < bbox2.minz && bbox.maxz > bbox2.minz) ||
        ((bbox.minz < bbox2.maxz && bbox.maxz > bbox2.maxz))) {
        xcollision = true;
    }

    if (xcollision && zcollision) {
        cout << "충돌" << endl;
        return true;
    }
    else
        return false;
}

bool radius_collision(Shape shape1, Shape shape2) {
    float distance = (shape1.pos.x - shape2.pos.x) * (shape1.pos.x - shape2.pos.x) + (shape1.pos.z - shape2.pos.z) * (shape1.pos.z - shape2.pos.z);
    float rad_sum_sq = (shape1.radius + shape2.radius) * (shape1.radius + shape2.radius);
    
    if (distance < rad_sum_sq) {
        cout << "distance : " << distance << endl;
        cout << "rd sum : " << rad_sum_sq << endl;
        return true;
    }
    else
        return false;
}