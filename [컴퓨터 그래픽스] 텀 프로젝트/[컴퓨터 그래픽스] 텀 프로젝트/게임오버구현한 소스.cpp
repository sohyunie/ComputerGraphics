#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include "ReadObj.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
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

#pragma warning(disable:4996)

#define WIDTH 800
#define HEIGHT 600

#define SHAPE_SIZE 0.5f // Enemy size

#define SIZE 30 // 맵 사이즈
#define MONSTER_SIZE 30
#define MAX_TIME 120
#define MAX_ITEM 5

#define SOUND_FILE_NAME_BGM_1	"../sound/backgroundmusic.wav"
using namespace std;

normal_distribution <float>uid_mColor{ 0.0,1.0 };   // 적의 색
normal_distribution <float>uid_mSpeed{ 8.0 ,10.0 }; // 적의 속도
normal_distribution <float>uid_mDir{ -1.0 , 1.0 };  // 적의 방향
normal_distribution <float>uid_mPos{ -60.0, 60.0 }; // 적의 초기위치
default_random_engine dre((size_t)time(NULL));

const float BOMB_TIME = 3.0;
const float VIEW_TIME = 10.0f;

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
    TIME_ITEM = 3,
    VIEW_ITEM = 4,
    FIXED_WALL = 5,
};

struct Shape {
    BOARD_TYPE type;
    Vector3 color;
    Vector3 scale;
    Vector3 pos;
    Vector3 dir;
    float radius;
    bool isAlive;
    int hitCount = 3;
    float speed;

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
void InitText();
GLvoid Reshape(int, int);
void drawScene();
void CalculateLight(float, float, float, float);

void CalculatePointLight(Shape shape, float amb);
int Loadfile(int mapCollect);
void DrawBoard();
void CameraSetting(GLuint s_program, Vector3 cameraPosition, Vector3 cameraDir, float yPos);

// 함수 선언
void throw_bomb();
void DrawPlayer();
void DrawCube();
void Draw2ndCube();
void DrawKey();
void InitGame(int num);
void InitShape();
float get_time();
float currentTime();
bool CollisionCheck(Shape, Shape);

// GL Function
void TimerFunction(int);
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

///////////////////////////////////////////////////////////////
// 맵 파일 입출력

int mapNumber;
Shape boardShape[SIZE][SIZE];
Shape monster[MONSTER_SIZE];

int key_sum = 0;

unsigned int texture[10];

Vector3 cameraPos = Vector3(0.0f, 10.0f, 40.0f);

Vector3 lightPos = Vector3(0.0f, 20.0f, 0.0f);
Vector3 lightColor = Vector3(1.0f, 1.0f, 1.0f);

uniform_real_distribution<> random_pos_urd{ -15.0, 15.0 };

Shape bombShape = Shape();
Shape player = Shape();
float InGameTime = 0;

bool bomb_mode = false;     // 폭탄 던지기

// time
float delta_time = 0.0f;
float lastFrame = 0.0f;
float addtionalTime = 0.0f;
bool isViewItem = false;
int viewItemCount = 0;
bool isPlayGame = true;
bool isClear = false;

unsigned int flaglocation;

bool isRestart = false;
bool isOver = false;

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

// 색 지정
glm::vec3 Red = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 Green = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 Blue = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 Gray = glm::vec3(0.7f, 0.7f, 0.7f);
glm::vec3 White = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Black = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 Yellow = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 Brown = glm::vec3(0.6f, 0.3f, 0.0f);

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(500, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Example18");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "GLEW Initialized\n";
    glewInit();

    mapNumber = rand() % 5 + 1;

    Loadfile(0);
    InitShape();
    InitBuffer();
    InitShader();
    InitText();

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
    //cout << player.pos.x << ", " << player.pos.z << endl;
}

// 큐브 그리기 함수 -> 맵 그릴 때 사용
void DrawCube(Shape shape)
{
    // get_bb(shape.GetBB());
    glm::mat4 S = glm::mat4(1.0f);
    glm::mat4 T = glm::mat4(1.0f);
    S = glm::scale(glm::mat4(1.0f), glm::vec3(shape.scale.x, shape.scale.y, shape.scale.z));
    T = glm::translate(glm::mat4(1.0f), glm::vec3(shape.pos.x, shape.pos.y + 1, shape.pos.z));
    glm::mat4 cubeSTR = T * S;
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
    glm::mat4 seccubeSTR = T * S;
    unsigned int transform2ndCube = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transform2ndCube, 1, GL_FALSE, glm::value_ptr(seccubeSTR));
    unsigned int colorCube = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorCube, shape.color.x, shape.color.y, shape.color.z);

    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size());
}

void renderBitmapString(float x, float y, float z, void* font, char* string)
{
    char* c;
    glRasterPos3f(x, y, z);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void PrintUI()
{
    if (isPlayGame) {
        //glClearColor(0.0, 0.0, 0.0, 0.0);
        string text = "TIME : " + to_string(currentTime()).substr(0, 4);
        const char* string = text.data();
        glColor3f(0.5f, 0.5f, 0.5f);
        glRasterPos2f(-0.9, 0.9);  // 문자 출력할 위치 설정

        int len = (int)strlen(string);
        for (int i = 0; i < len; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
        }

        text = "KEY : " + to_string(key_sum);
        string = text.data();
        glColor3f(0.5f, 0.5f, 0.5f);
        glRasterPos2f(-0.9, 0.8);  // 문자 출력할 위치 설정

        len = (int)strlen(string);
        for (int i = 0; i < len; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
        }
    }
    //else {
    //    string text = "SPACE BAR를 누르면 재시작 가능합니다.";
    //    const char* string = text.data();
    //    glColor3f(0.5f, 0.5f, 0.5f);
    //    glRasterPos2f(-0.2, -0.2);  // 문자 출력할 위치 설정

    //    int len = (int)strlen(string);
    //    for (int i = 0; i < len; i++) {
    //        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    //    }
    //    if (isClear) {
    //        text = "GAME CLEAR";
    //        const char* string = text.data();
    //        glColor3f(0.5f, 0.5f, 0.5f);
    //        glRasterPos2f(-0.2, 0.0);  // 문자 출력할 위치 설정

    //        int len = (int)strlen(string);
    //        for (int i = 0; i < len; i++) {
    //            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    //        }
    //    }
    //    else {
    //        text = "GAME OVER";
    //        const char* string = text.data();
    //        glColor3f(0.5f, 0.5f, 0.5f);
    //        glRasterPos2f(-0.2, 0.0);  // 문자 출력할 위치 설정

    //        int len = (int)strlen(string);
    //        for (int i = 0; i < len; i++) {
    //            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    //        }
    //    }
    //}

}

void DrawKey(Shape shape) {
    glm::mat4 S = glm::mat4(1.0f);
    glm::mat4 T = glm::mat4(1.0f);

    S = glm::scale(glm::mat4(1.0f), glm::vec3(shape.scale.x, shape.scale.y, shape.scale.z));
    T = glm::translate(glm::mat4(1.0f), glm::vec3(shape.pos.x, shape.pos.y, shape.pos.z));
    glm::mat4 cubeSTR = T * S;
    unsigned int transformCube = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transformCube, 1, GL_FALSE, glm::value_ptr(cubeSTR));
    unsigned int colorCube = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorCube, shape.color.x, shape.color.y, shape.color.z);

    glBindVertexArray(VAO[2]);
    glDrawArrays(GL_TRIANGLES, 0, pyramid_vertices.size());

    CalculatePointLight(shape, 0.5f);
}

void DrawPlain() {
    glm::mat4 S = glm::mat4(1.0f);
    glm::mat4 T = glm::mat4(1.0f);

    S = glm::scale(glm::mat4(1.0f), glm::vec3(100, 0.03, 100));
    T = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    glm::mat4 cubeSTR = T * S;
    unsigned int transformCube = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transformCube, 1, GL_FALSE, glm::value_ptr(cubeSTR));
    unsigned int colorCube = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorCube, 0.2, 0.2, 0.2);

    glBindVertexArray(VAO[2]);
    glDrawArrays(GL_TRIANGLES, 0, pyramid_vertices.size());
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
            case BOARD_TYPE::TIME_ITEM:
                DrawKey(boardShape[i][j]);
            case BOARD_TYPE::VIEW_ITEM:
                DrawKey(boardShape[i][j]);
                break;
            }
        }
    }
}

void InitGame(int num)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            boardShape[i][j].type = BOARD_TYPE::NONE;
        }
    }

    Loadfile(num);
    for (int i = 0; i < MONSTER_SIZE; i++)
        monster[i].isAlive = false;

    player.pos.x = 0;
    player.pos.y = 0;
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
    if (monster.isAlive) {
        glm::mat4 monsterSTR = glm::mat4(1.0f);
        qobj = gluNewQuadric();
        glm::vec3 monsterPos = glm::vec3(monster.pos.x, monster.pos.y, monster.pos.z);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(4, 4, 4));
        glm::mat4 T = glm::translate(glm::mat4(1.0f), monsterPos);
        monsterSTR = T * S;
        unsigned int Planet = glGetUniformLocation(s_program[0], "Transform");
        glUniformMatrix4fv(Planet, 1, GL_FALSE, glm::value_ptr(monsterSTR));

        unsigned int Color_Bomb = glGetUniformLocation(s_program[1], "in_Color");
        glUniform3f(Color_Bomb, monster.color.x, monster.color.y, monster.color.z);

        gluSphere(qobj, 1.0, 20, 20);
    }
}

void InitShape() {
    //PlaySound(TEXT(SOUND_FILE_NAME_BGM_1), NULL, SND_ASYNC | SND_SYNC);

    for (int i = 0; i < MONSTER_SIZE; i++) {
        monster[i].pos = Vector3(130 + uid_mPos(dre), 1.5f, 130 + uid_mPos(dre));
        monster[i].color = Vector3(uid_mColor(dre), uid_mColor(dre), uid_mColor(dre));
        monster[i].dir = Vector3(uid_mDir(dre), 0.0f, uid_mDir(dre));
        monster[i].speed = uid_mSpeed(dre);
        monster[i].radius = 4.0f;
        monster[i].isAlive = true;
    }

    bombShape.pos.y = 2.0f;
    player.radius = 0.3f;
}

void DrawGameOverImage() {
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.5f);
    glm::vec3 cameraDir = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.5f);

    glm::mat4 T = glm::mat4(1.0f);
    glm::mat4 V = glm::mat4(1.0f);
    glm::mat4 P = glm::mat4(1.0f);

    T = glm::translate(T, glm::vec3(-0.6f, 0.0f, 0.0f));

    unsigned int Line_Location = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Line_Location, 1, GL_FALSE, glm::value_ptr(T));

    unsigned int vLine_Location = glGetUniformLocation(s_program[0], "View");
    V = glm::lookAt(cameraPos, cameraDir, cameraUp);
    glUniformMatrix4fv(vLine_Location, 1, GL_FALSE, glm::value_ptr(V));

    unsigned int pLine_Location = glGetUniformLocation(s_program[0], "Projection");
    P = glm::perspective(glm::radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 200.0f);
    // P = glm::translate(P, glm::vec3(0.0f, 0.0f, -1.0f));
    glUniformMatrix4fv(pLine_Location, 1, GL_FALSE, glm::value_ptr(P));

    glm::mat4 TestT = glm::mat4(1.0f);
    unsigned int Test = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Test, 1, GL_FALSE, glm::value_ptr(TestT));

    flaglocation = glGetUniformLocation(s_program[0], "flag");
    glUniform1i(flaglocation, 1);

    glBindVertexArray(VAO[3]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

void DrawGameClearImage() {
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.3f);
    glm::vec3 cameraDir = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.5f);

    glm::mat4 T = glm::mat4(1.0f);
    glm::mat4 V = glm::mat4(1.0f);
    glm::mat4 P = glm::mat4(1.0f);

    T = glm::translate(T, glm::vec3(-0.6f, 0.0f, 0.0f));

    unsigned int Line_Location = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Line_Location, 1, GL_FALSE, glm::value_ptr(T));

    unsigned int vLine_Location = glGetUniformLocation(s_program[0], "View");
    V = glm::lookAt(cameraPos, cameraDir, cameraUp);
    glUniformMatrix4fv(vLine_Location, 1, GL_FALSE, glm::value_ptr(V));

    unsigned int pLine_Location = glGetUniformLocation(s_program[0], "Projection");
    P = glm::perspective(glm::radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 200.0f);
    // P = glm::translate(P, glm::vec3(0.0f, 0.0f, -1.0f));
    glUniformMatrix4fv(pLine_Location, 1, GL_FALSE, glm::value_ptr(P));

    glm::mat4 TestT = glm::mat4(1.0f);
    unsigned int Test = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Test, 1, GL_FALSE, glm::value_ptr(TestT));

    flaglocation = glGetUniformLocation(s_program[0], "flag");
    glUniform1i(flaglocation, 1);

    glBindVertexArray(VAO[3]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawScene()
{
    if (!isPlayGame) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else {
        glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CalculateLight(lightPos.x, lightPos.y, lightPos.z, 0.0);
    PrintUI();

    glUseProgram(s_program[0]);
    glUseProgram(s_program[1]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    CalculateLight(lightPos.x, lightPos.y, lightPos.z, 0.5);

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

    //if (isPlayGame)
    //    CameraSetting(s_program[0], player.pos, dir, yPos);
    //else
    //    CameraSetting(s_program[0], Vector3(1000, 1000, 1000), dir, yPos);

    if (isRestart) {
        if (isOver) {
            DrawGameOverImage();
        }
        else {
            DrawGameClearImage();
        }
    }
    else {
        glUniform1i(flaglocation, 0);

        if (deltaAngle)
            computeDir(deltaAngle);

        if (deltaMove)
            computePos(deltaMove);

        if (!isFPS)
            dir = Vector3(player.pos.x + lx * 100, player.pos.y + ly, player.pos.z + lz * 100);
        else {
            dir = Vector3(player.pos.x + lx, player.pos.y + ly, player.pos.z + lz);
            yPos *= 20;
        }
        CameraSetting(s_program[0], player.pos, dir, yPos);

        DrawPlain();
        DrawBoard();
        DrawPlayer(player);

        // 폭탄
        if (bomb_mode) {
            throw_bomb();
        }

        for (int i = 0; i < MONSTER_SIZE; i++) {
            DrawMonster(monster[i]);
        }
    }
    glutPostRedisplay();
    glutSwapBuffers();
}

void releaseKey(int key, int x, int y) {

    switch (key)
    {
    case GLUT_KEY_LEFT:
    case GLUT_KEY_RIGHT: deltaAngle = 0; break;

    case GLUT_KEY_UP:
    case GLUT_KEY_DOWN: deltaMove = 0; break;
    }
}

void RestartGame() {
    isClear = false;
    isPlayGame = true;
    isFPS = false;
    isRestart = false;
    isOver = false;
    key_sum = 0;
    Loadfile(0);
    InitShape();
    addtionalTime = 0;
    InGameTime = 0;
    player.pos = Vector3(0, 0, 0);
}

void Keyboard(unsigned char key, int x, int y) {
    if (!isPlayGame) {
        if (key == ' ') {
            RestartGame();
        }
        return;
    }
    switch (key) {
    case '1':
        if (isViewItem)
            isFPS = false;
        break;
    case '3':
        if (isViewItem)
            isFPS = true;
        break;
    case 'b':
        // 일단 보이는것만.. b로 구현함 ㅠㅠ
        if (!bomb_mode) {
            bombShape.pos = Vector3(player.pos.x, 2.0f, player.pos.z);
            bombShape.dir = Vector3(lx, 0, lz);
            bomb_mode = true;
        }
        break;
    case 'o':
        isRestart = true;
        isOver = true;
        break;
    case 'c':
        isRestart = true;
        isOver = false;
        break;
    }
}

void SpecialKeyboard(int key, int xx, int yy)
{
    if (!isPlayGame) {
        return;
    }

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
float elapsedFPSTime = VIEW_TIME;
Vector3 priorPos = Vector3();
void TimerFunction(int value) {
    get_time();

    InGameTime += delta_time;

    if (isPlayGame) {
        if (currentTime() < 0) {
            // GameOver;
            isRestart = true;
            isOver = true;
            InitGame(2);
            isPlayGame = false;
            isFPS = 3;
        }

        if (isFPS == true) {
            elapsedFPSTime -= 0.1;
            if (0 > elapsedFPSTime) {
                viewItemCount++;
                isFPS = false;
                elapsedFPSTime = VIEW_TIME;
                if (viewItemCount >= 1) {
                    isViewItem = true;
                }
            }
        }

        if (bomb_mode) {
            if (elapsedTime > 0) {
                elapsedTime -= 0.1f;
                bombShape.pos.x += bombShape.dir.x;
                bombShape.pos.z += bombShape.dir.z;
                // Monster Collision
                for (int i = 0; i < MONSTER_SIZE; i++) {
                    if (monster[i].isAlive == false)
                        continue;

                    if (CollisionCheck(monster[i], bombShape)) {
                        monster[i].hitCount--;
                        monster[i].color.x -= 0.1;
                        monster[i].color.y -= 0.1;
                        monster[i].color.z -= 0.1;
                        if (monster[i].hitCount <= 0) {
                            monster[i].isAlive = false;
                        }
                        bombShape.pos.x = 0;
                        bombShape.pos.z = 0;
                        bomb_mode = false;
                        elapsedTime = BOMB_TIME;

                        printf("Collision MONSTER to BOMB\n");
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
            if (monster[i].isAlive == false)
                continue;

            monster[i].pos.x += 0.01 * monster[i].dir.x * monster[i].speed;
            monster[i].pos.z += 0.01 * monster[i].dir.z * monster[i].speed;
            if (CollisionCheck(monster[i], player)) {
                monster[i].pos.x += lx * 10;
                monster[i].pos.z += lz * 10;
                addtionalTime -= 10.f;
                cout << "Monster and Player collide" << endl;
            }
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
                    if (CollisionCheck(boardShape[i][j], player)) {
                        player.pos.x = priorPos.x;
                        player.pos.z = priorPos.z;
                        cout << "[Collision] WALL PLAYER_" << i << "_" << j << endl;
                    }
                    break;
                case BOARD_TYPE::FIXED_WALL:
                    if (CollisionCheck(boardShape[i][j], player)) {
                        player.pos.x = priorPos.x;
                        player.pos.z = priorPos.z;
                        cout << "[Collision] WALL PLAYER_" << i << "_" << j << endl;
                    }
                    for (int k = 0; k < MONSTER_SIZE; ++k) {
                        if (CollisionCheck(boardShape[i][j], monster[k])) {
                            monster[k].pos.x -= monster[k].dir.x * 0.5f;
                            monster[k].pos.z -= monster[k].dir.z * 0.5f;
                            monster[k].dir.x *= -1;
                            monster[k].dir.z *= -1;
                        }
                    }
                    break;
                case BOARD_TYPE::ITEM:
                    if (CollisionCheck(boardShape[i][j], player)) {
                        boardShape[i][j].type = NONE;
                        key_sum++;
                        if (key_sum >= MAX_ITEM) {
                            isRestart = true;
                            isOver = false;
                            InitGame(1);
                            isPlayGame = false;
                            isClear = true;
                        }
                        cout << "[Collision] ITEM PLAYER_" << i << "_" << j << endl;
                        cout << "현재 아이템 SCORE : " << key_sum << endl;
                    }
                    break;
                case BOARD_TYPE::TIME_ITEM:
                    if (CollisionCheck(boardShape[i][j], player)) {
                        boardShape[i][j].type = NONE;
                        addtionalTime += 5.0f;
                        cout << "[Collision] TIME ITEM PLAYER_" << i << "_" << j << endl;
                    }
                    break;
                case BOARD_TYPE::VIEW_ITEM:
                    if (CollisionCheck(boardShape[i][j], player)) {
                        boardShape[i][j].type = NONE;
                        isFPS = true;

                        cout << "[Collision] VIEW ITEM PLAYER_" << i << "_" << j << endl;
                        cout << "현재 아이템 SCORE : " << key_sum << endl;
                    }
                    break;
                }
            }
        }
        priorPos = player.pos;
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
    return MAX_TIME - round(InGameTime / 100) / 10 + addtionalTime;
}

// 폭탄
void throw_bomb() {
    qobj = gluNewQuadric();
    glm::vec3 bomb_pos = glm::vec3(bombShape.pos.x, bombShape.pos.y, bombShape.pos.z);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0));
    glm::mat4 T = glm::translate(glm::mat4(1.0f), bomb_pos);
    glm::mat4 Bomb_STR = S * T;
    unsigned int Bomb = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Bomb, 1, GL_FALSE, glm::value_ptr(Bomb_STR));

    unsigned int Color_Bomb = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(Color_Bomb, Red.r, Red.g, Red.b);

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
        fp = fopen("MAP_1.txt", "rt");
        break;
    case 1:
        fp = fopen("CLEAR_MAP.txt", "rt");
        break;
    case 2:
        fp = fopen("GAMEOVER_MAP.txt", "rt");
        break;
    case 3:
        fp = fopen("MAP_4.txt", "rt");
        break;
    case 4:
        fp = fopen("MAP_5.txt", "rt");
        break;
    }

    if (fp == NULL)
    {
        printf("\n board gen fail\n");
        return 1;
    }

    int cha;

    while (feof(fp) == 0)
    {
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                fscanf(fp, "%d", &cha);

                boardShape[i][j].type = (BOARD_TYPE)cha;

                boardShape[i][j].pos = Vector3((i * 7.5f - 35), 0, (j * 7.5f - 35));

                switch (boardShape[i][j].type) {
                case BOARD_TYPE::ITEM:
                    boardShape[i][j].color = Vector3(Yellow.r, Yellow.g, Yellow.b);
                    boardShape[i][j].scale = Vector3(1.0, 1.0, 1.0);
                    boardShape[i][j].radius = 3.5f;
                    break;
                case BOARD_TYPE::TIME_ITEM:
                    boardShape[i][j].color = Vector3(Red.r, Red.g, Red.b);
                    boardShape[i][j].scale = Vector3(1.0, 1.0, 1.0);
                    boardShape[i][j].radius = 3.5f;
                    break;
                case BOARD_TYPE::VIEW_ITEM:
                    boardShape[i][j].color = Vector3(Blue.r, Blue.g, Blue.b);
                    boardShape[i][j].scale = Vector3(1.0, 1.0, 1.0);
                    boardShape[i][j].radius = 3.5f;
                    break;
                case BOARD_TYPE::FIXED_WALL:
                    boardShape[i][j].radius = 3.5f;
                    boardShape[i][j].scale = Vector3(3.0, 3.0, 3.0);
                    boardShape[i][j].color = Vector3(0.7, 0.7, 0.7);
                    break;
                case BOARD_TYPE::WALL:
                    boardShape[i][j].radius = 3.5f;
                    boardShape[i][j].scale = Vector3(3.0, 3.0, 3.0);
                    boardShape[i][j].color = Vector3(0.3, 0.3, 0.3);
                    break;
                }
            }
        }
    }

    fclose(fp);

    return 1;
}

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

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Background), Background, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //--- 위치 속성
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말값 속성
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //--- 텍스처 좌표 속성
    glEnableVertexAttribArray(2);
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
    unsigned int light_pos = glGetUniformLocation(s_program[0], "g_lightPos");
    glUniform3f(light_pos, lgt_x, lgt_y, lgt_z);

    unsigned int light_color = glGetUniformLocation(s_program[0], "g_lightColor");
    glUniform3f(light_color, lightColor.x, lightColor.y, lightColor.z);

    unsigned int view_pos = glGetUniformLocation(s_program[0], "g_cameraPos");
    glUniform3f(view_pos, player.pos.x, player.pos.y, player.pos.z);

    unsigned int ambientLight_on = glGetUniformLocation(s_program[0], "g_lightAmbient");
    glUniform3f(ambientLight_on, amb, amb, amb);
}

void CalculatePointLight(Shape shape, float amb) {
    unsigned int light_pos = glGetUniformLocation(s_program[0], "g_lightPos");
    glUniform3f(light_pos, shape.pos.x, shape.pos.y + 2, shape.pos.z);

    unsigned int light_color = glGetUniformLocation(s_program[0], "g_lightColor");
    glUniform3f(light_color, lightColor.x, lightColor.y, lightColor.z);

    unsigned int view_pos = glGetUniformLocation(s_program[0], "g_cameraPos");
    glUniform3f(view_pos, player.pos.x, player.pos.y, player.pos.z);

    unsigned int ambientLight_on = glGetUniformLocation(s_program[0], "g_lightAmbient");
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
        zcollision = true;
    }

    if (xcollision && zcollision) {
        return true;
    }
    else
        return false;
}

void InitText() {
    int width[10], height[10], nrChannels[10];
    stbi_set_flip_vertically_on_load(true);

    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data1 = stbi_load("GameOver.png", &width[0], &height[0], &nrChannels[0], 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[0], height[0], 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data1);

    glGenTextures(1, &texture[1]);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data2 = stbi_load("gclear.png", &width[1], &height[1], &nrChannels[1], 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[1], height[1], 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data2);

    glGenTextures(1, &texture[2]);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data3 = stbi_load("bg.png", &width[2], &height[2], &nrChannels[2], 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[2], height[2], 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data3);
}