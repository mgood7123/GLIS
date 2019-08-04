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

#include <cstdint>
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <pthread.h>

#include "compositor.h"
#include "logger.h"
#include "GLIS.h"
#include <vector>

#define LOG_TAG "EglSample"

class GLIS_CLASS CompositorMain;
class GLIS_CLASS Compositor[2];

extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeSetSurface(JNIEnv* jenv,
                                                                                    jclass type,
                                                                                    jobject surface)
{
    if (surface != nullptr) {
        CompositorMain.native_window = ANativeWindow_fromSurface(jenv, surface);
        LOG_INFO("Got window %p", CompositorMain.native_window);
    } else {
        LOG_INFO("Releasing window");
        ANativeWindow_release(CompositorMain.native_window);
        CompositorMain.native_window = nullptr;
    }
}

bool makeWindow(int index, int x, int y, int w, int h) {
    auto X = static_cast<GLint>(x);
    auto Y = static_cast<GLint>(y);
    auto W = static_cast<GLsizei>(w);
    auto H = static_cast<GLsizei>(h);
    glEnable(GL_SCISSOR_TEST);
    glScissor(X,Y,W,H);
    glClearColor(0.0F, 1.0F, 1.0F, 1.0F);
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
    EGLContext MainContext;
};

GLuint renderedTexture;
GLsync CHILD;

void Xmain(struct window *window) {
//    if (GLIS_setupOnScreenRendering(Compositor[window->index], window->MainContext)) {
    if (GLIS_setupOffScreenRendering(Compositor[window->index], window->w, window->h, window->MainContext)) {
        GLIS_error_to_string();
        // TODO: Xorg uses Textures to render, specifically Xorg renders FROM textures and DOES NOT modify them

        // create a new frame buffer
        GLuint FBOID;
        GLIS_error_to_string_exec(glGenFramebuffers(1, &FBOID));
        GLIS_error_to_string_exec(glBindFramebuffer(GL_FRAMEBUFFER, FBOID));
        GLuint rboColorId;
        GLIS_error_to_string_exec(glGenRenderbuffers(1, &rboColorId));
        GLIS_error_to_string_exec(glBindRenderbuffer(GL_RENDERBUFFER, rboColorId));
        GLIS_error_to_string_exec(glRenderbufferStorage(GL_RENDERBUFFER,
                                                        GL_RGB8,
                                                        Compositor[window->index].width,
                                                        Compositor[window->index].height
                                                        ));
        GLIS_error_to_string_exec(glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                            GL_COLOR_ATTACHMENT0,
                                                            GL_RENDERBUFFER,
                                                            rboColorId));
        GLIS_error_to_string_exec(glBindFramebuffer(GL_FRAMEBUFFER, FBOID));

        GLenum FramebufferStatus = GLIS_error_to_string_exec(
            glCheckFramebufferStatus(GL_FRAMEBUFFER));

        if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
            LOG_ERROR("framebuffer is not complete");
        else
            LOG_INFO("framebuffer is complete");

        // create a new texture
        GLIS_error_to_string_exec(glGenTextures(1, &renderedTexture));
        GLIS_error_to_string_exec(glBindTexture(GL_TEXTURE_2D, renderedTexture));
        GLIS_error_to_string_exec(glTexImage2D(GL_TEXTURE_2D,
                                               0,
                                               GL_RGBA,
                                               Compositor[window->index].width,
                                               Compositor[window->index].height,
                                               0,
                                               GL_RGBA,
                                               GL_UNSIGNED_BYTE,
                                               0));
        GLIS_error_to_string_exec(glGenerateMipmap(GL_TEXTURE_2D));
        GLIS_error_to_string_exec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLIS_error_to_string_exec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLIS_error_to_string_exec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        GLIS_error_to_string_exec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
        // Set "renderedTexture" as our colour attachement #0
        GLIS_error_to_string_exec(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0));
        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        GLIS_error_to_string_exec(glDrawBuffers(1, DrawBuffers)); // "1" is the size of DrawBuffers

        {
            GLuint shaderProgram, vertexShader, fragmentShader;
            // SHADER PART 1
            {
                const char *vertexSource = R"glsl( #version 320 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
)glsl";

                const char *fragmentSource = R"glsl( #version 320 es
out highp vec4 FragColor;

in highp vec3 ourColor;
in highp vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)glsl";

                vertexShader = GLIS_createShader(GL_VERTEX_SHADER, vertexSource);
                fragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, fragmentSource);
                LOG_INFO("Creating Shader program");
                shaderProgram = GLIS_error_to_string_exec(glCreateProgram());
                LOG_INFO("Attaching vertex Shader to program");
                GLIS_error_to_string_exec(glAttachShader(shaderProgram, vertexShader));
                LOG_INFO("Attaching fragment Shader to program");
                GLIS_error_to_string_exec(glAttachShader(shaderProgram, fragmentShader));
                LOG_INFO("Linking Shader program");
                GLIS_error_to_string_exec(glLinkProgram(shaderProgram));
                LOG_INFO("Validating Shader program");
                GLboolean ProgramIsValid = GLIS_error_to_string_exec(
                    GLIS_validate_program(shaderProgram));
                assert(ProgramIsValid == GL_TRUE);
            }
            // set up vertex data (and buffer(s)) and configure vertex attributes
            // ------------------------------------------------------------------
            class GLIS_rect<GLint> r = GLIS_points_to_rect<GLint>(0, 0, 0, Compositor[window->index].width, Compositor[window->index].height);
            struct GLIS_vertex_map_rectangle<float> vmr = GLIS_build_vertex_data_rect<GLint, float>(0.0F, r, Compositor[window->index].width, Compositor[window->index].height);
            class GLIS_vertex_data<float> v = GLIS_build_vertex_rect<float>(vmr);
            v.print("%4.1ff");

            GLuint VBO, VAO, EBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, v.vertex_size, v.vertex, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, v.indices_size, v.indices, GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // color attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            // texture coord attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
            // SHADER PART 2
            {
                LOG_INFO("Using Shader program");
                GLIS_error_to_string_exec(glUseProgram(shaderProgram));

                glBindTexture(GL_TEXTURE_2D, renderedTexture);
                GLIS_error_to_string_exec(glBindVertexArray(VAO));
                GLIS_error_to_string_exec(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
                GLIS_error_to_string_exec(glBindVertexArray(0));
                CHILD = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
                EGLint error = eglGetError();
                if (CHILD == 0 || error == GL_INVALID_ENUM  || error == GL_INVALID_VALUE )
                {
                    LOG_ERROR("glFenceSync failed at workingFunction.");
                }
            }
        }
    }
}

void * ptm(void * arg) {
    auto * window = static_cast<struct window*>(arg);
    Xmain(window);
    GLIS_destroy_GLIS(Compositor[window->index]);
    return nullptr;
}

void * COMPOSITORMAIN(void * arg) {
    LOG_INFO("waiting for main Compositor to obtain a native window");
    while (CompositorMain.native_window == nullptr) {}
    Compositor[1].native_window = CompositorMain.native_window;
    LOG_INFO("main Compositor has obtained a native window");
    LOG_INFO("initializing main Compositor");
    if (GLIS_setupOnScreenRendering(CompositorMain)) {
        LOG_INFO("initialized main Compositor");
        long _threadId1;
        long _threadId2;
//        struct window *w1 = new struct window;
//        *w1 = {0, 0, 0, 500, 500};
        auto *w2 = new struct window;
        *w2 = {1, 0,0,200,200, CompositorMain.context};
//        pthread_create(&_threadId1, nullptr, ptm, w1);
        LOG_INFO("starting test application");
        pthread_create(&_threadId2, nullptr, ptm, w2);
        // https://arm-software.github.io/opengl-es-sdk-for-android/thread_sync.html
        LOG_INFO("waiting for CHILD to complete");
        while(CHILD == 0) {}
        LOG_INFO("CHILD has completed");
        LOG_INFO("synchronizing");
        glWaitSync(CHILD, 0, GL_TIMEOUT_IGNORED);
        GLint FramebufferStatus = GLIS_error_to_string_exec(
            glCheckFramebufferStatus(GL_FRAMEBUFFER));

        if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
            LOG_ERROR("framebuffer is not complete");
        else
            LOG_INFO("framebuffer is complete");

        // clear framebuffer
        GLIS_error_to_string_exec(glClearColor(1.0F, 0.0F, 1.0F, 1.0F));
        GLIS_error_to_string_exec(glClear(GL_COLOR_BUFFER_BIT));

        // render texture

        GLuint shaderProgram, vertexShader, fragmentShader;
        // SHADER PART 1
        {
            const char *vertexSource = R"glsl( #version 320 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
)glsl";

            const char *fragmentSource = R"glsl( #version 320 es
out highp vec4 FragColor;

in highp vec3 ourColor;
in highp vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
)glsl";

            vertexShader = GLIS_createShader(GL_VERTEX_SHADER, vertexSource);
            fragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, fragmentSource);
            LOG_INFO("Creating Shader program");
            shaderProgram = GLIS_error_to_string_exec(glCreateProgram());
            LOG_INFO("Attaching vertex Shader to program");
            GLIS_error_to_string_exec(glAttachShader(shaderProgram, vertexShader));
            LOG_INFO("Attaching fragment Shader to program");
            GLIS_error_to_string_exec(glAttachShader(shaderProgram, fragmentShader));
            LOG_INFO("Linking Shader program");
            GLIS_error_to_string_exec(glLinkProgram(shaderProgram));
            LOG_INFO("Validating Shader program");
            GLboolean ProgramIsValid = GLIS_error_to_string_exec(
                GLIS_validate_program(shaderProgram));
            assert(ProgramIsValid == GL_TRUE);
        }
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        GLIS_set_conversion_origin(GLIS_CONVERSION_ORIGIN_TOP_RIGHT);
        class GLIS_rect<GLint> r = GLIS_points_to_rect<GLint>(0, 400, 300, 1000, 1500);
        struct GLIS_vertex_map_rectangle<float> vmr = GLIS_build_vertex_data_rect<GLint, float>(0.0F, r, CompositorMain.width, CompositorMain.height);
        class GLIS_vertex_data<float> v = GLIS_build_vertex_rect<float>(vmr);
        v.print("%4.1ff");

        GLuint VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, v.vertex_size, v.vertex, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, v.indices_size, v.indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        // SHADER PART 2
        {
            LOG_INFO("Using Shader program");
            GLIS_error_to_string_exec(glUseProgram(shaderProgram));

            glBindTexture(GL_TEXTURE_2D, renderedTexture);
            GLIS_error_to_string_exec(glBindVertexArray(VAO));
            GLIS_error_to_string_exec(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
            GLIS_error_to_string_exec(glBindVertexArray(0));
        }

        // display system framebuffer
        if (!eglSwapBuffers(CompositorMain.display,
                            CompositorMain.surface)) {
            LOG_ERROR("eglSwapBuffers() returned error %d", eglGetError());
        }
    } else LOG_ERROR("failed to initialize main Compositor");
    return nullptr;
}


extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeOnStart(JNIEnv* jenv,
                                                                                    jclass type)
{
    long _threadId;
    LOG_INFO("starting main Compositor");
    pthread_create(&_threadId, nullptr, COMPOSITORMAIN, nullptr);
}
