#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Crosshair settings
float crosshairColor[3] = { 1.0f, 0.0f, 0.0f }; // Red by default
float crosshairSize = 35.0f;

void MakeWindowTransparent(GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);

    // Set the window style to layered (for transparency)
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);

    // Make the window transparent with a color key
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // Ensure the window is always on top
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CenterWindow(GLFWwindow* window, int width, int height) {
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    int xpos = (screenWidth - width) / 2;
    int ypos = (screenHeight - height) / 2;

    glfwSetWindowPos(window, xpos, ypos);
}

void RenderCross(int windowWidth, int windowHeight) {
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float size = crosshairSize; // Use the size from the slider

    glLineWidth(2.0f);
    glColor3f(crosshairColor[0], crosshairColor[1], crosshairColor[2]);

    glBegin(GL_LINES);
    glVertex2f(centerX - size, centerY);
    glVertex2f(centerX + size, centerY);

    glVertex2f(centerX, centerY - size);
    glVertex2f(centerX, centerY + size);
    glEnd();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    int windowWidth = 600;
    int windowHeight = 600;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "simplCrosshair", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, windowWidth, windowHeight);
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Setup ImGui binding
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    MakeWindowTransparent(window);
    CenterWindow(window, windowWidth, windowHeight);

    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);

    bool showImGuiWindow = false;
    bool isHKpressed = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // OpenGL rendering
        glClear(GL_COLOR_BUFFER_BIT);
        RenderCross(windowWidth, windowHeight);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (showImGuiWindow) {
            ImGui::Begin("Crosshair Settings", &showImGuiWindow);

            // Slider for crosshair size
            ImGui::SliderFloat("Size", &crosshairSize, 10.0f, 100.0f);

            // Color picker for crosshair color
            ImGui::ColorEdit3("Color", crosshairColor);

            ImGui::End();
        }

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);

        // Check for key press to toggle ImGui window
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS && isHKpressed == false) {
            showImGuiWindow = !showImGuiWindow;
            isHKpressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_RELEASE) {
            isHKpressed = false;
        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
