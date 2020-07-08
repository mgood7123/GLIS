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
#include <boost/interprocess/shared_memory_object.hpp>

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

class Client_Window {
public:
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    GLuint TEXTURE = 0;
};

class Client {
public:
    class GLIS_shared_memory shared_memory;
    SOCKET_SERVER * server = nullptr;
    size_t server_id = 0;
    size_t id = -1;
    size_t table_id = 0;
    bool connected = false;
};

void handleCommands(
        int & command, Client * client, bool & stop_drawing, serializer & in, serializer & out
) {
    if (command != -1) {
        if (client != nullptr) {
            CompositorMain.server.log_info(
                    "CLIENT ID: %zu, command: %d (%s)",
                    client->id, command, GLIS_command_to_string(command)
            );
        } else {
            CompositorMain.server.log_info(
                    "SERVER : command: %d (%s)", command, GLIS_command_to_string(command)
            );
        };
        assert(GLIS_command_is_valid(command));
    }

    if (command == GLIS_SERVER_COMMANDS.new_connection) {
        LOG_ERROR("registering new client");
        client = new Client;
        Object * O = CompositorMain.KERNEL.newObject(ObjectTypeProcess, 0, client);
        client->id = CompositorMain.KERNEL.table->findObject(O);
        LOG_ERROR("registered new client with client id: %zu", client->id);
        CompositorMain.server.socket_get_fd(client->shared_memory.fd);
        assert(ashmem_valid(client->shared_memory.fd));
        in.get<size_t>(&client->shared_memory.size);
        assert(client->shared_memory.size != -1);
        in.get<size_t>(&client->shared_memory.reference_count);
        assert(client->shared_memory.reference_count != -1);
        int w;
        int h;
        in.get<int>(&w);
        in.get<int>(&h);
        GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS__(client->shared_memory, w, h);
        char *s = SERVER_allocate_new_server(SERVER_START_REPLY_MANUALLY, client->table_id);
        long t; // unused
        int e = pthread_create(&t, nullptr, KEEP_ALIVE_MAIN_NOTIFIER, client);
        if (e != 0) {
            LOG_ERROR(
                    "CLIENT ID: %zu, pthread_create(): errno: %d (%s) | return: %d (%s)",
                    client->id, errno, strerror(errno), e, strerror(e)
            );
        } else {
            LOG_INFO(
                    "CLIENT ID: %zu, KEEP_ALIVE_MAIN_NOTIFIER thread successfully started",
                    client->id
            );
        }
        out.add_pointer<char>(s, 107);
        bool r = GLIS_shared_memory_open(client->shared_memory);
        out.add<bool>(r);
        if (!r) {
            LOG_ERROR(
                    "CLIENT ID: %zu, failed to open shared memory texture", client->id
            );
        }
        CompositorMain.server.socket_put_serial(out);
        LOG_ERROR("CLIENT ID: %zu, waiting for keep alive to connect", client->id);
        while(!client->connected);
        LOG_ERROR("CLIENT ID: %zu, keep alive has connected", client->id);
        while (client->shared_memory.slot.status.load_int8_t() == client->shared_memory.status.standby);
        client->shared_memory.slot.status.store_int8_t(client->shared_memory.status.standby);
    } else if (command == GLIS_SERVER_COMMANDS.new_window) {
        class Client_Window *x = new class Client_Window;
        x->x = client->shared_memory.slot.additional_data_0.type_int64_t.load_int64_t();
        x->y = client->shared_memory.slot.additional_data_1.type_int64_t.load_int64_t();
        x->w = client->shared_memory.slot.additional_data_2.type_int64_t.load_int64_t();
        x->h = client->shared_memory.slot.additional_data_3.type_int64_t.load_int64_t();
        size_t id = CompositorMain.KERNEL.table->findObject(CompositorMain.KERNEL.newObject(ObjectTypeWindow, 0, x));
        LOG_INFO(
                "CLIENT ID: %zu, window  %zu: %d,%d,%d,%d",
                client->id, id, x->x, x->y, x->w, x->h
        );
        client->shared_memory.slot.result_data_0.type_size_t.store_size_t(id);
        client->shared_memory.slot.status.store_int8_t(client->shared_memory.status.standby);
    } else if (command == GLIS_SERVER_COMMANDS.texture) {
        size_t window_id = client->shared_memory.slot.additional_data_2.type_size_t.load_size_t();
        LOG_INFO("CLIENT ID: %zu, received id: %zu", client->id, window_id);
        assert(window_id >= 0);
        assert(CompositorMain.KERNEL.table->table[window_id] != nullptr);
        Client_Window *x = reinterpret_cast<Client_Window *>(
                CompositorMain.KERNEL.table->table[window_id]->resource
        );
        int64_t w = client->shared_memory.slot.additional_data_0.type_int64_t.load_int64_t();
        int64_t h = client->shared_memory.slot.additional_data_1.type_int64_t.load_int64_t();
        LOG_INFO("CLIENT ID: %zu, received w: %llu, h: %llu", client->id, w, h);
        glGenTextures(1, &x->TEXTURE);
        glBindTexture(GL_TEXTURE_2D, x->TEXTURE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, client->shared_memory.slot.texture.load_ptr());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindTexture(GL_TEXTURE_2D, 0);
        client->shared_memory.slot.status.store_int8_t(client->shared_memory.status.standby);
        LOG_INFO("CLIENT ID: %zu, CLIENT has uploaded", client->id);
    } else if (command == GLIS_SERVER_COMMANDS.modify_window) {
        size_t window_id = client->shared_memory.slot.additional_data_4.type_size_t.load_size_t();
        LOG_INFO("CLIENT ID: %zu, modifying window (ID: %zu)", client->id, window_id);
        assert(window_id >= 0);
        assert(CompositorMain.KERNEL.table->table[window_id] != nullptr);
        class Client_Window *x = reinterpret_cast<Client_Window *>(
                CompositorMain.KERNEL.table->table[window_id]->resource
        );
        x->x = client->shared_memory.slot.additional_data_0.type_int64_t.load_int64_t();
        x->y = client->shared_memory.slot.additional_data_1.type_int64_t.load_int64_t();
        x->w = client->shared_memory.slot.additional_data_2.type_int64_t.load_int64_t();
        x->h = client->shared_memory.slot.additional_data_3.type_int64_t.load_int64_t();
        client->shared_memory.slot.status.store_int8_t(client->shared_memory.status.standby);
    } else if (command == GLIS_SERVER_COMMANDS.close_window) {
        size_t window_id = client->shared_memory.slot.additional_data_0.type_size_t.load_size_t();
        LOG_INFO("CLIENT ID: %zu, closing window (ID: %zu)", client->id, window_id);
        CompositorMain.KERNEL.table->DELETE(window_id);
        client->shared_memory.slot.status.store_int8_t(client->shared_memory.status.standby);
    } else if (command == GLIS_SERVER_COMMANDS.start_drawing) {
        stop_drawing = false;
        LOG_INFO("drawing started");
        out.add<bool>(true);
        client->server->socket_put_serial(out);
    } else if (command == GLIS_SERVER_COMMANDS.stop_drawing) {
        stop_drawing = true;
        LOG_INFO("drawing stopped");
        out.add<bool>(true);
        client->server->socket_put_serial(out);
    }
}

/*
[22:30] <emersion> 1. client renders using OpenGL
[22:30] <emersion> 2. OpenGL implementation sends the GPU buffer to the server, e.g. via the linux-dmabuf protocol
[22:30] <emersion> 3. server receives it and imports it via OpenGL
[22:31] <emersion> 4. server composites the client using OpenGL, e.g. on a buffer allocated via GBM
[22:31] <emersion> 5. server uses DRM to display the GBM buffer
 */

#include <boost/interprocess/managed_shared_memory.hpp>
// in Android 10 and assumably Android 9, the directory /data/local/traces/ exists and is world writable
// however i am not sure if this directory is Partition backed or RAM (tmpfs) backed
// eg if writing to /data/local/traces creates a new physical file in the partition the directory resides in
// or if writing to /data/local/traces creates a new virtual file in RAM as per Linux tmpfs based /tmp/

#include "libsu/libsu.h"

const char * shared_memory_name = "My Shared Memory X";

int COMPOSITORMAIN__() {
    bool hasRoot = libsu_has_root_access();
    libsu_LOG_INFO("libsu has root: %s", hasRoot ? "true" : "false");
    if (hasRoot) {
        libsu_processimage instance;
        bool r;
        std::string command_mkdir = "mkdir ";
        command_mkdir.append(BOOST_TEMPORARY_ANDROID_DATA_DIRECTORY);
        r = libsu_sudo(&instance, command_mkdir.c_str());
        libsu_LOG_INFO("libsu returned: %s", r ? "true" : "false");
        libsu_LOG_INFO("libsu instance return code: %d", instance.return_code);
        libsu_LOG_INFO("libsu stdout: %s", instance.string_stdout);
        libsu_LOG_INFO("libsu stderr: %s", instance.string_stderr);
        libsu_cleanup(&instance);

        std::string command_chmod = "chmod 777 ";
        command_chmod.append(BOOST_TEMPORARY_ANDROID_DATA_DIRECTORY);
        r = libsu_sudo(&instance, command_chmod.c_str());
        libsu_LOG_INFO("libsu returned: %s", r ? "true" : "false");
        libsu_LOG_INFO("libsu instance return code: %d", instance.return_code);
        libsu_LOG_INFO("libsu stdout: %s", instance.string_stdout);
        libsu_LOG_INFO("libsu stderr: %s", instance.string_stderr);
        libsu_cleanup(&instance);

        std::string command_mount = "mount -t tmpfs -o size=512m tmpfs ";
        command_mount.append(BOOST_TEMPORARY_ANDROID_DATA_DIRECTORY);
        r = libsu_sudo(&instance, command_mount.c_str());
        libsu_LOG_INFO("libsu returned: %s", r ? "true" : "false");
        libsu_LOG_INFO("libsu instance return code: %d", instance.return_code);
        libsu_LOG_INFO("libsu stdout: %s", instance.string_stdout);
        libsu_LOG_INFO("libsu stderr: %s", instance.string_stderr);
        libsu_cleanup(&instance);

        std::string command_umount = "umount ";
        command_umount.append(BOOST_TEMPORARY_ANDROID_DATA_DIRECTORY);
        r = libsu_sudo(&instance, command_umount.c_str());
        libsu_LOG_INFO("libsu returned: %s", r ? "true" : "false");
        libsu_LOG_INFO("libsu instance return code: %d", instance.return_code);
        libsu_LOG_INFO("libsu stdout: %s", instance.string_stdout);
        libsu_LOG_INFO("libsu stderr: %s", instance.string_stderr);
        libsu_cleanup(&instance);

        std::string command_rmdir = "rmdir ";
        command_rmdir.append(BOOST_TEMPORARY_ANDROID_DATA_DIRECTORY);
        r = libsu_sudo(&instance, command_rmdir.c_str());
        libsu_LOG_INFO("libsu returned: %s", r ? "true" : "false");
        libsu_LOG_INFO("libsu instance return code: %d", instance.return_code);
        libsu_LOG_INFO("libsu stdout: %s", instance.string_stdout);
        libsu_LOG_INFO("libsu stderr: %s", instance.string_stderr);
        libsu_cleanup(&instance);
    }

    struct shm_remove {
        shm_remove() { boost::interprocess::shared_memory_object::remove(shared_memory_name); }
        ~shm_remove(){ boost::interprocess::shared_memory_object::remove(shared_memory_name); }
    } remover;
    boost::interprocess::managed_shared_memory segment(
            boost::interprocess::create_only, shared_memory_name, 8000
    );
    segment.deallocate(segment.allocate(5000));

    LOG_INFO("called COMPOSITORMAIN__()");
    system(std::string(std::string("chmod -R 777 ") + executableDir).c_str());
    char *exe =
        const_cast<char *>(std::string(
            std::string(executableDir) + "/Arch/arm64-v8a/MovingWindows").c_str());
    char *args1[2] = {exe, 0};
    GLIS_FORK(exe, args1);

    char *exe2 =
        const_cast<char *>(std::string(
            std::string(executableDir) + "/Arch/arm64-v8a/MovingWindowsB").c_str());
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
        bool stop_drawing = false;
        GLIS_FPS fps;
        while(SYNC_STATE != STATE.request_shutdown) {
            fps.onFrameStart();

            // TODO: migrate to texture buffers:
            //  draw(texture[0]);
            //  draw[texture[1]);
            //  /* ... */

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
                    CompositorMain.server.log_info("read serial in %G milliseconds", end - start);
                    in.get<int>(&command);
                    handleCommands(command, nullptr, stop_drawing, in, out);
                } else if (CompositorMain.server.internaldata->server_should_close) goto draw;
                int page = 1;
                size_t index = 0;
                size_t page_size = CompositorMain.KERNEL.table->page_size;
                bool should_break = false;
                for (; page <= CompositorMain.KERNEL.table->Page.count() && !should_break; page++) {
                    index = ((page_size * page) - page_size);
                    for (; index < page_size * page && !should_break; index++) {
                        if (CompositorMain.KERNEL.table->table[index] != nullptr) {
                            if (
                                    CompositorMain.KERNEL.table->table[index]->type
                                    ==
                                    ObjectTypeProcess
                            ) {
                                Client *client = static_cast<Client *>(
                                        CompositorMain.KERNEL.table->table[index]->resource
                                );
                                if (client->shared_memory.slot.status.load_int8_t() != client->shared_memory.status.standby) {
                                    command = client->shared_memory.slot.command.load_int8_t();
                                    client->shared_memory.slot.command.store_int8_t(-1);
                                    LOG_ERROR("using client with CLIENT ID: %zu", client->id);
                                    handleCommands(command, client, stop_drawing, in, out);
                                }
                            }
                        }
                    }
                }
            }
            goto draw;
            draw:
            if (command != -1) LOG_INFO("rendering");
            glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);
            if (!stop_drawing) {
                int page = 1;
                size_t index = 0;
                size_t page_size = CompositorMain.KERNEL.table->page_size;
                int drawn = 0;
                for (; page <= CompositorMain.KERNEL.table->Page.count(); page++) {
                    index = ((page_size * page) - page_size);
                    for (; index < page_size * page; index++)
                        if (CompositorMain.KERNEL.table->table[index] != nullptr) {
                            if (
                                    CompositorMain.KERNEL.table->table[index]->type
                                    ==
                                    ObjectTypeWindow
                            ) {
                                class Client_Window *CW = static_cast<Client_Window *>(
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