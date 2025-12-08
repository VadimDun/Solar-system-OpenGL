#include <iostream>
#include <SFML/Window.hpp>
#include <GL/glew.h>
#include "shader.h"

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
