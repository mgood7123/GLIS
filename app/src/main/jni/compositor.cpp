//
// Copyright 2011 Tero Saarni
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// my current understanding of all this is that a compositor will render each application's frame buffer,
// and a window manager such as KDE or GNOME or I3,
// will work WITH the compositor retrieving information about windows and their position,
// then draw boarders around those windows and implement either stacking or tiling like functionality
// depending on the windowing system type and assumably send information back to the compositor
// such as updates on window changes
// for example if the window is minimized or its position changes,
// the compositor will then redraw itself as it sees fit according to the recieved information

#include <stdint.h>
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <pthread.h>

#include "compositor.h"
#include "logger.h"
#include "GLIS.h"
#include <vector>

#define LOG_TAG "EglSample"

class GLIS_CLASS Compositor[2];

extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeSetSurface(JNIEnv* jenv,
                                                                                    jclass type, jobject surface)
{
    if (surface != nullptr) {
        Compositor[0].native_window = ANativeWindow_fromSurface(jenv, surface);
        Compositor[1].native_window = ANativeWindow_fromSurface(jenv, surface);
        LOG_INFO("Got window %p", Compositor[0].native_window);
        LOG_INFO("Got window %p", Compositor[1].native_window);
    } else {
        LOG_INFO("Releasing window");
        ANativeWindow_release(Compositor[0].native_window);
        ANativeWindow_release(Compositor[1].native_window);
        Compositor[0].native_window = nullptr;
        Compositor[1].native_window = nullptr;
    }
}

bool makeWindow(int index, int x, int y, int w, int h) {
    GLint X = static_cast<GLint>(x);
    GLint Y = static_cast<GLint>(y);
    GLsizei W = static_cast<GLsizei>(w);
    GLsizei H = static_cast<GLsizei>(h);
    glEnable(GL_SCISSOR_TEST);
    glScissor(X,Y,W,H);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (!eglSwapBuffers(Compositor[index].display, Compositor[index].surface)) {
        LOG_ERROR("eglSwapBuffers() returned error %d", eglGetError());
    }
    return true;
}

struct window{
    int index;
    int x;
    int y;
    int w;
    int h;
};

void Xmain(struct window *window) {
//    if (GLIS_setupOnScreenRendering(Compositor[window->index])) {
    if (GLIS_setupOffScreenRendering(Compositor[window->index], window->w, window->h)) {
        GLIS_error_to_string();
        GLuint vbo; // VBO
        float vertices[] = {
                0.0f,  0.5f, // Vertex 1 (X, Y)
                0.5f, -0.5f, // Vertex 2 (X, Y)
                -0.5f, -0.5f  // Vertex 3 (X, Y)
        };
        GLuint vertexShader;
        GLuint tex; // TEXTURE
        // VBO
        {
            glGenBuffers(1, &vbo); // Generate 1 buffer
            GLIS_error_to_string();
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            GLIS_error_to_string();
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            GLIS_error_to_string();
        }
        // SHADER
        {
            const char* vertexSource = R"glsl(
    #version 150 core

    in vec2 position;

    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";
            // params returns a single boolean value indicating whether a shader compiler is supported.
            // GL_FALSE indicates that any call to glShaderSource, glCompileShader, or glReleaseShaderCompiler
            // will result in a GL_INVALID_OPERATION error being generated.
            GLboolean GLSC_supported;
            glGetBooleanv(GL_SHADER_COMPILER, &GLSC_supported);
            LOG_INFO("Supports Shader Compiler: %s", GLSC_supported == GL_TRUE ? "true" : "false");
            LOG_INFO("Creating Shader");
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            GLIS_error_to_string();
            GLint status;
            glShaderSource(vertexShader, 1, &vertexSource, &status);
            LOG_INFO("Compiling Shader");
            glCompileShader(vertexShader);
            GLIS_error_to_string();
            char buffer[512];
            glGetShaderInfoLog(vertexShader, 512, nullptr, buffer);
            LOG_INFO("glCompileShader log: '%s'", buffer);
            GLIS_error_to_string();
            LOG_INFO("Creating Shader program");
            GLuint shaderProgram = glCreateProgram();
            GLIS_error_to_string();
            LOG_INFO("Attaching Shader to program");
            glAttachShader(shaderProgram, vertexShader);
            GLIS_error_to_string();
            LOG_INFO("Linking Shader program");
            glLinkProgram(shaderProgram);
            GLIS_error_to_string();
            LOG_INFO("Using Shader program");
            glUseProgram(shaderProgram);
            GLIS_error_to_string();
            GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
            glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(posAttrib);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        // TEXTURE
        {
//            glGenTextures(1, &tex);
//            GLIS_error_to_string();
//            glBindTexture(GL_TEXTURE_2D, tex);
//            GLIS_error_to_string();
//            // wrapping
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//            // filtering
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            GLIS_error_to_string();
//            // load texture
//            // Black/white checkerboard
//            float pixels[] = {
//                    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
//                    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
//            };
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
//            GLIS_error_to_string();
        }
//        makeWindow(window->index, window->x, window->y, window->w, window->h);
    }
}

void * ptm(void * arg) {
    struct window * window = static_cast<struct window*>(arg);
    while (Compositor[window->index].native_window == nullptr) {}
    Xmain(window);
    GLIS_destroy_GLIS(Compositor[window->index]);
    return nullptr;
}


extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeOnStart(JNIEnv* jenv,
                                                                                    jclass type)
{
    long _threadId;
    struct window * w1 = new struct window;
    *w1 = {0,0,0,500,500};
    struct window * w2 = new struct window;
    *w2 = {1,500,500,500,500};
//    pthread_create(&_threadId, nullptr, ptm, w1);
        pthread_create(&_threadId, nullptr, ptm, w2);
}
