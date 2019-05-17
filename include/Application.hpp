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

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <nanovg.h>

#include <vector>

#include <FrameContext.hpp>
#include <View.hpp>

using namespace std;

class Application
{
    public:
        bool init();
        bool mainLoop();

        /**
         * Pushes a view on this applications's view stack
         * The view will automatically be resized to take
         * the whole screen, unless fullscreen is set to false
         */
        void pushView(View *view, bool fullscreen = true);

    private:
        GLFWwindow* window  = nullptr;
        NVGcontext *vg      = nullptr;

        FontStash fontStash;

        vector<View*> viewStack;

        void getWindowSize(int *width, int *height);

        void frame();
        void exit();
};
