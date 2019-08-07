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


GLuint renderedTexture;
GLsync PARENT = nullptr, CHILD = nullptr;

void Xmain(struct window *window) {
    if (GLIS_setupOffScreenRendering(Compositor[window->index], window->w, window->h, window->MainContext)) {
        LOG_INFO("waiting for PARENT to complete");
        while(PARENT == nullptr) {}
        LOG_INFO("PARENT has completed");
        LOG_INFO("synchronizing");
        glWaitSync(PARENT, 0, GL_TIMEOUT_IGNORED);
        LOG_INFO("synchronized");
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
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        class GLIS_rect<GLint> r = GLIS_points_to_rect<GLint>(0, 0, 0, Compositor[window->index].width, Compositor[window->index].height);
        struct GLIS_vertex_map_rectangle<float> vmr = GLIS_build_vertex_data_rect<GLint, float>(0.0F, r, Compositor[window->index].width, Compositor[window->index].height);
        class GLIS_vertex_data<float> v = GLIS_build_vertex_rect<float>(vmr);
        v.print("%4.1ff");

        GLuint vertex_array_object;
        GLuint vertex_buffer_object;
        GLuint element_buffer_object;
        LOG_INFO("Generating buffers");
        GLIS_error_to_string_exec_GL(glGenVertexArrays(1, &vertex_array_object));
        GLIS_error_to_string_exec_GL(glGenBuffers(1, &vertex_buffer_object));
        GLIS_error_to_string_exec_GL(glGenBuffers(1, &element_buffer_object));
        GLIS_error_to_string_exec_GL(glBindVertexArray(vertex_array_object));
        GLIS_error_to_string_exec_GL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object));
        GLIS_error_to_string_exec_GL(glBufferData(GL_ARRAY_BUFFER, v.vertex_size, v.vertex, GL_STATIC_DRAW));
        GLIS_error_to_string_exec_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object));
        GLIS_error_to_string_exec_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, v.indices_size, v.indices, GL_STATIC_DRAW));
        LOG_INFO("Initializing Attributes");
        v.init_attributes();

        LOG_INFO("Using Shader program");
        GLIS_error_to_string_exec_GL(glUseProgram(CHILDshaderProgram));

        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        GLIS_error_to_string_exec_GL(glBindVertexArray(vertex_array_object));
        GLIS_error_to_string_exec_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        GLIS_error_to_string_exec_GL(glBindVertexArray(0));
        CHILD = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        EGLint error = eglGetError();
        if (CHILD == nullptr || error == GL_INVALID_ENUM  || error == GL_INVALID_VALUE )
        {
            LOG_ERROR("glFenceSync failed at workingFunction.");
        }
        LOG_INFO("Cleaning up");
        GLIS_error_to_string_exec_GL(glDeleteProgram(CHILDshaderProgram));
        GLIS_error_to_string_exec_GL(glDeleteShader(CHILDfragmentShader));
        GLIS_error_to_string_exec_GL(glDeleteShader(CHILDvertexShader));
        GLIS_error_to_string_exec_GL(glBindVertexArray(0));
        GLIS_error_to_string_exec_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLIS_error_to_string_exec_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        GLIS_error_to_string_exec_GL(glDeleteVertexArrays(1, &vertex_array_object));
        GLIS_error_to_string_exec_GL(glDeleteBuffers(1,&vertex_buffer_object));
        GLIS_error_to_string_exec_GL(glDeleteBuffers(1, &element_buffer_object));
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
        PARENT = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        EGLint error = eglGetError();
        if (PARENT == nullptr || error == GL_INVALID_ENUM  || error == GL_INVALID_VALUE )
        {
            LOG_ERROR("glFenceSync failed at workingFunction.");
        }
        LOG_INFO("waiting for CHILD to complete");
        while(CHILD == nullptr) {}
        LOG_INFO("CHILD has completed");
        LOG_INFO("synchronizing");
        glWaitSync(CHILD, 0, GL_TIMEOUT_IGNORED);
        LOG_INFO("synchronized");
        CHILD = nullptr;
        GLint FramebufferStatus = GLIS_error_to_string_exec_GL(
            glCheckFramebufferStatus(GL_FRAMEBUFFER));

        if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
            LOG_ERROR("framebuffer is not complete");
        else
            LOG_INFO("framebuffer is complete");

        // clear framebuffer
        GLIS_error_to_string_exec_GL(glClearColor(1.0F, 0.0F, 1.0F, 1.0F));
        GLIS_error_to_string_exec_GL(glClear(GL_COLOR_BUFFER_BIT));

        // render texture

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
        GLIS_draw_rectangle<GLint>(GL_TEXTURE0, renderedTexture, 0, 0, 0, 1000, 1000, CompositorMain.width, CompositorMain.height);
        for (int i = 0; i < 10; i++) {
            GLint ii = i * 100;
            GLIS_draw_rectangle<GLint>(GL_TEXTURE0, renderedTexture, 0, ii, ii, ii+100, ii+100, CompositorMain.width, CompositorMain.height);
        }
//        GLIS_error_to_string_exec_GL(glClearColor(0.0F, 0.0F, 1.0F, 1.0F));
//        GLIS_error_to_string_exec_GL(glClear(GL_COLOR_BUFFER_BIT));
        PARENT = GLIS_error_to_string_exec_GL(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
        assert(PARENT != nullptr);
        // display system framebuffer
        GLIS_error_to_string_exec_EGL(eglSwapBuffers(CompositorMain.display, CompositorMain.surface));
        // clean up
        LOG_INFO("Cleaning up");
        PARENT = nullptr;
        GLIS_error_to_string_exec_GL(glDeleteProgram(PARENTshaderProgram));
        GLIS_error_to_string_exec_GL(glDeleteShader(PARENTfragmentShader));
        GLIS_error_to_string_exec_GL(glDeleteShader(PARENTvertexShader));
        GLIS_error_to_string_exec_GL(glDeleteTextures(1, &renderedTexture));
        GLIS_destroy_GLIS(CompositorMain);
        LOG_INFO("Destroyed main Compositor GLIS");
        LOG_INFO("Cleaned up");
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
