#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "shader.h"
#include "obj_loader.h"
#include "camera.h"
#include "solar_system.h"

// =====================================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ДЛЯ ОРБИТ
// =====================================================

GLuint orbitShaderProgram = 0;
GLuint orbitVAO = 0, orbitVBO = 0;
std::vector<float> orbitVertices;
std::vector<glm::vec3> orbitColors;
std::vector<int> orbitSegmentStarts;
std::vector<int> orbitSegmentCounts;
bool showOrbits = true;

// =====================================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ДЛЯ ИНСТАНЦИРОВАНИЯ
// =====================================================

OBJModel planetModel;              
InstancedShader* instancedShader = nullptr;
Camera* camera = nullptr;
SolarSystem* solarSystem = nullptr;

GLuint sunTexture = 0;
GLuint planetTexture = 0;

GLuint instanceVBO = 0;
GLuint instanceVAO = 0;
size_t instanceCount = 0;

// =====================================================
// ФУНКЦИИ ДЛЯ ОРБИТ
// =====================================================

std::vector<float> createOrbitCircle(float radius, int segments = 100) {
    std::vector<float> vertices;
    for (int i = 0; i <= segments; i++) {
        float angle = (i / (float)segments) * 2.0f * 3.14159265f;
        vertices.push_back(radius * std::cos(angle));  
        vertices.push_back(0.0f);                      
        vertices.push_back(radius * std::sin(angle));  
    }
    return vertices;
}

void initOrbitShader() {
    const char* orbitVertexShader = R"(
        #version 330 core
        layout(location = 0) in vec3 position;
        
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * view * vec4(position, 1.0);
        }
    )";
    
    const char* orbitFragmentShader = R"(
        #version 330 core
        uniform vec3 color;
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";
    
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &orbitVertexShader, NULL);
    glCompileShader(vertex);
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "Ошибка компиляции вертексного шейдера орбит:\n" << infoLog << std::endl;
    }
    
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &orbitFragmentShader, NULL);
    glCompileShader(fragment);
    
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "Ошибка компиляции фрагментного шейдера орбит:\n" << infoLog << std::endl;
    }
    
    orbitShaderProgram = glCreateProgram();
    glAttachShader(orbitShaderProgram, vertex);
    glAttachShader(orbitShaderProgram, fragment);
    glLinkProgram(orbitShaderProgram);
    
    glGetProgramiv(orbitShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(orbitShaderProgram, 512, NULL, infoLog);
        std::cerr << "Ошибка линковки шейдера орбит:\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    std::cout << "Шейдер для орбит скомпилирован" << std::endl;
}

void initOrbits() {
    if (!solarSystem) return;
    
    const auto& bodies = solarSystem->getBodies();
    
    std::vector<glm::vec3> colors = {
        glm::vec3(1.0f, 0.5f, 0.0f),  
        glm::vec3(0.0f, 0.8f, 1.0f),  
        glm::vec3(0.0f, 1.0f, 0.5f),  
        glm::vec3(1.0f, 0.0f, 0.5f),  
        glm::vec3(0.5f, 0.0f, 1.0f),  
        glm::vec3(1.0f, 1.0f, 0.0f),  
        glm::vec3(1.0f, 0.8f, 0.0f),  
    };
    
    for (size_t i = 1; i < bodies.size(); i++) {  
        const auto& body = bodies[i];
        if (body.orbitRadius > 0.0f) {
            auto circleVertices = createOrbitCircle(body.orbitRadius);
            
            orbitSegmentStarts.push_back(orbitVertices.size() / 3);  
            orbitSegmentCounts.push_back(circleVertices.size() / 3); 
            
            orbitVertices.insert(orbitVertices.end(), 
                                circleVertices.begin(), 
                                circleVertices.end());
            
            int colorIndex = (i - 1) % colors.size();
            orbitColors.push_back(colors[colorIndex]);
        }
    }
    
    if (!orbitVertices.empty()) {
        glGenVertexArrays(1, &orbitVAO);
        glGenBuffers(1, &orbitVBO);
        
        glBindVertexArray(orbitVAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
        glBufferData(GL_ARRAY_BUFFER, 
                    orbitVertices.size() * sizeof(float),
                    orbitVertices.data(),
                    GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                            3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void renderOrbits(const glm::mat4& view, const glm::mat4& projection) {
    if (!showOrbits || orbitVAO == 0 || orbitShaderProgram == 0) return;
    
    glUseProgram(orbitShaderProgram);
    
    GLint viewLoc = glGetUniformLocation(orbitShaderProgram, "view");
    GLint projLoc = glGetUniformLocation(orbitShaderProgram, "projection");
    GLint colorLoc = glGetUniformLocation(orbitShaderProgram, "color");
    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    glDisable(GL_DEPTH_TEST);
    
    glLineWidth(1.5f);
    
    glBindVertexArray(orbitVAO);
    
    for (size_t i = 0; i < orbitSegmentStarts.size(); i++) {
        glUniform3fv(colorLoc, 1, glm::value_ptr(orbitColors[i]));
        glDrawArrays(GL_LINE_STRIP, 
                    orbitSegmentStarts[i], 
                    orbitSegmentCounts[i]);
    }
    
    glBindVertexArray(0);
    
    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);
}

// =====================================================
// УПРАВЛЕНИЕ КАМЕРОЙ И ОРБИТАМИ
// =====================================================

void handleInput(sf::Window& window, float deltaTime) {
    float moveSpeed = 5.0f * deltaTime;
    float rotateSpeed = 50.0f * deltaTime;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        camera->moveForward(moveSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        camera->moveBackward(moveSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        camera->moveLeft(moveSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        camera->moveRight(moveSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        camera->moveUp(moveSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        camera->moveDown(moveSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        camera->rotatePitch(rotateSpeed);  
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        camera->rotatePitch(-rotateSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        camera->rotateYaw(-rotateSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        camera->rotateYaw(rotateSpeed);
    }

    static bool oKeyPressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
        if (!oKeyPressed) {
            showOrbits = !showOrbits;
            std::cout << "Орбиты: " << (showOrbits ? "ВКЛ" : "ВЫКЛ") << std::endl;
            oKeyPressed = true;
        }
    } else {
        oKeyPressed = false;
    }

    static bool rKeyPressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        if (!rKeyPressed) {
            delete camera;
            camera = new Camera(glm::vec3(0.0f, 10.0f, 30.0f));
            std::cout << "Камера сброшена в начальную позицию" << std::endl;
            rKeyPressed = true;
        }
    } else {
        rKeyPressed = false;
    }
}

int main() {
    setlocale(LC_ALL, "ru");

    sf::Window window(sf::VideoMode(900, 900), "OpenGL Assignments", sf::Style::Default, sf::ContextSettings(32));

    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    GLenum glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        std::cerr << "GLEW initialization error: " << glewGetErrorString(glewInitResult) << std::endl;
        return -1;
    }

    if (!GLEW_VERSION_3_3) {
        std::cerr << "OpenGL 3.3 not supported!" << std::endl;
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "\nТекущая фигура 1: Градиентный тетраэдр" << std::endl;
    std::cout << "# Управление: стрелки - перемещение по X/Y, W/S - перемещение по Z\n" << std::endl;

    Init();

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            else if (event.type == sf::Event::KeyPressed) {
                //if (event.key.code == sf::Keyboard::Num1) {
                //    Release();
                //    currentAssignment = ASSIGNMENT_1;
                //    Init();
                //    std::cout << "Текущая фигура 1: Градиентный тетраэдр" << std::endl;
                //    std::cout << "# Управление: стрелки - перемещение по X/Y, W/S - перемещение по Z\n" << std::endl;
                //}
                //else if (event.key.code == sf::Keyboard::Num2) {
                //    Release();
                //    currentAssignment = ASSIGNMENT_2;
                //    Init();
                //    std::cout << "Текущая фигура 2: Кубик с наложенной на него текстурой\n" << std::endl;
                //}
                //else if (event.key.code == sf::Keyboard::Num3) {
                //    Release();
                //    currentAssignment = ASSIGNMENT_3;
                //    Init();
                //    std::cout << "Текущая фигура 3: Кубик с двумя смешанными текстурами\n" << std::endl;
                //}
                //else if (event.key.code == sf::Keyboard::Num4) {
                //    Release();
                //    currentAssignment = ASSIGNMENT_4;
                //    Init();
                //    std::cout << "Текущая фигура 4: Градиентный круг" << std::endl;
                //     std::cout << "# Управление: W/S - перемещение по Y, A/D - перемещение по X\n" << std::endl;
                //}
            }
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        window.display();
    }

    Release();
    return 0;
}
