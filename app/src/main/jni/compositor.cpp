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
        const_cast<char *>(std::string(
            std::string(executableDir) + "/Arch/arm64-v8a/MYPRIVATEAPP").c_str());
    char *args[2] = {exe, 0};
    GLIS_FORK(args[0], args);
    char *exe2 =
        const_cast<char *>(std::string(
            std::string(executableDir) + "/Arch/arm64-v8a/MovingWindows").c_str());
    char *args2[2] = {exe2, 0};
    GLIS_FORK(args2[0], args2);


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
        struct Client_Window {
            int x;
            int y;
            int w;
            int h;
            GLuint TEXTURE;
        };
        while(SYNC_STATE != STATE.request_shutdown) {
            SERVER_LOG_INFO("%swaiting for connection", CompositorMain.server.TAG);
            int CMD = 0;
            bool redraw = false;
            if (CompositorMain.server.socket_accept()) {
                SERVER_LOG_INFO("%sconnection obtained", CompositorMain.server.TAG);
                if (SERVER_LOG_TRANSFER_INFO)
                    SERVER_LOG_INFO("%sretrieving header", CompositorMain.server.TAG);
                if (CompositorMain.server.socket_get_header()) { // false if fails
                    if (SERVER_LOG_TRANSFER_INFO)
                        SERVER_LOG_INFO("%sretrieved header", CompositorMain.server.TAG);
                    if (SERVER_LOG_TRANSFER_INFO)
                        SERVER_LOG_INFO("%sprocessing header", CompositorMain.server.TAG);
                    CMD = CompositorMain.server.internaldata->HEADER->get.command();
                    SERVER_LOG_INFO("%sCMD: %d", CompositorMain.server.TAG, CMD);
                    CompositorMain.server.internaldata->HEADER->put.expect_data(true);
                    CompositorMain.server.internaldata->REPLY = SOCKET_HEADER();
                    CompositorMain.server.internaldata->REPLY->put.expect_data(true);
                    size_t send_length = 0;
                    if (CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.texture ||
                        CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.modify_window ||
                        CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.close_window) {
                        send_length = 0;
                        redraw = true;
                    } else if (CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.new_window) {
                        send_length = sizeof(size_t);
                        redraw = true;
                    }
                    CompositorMain.server.internaldata->REPLY->put.length(send_length);
                    CompositorMain.server.internaldata->REPLY->put.response(
                        SERVER_MESSAGES.SERVER_MESSAGE_RESPONSE.OK);
                    if (SERVER_LOG_TRANSFER_INFO)
                        SERVER_LOG_INFO("%sprocessed header", CompositorMain.server.TAG);
                    if (SERVER_LOG_TRANSFER_INFO)
                        SERVER_LOG_INFO("%ssending header", CompositorMain.server.TAG);
                    if (CompositorMain.server.socket_put_header()) { // false if fails
                        if (SERVER_LOG_TRANSFER_INFO)
                            SERVER_LOG_INFO("%ssent header", CompositorMain.server.TAG);
                        if (CompositorMain.server.socket_header_expect_data()) { // false if fails
                            if (SERVER_LOG_TRANSFER_INFO)
                                SERVER_LOG_INFO("%sexpecting data", CompositorMain.server.TAG);
                            if (SERVER_LOG_TRANSFER_INFO)
                                SERVER_LOG_INFO("%sobtaining data", CompositorMain.server.TAG);
                            if (CompositorMain.server.socket_get_data()) { // false if fails
                                if (SERVER_LOG_TRANSFER_INFO)
                                    SERVER_LOG_INFO("%sprocessing data", CompositorMain.server.TAG);
                                size_t id;
                                if (CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.texture) {
                                    size_t Client_id = reinterpret_cast<size_t *>(
                                        CompositorMain.server.internaldata->DATA->data
                                    )[0];
                                    LOG_INFO("received id: %zu", Client_id);
                                    GLsizei w = static_cast<GLsizei>(reinterpret_cast<size_t *>(
                                        CompositorMain.server.internaldata->DATA->data
                                    )[1]);
                                    LOG_INFO("received w: %d", w);
                                    GLsizei h = static_cast<GLsizei>(
                                        reinterpret_cast<size_t *>(
                                            CompositorMain.server.internaldata->DATA->data
                                        )[2]);
                                    LOG_INFO("received h: %d", h);
                                    struct Client_Window *CW = static_cast<Client_Window *>(CompositorMain.KERNEL.table->table[Client_id]->resource);
                                    GLIS_error_to_string_exec_GL(
                                        glGenTextures(1, &CW->TEXTURE));
                                    GLIS_error_to_string_exec_GL(
                                        glBindTexture(GL_TEXTURE_2D, CW->TEXTURE));
                                    GLIS_error_to_string_exec_GL(
                                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                                                     w,
                                                     h, 0, GL_RGBA,
                                                     GL_UNSIGNED_BYTE,
                                                     &reinterpret_cast<GLuint *>(
                                                         CompositorMain.server.internaldata->DATA->data
                                                     )[GLIS_TEXTURE_OFFSET]));
                                    GLIS_error_to_string_exec_GL(glGenerateMipmap(GL_TEXTURE_2D));
                                    GLIS_error_to_string_exec_GL(
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                                        GL_NEAREST));
                                    GLIS_error_to_string_exec_GL(
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                                        GL_LINEAR));
                                    GLIS_error_to_string_exec_GL(
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                                        GL_CLAMP_TO_BORDER));
                                    GLIS_error_to_string_exec_GL(
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                                        GL_CLAMP_TO_BORDER));
                                    GLIS_error_to_string_exec_GL(glBindTexture(GL_TEXTURE_2D, 0));
                                } else if (CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.new_window) {
                                    int *win =
                                        reinterpret_cast<int *>(
                                            CompositorMain.server.internaldata->DATA->data
                                        );
                                    struct Client_Window *x = new struct Client_Window;
                                    x->x = win[0];
                                    x->y = win[1];
                                    x->w = win[2];
                                    x->h = win[3];
                                    id = CompositorMain.KERNEL.table->findObject(
                                        CompositorMain.KERNEL.newObject(0, 0, x));
                                    SERVER_LOG_INFO("%swindow %zu: %d,%d,%d,%d",
                                                    CompositorMain.server.TAG, id, win[0], win[1],
                                                    win[2], win[3]);
                                } else if (CMD ==
                                           SERVER_MESSAGES.SERVER_MESSAGE_TYPE.modify_window) {
                                    struct GLIS_Client_Window *win =
                                        reinterpret_cast<struct GLIS_Client_Window *>(
                                            CompositorMain.server.internaldata->DATA->data
                                        );
                                    assert(win != 0);
                                    assert(win != nullptr);
                                    assert(win->window_id >= 0);
                                    assert(
                                        CompositorMain.KERNEL.table->table[win->window_id] !=
                                        nullptr
                                    );
                                    struct Client_Window *c = reinterpret_cast<Client_Window *>(
                                        CompositorMain.KERNEL.table->table[win->window_id]->resource
                                    );
                                    c->x = win->x;
                                    c->y = win->y;
                                    c->w = win->w;
                                    c->h = win->h;
                                } else if (CMD ==
                                           SERVER_MESSAGES.SERVER_MESSAGE_TYPE.close_window) {
                                    CompositorMain.KERNEL.table->DELETE(
                                        *reinterpret_cast<size_t *>(
                                            CompositorMain.server.internaldata->DATA->data
                                        )
                                    );
                                }
                                if (SERVER_LOG_TRANSFER_INFO)
                                    SERVER_LOG_INFO("%sprocessed data", CompositorMain.server.TAG);
                                if (CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.new_window) {
                                    size_t len = send_length;
                                    CompositorMain.server.internaldata->REPLY = SOCKET_DATA(&id,
                                                                                            len);
                                    if (SERVER_LOG_TRANSFER_INFO)
                                        SERVER_LOG_INFO("%ssending id %zu",
                                                        CompositorMain.server.TAG,
                                                        id);
                                    if (SERVER_LOG_TRANSFER_INFO)
                                        SERVER_LOG_INFO("%ssending data",
                                                        CompositorMain.server.TAG);
                                    if (CompositorMain.server.socket_put_data()) {
                                        if (SERVER_LOG_TRANSFER_INFO)
                                            SERVER_LOG_INFO("%ssent data",
                                                            CompositorMain.server.TAG);
                                    } else
                                        SERVER_LOG_ERROR("%sfailed to send data",
                                                         CompositorMain.server.TAG);
                                }
                                SOCKET_DELETE(&CompositorMain.server.internaldata->HEADER);
                                SOCKET_DELETE(&CompositorMain.server.internaldata->DATA);
                            } else
                                SERVER_LOG_ERROR("%sfailed to obtain data",
                                                 CompositorMain.server.TAG);
                        }
                    } else
                        SERVER_LOG_ERROR("%sfailed to send header", CompositorMain.server.TAG);
                } else
                    SERVER_LOG_ERROR("%sfailed to get header", CompositorMain.server.TAG);
            } else
                SERVER_LOG_ERROR("%sfailed to obtain a connection", CompositorMain.server.TAG);
            LOG_INFO("CLIENT has uploaded");
            if (redraw) {
                double start = now_ms();
                LOG_INFO("rendering");
                GLIS_error_to_string_exec_GL(glClearColor(0.0F, 0.0F, 1.0F, 1.0F));
                GLIS_error_to_string_exec_GL(glClear(GL_COLOR_BUFFER_BIT));
                int page = 1;
                size_t index = 0;
                size_t page_size = CompositorMain.KERNEL.table->page_size;
                int drawn = 0;
                double startK = now_ms();
                for (; page <= CompositorMain.KERNEL.table->Page.count(); page++) {
                    index = ((page_size * page) - page_size);
                    for (; index < page_size * page; index++)
                        if (CompositorMain.KERNEL.table->table[index] != nullptr) {
                            struct Client_Window *CW = static_cast<Client_Window *>(CompositorMain.KERNEL.table->table[index]->resource);
                            double startR = now_ms();
                            GLIS_draw_rectangle<GLint>(GL_TEXTURE0,
                                                       CW->TEXTURE,
                                                       0, CW->x,
                                                       CW->y, CW->w, CW->h,
                                                       CompositorMain.width, CompositorMain.height);
                            drawn++;
                            double endR = now_ms();
                        }
                }
                double endK = now_ms();
                LOG_INFO("Drawn %d %s in %G milliseconds", drawn,
                         drawn == 1 ? "window" : "windows", endK - startK);
                GLIS_Sync_GPU();
                GLIS_error_to_string_exec_EGL(
                    eglSwapBuffers(CompositorMain.display, CompositorMain.surface));
                GLIS_Sync_GPU();
                double end = now_ms();
                LOG_INFO("rendered in %G milliseconds", end - start);
            }
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
