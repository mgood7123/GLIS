// LINUX

#include <glis/glis.hpp>

const char *GLIS_COMPOSITOR_VERTEX_SOURCE = R"glsl( #version 300 es
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

const char *GLIS_COMPOSITOR_FRAGMENT_SOURCE = R"glsl( #version 300 es
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

bool stop_drawing = false;

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
    class GLIS::GLIS_shared_memory shared_memory;
    SOCKET_SERVER * server = nullptr;
    size_t server_id = 0;
    size_t id = -1;
    size_t table_id = 0;
    bool connected = false;
};

void handleCommands(
        GLIS & glis, GLIS_CLASS & CompositorMain,
        int & command, Client * client, bool & stop_drawing, serializer & in, serializer & out
) {
    if (command != -1) {
        if (client != nullptr) {
            CompositorMain.server.log_info(
                    "CLIENT ID: %zu, command: %d (%s)",
                    client->id, command, glis.GLIS_command_to_string(command)
            );
        } else {
            CompositorMain.server.log_info(
                    "SERVER : command: %d (%s)", command, glis.GLIS_command_to_string(command)
            );
        };
        assert(glis.GLIS_command_is_valid(command));
    }

    if (command == glis.GLIS_SERVER_COMMANDS.new_connection) {
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
        glis.GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS__(client->shared_memory, w, h);
        char *s = SERVER_allocate_new_server(SERVER_START_REPLY_MANUALLY, client->table_id);
        pthread_t t; // unused
        int e = pthread_create(&t, nullptr, GLIS::KEEP_ALIVE_MAIN_NOTIFIER, client);
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
        bool r = glis.GLIS_shared_memory_open(client->shared_memory);
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
    } else if (command == glis.GLIS_SERVER_COMMANDS.new_window) {
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
    } else if (command == glis.GLIS_SERVER_COMMANDS.texture) {
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
        glGenTextures(1, &x->TEXTURE); glis.GLIS_error_to_string_GL("glGenTextures");
        glBindTexture(GL_TEXTURE_2D, x->TEXTURE); glis.GLIS_error_to_string_GL("glBindTexture");
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h); glis.GLIS_error_to_string_GL("glTexStorage2D");
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, client->shared_memory.slot.texture.load_ptr()); glis.GLIS_error_to_string_GL("glTexSubImage2D");
        glGenerateMipmap(GL_TEXTURE_2D); glis.GLIS_error_to_string_GL("glGenerateMipmap");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); glis.GLIS_error_to_string_GL("glTexParameteri");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glis.GLIS_error_to_string_GL("glTexParameteri");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); glis.GLIS_error_to_string_GL("glTexParameteri");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); glis.GLIS_error_to_string_GL("glTexParameteri");
        glBindTexture(GL_TEXTURE_2D, 0); glis.GLIS_error_to_string_GL("glBindTexture");
        client->shared_memory.slot.status.store_int8_t(client->shared_memory.status.standby);
        LOG_INFO("CLIENT ID: %zu, CLIENT has uploaded", client->id);
    } else if (command == glis.GLIS_SERVER_COMMANDS.modify_window) {
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
    } else if (command == glis.GLIS_SERVER_COMMANDS.close_window) {
        size_t window_id = client->shared_memory.slot.additional_data_0.type_size_t.load_size_t();
        LOG_INFO("CLIENT ID: %zu, closing window (ID: %zu)", client->id, window_id);
        CompositorMain.KERNEL.table->DELETE(window_id);
        client->shared_memory.slot.status.store_int8_t(client->shared_memory.status.standby);
    } else if (command == glis.GLIS_SERVER_COMMANDS.start_drawing) {
        stop_drawing = false;
        LOG_INFO("drawing started");
        out.add<bool>(true);
        client->server->socket_put_serial(out);
    } else if (command == glis.GLIS_SERVER_COMMANDS.stop_drawing) {
        stop_drawing = true;
        LOG_INFO("drawing stopped");
        out.add<bool>(true);
        client->server->socket_put_serial(out);
    }
}

void GLIS_COMPOSITOR_DEFAULT_DRAW_FUNCTION(GLIS & glis, GLIS_CLASS & CompositorMain, GLIS_FONT & font, GLIS_FPS & fps) {
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
            handleCommands(glis, CompositorMain, command, nullptr, stop_drawing, in, out);
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
                        if (client->shared_memory.data == nullptr) {
                            CompositorMain.KERNEL.table->DELETE(index);
                        } else if (client->shared_memory.slot.status.load_int8_t() != client->shared_memory.status.standby) {
                            command = client->shared_memory.slot.command.load_int8_t();
                            client->shared_memory.slot.command.store_int8_t(-1);
                            LOG_ERROR("using client with CLIENT ID: %zu", client->id);
                            handleCommands(glis, CompositorMain, command, client, stop_drawing, in, out);
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
        // TODO: remove client and client windows upon client death
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
                        glis.GLIS_draw_rectangle<GLint>(GL_TEXTURE0, CW->TEXTURE,
                                                        0,
                                                        // pos
                                                        CW->x, CW->y,
                                                        // dimens
                                                        CW->w, CW->h,
                                                        // max dimens
                                                        GLIS_COMMON_WIDTH, GLIS_COMMON_HEIGHT
                        );
                        drawn++;
                    }
                }
        }
    }
    fps.onFrameEnd();
    std::string text = std::string("FPS: ") + std::to_string(fps.averageFps);
    font.GLIS_font_RenderText(text, 0, 20, font.GLIS_font_color_black);
    glis.GLIS_error_to_string_GL("before GLIS_Sync_GPU");
    glis.GLIS_Sync_GPU();
    glis.GLIS_SwapBuffers(CompositorMain);
    glis.GLIS_Sync_GPU();
}

void GLIS_COMPOSITOR_DEFAULT_RESIZE_FUNCTION(GLIS & glis, GLIS_CLASS & CompositorMain, GLIS_FONT & font, GLIS_FPS & fps, GLsizei width, GLsizei height) {
    glViewport(0, 0, width, height);
}

void GLIS_COMPOSITOR_DEFAULT_CLOSE_FUNCTION(GLIS & glis, GLIS_CLASS & CompositorMain, GLIS_FONT & font, GLIS_FPS & fps) {
    GLIS_COMPOSITOR_REQUEST_SHUTDOWN(glis, CompositorMain);
}

void GLIS_COMPOSITOR_BEFORE_REQUEST_STARTUP(GLIS & glis) {
    glis.SYNC_STATE = glis.STATE.initialized;
}

void GLIS_COMPOSITOR_REQUEST_STARTUP(GLIS & glis) {
    LOG_INFO("waiting for Compositor to initialize");
    while (glis.SYNC_STATE != glis.STATE.initialized) {}
    LOG_INFO("requesting SERVER startup");
    glis.SYNC_STATE = glis.STATE.request_startup;
}

void GLIS_COMPOSITOR_HANDLE_STARTUP_REQUEST(GLIS & glis) {
    while (glis.SYNC_STATE != glis.STATE.request_startup);
    LOG_INFO("starting up");
    glis.SYNC_STATE = glis.STATE.response_starting_up;
}

void GLIS_COMPOSITOR_REQUEST_SHUTDOWN(GLIS & glis, GLIS_CLASS & CompositorMain) {
    glis.SYNC_STATE = glis.STATE.request_shutdown;
}

void GLIS_COMPOSITOR_REQUEST_SHUTDOWN_MT(GLIS & glis, GLIS_CLASS & CompositorMain) {
    glis.SYNC_STATE = glis.STATE.request_shutdown;
    LOG_INFO("requesting SERVER shutdown");
    while (glis.SYNC_STATE != glis.STATE.response_shutdown) {}
    LOG_INFO("SERVER has shutdown");
}

void GLIS_COMPOSITOR_HANDLE_SHUTDOWN_REQUEST(
        GLIS &glis, GLIS_CLASS &CompositorMain,
        GLuint & vertexShader, GLuint & fragmentShader, GLuint & shaderProgram
) {
    glis.SYNC_STATE = glis.STATE.response_shutting_down;
    LOG_INFO("shutting down");
    CompositorMain.server.shutdownServer();
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glis.GLIS_destroy_GLIS(CompositorMain);
    glis.destroyX11Window(CompositorMain);
    LOG_INFO("Destroyed main Compositor GLIS");
    LOG_INFO("shut down");
    glis.SYNC_STATE = glis.STATE.response_shutdown;
}

void GLIS_COMPOSITOR_DO_MAIN(
        GLIS & glis, GLIS_CLASS & CompositorMain, GLIS_FPS & fps,
        GLIS_FONT & font, const char * font_path, int font_size,
        void (*compositorLoop) (
                GLIS & glis,
                GLIS_CLASS & glis_class,
                GLIS_FONT & font,
                GLIS_FPS & fps,
                void (*draw)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &),
                void (*onWindowResize)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &, GLsizei, GLsizei),
                void (*onWindowClose)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &)
        ),
        void (*draw)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &),
        void (*onWindowResize)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &, GLsizei, GLsizei),
        void (*onWindowClose)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &)
) {
    LOG_INFO("initializing main Compositor");
    if (glis.GLIS_setupOnScreenRendering(CompositorMain)) {
        glViewport(0, 0, CompositorMain.width, CompositorMain.height);
        assert(font.GLIS_load_font(font_path, 0, font_size));
        font.GLIS_font_set_RenderText_w_h(CompositorMain.width, CompositorMain.height);
        CompositorMain.server.startServer(SERVER_START_REPLY_MANUALLY);
        LOG_INFO("initialized main Compositor");
        GLuint shaderProgram;
        GLuint vertexShader;
        GLuint fragmentShader;
        glis.GLIS_build_simple_shader_program(
                vertexShader, GLIS_COMPOSITOR_VERTEX_SOURCE,
                fragmentShader, GLIS_COMPOSITOR_FRAGMENT_SOURCE,
                shaderProgram
        );
        LOG_INFO("Using Shader program");
        glUseProgram(shaderProgram);
        SERVER_LOG_TRANSFER_INFO = false;
        glis.SYNC_STATE = glis.STATE.response_started_up;
        LOG_INFO("started up");
        while(glis.SYNC_STATE != glis.STATE.request_shutdown) {
            compositorLoop(glis, CompositorMain, font, fps, draw, onWindowResize, onWindowClose);
        }
        GLIS_COMPOSITOR_HANDLE_SHUTDOWN_REQUEST(glis, CompositorMain, vertexShader, fragmentShader,
                                                shaderProgram);
    } else LOG_ERROR("failed to initialize main Compositor");
    // TODO: return something
}