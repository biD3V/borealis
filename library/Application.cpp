/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <Borealis.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad.h>

#define GLM_FORCE_PURE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

constexpr uint32_t WINDOW_WIDTH = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;
constexpr const char* WINDOW_TITLE = WINDOW_NAME;

using namespace std;

// glfw code from the glfw hybrid app by fincs
// https://github.com/fincs/hybrid_app

static void windowFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (!width || !height)
        return;

    printf("Window size changed to %dx%d\n", width, height);
    glViewport(0, 0, width, height);
}

static void joystickCallback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        printf("Joystick %d connected\n", jid);
        if (glfwJoystickIsGamepad(jid))
            printf("Joystick %d is gamepad: \"%s\"\n", jid, glfwGetGamepadName(jid));
    }
    else if (event == GLFW_DISCONNECTED)
        printf("Joystick %d disconnected\n", jid);
}

static void windowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Check for toggle-fullscreen combo
    if (key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT && action == GLFW_PRESS)
    {
        static int saved_x, saved_y, saved_width, saved_height;

        if (!glfwGetWindowMonitor(window))
        {
            // Back up window position/size
            glfwGetWindowPos(window, &saved_x, &saved_y);
            glfwGetWindowSize(window, &saved_width, &saved_height);

            // Switch to fullscreen mode
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            // Switch back to windowed mode
            glfwSetWindowMonitor(window, nullptr, saved_x, saved_y, saved_width, saved_height, GLFW_DONT_CARE);
        }
    }
}

bool Application::init()
{
    // Init glfw
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
    if (!glfwInit())
    {
        printf("Failed to initialize glfw\n");
        return false;
    }

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    this->window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window)
    {
        printf("glfw: failed to create window");
        glfwTerminate();
        return false;
    }

    // Configure window
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, windowFramebufferSizeCallback);
    glfwSetKeyCallback(window, windowKeyCallback);
    glfwSetJoystickCallback(joystickCallback);

    // Load OpenGL routines using glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    printf("GL Vendor: %s\n", glGetString(GL_VENDOR));
    printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("GL Version: %s\n", glGetString(GL_VERSION));

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        GLFWgamepadstate state;
        printf("Gamepad detected: %s\n", glfwGetGamepadName(GLFW_JOYSTICK_1));
        glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
    }

    // Initialize the scene
    this->vg = nvgCreateGL3(NVG_STENCIL_STROKES);
    if (!vg)
    {
        printf("Unable to init nanovg\n");
        glfwTerminate();
        return false;
    }

    windowFramebufferSizeCallback(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetTime(0.0);

    // Load fonts
    this->fontStash.regular = nvgCreateFont(this->vg, "regular", ASSET("Inter-Regular.ttf")); //TODO: Load shared font on Switch

    //TODO: Load symbols shared font as a fallback
    //TODO: Font Awesome as fallback too?

    return true;
}

bool Application::mainLoop()
{
    // glfw events
    bool is_active;
    do
    {
        is_active = !glfwGetWindowAttrib(this->window, GLFW_ICONIFIED);
        if (is_active)
            glfwPollEvents();
        else
            glfwWaitEvents();

        if (glfwWindowShouldClose(this->window))
        {
            this->exit();
            return false;
        }
    } while (!is_active);

    // Gamepad
    GLFWgamepadstate gamepad = {};
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
    {
        // Gamepad not available, so let's fake it with keyboard
        gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]  = glfwGetKey(window, GLFW_KEY_LEFT);
        gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = glfwGetKey(window, GLFW_KEY_RIGHT);
        gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]    = glfwGetKey(window, GLFW_KEY_UP);
        gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]  = glfwGetKey(window, GLFW_KEY_DOWN);
        gamepad.buttons[GLFW_GAMEPAD_BUTTON_START]      = glfwGetKey(window, GLFW_KEY_ESCAPE);
    }

    // Exit by pressing Start (aka Plus)
    if (gamepad.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return true;
    }

    // Render
    this->frame();
    glfwSwapBuffers(window);

    return true;
}

void Application::getWindowSize(int *width, int *height)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    if (width)
        *width = viewport[2];

    if (height)
        *height = viewport[3];
}

void Application::frame()
{
    // Frame context
    FrameContext frameContext = FrameContext();

    this->getWindowSize(&frameContext.windowWidth, &frameContext.windowHeight);

    frameContext.pixelRatio     = (float)frameContext.windowWidth / (float)frameContext.windowHeight;
    frameContext.vg             = this->vg;
    frameContext.fontStash      = &this->fontStash;

    nvgBeginFrame(this->vg, frameContext.windowWidth, frameContext.windowHeight, frameContext.pixelRatio);

    // GL Clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //TODO: Use theme color here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Draw all views in the stack
    // until we find one that's not translucent
    for (int i = this->viewStack.size()-1; i >= 0; i--)
    {
        View *view = this->viewStack[i];
        view->frame(&frameContext);

        if (!view->isTranslucent())
            break;
    }

    // End frame
    nvgEndFrame(this->vg);
}

void Application::exit()
{
    if (this->vg)
        nvgDeleteGL3(this->vg);

    glfwTerminate();
}

// TODO: Replace this bad fullscreen by an actual layout (to trigger a re-layout on window resize)
void Application::pushView(View *view, bool fullscreen)
{
    if (fullscreen)
    {
        int width, height;
        this->getWindowSize(&width, &height);
        view->setBoundaries(0, 0, width, height);
    }

    this->viewStack.push_back(view);
}