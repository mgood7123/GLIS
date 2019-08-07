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
        SYNC_STATE = STATE.shutting_down;
        while (SYNC_STATE != STATE.shutdown) {}
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
    GLIS_error_to_string_exec_EGL(eglSwapBuffers(Compositor[index].display, Compositor[index].surface));
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

const char *PARENTvertexSource = R"glsl( #version 320 es
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

const char *PARENTfragmentSource = R"glsl( #version 320 es
out highp vec4 FragColor;

in highp vec3 ourColor;
in highp vec2 TexCoord;

uniform sampler2D texture1;
//uniform sampler2D texture2;

void main()
{
    FragColor = texture(texture1, TexCoord);
/*
    FragColor = mix(
        texture(texture1, TexCoord), // texture 1
        texture(texture2, TexCoord), // texture 2
        0.2 // interpolation,
        // If the third value is 0.0 it returns the first input
        // If it's 1.0 it returns the second input value.
        // A value of 0.2 will return 80% of the first input color and 20% of the second input color
        // resulting in a mixture of both our textures.
    );
*/
}
)glsl";



GLuint renderedTexture;

void Xmain(struct window *window) {
    if (GLIS_setupOffScreenRendering(Compositor[window->index], window->w, window->h, window->MainContext)) {
        const char * CHILDvertexSource = R"glsl( #version 320 es
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

        const char *CHILDfragmentSource = R"glsl( #version 320 es
out highp vec4 FragColor;
in highp vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)glsl";
        GLIS_error_to_string();
        // create a new texture
        GLuint FB;
        GLuint RB;
        GLIS_texture_buffer(FB, RB, renderedTexture, Compositor[window->index].width, Compositor[window->index].height);

        GLuint CHILDshaderProgram;
        GLuint CHILDvertexShader;
        GLuint CHILDfragmentShader;
        CHILDvertexShader = GLIS_createShader(GL_VERTEX_SHADER, CHILDvertexSource);
        CHILDfragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, CHILDfragmentSource);
        LOG_INFO("Creating Shader program");
        CHILDshaderProgram = GLIS_error_to_string_exec_GL(glCreateProgram());
        LOG_INFO("Attaching vertex Shader to program");
        GLIS_error_to_string_exec_GL(glAttachShader(CHILDshaderProgram, CHILDvertexShader));
        LOG_INFO("Attaching fragment Shader to program");
        GLIS_error_to_string_exec_GL(glAttachShader(CHILDshaderProgram, CHILDfragmentShader));
        LOG_INFO("Linking Shader program");
        GLIS_error_to_string_exec_GL(glLinkProgram(CHILDshaderProgram));
        LOG_INFO("Validating Shader program");
        GLboolean ProgramIsValid = GLIS_validate_program(CHILDshaderProgram);
        assert(ProgramIsValid == GL_TRUE);

        LOG_INFO("Using Shader program");
        GLIS_error_to_string_exec_GL(glUseProgram(CHILDshaderProgram));

        GLIS_draw_rectangle<GLint>(GL_TEXTURE0, renderedTexture, 0, 0, 0, Compositor[window->index].width, Compositor[window->index].height, Compositor[window->index].width, Compositor[window->index].height);
        GLIS_upload_texture(renderedTexture);

        LOG_INFO("Cleaning up");
        GLIS_error_to_string_exec_GL(glDeleteProgram(CHILDshaderProgram));
        GLIS_error_to_string_exec_GL(glDeleteShader(CHILDfragmentShader));
        GLIS_error_to_string_exec_GL(glDeleteShader(CHILDvertexShader));
        GLIS_error_to_string_exec_GL(glDeleteTextures(1, &renderedTexture));
        GLIS_error_to_string_exec_GL(glDeleteRenderbuffers(1, &RB));
        GLIS_error_to_string_exec_GL(glDeleteFramebuffers(1, &FB));
        GLIS_destroy_GLIS(Compositor[window->index]);
        LOG_INFO("Destroyed sub Compositor GLIS");
        LOG_INFO("Cleaned up");
    }
}

void * ptm(void * arg) {
    auto * window = static_cast<struct window*>(arg);
    Xmain(window);
    GLIS_destroy_GLIS(Compositor[window->index]);
    return nullptr;
}

void * COMPOSITORMAIN(void * arg) {
    SYNC_STATE = STATE.no_state;
    LOG_INFO("waiting for main Compositor to obtain a native window");
    while (CompositorMain.native_window == nullptr) {}
    Compositor[1].native_window = CompositorMain.native_window;
    LOG_INFO("main Compositor has obtained a native window");
    LOG_INFO("starting up");
    SYNC_STATE = STATE.starting_up;
    LOG_INFO("initializing main Compositor");
    if (GLIS_setupOnScreenRendering(CompositorMain)) {
        LOG_INFO("initialized main Compositor");
        GLuint PARENTshaderProgram;
        GLuint PARENTvertexShader;
        GLuint PARENTfragmentShader;
        PARENTvertexShader = GLIS_createShader(GL_VERTEX_SHADER, PARENTvertexSource);
        PARENTfragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, PARENTfragmentSource);
        LOG_INFO("Creating Shader program");
        PARENTshaderProgram = GLIS_error_to_string_exec_GL(glCreateProgram());
        LOG_INFO("Attaching vertex Shader to program");
        GLIS_error_to_string_exec_GL(glAttachShader(PARENTshaderProgram, PARENTvertexShader));
        LOG_INFO("Attaching fragment Shader to program");
        GLIS_error_to_string_exec_GL(glAttachShader(PARENTshaderProgram, PARENTfragmentShader));
        LOG_INFO("Linking Shader program");
        GLIS_error_to_string_exec_GL(glLinkProgram(PARENTshaderProgram));
        LOG_INFO("Validating Shader program");
        GLboolean ProgramIsValid = GLIS_validate_program(PARENTshaderProgram);
        assert(ProgramIsValid == GL_TRUE);
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        GLIS_set_conversion_origin(GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT);
        LOG_INFO("Using Shader program");
        GLIS_error_to_string_exec_GL(glUseProgram(PARENTshaderProgram));
        GLIS_error_to_string_exec_GL(glClearColor(0.0F, 0.0F, 1.0F, 1.0F));
        GLIS_error_to_string_exec_GL(glClear(GL_COLOR_BUFFER_BIT));

        SYNC_STATE = STATE.started_up;
        LOG_INFO("started up");

        long _threadId;
        auto *w2 = new struct window;
        *w2 = {1, 0,0,200,200, CompositorMain.context};
        pthread_create(&_threadId, nullptr, ptm, w2);

        while(SYNC_STATE != STATE.shutting_down) {
            LOG_INFO("waiting for CLIENT to upload");
            while (SYNC_STATE != STATE.upload) {}
            LOG_INFO("CLIENT has uploaded");
            LOG_INFO("waiting for CLIENT to request render");
            while (SYNC_STATE != STATE.render) {}
            LOG_INFO("CLIENT has requested render");
            LOG_INFO("rendering");
            GLIS_error_to_string_exec_GL(glClearColor(0.0F, 0.0F, 1.0F, 1.0F));
            GLIS_error_to_string_exec_GL(glClear(GL_COLOR_BUFFER_BIT));
            GLIS_draw_rectangle<GLint>(GL_TEXTURE0, renderedTexture, 0, 0, 0, 1000, 1000,
                                       CompositorMain.width, CompositorMain.height);
            for (int i = 0; i < 10; i++) {
                GLint ii = i * 100;
                GLIS_draw_rectangle<GLint>(GL_TEXTURE0, renderedTexture, 0, ii, ii, ii + 100,
                                           ii + 100, CompositorMain.width, CompositorMain.height);
            }
            GLIS_Sync_GPU();
            GLIS_error_to_string_exec_EGL(
                eglSwapBuffers(CompositorMain.display, CompositorMain.surface));
            GLIS_Sync_GPU();
            LOG_INFO("rendered");
            SYNC_STATE = STATE.rendered;
        }
        LOG_INFO("shutting down");

        // clean up
        LOG_INFO("Cleaning up");
        GLIS_error_to_string_exec_GL(glDeleteProgram(PARENTshaderProgram));
        GLIS_error_to_string_exec_GL(glDeleteShader(PARENTfragmentShader));
        GLIS_error_to_string_exec_GL(glDeleteShader(PARENTvertexShader));
        GLIS_error_to_string_exec_GL(glDeleteTextures(1, &renderedTexture));
        GLIS_destroy_GLIS(CompositorMain);
        LOG_INFO("Destroyed main Compositor GLIS");
        LOG_INFO("Cleaned up");
        SYNC_STATE = STATE.shutdown;
        LOG_INFO("shut down");
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
