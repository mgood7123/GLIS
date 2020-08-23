// LINUX

#include <glis/glis.hpp>

// lots of detail here about Windows UI
// https://docs.microsoft.com/en-us/windows/win32/uxguide/inter-mouse#pointer-shapes

bool stop_drawing = false;

GLIS_Surface compositor_surface;
GLIS_NDC_Tools::Grid compositor_grid(GLIS_COMMON_WIDTH, GLIS_COMMON_HEIGHT);

constexpr bool SHOULD_CACHE_TEXTURES = true;

class Client_Window {
public:
    Magnum::GL::Texture2D texture2D;
    void * texture_data = nullptr;
    int texture_data_size = 0;
    int texture_w = 0;
    int texture_h = 0;
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    void genTexture() {
        // cache texture

        const Magnum::Vector2i s = {texture_w, texture_h};

        texture2D = Magnum::GL::Texture2D();
        texture2D.setStorage(1, Magnum::GL::TextureFormat::RGBA8, s);

        texture2D.setSubImage(
                0,
                {},
                std::move(
                        Magnum::ImageView2D(
                                Magnum::PixelFormat::RGBA8Unorm,
                                s,
                                std::move(
                                        Corrade::Containers::ArrayView<const uint32_t> (
                                                static_cast<uint32_t *>(texture_data),
                                                texture_w*texture_h
                                        )
                                )
                        )

                )
        ).generateMipmap();
    }

    Client_Window() {
        puts("Client_Window constructor");
        fflush(stdout);
    }

    Client_Window(const Client_Window &x) {
        puts("Client_Window copy constructor");
        fflush(stdout);
        this->x = x.x;
        y = x.y;
        w = x.w;
        h = x.h;
        texture_w = x.texture_w;
        texture_h = x.texture_h;
        texture_data_size = x.texture_data_size;
        if (x.texture_data != nullptr) {
            if (texture_data != nullptr) free(texture_data);
            texture_data = malloc(texture_data_size);
            memcpy(texture_data, x.texture_data, texture_data_size);
        }
        genTexture();
    }

    Client_Window(Client_Window &&x) {
        puts("Client_Window move constructor");
        fflush(stdout);
        std::swap(this->x, x.x);
        std::swap(y, x.y);
        std::swap(w, x.w);
        std::swap(h, x.h);
        std::swap(texture_w, x.texture_w);
        std::swap(texture_h, x.texture_h);
        std::swap(texture_data_size, x.texture_data_size);
        std::swap(texture_data, x.texture_data);
        std::swap(texture2D, x.texture2D);
    }

    Client_Window & operator=(const Client_Window &x) {
        puts("Client_Window copy assignment");
        fflush(stdout);
        this->x = x.x;
        y = x.y;
        w = x.w;
        h = x.h;
        texture_w = x.texture_w;
        texture_h = x.texture_h;
        texture_data_size = x.texture_data_size;
        if (x.texture_data != nullptr) {
            if (texture_data != nullptr) free(texture_data);
            texture_data = malloc(texture_data_size);
            memcpy(texture_data, x.texture_data, texture_data_size);
        }
        genTexture();
        return *this;
    }

    Client_Window & operator=(Client_Window &&x) {
        puts("Client_Window move assignment");
        fflush(stdout);
        std::swap(this->x, x.x);
        std::swap(y, x.y);
        std::swap(w, x.w);
        std::swap(h, x.h);
        std::swap(texture_w, x.texture_w);
        std::swap(texture_h, x.texture_h);
        std::swap(texture_data_size, x.texture_data_size);
        std::swap(texture_data, x.texture_data);
        std::swap(texture2D, x.texture2D);
        return *this;
    }

    ~Client_Window() {
        puts("Client_Window Destructor");
        fflush(stdout);
        if (texture_data != nullptr) free(texture_data);
    }
};

class Client {
public:
    class GLIS::GLIS_shared_memory shared_memory;
    SOCKET_SERVER * server = nullptr;
    size_t server_id = 0;
    size_t id = -1;
    size_t table_id = 0;
    bool connected = false;
    pthread_t thread_id;
};

GLIS_FONT::atlas *text_size = nullptr;

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
        client->shared_memory.init();
        Object * O = CompositorMain.KERNEL.newObject(ObjectTypeProcess, ObjectFlagAutoDeallocateResource, client);
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
        int e = pthread_create(&client->thread_id, nullptr, GLIS::KEEP_ALIVE_MAIN_NOTIFIER, client);
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
        size_t id = CompositorMain.KERNEL.table->findObject(CompositorMain.KERNEL.newObject(ObjectTypeWindow, ObjectFlagAutoDeallocateResource, x));
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
        Client_Window *x = CompositorMain.KERNEL.table->table[window_id]->resource.get<Client_Window*>();
        x->texture_w = client->shared_memory.slot.additional_data_0.type_int64_t.load_int64_t();
        x->texture_h = client->shared_memory.slot.additional_data_1.type_int64_t.load_int64_t();
        if (x->texture_data != nullptr) free(x->texture_data);
        x->texture_data_size = client->shared_memory.slot.texture.size;
        x->texture_data = malloc(x->texture_data_size);
        memcpy(x->texture_data, client->shared_memory.slot.texture.load_ptr(), x->texture_data_size);
        LOG_INFO("CLIENT ID: %zu, received w: %llu, h: %llu", client->id, x->texture_w, x->texture_h);

        if (SHOULD_CACHE_TEXTURES) x->genTexture();

        client->shared_memory.slot.status.store_int8_t(client->shared_memory.status.standby);
        LOG_INFO("CLIENT ID: %zu, CLIENT has uploaded", client->id);
    } else if (command == glis.GLIS_SERVER_COMMANDS.modify_window) {
        size_t window_id = client->shared_memory.slot.additional_data_4.type_size_t.load_size_t();
        LOG_INFO("CLIENT ID: %zu, modifying window (ID: %zu)", client->id, window_id);
        assert(window_id >= 0);
        assert(CompositorMain.KERNEL.table->table[window_id] != nullptr);
        Client_Window *x = CompositorMain.KERNEL.table->table[window_id]->resource.get<Client_Window*>();
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

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(GLIS_COMPOSITOR_DEFAULT_DRAW_FUNCTION, glis, CompositorMain, font, fps) {
    GLIS_FPS command_processing;
    GLIS_FPS clear;
    GLIS_FPS drawing;
    GLIS_FPS counts;

    fps.onFrameStart();

    command_processing.onFrameStart();

    // TODO: migrate to texture buffers:
    //  draw(texture[0]);
    //  draw[texture[1]);
    //  /* ... */
    //  // have we completed this?

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
                        Client *client = CompositorMain.KERNEL.table->table[index]->resource.get<Client *>();

                        // client->connected can change during keep alive shutdown
                        // if the keep alive notifier has ended upon locking this lock
                        // then it is safe to assume that the lock will not be locked again
                        GLIS_INTERNAL_LOCK.lock();
                        if (client->connected) {
                            CompositorMain.server.log_info("CLIENT ID: %zu, client connected", client->id);
                            if (client->shared_memory.slot.status.load_int8_t() !=
                                client->shared_memory.status.standby) {
                                command = client->shared_memory.slot.command.load_int8_t();
                                client->shared_memory.slot.command.store_int8_t(-1);
                                LOG_ERROR("using client with CLIENT ID: %zu", client->id);
                                handleCommands(glis, CompositorMain, command, client,
                                               stop_drawing, in, out);
                            }
                            GLIS_INTERNAL_LOCK.unlock();
                        } else {
                            CompositorMain.server.log_info("CLIENT ID: %zu, client disconnected", client->id);
                            int * res;
                            int s = pthread_join(client->thread_id, reinterpret_cast<void **>(&res));
                            if (s != 0) LOG_ERROR("ERROR: pthread_join returned %d", s);
                            else LOG_INFO("KEEP_ALIVE_MAIN_NOTIFIER returned %d", *res);
                            delete res;
                            GLIS_INTERNAL_LOCK.unlock();
                            if (client->shared_memory.data == nullptr) {
                                // after this returns, client will point to invalid data
                                CompositorMain.KERNEL.table->DELETE(index);
                            }
                        }
                    }
                }
            }
        }
    }
    goto draw;
    draw:
    command_processing.onFrameEnd();
    std::string text_01 =
            std::string("processed commands in: ")
            + std::to_string(command_processing.frameLength)
            + " milliseconds";
    const char * text_command_time = text_01.c_str();
    if (command != -1) LOG_INFO("rendering");
    clear.onFrameStart();
    compositor_surface.clear();
    clear.onFrameEnd();
    std::string text_02 =
            std::string("cleared display in:    ")
            + std::to_string(clear.frameLength)
            + " milliseconds";
    const char * text_clear_time = text_02.c_str();
    counts.onFrameStart();
    int count = CompositorMain.KERNEL.table->Page.count();
    size_t page_size = CompositorMain.KERNEL.table->page_size;
    int drawn = 0;
    int windows = 0;
    int clients = 0;
    {
        size_t index = 0;
        for (int page = 1; page <= count; page++) {
            size_t page_ = page_size * page;
            index = page_ - page_size;
            for (; index < page_; index++)
                if (CompositorMain.KERNEL.table->table[index] != nullptr) {
                    ObjectType type = CompositorMain.KERNEL.table->table[index]->type;
                    if (type == ObjectTypeProcess) clients++;
                    if (type == ObjectTypeWindow) windows++;
                }
        }
    }
    counts.onFrameEnd();
    std::string text_03 = std::string("clients:               ") + std::to_string(clients);
    const char * text_clients = text_03.c_str();
    std::string text_04 = std::string("windows:               ") + std::to_string(windows);
    const char * text_windows = text_04.c_str();
    std::string text_05 = std::string("counted in:            ")
                          + std::to_string(counts.frameLength)
                          + " milliseconds";
    const char * text_counts = text_05.c_str();
    drawing.onFrameStart();
    if (!stop_drawing && count != 0) {
        int page = 1;
        size_t index = 0;
        // TODO: remove client and client windows upon client death
        for (; page <= count; page++) {
            size_t page_ = page_size * page;
            index = page_ - page_size;
            for (; index < page_; index++)
                if (CompositorMain.KERNEL.table->table[index] != nullptr) {
                    if (
                        CompositorMain.KERNEL.table->table[index]->type
                        ==
                        ObjectTypeWindow
                    ) {
                        Client_Window *x = CompositorMain.KERNEL.table->table[index]->resource.get<Client_Window*>();

                        if (x->texture_data == nullptr) continue;

                        if (!SHOULD_CACHE_TEXTURES) x->genTexture();

                        const Magnum::Vector2 & topLeft =     {compositor_grid.x[x->x], compositor_grid.y[x->y]};
                        const Magnum::Vector2 & bottomRight = {compositor_grid.x[x->w], compositor_grid.y[x->h]};
                        const Magnum::Vector2 topRight {bottomRight[0], topLeft[1]};
                        const Magnum::Vector2 bottomLeft {topLeft[0], bottomRight[1]};

                        // cache shader
                        if (compositor_surface.shaderReadTexture == nullptr)
                            compositor_surface.shaderReadTexture = new Magnum::Shaders::Flat2D(
                                    Magnum::Shaders::Flat2D::Flag::Textured);

                        compositor_surface.shaderReadTexture->bindTexture(x->texture2D)
                            .setColor({1.0f,1.0f,1.0f,1.0f})
                            .draw(
                                compositor_surface.mesh.buildPlaneMesh(topLeft, topRight, bottomRight, bottomLeft)
                            );

                        drawn++;
                    }
                }
        }
    }
    drawing.onFrameEnd();
    std::string text_06 =
            std::string("drawn textures in:     ")
            + std::to_string(drawing.frameLength)
            + " milliseconds";
    const char * text_draw_time = text_06.c_str();
    std::string text_07 =
            std::string("textures:              ")
            + std::to_string(drawn);
    const char * text_textures = text_07.c_str();
    fps.onFrameEnd();
    std::string text_08 =
            std::string("frame completed in:    ")
            + std::to_string(fps.frameLength)
            + " milliseconds";
    const char * text_frame_time = text_08.c_str();
    std::string text_09 = std::string("FPS: ") + std::to_string(fps.averageFps);
    const char * text_fps = text_09.c_str();

    font.render_text(text_fps, text_size, 0, text_size->size*1, font.colors.white);

    font.render_text(text_clients, text_size, 0, text_size->size*2, font.colors.white);
    font.render_text(text_windows, text_size, 0, text_size->size*3, font.colors.white);
    font.render_text(text_counts, text_size, 0, text_size->size*4, font.colors.white);
    font.render_text(text_textures, text_size, 0, text_size->size*5, font.colors.white);

    font.render_text(text_command_time, text_size, 0, text_size->size*6, font.colors.white);
    font.render_text(text_clear_time, text_size, 0, text_size->size*7, font.colors.white);
    font.render_text(text_draw_time, text_size, 0, text_size->size*8, font.colors.white);
    font.render_text(text_frame_time, text_size, 0, text_size->size*9, font.colors.white);
    glis.GLIS_SwapBuffers(CompositorMain);

    // TODO: should fps.onFrameEnd() be moved to here?

}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(GLIS_COMPOSITOR_DEFAULT_RESIZE_FUNCTION, glis, CompositorMain, font, fps) {
    glis.GLIS_Viewport(CompositorMain);
    compositor_surface.resize({CompositorMain.width, CompositorMain.height});
    font.set_max_width_height(CompositorMain);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(GLIS_COMPOSITOR_DEFAULT_CLOSE_FUNCTION, glis, CompositorMain, font, fps) {
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
    glis.destroyX11Window(CompositorMain);
    glis.GLIS_destroy_GLIS(CompositorMain);
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
                GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowDraw),
                GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowResize),
                GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowClose)
        ),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowDraw),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowResize),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowClose)
) {
    LOG_INFO("initializing main Compositor");
    glis.GLIS_error_to_string_GL("before onscreen setup");
    if (glis.GLIS_setupOnScreenRendering(CompositorMain)) {
        CompositorMain.contextMagnum.create();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glis.GLIS_error_to_string_GL("after onscreen setup");
        glis.GLIS_Viewport(CompositorMain);
        glis.GLIS_error_to_string_GL("glViewPort");
        font.set_max_width_height(CompositorMain);
        font.add_font("default", font_path);
        text_size = font.add_font_size("default", font_size);
        CompositorMain.server.startServer(SERVER_START_REPLY_MANUALLY);
        LOG_INFO("initialized main Compositor");
        GLuint shaderProgram;
        GLuint vertexShader;
        GLuint fragmentShader;
        glis.GLIS_build_simple_texture_shader_program(
                vertexShader,
                fragmentShader,
                shaderProgram
        );
        LOG_INFO("Using Shader program");
        glUseProgram(shaderProgram);
        SERVER_LOG_TRANSFER_INFO = false;
        glis.SYNC_STATE = glis.STATE.response_started_up;
        LOG_INFO("started up");
        while(glis.SYNC_STATE != glis.STATE.request_shutdown) {
            compositorLoop(glis, CompositorMain, font, fps, onWindowDraw, onWindowResize, onWindowClose);
        }
        GLIS_COMPOSITOR_HANDLE_SHUTDOWN_REQUEST(glis, CompositorMain, vertexShader, fragmentShader,
                                                shaderProgram);
    } else LOG_ERROR("failed to initialize main Compositor");
    // TODO: return something
}
