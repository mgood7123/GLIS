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

// X uses a buffer to draw from, and this buffer must have a supported pixel format,
// and applications which write to this buffer must obtain a handle to this buffer,
// then convert their pixel data to match the buffer's pixel format then write to buffer

//  A client can choose to use any pixel format the server understand.
//  Buffer allocation is nowadays done by the client, and handle passed to the server,
//  but the opposite also exists.

// all of this is fundamentally incompatible with networking, because it relies on shared memory

// in networking case the pixel data is sent to the server, along with control messages,
// in shared memory case it is stored directly in the buffer,
// and control messages get sent to the server from the client

#include <cstdint>
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <pthread.h>
#include <vector>
#include <android/log.h>

#include "logger.h"
#include "GLIS.h"

#define LOG_TAG "EglSample"

class GLIS_CLASS CompositorMain;

char *executableDir;

extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeSetSurface(JNIEnv* jenv,
                                                                                    jclass type,
                                                                                    jobject surface)
{
    if (surface != nullptr) {
        CompositorMain.native_window = ANativeWindow_fromSurface(jenv, surface);
        LOG_INFO("Got window %p", CompositorMain.native_window);
        LOG_INFO("requesting SERVER startup");
        SYNC_STATE = STATE.request_startup;
    } else {
        SYNC_STATE = STATE.request_shutdown;
        LOG_INFO("requesting SERVER shutdown");
        while (SYNC_STATE != STATE.response_shutdown) {}
        LOG_INFO("SERVER has shutdown");
        LOG_INFO("Releasing window");
        ANativeWindow_release(CompositorMain.native_window);
        CompositorMain.native_window = nullptr;
    }
}

const char *vertexSource = R"glsl( #version 320 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = vec4(aColor, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char *fragmentSource = R"glsl( #version 320 es
out highp vec4 FragColor;

in highp vec4 ourColor;
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

void * COMPOSITORMAIN(void * arg) {

    system(std::string(std::string("chmod -R 777 ") + executableDir).c_str());
    char *exe =
        const_cast<char *>(std::string(std::string(executableDir) + "/MYPRIVATEAPP").c_str());
    char *args[2] = {exe, 0};
    GLIS_FORK(args[0], args);


    SYNC_STATE = STATE.no_state;
    while (SYNC_STATE != STATE.request_startup) {}
    LOG_INFO("starting up");
    SYNC_STATE = STATE.response_starting_up;
    LOG_INFO("initializing main Compositor");
    if (GLIS_setupOnScreenRendering(CompositorMain)) {
        CompositorMain.server.startServer(SERVER_START_REPLY_MANUALLY);
        LOG_INFO("initialized main Compositor");
        GLuint shaderProgram;
        GLuint vertexShader;
        GLuint fragmentShader;
        vertexShader = GLIS_createShader(GL_VERTEX_SHADER, vertexSource);
        fragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, fragmentSource);
        LOG_INFO("Creating Shader program");
        shaderProgram = GLIS_error_to_string_exec_GL(glCreateProgram());
        LOG_INFO("Attaching vertex Shader to program");
        GLIS_error_to_string_exec_GL(glAttachShader(shaderProgram, vertexShader));
        LOG_INFO("Attaching fragment Shader to program");
        GLIS_error_to_string_exec_GL(glAttachShader(shaderProgram, fragmentShader));
        LOG_INFO("Linking Shader program");
        GLIS_error_to_string_exec_GL(glLinkProgram(shaderProgram));
        LOG_INFO("Validating Shader program");
        GLboolean ProgramIsValid = GLIS_validate_program(shaderProgram);
        assert(ProgramIsValid == GL_TRUE);
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        GLIS_set_conversion_origin(GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT);
        LOG_INFO("Using Shader program");
        GLIS_error_to_string_exec_GL(glUseProgram(shaderProgram));
        GLIS_error_to_string_exec_GL(glClearColor(0.0F, 0.0F, 1.0F, 1.0F));
        GLIS_error_to_string_exec_GL(glClear(GL_COLOR_BUFFER_BIT));

        SYNC_STATE = STATE.response_started_up;
        LOG_INFO("started up");
        while(SYNC_STATE != STATE.request_shutdown) {
            if (IPC != IPC_MODE.socket) {
                LOG_INFO("waiting for CLIENT to upload");
                SYNC_STATE = STATE.request_upload;
                while (SYNC_STATE != STATE.response_uploading) {}
                while (SYNC_STATE != STATE.response_uploaded) {}
            }
            GLIS_get_texture(CompositorMain.server, GLIS_current_texture, CompositorMain.width,
                             CompositorMain.height);
            LOG_INFO("CLIENT has uploaded");
            if (IPC != IPC_MODE.socket) {
                LOG_INFO("waiting for CLIENT to request render");
                while (SYNC_STATE != STATE.request_render) {}
                LOG_INFO("CLIENT has requested render");
                SYNC_STATE = STATE.response_rendering;
            }
            LOG_INFO("rendering");
            GLIS_error_to_string_exec_GL(glClearColor(0.0F, 0.0F, 1.0F, 1.0F));
            GLIS_error_to_string_exec_GL(glClear(GL_COLOR_BUFFER_BIT));
            GLIS_draw_rectangle<GLint>(GL_TEXTURE0, GLIS_current_texture, 0, 0, 0, 1000, 1000,
                                       CompositorMain.width, CompositorMain.height);
            for (int i = 0; i < 10; i++) {
                GLint ii = i * 100;
                GLIS_draw_rectangle<GLint>(GL_TEXTURE0, GLIS_current_texture, 0, ii, ii, ii + 100,
                                           ii + 100, CompositorMain.width, CompositorMain.height);
            }
            GLIS_Sync_GPU();
            GLIS_error_to_string_exec_EGL(
                eglSwapBuffers(CompositorMain.display, CompositorMain.surface));
            GLIS_Sync_GPU();
            if (IPC != IPC_MODE.socket) SYNC_STATE = STATE.response_rendered;
            LOG_INFO("rendered");
        }
        SYNC_STATE = STATE.response_shutting_down;
        LOG_INFO("shutting down");
        CompositorMain.server.shutdownServer();

        // clean up
        LOG_INFO("Cleaning up");
        GLIS_error_to_string_exec_GL(glDeleteProgram(shaderProgram));
        GLIS_error_to_string_exec_GL(glDeleteShader(fragmentShader));
        GLIS_error_to_string_exec_GL(glDeleteShader(vertexShader));
        GLIS_destroy_GLIS(CompositorMain);
        LOG_INFO("Destroyed main Compositor GLIS");
        LOG_INFO("Cleaned up");
        LOG_INFO("shut down");
        SYNC_STATE = STATE.response_shutdown;
    } else LOG_ERROR("failed to initialize main Compositor");
    return nullptr;
}


extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeOnStart(JNIEnv* jenv,
                                                                                 jclass type,
                                                                                 jstring
                                                                                 ExecutablesDir) {
    jboolean val;
    const char *a = jenv->GetStringUTFChars(ExecutablesDir, &val);
    size_t len = (strlen(a) + 1) * sizeof(char);
    executableDir = static_cast<char *>(malloc(len));
    memcpy(executableDir, a, len);
    jenv->ReleaseStringUTFChars(ExecutablesDir, a);
    long _threadId;
    LOG_INFO("starting main Compositor");
    pthread_create(&_threadId, nullptr, COMPOSITORMAIN, nullptr);
}
