#pragma warning(disable : 4996)
#include <iostream>
#include "Angel.h" 
#include "shader.h"

using namespace std;

// ---------- INITIALIZE VARIABLES BEGINNING ----------
typedef vec4 color4;
typedef vec4 point4;

bool lightMove = false;

vec3 location(-0.9, 0.8, -2.8);
vec3 velocity(0.0, 0.0000001, 0.0);
vec3 accelaration(0.0, 0.00000001, 0.0);
vec4 white(1.0, 1.0, 1.0, 1.0);
vec4 currentColor = white;

static int window;
static int menu_id;
static int materialSelectionMenu;
static int colorSelectionMenu;
static int drawingModeMenu;
static int lightTypeSelectionMenu;
static int lightOnOffMenu;
static int shadingTypeSelectionMenu;
static int textureSelectionMenu;
static int lightPositionSelectionMenu;

static int valueMenu = 0;
int colorSelector = 0;

enum {
    Xaxis = 0,
    Yaxis = 1,
    Zaxis = 2,
    NumAxes = 3
};

int Axis = Xaxis;
int Zmeter = Zaxis;

GLfloat Theta[NumAxes] = { 0.0, -20, 20 };
GLfloat aspect;
GLuint  ModelView, Projection;
GLuint  program;

void colorChanger(std::string);

point4 light_position(10.0, 20.0, -5.0, 1.0);
color4 light_ambient(0.1, 0.1, 0.1, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.4, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);

color4 ambient_product;
color4 diffuse_product;
color4 specular_product;
color4 ambient_product2;
color4 diffuse_product2;
color4 specular_product2;

float  material_shininess = 1.0;

GLuint textures[2];
GLubyte* basketballImage;
GLubyte* earthImage;
// ---------- INITIALIZE VARIABLES END ----------

// ---------- SPHERE BEGINNING ----------
const int NumTriangles = 1024;
const int NumVerticesSphare = 3 * NumTriangles;

point4 pointsSphere[NumVerticesSphare];
vec3   normalsSphere[NumVerticesSphare];
color4 colorSphere[NumVerticesSphare];
vec3   texture[NumVerticesSphare];

int indexSphere = 0;

// taken from textbook - Appendix 7
void triangle(const point4& a, const point4& b, const point4& c) {

    vec3  normal = normalize(cross(b - a, c - b));

    normalsSphere[indexSphere] = normal;
    colorSphere[indexSphere] = currentColor;
    pointsSphere[indexSphere] = a;
    texture[indexSphere] = vec3(a.x, a.y, a.z);
    indexSphere++;

    normalsSphere[indexSphere] = normal;
    colorSphere[indexSphere] = currentColor;
    pointsSphere[indexSphere] = b;
    texture[indexSphere] = vec3(b.x, b.y, b.z);
    indexSphere++;

    normalsSphere[indexSphere] = normal;
    colorSphere[indexSphere] = currentColor;
    pointsSphere[indexSphere] = c;
    texture[indexSphere] = vec3(c.x, c.y, c.z);
    indexSphere++;
}

float sizeSphere = 5.0;

// taken from textbook - Appendix 7
point4 unit(const point4& p) {

    float len = p.x * p.x + p.y * p.y + p.z * p.z;
    point4 t;

    if (len > DivideByZeroTolerance) {
        t = p / sqrt(len);
        t.w = sizeSphere;
    }

    return t;
}

// taken from textbook - Appendix 7
void divide_triangle(const point4& a, const point4& b, const point4& c, int count) {

    if (count > 0) {
        point4 v1 = unit(a + b);
        point4 v2 = unit(a + c);
        point4 v3 = unit(b + c);
        divide_triangle(a, v1, v2, count - 1);
        divide_triangle(c, v2, v3, count - 1);
        divide_triangle(b, v3, v1, count - 1);
        divide_triangle(v1, v3, v2, count - 1);
    } else {
        triangle(a, b, c);
    }
}

void tetrahedron(int count) {

    point4 v[4] = {

        vec4(0.0, 0.0, 1.0, 100.0),
        vec4(0.0, 0.942809, -0.333333, 100.0),
        vec4(-0.816497, -0.471405, -0.333333, 100.0),
        vec4(0.816497, -0.471405, -0.333333, 100.0)
    };

    divide_triangle(v[0], v[1], v[2], count);
    divide_triangle(v[3], v[2], v[1], count);
    divide_triangle(v[0], v[3], v[1], count);
    divide_triangle(v[0], v[2], v[3], count);
}
// ---------- SPHERE END ----------

// ---------- READ PPM BEGINNING ----------
// taken from the example in the homework pdf
void readPPMofBasketball() {

    FILE* fd;
    int k, n, m;
    char c;
    int i;
    char b[] = "basketball.ppm";
    int red, blue, green;

    fd = fopen(b, "r");
    fscanf(fd, "%[^\n] ", b);

    if (b[0] != 'P' || b[1] != '3') {
        printf("Not a ppm file.");
        exit(1);
    }
    printf("Valid file.");
    fscanf(fd, "%c", &c);

    while (c == '#') {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n", b);
        fscanf(fd, "%c", &c);
    }

    ungetc(c, fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);

    int nm = n * m;

    basketballImage = (GLubyte*)malloc(3 * sizeof(GLubyte) * nm);

    for (i = nm; i > 0; i--) {

        fscanf(fd, "%d %d %d", &red, &green, &blue);
        basketballImage[3 * nm - 3 * i] = red;
        basketballImage[3 * nm - 3 * i + 1] = green;
        basketballImage[3 * nm - 3 * i + 2] = blue;

    }
}

// taken from the example in the homework pdf
void readPPMofWorldMap() {

    FILE* fd;
    int k, n, m;
    char c;
    int i;
    char b[] = "earth.ppm";
    int red, blue, green;

    fd = fopen(b, "r");
    fscanf(fd, "%[^\n] ", b);

    if (b[0] != 'P' || b[1] != '3') {
        printf("File is not a PPM file.");
        exit(0);
    }
    printf("Valid PPM file.");
    fscanf(fd, "%c", &c);

    while (c == '#') {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n", b);
        fscanf(fd, "%c", &c);
    }

    ungetc(c, fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);

    int nm = n * m;

    earthImage = (GLubyte*)malloc(3 * sizeof(GLubyte) * nm);

    for (i = nm; i > 0; i--) {

        fscanf(fd, "%d %d %d", &red, &green, &blue);
        earthImage[3 * nm - 3 * i] = red;
        earthImage[3 * nm - 3 * i + 1] = green;
        earthImage[3 * nm - 3 * i + 2] = blue;

    }
}
// ---------- READ PPM END ----------

// ---------- INIT BEGINNING ----------
void init() {
    readPPMofBasketball();
    readPPMofWorldMap();

    glewInit();

    indexSphere = 0;
    tetrahedron(4);

    glGenTextures(2, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0,
        GL_RGB, GL_UNSIGNED_BYTE, basketballImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 1024, 0,
        GL_RGB, GL_UNSIGNED_BYTE, earthImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textures[1]);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(colorSphere) + sizeof(normalsSphere) + sizeof(texture), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointsSphere), pointsSphere);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsSphere), sizeof(colorSphere), colorSphere);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(colorSphere), sizeof(normalsSphere), normalsSphere);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(colorSphere) + sizeof(normalsSphere), sizeof(texture), texture);

    Shader ourShader("vshader.glsl", "fshader.glsl");
    ourShader.use();
    program = ourShader.getProgram();

    glUniform4fv(glGetUniformLocation(program, "colorDefiner"), 1, currentColor);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsSphere)));

    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsSphere) + sizeof(colorSphere)));

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsSphere) + sizeof(colorSphere) + sizeof(normalsSphere)));

    glUniform1f(glGetUniformLocation(program, "Flag"), 0);
    glUniform1i(glGetUniformLocation(program, "phong"), 1);

    ambient_product2 = light_ambient * material_ambient;
    diffuse_product2 = light_diffuse * material_diffuse;
    specular_product2 = light_specular * material_specular;

    point4 light_position(0.1, 0.1, -5.8, 1.0);
    color4 light_ambient(0.1, 0.1, 0.1, 1.0);

    color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
    color4 light_specular(1.0, 1.0, 1.0, 1.0);

    color4 material_ambient(1.0, 0.0, 1.0, 1.0);
    color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
    color4 material_specular(1.0, 0.8, 0.0, 1.0);
    float  material_shininess = 100.0;

    ambient_product = light_ambient * material_ambient;
    diffuse_product = light_diffuse * material_diffuse;
    specular_product = light_specular * material_specular;

    glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
    glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
    glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
    glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
    glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);

    glClearColor(0.0, 0.0, 0.0, 1.0);
}
// ---------- INIT END ----------

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4  model_view = (Scale(1.0, 1.0, 1.0) * Translate(location) *
        RotateX(Theta[Xaxis]) *
        RotateY(Theta[Yaxis]) *
        RotateZ(Theta[Zaxis]));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphare);
    glutSwapBuffers();
}

// taken from textbook - Appendix 7
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    GLfloat aspect = GLfloat(width) / height;
    mat4  projection = Perspective(45.0, aspect, 0.001, 100);
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

static void Timer(int n) { 

    if (location.y <= -0.8) {
        velocity = vec3(-0.008, -velocity.y * 0.75 + 0.00000001, 0.0);
    }

    if (location.y >= 0.8) {
        velocity = vec3(-0.008, -velocity.y * 0.75, 0.0);
    }

    if (location.y < -0.8) {
        location.y = -0.8;
    }

    velocity.operator+=(accelaration);
    accelaration.y = 0.001;


    if (lightMove) {
        vec4 newspecular(1.0, 1.0, 1.0, 1.0);
        glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, newspecular);
        light_position = vec4(light_position.x + location.x, light_position.y + location.y, light_position.z, 1.0);
        glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
    }
    else {
        glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);

        vec4 light_position2 = vec4(1.0, 1.0, -1.0, 1.0);

        glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position2);
    }

    location.operator-=(velocity);

    glutTimerFunc(10, Timer, 0);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 033:
    case 'q':
    case 'Q':
        exit(EXIT_SUCCESS);
    case 'i':
    case 'I':
        location = vec3(-0.9, 0.8, -2.8);
    case 'h':
    case 'H':
        printf("Instructions\n");
        printf("Left Click: Open Menu\n");
        printf("i/I: init position\n");
        printf("q/Q: exit from program\n");
    }
}

void menu(int num) { 
    if (num == 0) {
        glutDestroyWindow(window);
        exit(0);
    }
    else if (num == 5) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (num == 6) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUniform1i(glGetUniformLocation(program, "flag"), 2);
    }
    else if (num == 3) {
        glUniform1i(glGetUniformLocation(program, "flag"), 1);
        colorChanger("blue");
    }
    else if (num == 4) {
        glUniform1i(glGetUniformLocation(program, "flag"), 1);
        colorChanger("red");
    }
    else if (num == 7) {
        glUniform1i(glGetUniformLocation(program, "flag"), 1);
        colorChanger("green");
    }
    else if (num == 8) {
        glUniform1i(glGetUniformLocation(program, "flag"), 1);
        colorChanger("yellow");
    }
    else if (num == 9) {
        glUniform1i(glGetUniformLocation(program, "flag"), 1);
        colorChanger("white");
    }
    else if (num == 11) {
        material_shininess = 5;
        glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
    }
    else if (num == 2) { 
        material_shininess = 1000000;
        glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
    }
    else if (num == 21) {
        glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product2);
        glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product2);
        glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product2);
        glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
    }
    else if (num == 22) { 
        glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
        glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
        glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
        glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
    }
    else if (num == 31) {
        ambient_product = light_ambient * material_ambient; 
        diffuse_product = light_diffuse * material_diffuse;
        specular_product = light_specular * material_specular; 

        glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
        glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
        glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
        glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
    }
    else if (num == 32) { 
        ambient_product = 0; 
        diffuse_product = 0; 
        specular_product = 0; 

        glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
        glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
        glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
        glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
    }
    else if (num == 51) { 
        colorChanger("white");
        glUniform1i(glGetUniformLocation(program, "flag"), 0);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
    }
    else if (num == 52) { 
        colorChanger("white");
        glUniform1i(glGetUniformLocation(program, "flag"), 0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
    }
    else if (num == 52) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
        glUniform1i(glGetUniformLocation(program, "flag"), 0);
    }
    else if (num == 41) { 
        glUniform1i(glGetUniformLocation(program, "phong"), 1);
    }
    else if (num == 42) {
        glUniform1i(glGetUniformLocation(program, "phong"), 0);
    }
    else if (num == 61) {
        lightMove = false;
    }
    else if (num == 62) {
        lightMove = true;
    }
    else { valueMenu = num; }

    glutPostRedisplay();
}

void colorChanger(std::string k) { 

    if (k == "blue") {
        vec4 blue(0.0, 0.0, 1.0, 1.0);
        currentColor = blue;
    } else if (k == "red") {
        vec4 red(1.0, 0.0, 0.0, 1.0);
        currentColor = red;
    } else if (k == "yellow") {
        vec4 yellow(1.0, 1.0, 0.0, 1.0);
        currentColor = yellow;
    } else if (k == "green") {
        vec4 green(0.0, 1.0, 0.0, 1.0);
        currentColor = green;
    } else if (k == "white") {
        vec4 white(1.0, 1.0, 1.0, 1.0);
        currentColor = white;
    } else if (k == "black") {
        vec4 black(0.0, 0.0, 0.0, 1.0);
        currentColor = black;
    }
    glUniform4fv(glGetUniformLocation(program, "colorDefiner"), 1, currentColor);
}

void createMenu(void) { 

    drawingModeMenu = glutCreateMenu(menu); 

    glutAddMenuEntry("wireframe", 5);
    glutAddMenuEntry("solid", 6);
    glutAddMenuEntry("shading", 61);

    colorSelectionMenu = glutCreateMenu(menu); 

    glutAddMenuEntry("Blue", 3);
    glutAddMenuEntry("Red", 4);
    glutAddMenuEntry("Green", 7);
    glutAddMenuEntry("Yellow", 8);
    glutAddMenuEntry("White", 9);
    glutAddMenuEntry("Black", 10);

    materialSelectionMenu = glutCreateMenu(menu); 

    glutAddMenuEntry("plastic", 2);
    glutAddMenuEntry("metallic", 11);

    lightTypeSelectionMenu = glutCreateMenu(menu); 

    glutAddMenuEntry("directional", 21);
    glutAddMenuEntry("point", 22);

    lightOnOffMenu = glutCreateMenu(menu);

    glutAddMenuEntry("lights On", 31);
    glutAddMenuEntry("lights Off", 32);

    shadingTypeSelectionMenu = glutCreateMenu(menu); 

    glutAddMenuEntry("phong", 41);
    glutAddMenuEntry("gourad", 42);

    textureSelectionMenu = glutCreateMenu(menu); 

    glutAddMenuEntry("earth", 51);
    glutAddMenuEntry("basketball", 52);

    lightPositionSelectionMenu = glutCreateMenu(menu); 

    glutAddMenuEntry("fixed position", 61);
    glutAddMenuEntry("with sphere", 62);

    menu_id = glutCreateMenu(menu);

    glutAddSubMenu("Object Meterial", materialSelectionMenu);
    glutAddSubMenu("Drawing Mode", drawingModeMenu);
    glutAddSubMenu("Light Mode", lightTypeSelectionMenu);
    glutAddSubMenu("Light ON/OFF", lightOnOffMenu);
    glutAddSubMenu("Light Position", lightPositionSelectionMenu);
    glutAddSubMenu("Shading Mode", shadingTypeSelectionMenu);
    glutAddSubMenu("Texture Type", textureSelectionMenu);
    glutAddSubMenu("Color", colorSelectionMenu);
    glutAddMenuEntry("Quit", 0);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("Comp 410 - Hw2");
    init();
    createMenu();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    Timer(1);
    glutMainLoop();
    return 0;
}