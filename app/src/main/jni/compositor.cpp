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
// the compositor will then redraw itself as it sees fit according to the received information

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
#include "GLIS_COMMANDS.h"

#define LOG_TAG "EglSample"

class GLIS_CLASS CompositorMain;

char *executableDir;

extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeSetSurface(JNIEnv* jenv,
                                                                                    jobject type,
                                                                                    jobject surface)
{
    if (surface != nullptr) {
        CompositorMain.native_window = ANativeWindow_fromSurface(jenv, surface);
        LOG_INFO("Got window %p", CompositorMain.native_window);
        LOG_INFO("waiting for Compositor to initialize");
        while (SYNC_STATE != STATE.initialized) {}
        LOG_INFO("requesting SERVER startup");
        SYNC_STATE = STATE.request_startup;
    } else {
        CompositorMain.server.shutdownServer();
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

/*
[22:30] <emersion> 1. client renders using OpenGL
[22:30] <emersion> 2. OpenGL implementation sends the GPU buffer to the server, e.g. via the linux-dmabuf protocol
[22:30] <emersion> 3. server receives it and imports it via OpenGL
[22:31] <emersion> 4. server composites the client using OpenGL, e.g. on a buffer allocated via GBM
[22:31] <emersion> 5. server uses DRM to display the GBM buffer
 */

int COMPOSITORMAIN__() {

    LOG_INFO("called COMPOSITORMAIN__()");
    system(std::string(std::string("chmod -R 777 ") + executableDir).c_str());
    char *exe =
        const_cast<char *>(std::string(
            std::string(executableDir) + "/Arch/arm64-v8a/MYPRIVATEAPP").c_str());
    char *args1[2] = {exe, 0};
//    GLIS_FORK(exe, args1);

    char *exe2 =
        const_cast<char *>(std::string(
            std::string(executableDir) + "/Arch/arm64-v8a/MovingWindows").c_str());
    char *args2[2] = {exe2, 0};
    GLIS_FORK(exe2, args2);

    SYNC_STATE = STATE.initialized;
    while (SYNC_STATE != STATE.request_startup);
    LOG_INFO("starting up");
    SYNC_STATE = STATE.response_starting_up;
    LOG_INFO("initializing main Compositor");
    if (GLIS_setupOnScreenRendering(CompositorMain)) {
        std::string f = std::string(executableDir) + "/fonts/Vera.ttf";

        assert(GLIS_load_font(f.c_str(), 0, 128));
        GLIS_font_set_RenderText_w_h(CompositorMain.width, CompositorMain.height);

        CompositorMain.server.startServer(SERVER_START_REPLY_MANUALLY);
        LOG_INFO("initialized main Compositor");
        GLuint shaderProgram;
        GLuint vertexShader;
        GLuint fragmentShader;
        vertexShader = GLIS_createShader(GL_VERTEX_SHADER, vertexSource);
        fragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, fragmentSource);
        LOG_INFO("Creating Shader program");
        shaderProgram = glCreateProgram();
        LOG_INFO("Attaching vertex Shader to program");
        glAttachShader(shaderProgram, vertexShader);
        LOG_INFO("Attaching fragment Shader to program");
        glAttachShader(shaderProgram, fragmentShader);
        LOG_INFO("Linking Shader program");
        glLinkProgram(shaderProgram);
        LOG_INFO("Validating Shader program");
        GLboolean ProgramIsValid = GLIS_validate_program(shaderProgram);
        assert(ProgramIsValid == GL_TRUE);
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        GLIS_set_conversion_origin(GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT);
        LOG_INFO("Using Shader program");
        glUseProgram(shaderProgram);
        SERVER_LOG_TRANSFER_INFO = true;
        SYNC_STATE = STATE.response_started_up;
        LOG_INFO("started up");
        struct Client_Window {
            int x;
            int y;
            int w;
            int h;
            GLuint TEXTURE;
        };
        struct Client {
            class GLIS_shared_memory parameters;
            class GLIS_shared_memory texture;
        };
        bool stop_drawing = false;
        double program_start = now_ms();
        GLIS_FPS fps;
        while(SYNC_STATE != STATE.request_shutdown) {
            fps.onFrameStart();

            // TODO: migrate to texture buffers:
            //  draw(texture[0]);
            //  draw[texture[1]);
            //  /* ... */

            double loop_start = now_ms();

            bool redraw = false;
            bool needsSwap = false;
            bool accepted = false;
            serializer in;
            serializer out;
            int command = -1;
            if (IPC == IPC_MODE.shared_memory) {
                // try to connect
                if (CompositorMain.server.socket_accept_non_blocking()) {
                    // connected
                    double start = now_ms();
                    CompositorMain.server.socket_get_serial(in);
                    double end = now_ms();
                    LOG_INFO("read serial in %G milliseconds", end - start);
                } else if (CompositorMain.server.internaldata->server_should_close) goto draw;
            }
            in.get<int>(&command);
//            if (IPC == IPC_MODE.socket)
//                LOG_INFO_SERVER("%scommand: %d (%s)",
//                                CompositorMain.server.TAG, command,
//                                GLIS_command_to_string(command));
            if (command != -1) LOG_INFO("command: %d (%s)", command, GLIS_command_to_string(command));
            if (command == GLIS_SERVER_COMMANDS.new_connection) {
                Client * x = new Client;
                int client_id = CompositorMain.KERNEL.table->findObject(
                        CompositorMain.KERNEL.newObject(ObjectTypeProcess, 0, x));
                struct pa {
                    size_t table_id;

                    class GLIS_shared_memory * parameters;
                } p;
                p.parameters = &x->parameters;
                char *s = SERVER_allocate_new_server(SERVER_START_REPLY_MANUALLY, p.table_id);
                long t; // unused
                int e = pthread_create(&t, nullptr, KEEP_ALIVE_MAIN_NOTIFIER, &p);
                if (e != 0)
                    LOG_ERROR("pthread_create(): errno: %d (%s) | return: %d (%s)", errno,
                              strerror(errno), e,
                              strerror(e));
                else
                    LOG_INFO("KEEP_ALIVE_MAIN_NOTIFIER thread successfully started");
                out.add_pointer<char>(s, 107);
                if (IPC == IPC_MODE.shared_memory) {
                    LOG_INFO("sending id %zu", client_id);
                }
                out.add<size_t>(client_id);
                CompositorMain.server.socket_put_serial(out);
            } else if (command == GLIS_SERVER_COMMANDS.shm_params) {
                size_t id;
                in.get<size_t>(&id);
                assert(CompositorMain.KERNEL.table->table[id] != nullptr);
                struct Client * client = reinterpret_cast<Client *>(
                        CompositorMain.KERNEL.table->table[id]->resource
                );
                CompositorMain.server.socket_get_fd(client->parameters.fd);
                assert(ashmem_valid(client->parameters.fd));
                in.get<size_t>(&client->parameters.size);
                in.get<size_t>(&client->parameters.reference_count);
                LOG_INFO("size: %d, ref count: %d", client->parameters.size, client->parameters.reference_count);
                assert(client->parameters.reference_count != 0);
                CompositorMain.server.socket_get_fd(client->texture.fd);
                assert(ashmem_valid(client->texture.fd));
                in.get<size_t>(&client->texture.size);
                in.get<size_t>(&client->texture.reference_count);
                LOG_INFO("size: %d, ref count: %d", client->texture.size, client->texture.reference_count);
                assert(client->texture.reference_count != 0);
                if (GLIS_shared_memory_open(client->parameters)) {
                    // is parameters even needed?
                    if (GLIS_shared_memory_open(client->texture)) {
                        out.add<bool>(true);
                        CompositorMain.server.socket_put_serial(out);
                    } else
                        LOG_ERROR("failed to open shared memory texture");
                } else
                    LOG_ERROR("failed to open shared memory parameter");
            } else if (command == GLIS_SERVER_COMMANDS.new_window) {
                redraw = true;
                size_t id_;
                in.get<size_t>(&id_);
                assert(CompositorMain.KERNEL.table->table[id_] != nullptr);
                struct Client * client = reinterpret_cast<Client *>(
                        CompositorMain.KERNEL.table->table[id_]->resource
                );
                int *win;
                assert(in.get_raw_pointer<int>(&win) == 4); // must have 4 indexes
                struct Client_Window *x = new struct Client_Window;
                x->x = win[0];
                x->y = win[1];
                x->w = win[2];
                x->h = win[3];
                size_t id = CompositorMain.KERNEL.table->findObject(
                    CompositorMain.KERNEL.newObject(ObjectTypeWindow, 0, x));
                LOG_INFO_SERVER("%swindow %zu: %d,%d,%d,%d",
                                CompositorMain.server.TAG, id, win[0], win[1], win[2], win[3]);
                if (SERVER_LOG_TRANSFER_INFO)
                    LOG_INFO_SERVER("%ssending id %zu", CompositorMain.server.TAG, id);
                out.add<int>(id);
                CompositorMain.server.socket_put_serial(out);
                redraw = true;
            } else if (command == GLIS_SERVER_COMMANDS.texture) {
                redraw = true;
                size_t id;
                in.get<size_t>(&id);
                assert(CompositorMain.KERNEL.table->table[id] != nullptr);
                struct Client * client = reinterpret_cast<Client *>(
                        CompositorMain.KERNEL.table->table[id]->resource
                );
                size_t window_id;
                in.get<size_t>(&window_id);
//                if (IPC == IPC_MODE.socket) {
//                    if (SERVER_LOG_TRANSFER_INFO)
//                        LOG_INFO_SERVER("%sreceived id: %zu", CompositorMain.server.TAG, Client_id);
                if (IPC == IPC_MODE.shared_memory) {
                    LOG_INFO("received id: %zu", window_id);
                }
                GLint *tex_dimens;
                assert(in.get_raw_pointer<GLint>(&tex_dimens) == 2);
//                if (IPC == IPC_MODE.socket) {
//                    if (SERVER_LOG_TRANSFER_INFO)
//                        LOG_INFO_SERVER("%sreceived w: %d, h: %d",
//                                        CompositorMain.server.TAG, tex_dimens[0], tex_dimens[1]);
//                } else {
                if (IPC == IPC_MODE.shared_memory) {
                    LOG_INFO("received w: %d, h: %d", tex_dimens[0], tex_dimens[1]);
                }
                struct Client_Window *CW = static_cast<Client_Window *>(
                        CompositorMain.KERNEL.table->table[window_id]->resource
                );

                glGenTextures(1, &CW->TEXTURE);
                glBindTexture(GL_TEXTURE_2D, CW->TEXTURE);

                GLuint *texdata = nullptr;
//                if (IPC == IPC_MODE.socket) {
//                    in.get_raw_pointer<GLuint>(&texdata);
//                }
                if (IPC == IPC_MODE.shared_memory) {
                    LOG_INFO("reading texture");
                    assert(GLIS_shared_memory_read_texture(client->texture,
                                                           reinterpret_cast<int8_t **>(&texdata)));
                    LOG_INFO("read texture");
                }

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                             tex_dimens[0], tex_dimens[1],
                             0, GL_RGBA, GL_UNSIGNED_BYTE, texdata
                );
                if (texdata != nullptr) free(texdata);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                glBindTexture(GL_TEXTURE_2D, 0);
            } else if (command == GLIS_SERVER_COMMANDS.modify_window) {
                redraw = true;
                size_t window_id;
                in.get<size_t>(&window_id);
                int *win;
                assert(in.get_raw_pointer<int>(&win) == 4); // must have 4 indexes
                assert(win != 0);
                assert(win != nullptr);
                assert(window_id >= 0);
                assert(CompositorMain.KERNEL.table->table[window_id] != nullptr);
                struct Client_Window *c = reinterpret_cast<Client_Window *>(
                    CompositorMain.KERNEL.table->table[window_id]->resource
                );
                c->x = win[0];
                c->y = win[1];
                c->w = win[2];
                c->h = win[3];
            } else if (command == GLIS_SERVER_COMMANDS.close_window) {
                redraw = true;
                size_t window_id;
                in.get<size_t>(&window_id);
                CompositorMain.KERNEL.table->DELETE(window_id);
            } else if (command == GLIS_SERVER_COMMANDS.shm_texture) {
                double start = now_ms();
                size_t id;
                in.get<size_t>(&id);
                assert(CompositorMain.KERNEL.table->table[id] != nullptr);
                struct Client * client = reinterpret_cast<Client *>(
                        CompositorMain.KERNEL.table->table[id]->resource
                );
                assert(ashmem_valid(client->texture.fd));
                LOG_INFO_SERVER("client->parameters.reference_count = %zu",
                                client->texture.reference_count);
                GLIS_shared_memory_increase_reference(client->texture);
                out.add<size_t>(client->texture.size);
                out.add<size_t>(client->texture.reference_count);
//                if (IPC == IPC_MODE.socket) {
//                    if (SERVER_LOG_TRANSFER_INFO)
//                        LOG_INFO_SERVER("%ssending id %d, size: %zu",
//                                        CompositorMain.server.TAG,
//                                        client->texture.fd,
//                                        client->texture.size);
//                }
                if (IPC == IPC_MODE.shared_memory) {
                    LOG_INFO("sending id %d, size: %zu",
                             client->texture.fd,
                             client->texture.size);
                }
                CompositorMain.server.socket_put_fd(client->texture.fd);
                CompositorMain.server.socket_put_serial(out);
                LOG_INFO_SERVER("client->parameters.reference_count = %zu",
                                client->texture.reference_count);
                double end = now_ms();
                LOG_INFO("send texture file descriptor in %G milliseconds", end - start);
            } else if (command == GLIS_SERVER_COMMANDS.start_drawing) {
                stop_drawing = false;
                redraw = true;
                LOG_INFO("drawing started");
                out.add<bool>(true);
                CompositorMain.server.socket_put_serial(out);
            } else if (command == GLIS_SERVER_COMMANDS.stop_drawing) {
                stop_drawing = true;
                redraw = true;
                LOG_INFO("drawing stopped");
                out.add<bool>(true);
                CompositorMain.server.socket_put_serial(out);
            }
            if (command != -1) assert(CompositorMain.server.socket_unaccept());
            if (command != -1) LOG_INFO("CLIENT has uploaded");
            goto draw;
            draw:
            double start = now_ms();
            // something is wrong with redraw...
            // when redraw becomes false, then true, redraw does not reliably redraw anymore
            // avoid using it for now and always redraw every frame
            if (command != -1) LOG_INFO("rendering");
            glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);
            if (!stop_drawing) {
                int page = 1;
                size_t index = 0;
                size_t page_size = CompositorMain.KERNEL.table->page_size;
                int drawn = 0;
                double startK = now_ms();
                for (; page <= CompositorMain.KERNEL.table->Page.count(); page++) {
                    index = ((page_size * page) - page_size);
                    for (; index < page_size * page; index++)
                        if (CompositorMain.KERNEL.table->table[index] != nullptr) {
                            if (
                                    CompositorMain.KERNEL.table->table[index]->type
                                    ==
                                    ObjectTypeWindow
                            ) {
                                struct Client_Window *CW = static_cast<Client_Window *>(
                                        CompositorMain.KERNEL.table->table[index]->resource
                                );
                                GLIS_draw_rectangle<GLint>(GL_TEXTURE0,
                                                           CW->TEXTURE,
                                                           0, CW->x,
                                                           CW->y, CW->w, CW->h,
                                                           CompositorMain.width,
                                                           CompositorMain.height
                                );
                                drawn++;
                            }
                        }
                }
                double endK = now_ms();
                if (command != -1) LOG_INFO("Drawn %d %s in %G milliseconds", drawn,
                         drawn == 1 ? "window" : "windows", endK - startK);
            }
            double end = now_ms();
            if (command != -1) {
                LOG_INFO("rendered in %G milliseconds", end - start);
                LOG_INFO("since loop start: %G milliseconds", end - loop_start);
                LOG_INFO("since start: %G milliseconds", end - program_start);
            }
            fps.onFrameEnd();
            std::string text = std::string("FPS: ") + std::to_string(fps.averageFps);
            GLIS_font_RenderText(text, 0, 20, GLIS_font_color_white);
            GLIS_Sync_GPU();
            GLIS_SwapBuffers(CompositorMain);
            GLIS_Sync_GPU();
        }
        SYNC_STATE = STATE.response_shutting_down;
        LOG_INFO("shutting down");

        // clean up
        LOG_INFO("Cleaning up");
        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        GLIS_destroy_GLIS(CompositorMain);
        LOG_INFO("Destroyed main Compositor GLIS");
        LOG_INFO("Cleaned up");
        LOG_INFO("shut down");
        SYNC_STATE = STATE.response_shutdown;
    } else LOG_ERROR("failed to initialize main Compositor");
    return 0;
}

void * COMPOSITORMAIN(void * arg) {
    int * ret = new int;
    LOG_INFO("calling COMPOSITORMAIN__()");
    *ret = COMPOSITORMAIN__();
    return ret;
}

long COMPOSITORMAIN_threadId;
extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeOnStart(JNIEnv* jenv,
                                                                                 jobject type,
                                                                                 jstring
                                                                                 ExecutablesDir) {
    jboolean val;
    const char *a = jenv->GetStringUTFChars(ExecutablesDir, &val);
    size_t len = (strlen(a) + 1) * sizeof(char);
    executableDir = static_cast<char *>(malloc(len));
    memcpy(executableDir, a, len);
    jenv->ReleaseStringUTFChars(ExecutablesDir, a);
    LOG_INFO("starting main Compositor");
    int e = pthread_create(&COMPOSITORMAIN_threadId, nullptr, COMPOSITORMAIN, nullptr);
    if (e != 0)
        LOG_ERROR("pthread_create(): errno: %d (%s) | return: %d (%s)", errno, strerror(errno), e,
                  strerror(e));
    else
        LOG_INFO("Compositor thread successfully started");
}

extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeOnStop(JNIEnv* jenv,
                                                                                jobject type) {
    LOG_INFO("waiting for main Compositor to stop");
    int * ret;
    int e = pthread_join(COMPOSITORMAIN_threadId, reinterpret_cast<void **>(&ret));
    if (e != 0)
        LOG_ERROR("pthread_join(): errno: %d (%s) | return: %d (%s)", errno, strerror(errno), e,
                  strerror(e));
    else
        LOG_INFO("main Compositor has stopped: return code: %d", *ret);
    delete ret;
}