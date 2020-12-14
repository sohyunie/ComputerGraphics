#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
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

#define SIZE 22 // 맵 사이즈
using namespace std;

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
void Mouse(int, int, int, int);
void CalculateLight(float, float, float, float);
int Loadfile(int mapCollect);
void DrawBoard();
void CameraSetting(GLuint s_program, Vector3 cameraPosition, Vector3 cameraDir);

// 함수 선언
void throw_bomb();
void DrawPlayer();
void DrawCube();
void Draw2ndCube();
void DrawEnemy();
void DrawKey();

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

int mapCollect = rand() % 4;
float colorbuffer[4][3] = { 0 };

Shape boardShape[SIZE][SIZE];

float objectSize = 1;
bool isChanged = false;

float r;
float g;
float b;

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
float player_zpos = 20.0f;

Vector3 cameraDir = Vector3();

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

float bomb_z_pos = player_zpos - 5.0;

// 충돌체크를 위한 radius 선언
float player_radius = 1.0f;
float enemy_radius = 0.75f;
float bomb_radius = 0.3f;

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
glm::mat4 cubeSTR = glm::mat4(1.0f);
glm::mat4 seccubeSTR = glm::mat4(1.0f);
glm::mat4 thdcubeSTR = glm::mat4(1.0f);
glm::mat4 EnemySTR = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

void main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(500, 100);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("Example18");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "GLEW Initialized\n";
    glewInit();


    Loadfile(1);
    InitBuffer();
    InitShader();
    InitTexture();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(50, TimerFunction, 1);
    glutMainLoop();
}

// 플레이어 그리기 함수
void DrawPlayer()
{
    glm::mat4 STR = glm::mat4(1.0f);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.7, 0.7, 0.7));
    glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), float(glm::radians(180.0f)), glm::vec3(0.0, 1.0, 0.0));
    STR = Ry * S;

    glm::mat4 cubeSTR = glm::mat4(1.0f);
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3((float)player_xpos, (float)player_ypos, (float)player_zpos));
    cubeSTR = T * Ry * S;
    unsigned int Player = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Player, 1, GL_FALSE, glm::value_ptr(cubeSTR));
    unsigned int Color_Player = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(Color_Player, Blue.r, Blue.g, Blue.b);

    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_TRIANGLES, 0, robot_vertices.size());
}

// 큐브 그리기 함수 -> 맵 그릴 때 사용
void DrawCube(Shape shape)
{
    glm::mat4 S = glm::mat4(1.0f);
    glm::mat4 T = glm::mat4(1.0f);
    glm::mat4 STR = glm::mat4(1.0f);

    S = glm::scale(glm::mat4(1.0f), glm::vec3(shape.scale.x, shape.scale.y, shape.scale.z));
    STR *= S;

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
    glm::mat4 STR = glm::mat4(1.0f);

    S = glm::scale(glm::mat4(1.0f), glm::vec3(shape.scale.x, shape.scale.y, shape.scale.z));
    STR *= S;

    T = glm::translate(glm::mat4(1.0f), glm::vec3(shape.pos.x, shape.pos.y + 2.0f, shape.pos.z));
    seccubeSTR = S * T;
    unsigned int transform2ndCube = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transform2ndCube, 1, GL_FALSE, glm::value_ptr(seccubeSTR));
    unsigned int colorCube = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorCube, shape.color.x, shape.color.y, shape.color.z);

    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size());
}

void Draw3ndCube(Shape shape) {
    glm::mat4 S = glm::mat4(1.0f);
    glm::mat4 T = glm::mat4(1.0f);
    glm::mat4 STR = glm::mat4(1.0f);

    S = glm::scale(glm::mat4(1.0f), glm::vec3(shape.scale.x, shape.scale.y, shape.scale.z));
    STR *= S;

    T = glm::translate(glm::mat4(1.0f), glm::vec3(shape.pos.x, shape.pos.y + 4.0f, shape.pos.z));
    thdcubeSTR = S * T;
    unsigned int transform3rdCube = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transform3rdCube, 1, GL_FALSE, glm::value_ptr(thdcubeSTR));
    unsigned int colorCube = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorCube, shape.color.x, shape.color.y, shape.color.z);

    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size());
}

void DrawEnemy() {
    //glBindVertexArray(vao[1]);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, texture[0]);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

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
    glm::mat4 STR = glm::mat4(1.0f);

    S = glm::scale(glm::mat4(1.0f), glm::vec3(shape.scale.x, shape.scale.y, shape.scale.z));
    STR *= S;

    T = glm::translate(glm::mat4(1.0f), glm::vec3(shape.pos.x, shape.pos.y, shape.pos.z));
    cubeSTR = S * T;
    unsigned int transformCube = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(transformCube, 1, GL_FALSE, glm::value_ptr(cubeSTR));
    unsigned int colorCube = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(colorCube, shape.color.x, shape.color.y, shape.color.z);

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
                break;
            }
        }
    }
}

void drawScene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(s_program[0]);
    glUseProgram(s_program[1]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    CalculateLight(light_x, light_y, light_z, 0.5);

    //// 시점
    //if (!threed_mode) {
    //    view = glm::lookAt(cameraPosM, cameraPosM + cameraFront, cameraUp);
    //    GLuint viewlocation = glGetUniformLocation(s_program[0], "View");
    //    glUniformMatrix4fv(viewlocation, 1, GL_FALSE, value_ptr(view));
    //}
    //else {
    //    view = glm::lookAt(cameraPos3, cameraDirection, cameraUp);
    //    GLuint viewlocation = glGetUniformLocation(s_program[0], "View");
    //    glUniformMatrix4fv(viewlocation, 1, GL_FALSE, value_ptr(view));
    //}

    //glm::mat4 projection = glm::mat4(1.0f);
    //projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    //GLuint Projectionlocation = glGetUniformLocation(s_program[0], "Projection");
    //glUniformMatrix4fv(Projectionlocation, 1, GL_FALSE, value_ptr(projection));

    CameraSetting(s_program[0], Vector3(player_xpos, player_ypos, player_zpos), cameraDir);
    DrawBoard();
    DrawPlayer();

    if (enemy_valid) {
        DrawEnemy();
    }

    // 폭탄
    if (bomb_mode) {
        throw_bomb();
        // throw_bomb = false;
        // 적 구현 이후에 timer함수에서 움직이는거 구현
    }

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
        //if (!threed_mode) {
        //    cameraPos += cameraSpeed * cameraFront;
        //}
        player_zpos -= 1.0f;
        /*cameraDir = Vector3(0, 0, -1);*/
        break;
    case 'a':
        //if (!threed_mode) {
        //    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        //}
        player_xpos -= 1.0f;
        //cameraDir = Vector3(-1, 0, 0);
        break;
    case 's':
        //if (!threed_mode) {
        //    cameraPos -= cameraFront * cameraSpeed;
        //}
        player_zpos += 1.0f;
        //cameraDir = Vector3(0, 0, 1);
        break;
    case 'd':
        //if (!threed_mode) {
        //    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        //}
        player_xpos += 1.0f;
        //cameraDir = Vector3(1, 0, 0);
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
    case 'e':
        random_xpos = random_pos_urd(dre);
        enemy_xpos = random_xpos;
        enemy_valid = true;
        break;
    }
    glutPostRedisplay();
}

void TimerFunction(int value) {
    if (bomb_mode) {
        if (bomb_z_pos > -50) {
            bomb_z_pos -= 0.3f;
            // 충돌체크 여기다 해주면 됨
        }
        else {
            bomb_z_pos = player_zpos - 5.0f;
            bomb_mode = false;
        }
    }

    if (enemy_valid) {
        // 임시 충돌체크
        if (enemy_zpos <= (player_zpos - 5.0)) {
            enemy_zpos += 0.5f;
        }
        else {
            enemy_zpos = -30.0f;
            enemy_valid = false;
        }
    }
    glutTimerFunc(10, TimerFunction, 1);
}

// 카메라 시점 변환시 사용하는 마우스 콜백 함수
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
    //cameraFront = glm::normalize(front);

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


// 잔여 HP 표현하는 delta_time 얻는 함수
void get_time() {
    float currentFrame = glutGet(GLUT_ELAPSED_TIME);
    delta_time = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

// 폭탄
void throw_bomb() {
    qobj = gluNewQuadric();
    glm::vec3 bomb_pos = glm::vec3(player_xpos, 0.0f, bomb_z_pos);
    S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0));
    T = glm::translate(glm::mat4(1.0f), bomb_pos);
    Bomb_STR = S * T;
    unsigned int Planet = glGetUniformLocation(s_program[0], "Transform");
    glUniformMatrix4fv(Planet, 1, GL_FALSE, glm::value_ptr(Bomb_STR));

    unsigned int Color_Bomb = glGetUniformLocation(s_program[1], "in_Color");
    glUniform3f(Color_Bomb, Red.r, Red.g, Red.b);

    CalculateLight(bomb_pos.x, bomb_pos.y, bomb_pos.z, 1.0);

    gluSphere(qobj, 0.5, 20, 20);
}

void get_bb() {

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


                if (boardShape[i][j].type == ITEM) {
                    boardShape[i][j].color = Vector3(Yellow.r, Yellow.g, Yellow.b);
                    boardShape[i][j].scale = Vector3(0.5, 0.5, 0.5);

                    if (i < SIZE / 2 && j < SIZE / 2) {
                        boardShape[i][j].pos = Vector3((i * 9.0f - 15), 0.0f, (j * 9.0f - 15));
                    }
                    else if (i < SIZE / 2 && j > SIZE / 2) {
                        boardShape[i][j].pos = Vector3((i * 9.0f - 15), 0.0f, j * 9.0f);
                    }
                    else if (i > SIZE / 2 && j < SIZE / 2) {
                        boardShape[i][j].pos = Vector3(i * 9.0f, 0.0f, (j * 9.0f - 15));
                    }
                    else if (i > SIZE / 2 && j > SIZE / 2) {
                        boardShape[i][j].pos = Vector3(i * 9.0f, 0.0f, j * 9.0f);
                    }
                }
                else {
                    boardShape[i][j].scale = Vector3(3.0, 3.0, 3.0);

                    if (i < SIZE / 2 && j < SIZE / 2) {
                        boardShape[i][j].pos = Vector3((i * 2.5f - 15), 0, (j * 2.5f - 15));
                    }
                    else if (i < SIZE / 2 && j > SIZE / 2) {
                        boardShape[i][j].pos = Vector3((i * 2.5f - 15), 0, j * 2.5f);
                    }
                    else if (i > SIZE / 2 && j < SIZE / 2) {
                        boardShape[i][j].pos = Vector3(i * 2.5f, 0, (j * 2.5f - 15));
                    }
                    else if (i > SIZE / 2 && j > SIZE / 2) {
                        boardShape[i][j].pos = Vector3(i * 2.5f, 0, j * 2.5f);
                    }

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

// 이젠 맵을 그릴거예요

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

    // 	
    /*glBindVertexArray(VAO[2]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RecF), RecF, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RecR), RecR, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RecL), RecL, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RecT), RecT, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RecD), RecD, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RecB), RecB, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);*/
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

void InitTexture()
{
    int width[10], height[10], nrChannels[10];
    stbi_set_flip_vertically_on_load(true);
    //BITMAPINFO* bmp;
    //--- texture 1
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data1 = stbi_load("1.bmp", &width[0], &height[0], &nrChannels[0], 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[0], height[0], 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
    glGenerateMipmap(GL_TEXTURE_2D);
    //glUseProgram(s_program);
    //int tLocation1 = glGetUniformLocation(s_program, "exTexture");
    //glUniform1i(tLocation1, 0);
    stbi_image_free(data1);


    //--- texture 2
    glGenTextures(1, &texture[1]);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load("2.bmp", &width[1], &height[1], &nrChannels[1], 0);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width[1], height[1], 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
    stbi_image_free(data2);

    //--- texture 3
    glGenTextures(1, &texture[2]);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data3 = stbi_load("3.bmp", &width[2], &height[2], &nrChannels[2], 0);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width[2], height[2], 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
    stbi_image_free(data3);
    //--- texture 3
    glGenTextures(1, &texture[3]);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data4 = stbi_load("4.bmp", &width[3], &height[3], &nrChannels[3], 0);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width[3], height[3], 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
    stbi_image_free(data4);

    glGenTextures(1, &texture[4]);
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data5 = stbi_load("5.bmp", &width[4], &height[4], &nrChannels[4], 0);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width[4], height[4], 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
    stbi_image_free(data5);


    glGenTextures(1, &texture[5]);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data6 = stbi_load("6.bmp", &width[5], &height[5], &nrChannels[5], 0);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width[5], height[5], 0, GL_RGB, GL_UNSIGNED_BYTE, data6);
    stbi_image_free(data6);
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

float camXpos = 0.0f;
float camYpos = 0.0f;
float camZpos = 0.0f;

void CameraSetting(GLuint s_program, Vector3 cameraPosition, Vector3 cameraDir)
{
    glm::mat4 TR = glm::mat4(1.0f);
    glm::mat4 Tx = glm::mat4(1.0f);
    glm::mat4 TxY = glm::mat4(1.0f);
    //int modelLoc = glGetUniformLocation(s_program, "Transform");
        //Tx = glm::translate(Tx, glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));
        //Rx = glm::rotate(Rx, glm::radians((float)cameraRotateangle), glm::vec3(0.0, 1.0, 0.0));
        //TR = Tx;
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(TR));

    int projLoc = glGetUniformLocation(s_program, "Projection");
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians((float)45.0f), 800.f / 600.f, 0.1f, 100.0f);
    camXpos = player_xpos;
    camYpos = player_ypos + 6.0f;
    camZpos = player_zpos - 3.0f;
    glm::vec3 cameraPos = glm::vec3(camXpos, camYpos, camZpos);
    glm::vec3 cameraDirection = glm::vec3(camXpos, camYpos - 2.0f, camZpos - 7.0f);
    /*glm::vec3 cameraPos = glm::vec3(player_xpos, player_ypos, player_zpos);
    glm::vec3 cameraDirection = glm::vec3(0, 0, 0);
    */
    //cout << "cameraPosition" << endl;
     //cout << cameraDirection.x << endl;
     //cout << cameraDirection.y << endl;
     //cout << cameraDirection.z << endl;
    glm::vec3 cameraUp = glm::vec3(0, 1, 0);
    glm::mat4 view = glm::mat4(1.0f);

    int viewLoc = glGetUniformLocation(s_program, "View");

    view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    //RxY = glm::rotate(RxY, glm::radians((float)cameraRotateangleY), glm::vec3(0.0, 1.0, 0.0));
        //Tx = glm::translate(Tx, glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));
        //view *= Tx;

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
}