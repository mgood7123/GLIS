//
// Created by smallville7123 on 12/07/20.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include <glis/internal/common.hpp>
#include <glis/internal/log.hpp>
#include <glis/internal/context.hpp>
#include <glis/internal/fps.hpp>
#include <glis/ipc/shm.hpp>

extern bool GLIS_LOG_PRINT_NON_ERRORS;
extern bool GLIS_LOG_PRINT_VERTEX;
extern bool GLIS_LOG_PRINT_CONVERSIONS;
extern bool GLIS_LOG_PRINT_SHAPE_INFO;
extern bool GLIS_ABORT_ON_ERROR;
extern bool GLIS_ABORT_ON_DEBUG_LEVEL_API;

class GLIS {
public:
    void GLIS_FORK(const char *__file, char *const *__argv);

    static void GLIS_error_to_string_GL(const char *name, GLint err);

    static void GLIS_error_to_string_EGL(const char *name, EGLint err);

    static void GLIS_error_to_string_GL(GLint err);

    static void GLIS_error_to_string_EGL(EGLint err);

    static void GLIS_error_to_string_GL();

    static void GLIS_error_to_string_EGL();

    static void GLIS_error_to_string();

    static void GLIS_error_to_string_GL(const char *name);

    static void GLIS_error_to_string_EGL(const char *name);

    static void GLIS_error_to_string(const char *name);

    static void GLIS_GL_INFORMATION();

    static void GLIS_EGL_INFORMATION(EGLDisplay &DISPLAY);

    void GLIS_destroy_GLIS(class GLIS_CLASS &GLIS);

    bool GLIS_initialize_display(class GLIS_CLASS &GLIS);

    bool GLIS_initialize_configuration(class GLIS_CLASS &GLIS);

    bool GLIS_initialize_surface_CreateWindowSurface(class GLIS_CLASS &GLIS);

    bool GLIS_initialize_surface_CreatePbufferSurface(class GLIS_CLASS &GLIS);

    bool GLIS_create_context(class GLIS_CLASS &GLIS);

    bool GLIS_switch_to_context(class GLIS_CLASS &GLIS);

    bool GLIS_get_width_height(class GLIS_CLASS &GLIS);

    static void on_gl_error(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar *message,
                            const void *userParam);

    static void enable_debug_callbacks(void);

    bool GLIS_initialize(class GLIS_CLASS &GLIS, GLint surface_type, bool debug);

    bool GLIS_setupOnScreenRendering(class GLIS_CLASS &GLIS, EGLContext shared_context);

    bool GLIS_setupOnScreenRendering(class GLIS_CLASS &GLIS);

    bool
    GLIS_setupOffScreenRendering(class GLIS_CLASS &GLIS, int w, int h, EGLContext shared_context);

    bool GLIS_setupOffScreenRendering(class GLIS_CLASS &GLIS, int w, int h);

    GLboolean GLIS_ShaderCompilerSupported();

    GLuint GLIS_createShader(GLenum shaderType, const char *&src);

    GLboolean GLIS_validate_program_link(GLuint &Program);

    GLboolean GLIS_validate_program_valid(GLuint &Program);

    GLboolean GLIS_validate_program(GLuint &Program);

    void GLIS_set_texture(GLenum textureUnit, GLuint & texture);

    void GLIS_texture_buffer(GLuint &framebuffer, GLuint &renderbuffer, GLuint &texture,
                             GLint &texture_width, GLint &texture_height);

    GLuint GLIS_current_texture = 0;

    void GLIS_Sync_GPU();

    class STATE {
    public:
        int no_state = -1;
        int initialized = 0;
        int request_startup = 1;
        int response_starting_up = 2;
        int response_started_up = 3;
        int request_shutdown = 4;
        int response_shutting_down = 5;
        int response_shutdown = 6;
        int request_upload = 7;
        int response_uploading = 8;
        int response_uploaded = 9;
        int request_render = 10;
        int response_rendering = 11;
        int response_rendered = 12;
    } STATE;

    int SYNC_STATE = STATE.no_state;

    GLuint *TEXDATA = nullptr;
    size_t TEXDATA_LEN = 0;

    /**
     * buffer swapping is expensive if **vsync** is enabled
     *
     * if vsync is enabled, eglSwapBuffers always waits for
     * the screen to be refreshed once between eglSwapBuffers calls
     */
    EGLBoolean GLIS_SwapBuffers(class GLIS_CLASS &GLIS);

    void
    GLIS_resize(GLuint **TEXDATA, size_t &TEXDATA_LEN, int width_from, int height_from,
                int width_to,
                int height_to);

    #include <glis/templates/templates.hpp>

    struct {
        int texture = 1;
        int new_window = 2;
        int modify_window = 3;
        int close_window = 4;
        int shm_texture = 5;
        int shm_params = 6;
        int new_connection = 7;
        int start_drawing = 8;
        int stop_drawing = 9;
    } GLIS_SERVER_COMMANDS;

    SOCKET_CLIENT GLIS_CLIENT;

    const char *GLIS_command_to_string(int &command) {
        if (command == GLIS_SERVER_COMMANDS.texture) return "Texture Upload";
        else if (command == GLIS_SERVER_COMMANDS.new_window) return "Create New Window";
        else if (command == GLIS_SERVER_COMMANDS.modify_window) return "Modify Window";
        else if (command == GLIS_SERVER_COMMANDS.close_window) return "Close Window";
        else if (command == GLIS_SERVER_COMMANDS.shm_texture) return "Shared Memory Texture";
        else if (command == GLIS_SERVER_COMMANDS.shm_params) return "Shared Memory Parameters";
        else if (command == GLIS_SERVER_COMMANDS.new_connection) return "New Server Connection";
        else if (command == GLIS_SERVER_COMMANDS.start_drawing) return "Start Drawing";
        else if (command == GLIS_SERVER_COMMANDS.stop_drawing) return "Stop Drawing";
        else return "unknown";
    }

    bool GLIS_command_is_valid(int &command) {
        return command == GLIS_SERVER_COMMANDS.texture ||
               command == GLIS_SERVER_COMMANDS.new_window ||
               command == GLIS_SERVER_COMMANDS.modify_window ||
               command == GLIS_SERVER_COMMANDS.close_window ||
               command == GLIS_SERVER_COMMANDS.shm_texture ||
               command == GLIS_SERVER_COMMANDS.shm_params ||
               command == GLIS_SERVER_COMMANDS.new_connection ||
               command == GLIS_SERVER_COMMANDS.start_drawing ||
               command == GLIS_SERVER_COMMANDS.stop_drawing;
    }

// wayland consists of 1 global shm
// and per-client shm pools
// ALL shm are allocated by the clients
// and are retrieved by the server

// this is due to, if an out of memory occurs
// the kernel would kill the server first
// due to the server being the process
// that is allocating shared memory
// for the clients

// see https://gitlab.freedesktop.org/wayland/wayland/-/blob/master/src/wayland-shm.c#L261-328

// see https://man7.org/training/download/posix_shm_slides.pdf

    class GLIS_shared_memory {
    public:
        int fd = 0;
        int8_t *data = nullptr;
        size_t size = 0;
        size_t reference_count = 0;
        class slot_ {
        public:
            slot_(); // default constructor required
            class slot__ {
            public:
                slot__(); // default constructor required
                slot__(GLIS_shared_memory *pMemory);;
                GLIS_shared_memory * shared_memory = nullptr;
                int slot = 0;
                int size = 0;

                void * load_ptr();

                void store_int8_t(int8_t value);
                void store_int16_t(int16_t value);
                void store_int32_t(int32_t value);
                void store_int64_t(int64_t value);
                void store_size_t(size_t value);

                int8_t load_int8_t();
                int16_t load_int16_t();
                int32_t load_int32_t();
                int64_t load_int64_t();
                size_t load_size_t();
            };

            class multi_size {
            public:
                multi_size(); // default constructor required
                multi_size(GLIS_shared_memory *pMemory);;
                class slot__ type_int8_t;
                class slot__ type_int16_t;
                class slot__ type_int32_t;
                class slot__ type_int64_t;
                class slot__ type_size_t;
            };

            slot_(GLIS_shared_memory *shared_memory) {
                status = slot__(shared_memory);
                command = slot__(shared_memory);
                additional_data_0 = multi_size(shared_memory);
                additional_data_1 = multi_size(shared_memory);
                additional_data_2 = multi_size(shared_memory);
                additional_data_3 = multi_size(shared_memory);
                additional_data_4 = multi_size(shared_memory);
                result_data_0 = multi_size(shared_memory);
                result_data_1 = multi_size(shared_memory);
                result_data_2 = multi_size(shared_memory);
                result_data_3 = multi_size(shared_memory);
                result_data_4 = multi_size(shared_memory);
                texture = slot__(shared_memory);
            };

            class slot__ status;
            class slot__ command;
            class multi_size additional_data_0;
            class multi_size additional_data_1;
            class multi_size additional_data_2;
            class multi_size additional_data_3;
            class multi_size additional_data_4;
            class multi_size result_data_0;
            class multi_size result_data_1;
            class multi_size result_data_2;
            class multi_size result_data_3;
            class multi_size result_data_4;
            class slot__ texture;
            size_t total_size = 0;
        };
        slot_ slot = slot_(this);
        class status__ {
        public:
            int8_t standby = 0;
            int8_t ready_to_be_read = 1;
            int8_t ready_to_be_written = 2;
        } status;
    };

    GLIS_shared_memory GLIS_INTERNAL_SHARED_MEMORY;

    bool GLIS_SHARED_MEMORY_INITIALIZED = false;

#define GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS(SLOT_TOTAL_SIZE, SLOT_A, SLOT_A_SIZE) \
    SLOT_A.size = SLOT_A_SIZE; \
    SLOT_TOTAL_SIZE += SLOT_A.size

#define GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(SLOT_TOTAL_SIZE, SLOT_A, SLOT_A_SIZE, SLOT_B) \
    SLOT_A.size = SLOT_A_SIZE; \
    SLOT_A.slot = SLOT_B.slot + SLOT_B.size; \
    SLOT_TOTAL_SIZE += SLOT_A.size


    void GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS__(GLIS_shared_memory & shared_memory, int w, int h);

    bool GLIS_shared_memory_open(GLIS_shared_memory &sh);

    void GLIS_shared_memory_clear(GLIS_shared_memory &sh);

    bool GLIS_shared_memory_malloc(GLIS_shared_memory &sh, size_t size);

    bool GLIS_shared_memory_realloc(GLIS_shared_memory &sh, size_t size);

    static bool GLIS_shared_memory_free(GLIS_shared_memory &sh);

    SOCKET_CLIENT KEEP_ALIVE;

// this function gets called whenever a handle is passed
    void GLIS_shared_memory_increase_reference(GLIS_shared_memory &shared_memory);

    static void *KEEP_ALIVE_MAIN_NOTIFIER(void *arg);

    bool GLIS_start_drawing();

    bool GLIS_stop_drawing();

    void GLIS_sync_server(const char * operation, size_t id);

    bool GLIS_INIT_SHARED_MEMORY(int w, int h);

    size_t GLIS_new_window(int x, int y, int w, int h);

    bool GLIS_modify_window(size_t window_id, int x, int y, int w, int h);

    bool GLIS_close_window(size_t window_id);

    void
    GLIS_upload_texture(GLIS_CLASS &GLIS, size_t &window_id, GLuint &texture_id, GLint texture_width,
                        GLint texture_height);

    // uses the default texture

    void
    GLIS_upload_texture(GLIS_CLASS &GLIS, size_t &window_id, GLint texture_width,
                        GLint texture_height);

    bool getAndroidWindow(void * jenv, void * surface, GLIS_CLASS & GLIS, int width, int height);

    bool runUntilAndroidWindowClose(GLIS & glis, GLIS_CLASS & glis_class, GLIS_FONT & glis_font, GLIS_FPS & fps, void (*draw)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &), void (*onWindowResize)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &, GLsizei, GLsizei), void (*onWindowClose)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &));

    bool destroyAndroidWindow(GLIS_CLASS & GLIS);
    
    bool getX11Window(GLIS_CLASS & GLIS, int width, int height);

    bool runUntilX11WindowClose(GLIS & glis, GLIS_CLASS & glis_class, GLIS_FONT & glis_font, GLIS_FPS & fps, void (*draw)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &), void (*onWindowResize)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &, GLsizei, GLsizei), void (*onWindowClose)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &));

    bool destroyX11Window(GLIS_CLASS & GLIS);

    bool getWaylandWindow(GLIS_CLASS & GLIS, int width, int height);

    bool runUntilWaylandWindowClose(GLIS & glis, GLIS_CLASS & glis_class, GLIS_FONT & glis_font, GLIS_FPS & fps, void (*draw)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &), void (*onWindowResize)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &, GLsizei, GLsizei), void (*onWindowClose)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &));

    bool destroyWaylandWindow(GLIS_CLASS & GLIS);

    void GLIS_build_simple_shader_program(
            GLuint & vertexShader, const char *vertexSource,
            GLuint & fragmentShader, const char *fragmentSource,
            GLuint &shaderProgram
    );

    void GLIS_draw_high_resolution_square();

    void GLIS_framebuffer(GLuint &framebuffer, GLuint &renderbuffer, GLint &texture_width,
                          GLint &texture_height);

    void GLIS_texture(GLuint &texture);

    void GLIS_set_framebuffer(GLuint &framebuffer, GLuint &renderbuffer);

    void GLIS_set_default_framebuffer();

    void GLIS_set_default_texture(GLenum textureUnit);

    void disable_debug_callbacks(void);
};
#pragma clang diagnostic pop