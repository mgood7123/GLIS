//
// Created by konek on 8/28/2019.
//

#ifndef GLNE_GLIS_COMMANDS_H
#define GLNE_GLIS_COMMANDS_H

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
        size_t reference_count;
};

bool GLIS_shared_memory_malloc(GLIS_shared_memory &sh, size_t size) {
    sh.size = size;
    return SHM_create(sh.fd, &sh.data, sh.size);
}

bool GLIS_shared_memory_realloc(GLIS_shared_memory &sh, size_t size) {
    if (SHM_resize(sh.fd, size)) {
        sh.size = size;
        return true;
    }
    return false;
}

bool GLIS_shared_memory_free(GLIS_shared_memory &sh) {
    if (SHM_close(sh.fd)) {
        sh.data = nullptr;
        sh.size = 0;
        return true;
    }
    return false;
}

int8_t shared_memory_waiting_for_data = -1;
int8_t shared_memory_has_data = -2;
int8_t shared_memory_data_consumed = -3;
int8_t shared_memory_transfer_complete = -4;
int8_t shared_memory_allocated = -5;
int8_t shared_memory_waiting_for_allocation = -6;

bool LOG_SHARED_MEMORY_TRANSFER_INFO = true;

void GLIS_unsigned_underflow_check(size_t len, size_t subtract_by, size_t &out) {
    if ((len - subtract_by) > len) out = len;
    else out = subtract_by;
}

void GLIS_shared_memory_write(GLIS_shared_memory &sh, serializer &data) {
    assert(sh.data != nullptr);
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("initializing shared memory transfer");
    int8_t indexsize = 0;
    int8_t indexstate = sizeof(size_t);
    int8_t indexdata = sizeof(size_t) + sizeof(int8_t);
    assert(sh.size > indexdata);
    data.construct();
    reinterpret_cast<size_t *>(sh.data)[indexsize] = data.stream.data_len;
    size_t buffer = data.stream.data_len;
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) {
        LOG_INFO_SHM("total to write: %zu", buffer);
        LOG_INFO_SHM("buffer: %zu", buffer);
    }
    sh.data[indexstate] = shared_memory_waiting_for_allocation;
    while (sh.data[indexstate] != shared_memory_allocated);
    if (buffer >= data.stream.data_len) {
        memcpy(&sh.data[indexdata], data.stream.data, data.stream.data_len);
        if (LOG_SHARED_MEMORY_TRANSFER_INFO)
            LOG_INFO_SHM("'data.stream.data' -> 'sh.data[%d]' (size %zu)", indexdata,
                         data.stream.data_len);
        sh.data[indexstate] = shared_memory_has_data;
        while (sh.data[indexstate] != shared_memory_data_consumed);
    } else {
        int index = indexdata;
        while (data.stream.data_len > 0) {
            GLIS_unsigned_underflow_check(data.stream.data_len, buffer,
                                          reinterpret_cast<size_t *>(sh.data)[indexsize]);
            memcpy(&sh.data[index], &data.stream.data[index - indexdata],
                   reinterpret_cast<size_t *>(sh.data)[indexsize]);
            if (LOG_SHARED_MEMORY_TRANSFER_INFO)
                LOG_INFO_SHM("'data.stream.data[%zu]' -> 'sh.data[%zu]' (size %zu)",
                             index - indexdata, index,
                             reinterpret_cast<size_t *>(sh.data)[indexsize]);
            index += reinterpret_cast<size_t *>(sh.data)[indexsize];
            sh.data[indexstate] = shared_memory_has_data;
            while (sh.data[indexstate] != shared_memory_data_consumed);
            data.stream.data_len -= reinterpret_cast<size_t *>(sh.data)[indexsize];
        }
    }
    sh.data[indexstate] = shared_memory_transfer_complete;
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("shared memory transfer complete");
}

bool GLIS_shared_memory_read(GLIS_shared_memory &sh, serializer &data) {
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("initializing shared memory transfer");
    int8_t indexsize = 0;
    int8_t indexstate = sizeof(size_t);
    int8_t indexdata = sizeof(size_t) + sizeof(int8_t);
    assert(sh.size > indexdata);
    while (sh.data[indexstate] != shared_memory_waiting_for_allocation) {
        if (sh.reference_count == 0) {
            LOG_ERROR("reference_count == 0");
            return false;
        }
    }
    size_t buffer = reinterpret_cast<size_t *>(sh.data)[indexsize];
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) {
        LOG_INFO_SHM("total to read: %zu", buffer);
        LOG_INFO_SHM("buffer: %zu", buffer);
    }
    data.stream.allocate(reinterpret_cast<size_t *>(sh.data)[indexsize]);
    sh.data[indexstate] = shared_memory_allocated;
    if (buffer >= data.stream.data_len) { // if buffer is greater than or equal to data len
        while (sh.data[indexstate] != shared_memory_has_data) {
            if (sh.reference_count == 0) {
                LOG_ERROR("reference_count == 0");
                return false;
            }
        }
        memcpy(data.stream.data, &sh.data[indexdata], data.stream.data_len);
        if (LOG_SHARED_MEMORY_TRANSFER_INFO)
            LOG_INFO_SHM("'sh.data[%d]' -> 'data.stream.data' (size %zu)", indexdata,
                         data.stream.data_len);
        sh.data[indexstate] = shared_memory_data_consumed;
    } else {
        if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("reading buffered");
        size_t idx = 0;
        while (sh.data[indexstate] != shared_memory_transfer_complete) {
            while (sh.data[indexstate] != shared_memory_has_data) {
                if (sh.data[indexstate] == shared_memory_transfer_complete) break;
                if (sh.reference_count == 0) {
                    LOG_ERROR("reference_count == 0");
                    return false;
                }
            }
            if (sh.data[indexstate] == shared_memory_transfer_complete) break;
            memcpy(&data.stream.data[idx], &sh.data[indexdata + idx],
                   reinterpret_cast<size_t *>(sh.data)[indexsize]);
            if (LOG_SHARED_MEMORY_TRANSFER_INFO)
                LOG_INFO_SHM("'sh.data[%zu]' -> 'data.stream.data[%zu]' (size %zu)",
                             indexdata + idx, idx, reinterpret_cast<size_t *>(sh.data)[indexsize]);
            idx += reinterpret_cast<size_t *>(sh.data)[indexsize];
            sh.data[indexstate] = shared_memory_data_consumed;
        }
        if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("shared memory transfer complete");
    }
    data.deconstruct();
    return true;
}

void GLIS_shared_memory_write_texture(GLIS_shared_memory &sh, int8_t *texture, size_t &len) {
    assert(sh.data != nullptr);
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("initializing shared memory transfer");
    int8_t indexsize = 0;
    int8_t indexstate = sizeof(size_t);
    int8_t indexdata = sizeof(size_t) + sizeof(int8_t);
    assert(sh.size > indexdata);
    reinterpret_cast<size_t *>(sh.data)[indexsize] = len;
    size_t buffer = len;
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) {
        LOG_INFO_SHM("total to write: %zu", buffer);
        LOG_INFO_SHM("buffer: %zu", buffer);
    }
    sh.data[indexstate] = shared_memory_waiting_for_allocation;
    while (sh.data[indexstate] != shared_memory_allocated);
    if (buffer >= len) {
        memcpy(&sh.data[indexdata], texture, len);
        if (LOG_SHARED_MEMORY_TRANSFER_INFO)
            LOG_INFO_SHM("'texture' -> 'sh.data[%d]' (size %zu)", indexdata, len);
        sh.data[indexstate] = shared_memory_has_data;
        while (sh.data[indexstate] != shared_memory_data_consumed);
    } else {
        int index = indexdata;
        size_t len_tmp = len;
        while (len_tmp > 0) {
            GLIS_unsigned_underflow_check(len_tmp, buffer,
                                          reinterpret_cast<size_t *>(sh.data)[indexsize]);
            memcpy(&sh.data[index], &texture[index - indexdata],
                   reinterpret_cast<size_t *>(sh.data)[indexsize]);
            if (LOG_SHARED_MEMORY_TRANSFER_INFO)
                LOG_INFO_SHM("'texture[%zu]' -> 'sh.data[%zu]' (size %zu)", index - indexdata,
                             index, reinterpret_cast<size_t *>(sh.data)[indexsize]);
            index += reinterpret_cast<size_t *>(sh.data)[indexsize];
            sh.data[indexstate] = shared_memory_has_data;
            while (sh.data[indexstate] != shared_memory_data_consumed);
            len_tmp -= reinterpret_cast<size_t *>(sh.data)[indexsize];
        }
    }
    sh.data[indexstate] = shared_memory_transfer_complete;
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("shared memory transfer complete");
}

bool GLIS_shared_memory_read_texture(GLIS_shared_memory &sh, int8_t **texture) {
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("initializing shared memory transfer");
    int8_t indexsize = 0;
    int8_t indexstate = sizeof(size_t);
    int8_t indexdata = sizeof(size_t) + sizeof(int8_t);
    assert(sh.size > indexdata);
    while (sh.data[indexstate] != shared_memory_waiting_for_allocation)
        if (sh.reference_count == 0) {
            LOG_ERROR("reference_count == 0");
            return false;
        }
    size_t buffer = reinterpret_cast<size_t *>(sh.data)[indexsize];
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) {
        LOG_INFO_SHM("total to read: %zu", buffer);
        LOG_INFO_SHM("buffer: %zu", buffer);
    }
    *texture = static_cast<int8_t *>(malloc(reinterpret_cast<size_t *>(sh.data)[indexsize]));
    sh.data[indexstate] = shared_memory_allocated;
    if (buffer >= reinterpret_cast<size_t *>(sh.data)[indexsize]) {
        while (sh.data[indexstate] != shared_memory_has_data) {
            if (sh.reference_count == 0) {
                LOG_ERROR("reference_count == 0");
                return false;
            }
        }
        memcpy(*texture, &sh.data[indexdata], reinterpret_cast<size_t *>(sh.data)[indexsize]);
        if (LOG_SHARED_MEMORY_TRANSFER_INFO)
            LOG_INFO_SHM("'sh.data[%d]' -> '*texture' (size %zu)", indexdata,
                         reinterpret_cast<size_t *>(sh.data)[indexsize]);
        sh.data[indexstate] = shared_memory_data_consumed;
        return true;
    }
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("reading buffered");
    size_t idx = 0;
    while (sh.data[indexstate] != shared_memory_transfer_complete) {
        while (sh.data[indexstate] != shared_memory_has_data) {
            if (sh.data[indexstate] == shared_memory_transfer_complete) break;
            if (sh.reference_count == 0) {
                LOG_ERROR("reference_count == 0");
                return false;
            }
        }
        if (sh.data[indexstate] == shared_memory_transfer_complete) break;
        memcpy(&(*texture)[idx], &sh.data[indexdata + idx],
               reinterpret_cast<size_t *>(sh.data)[indexsize]);
        if (LOG_SHARED_MEMORY_TRANSFER_INFO)
            LOG_INFO_SHM("'sh.data[%zu]' -> '(*texture)[%zu]' (size %zu)", indexdata + idx, idx,
                         reinterpret_cast<size_t *>(sh.data)[indexsize]);
        idx += reinterpret_cast<size_t *>(sh.data)[indexsize];
        sh.data[indexstate] = shared_memory_data_consumed;
    }
    if (LOG_SHARED_MEMORY_TRANSFER_INFO) LOG_INFO_SHM("shared memory transfer complete");
    return true;
}

bool GLIS_shared_memory_get(GLIS_shared_memory &sh) {
//    assert(ashmem_valid(GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA.fd));
    SERVER_LOG_TRANSFER_INFO = true;
    SOCKET_CLIENT client;
    serializer cmd;
    serializer id;
    cmd.add<int>(GLIS_SERVER_COMMANDS.shm_texture);
    if (client.connect_to_server()) {
        if (client.socket_put_serial(cmd)) {
            client.socket_get_fd(sh.fd);
            if (client.socket_get_serial(id)) {
                if (client.disconnect_from_server()) {
                    id.get<size_t>(&sh.size);
                    id.get<size_t>(&sh.reference_count);
                    return true;
                } else
                    LOG_ERROR("failed to disconnect from the server");
            } else
                LOG_ERROR("failed to get serial from the server");
        } else
            LOG_ERROR("failed to send window to the server");
    } else
        LOG_ERROR("failed to connect to server");
    return false;
}

bool GLIS_shared_memory_params_get(GLIS_shared_memory &sh) {
    SERVER_LOG_TRANSFER_INFO = true;
    SOCKET_CLIENT client;
    serializer cmd;
    serializer id;
    cmd.add<int>(GLIS_SERVER_COMMANDS.shm_params);
    if (client.connect_to_server()) {
        if (client.socket_put_serial(cmd)) {
            client.socket_get_fd(sh.fd);
            if (client.socket_get_serial(id)) {
                if (client.disconnect_from_server()) {
                    id.get<size_t>(&sh.size);
                    id.get<size_t>(&sh.reference_count);
                    return true;
                } else
                    LOG_ERROR("failed to disconnect from the server");
            } else
                LOG_ERROR("failed to get serial from the server");
        } else
            LOG_ERROR("failed to send window to the server");
    } else
        LOG_ERROR("failed to connect to server");
    return false;
}

bool GLIS_shared_memory_open(GLIS_shared_memory &sh) {
    return SHM_open(sh.fd, &sh.data, sh.size);
}

void GLIS_shared_memory_clear(GLIS_shared_memory &sh) {
    memset(sh.data, 0, sh.size);
}

GLIS_shared_memory GLIS_INTERNAL_SHARED_MEMORY_PARAMETER;
GLIS_shared_memory GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA;
SOCKET_CLIENT KEEP_ALIVE;

// this function gets called whenever a handle is passed
void GLIS_shared_memory_increase_reference(GLIS_shared_memory &shared_memory) {
    shared_memory.reference_count++;
}

void *KEEP_ALIVE_MAIN_NOTIFIER(void *arg) {
    int *ret = new int;
    assert(arg != nullptr);
    struct pa {
        size_t table_id;

        class GLIS_shared_memory *params;
    } *p;
    p = static_cast<pa *>(arg);
    SOCKET_SERVER *server = SERVER_get(p->table_id);
    server->socket_accept();
    server->connection_wait_until_disconnect();
    server->shutdownServer();
    LOG_INFO_SERVER("params.reference_count = %zu", p->params->reference_count);
    p->params->reference_count--;
    LOG_INFO_SERVER("params.reference_count = %zu", p->params->reference_count);
    *ret = 0;
    return ret;
}

bool GLIS_start_drawing() {
    SERVER_LOG_TRANSFER_INFO = true;
    SOCKET_CLIENT client;
    serializer cmd;
    serializer server;
    cmd.add<int>(GLIS_SERVER_COMMANDS.start_drawing);
    if (client.connect_to_server()) {
        if (client.socket_put_serial(cmd)) {
            if (client.socket_get_serial(server)) {
                if (client.disconnect_from_server()) {
                    bool ret = false;
                    server.get<bool>(&ret);
                    return ret == true;
                } else
                    LOG_ERROR("failed to disconnect from the server");
            } else
                LOG_ERROR("failed to get serial from the server");
        } else
            LOG_ERROR("failed to send command to the server");
    } else
        LOG_ERROR("failed to connect to server");
    return false;
}

bool GLIS_stop_drawing() {
    SERVER_LOG_TRANSFER_INFO = true;
    SOCKET_CLIENT client;
    serializer cmd;
    serializer server;
    cmd.add<int>(GLIS_SERVER_COMMANDS.stop_drawing);
    if (client.connect_to_server()) {
        if (client.socket_put_serial(cmd)) {
            if (client.socket_get_serial(server)) {
                if (client.disconnect_from_server()) {
                    bool ret = false;
                    server.get<bool>(&ret);
                    return ret == true;
                } else
                    LOG_ERROR("failed to disconnect from the server");
            } else
                LOG_ERROR("failed to get serial from the server");
        } else
            LOG_ERROR("failed to send command to the server");
    } else
        LOG_ERROR("failed to connect to server");
    return false;
}

bool GLIS_SHARED_MEMORY_INITIALIZED = false;
size_t GLIS_client_id = -1;

bool GLIS_INIT_SHARED_MEMORY(int w, int h) {
    if (GLIS_SHARED_MEMORY_INITIALIZED) return true;

    SERVER_LOG_TRANSFER_INFO = true;
    SOCKET_CLIENT client;
    serializer cmd;
    serializer server;
    cmd.add<int>(GLIS_SERVER_COMMANDS.new_connection);
    if (client.connect_to_server()) {
        if (client.socket_put_serial(cmd)) {
            if (client.socket_get_serial(server)) {
                if (client.disconnect_from_server()) {
                    char *server_name;
                    server.get_raw_pointer<char>(&server_name);
                    KEEP_ALIVE.set_name(server_name);
                    delete[] server_name;
                    if (KEEP_ALIVE.connect_to_server()) {
                        server.get<size_t>(&GLIS_client_id);
                        assert(GLIS_shared_memory_malloc(GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA,
                                                         sizeof(size_t) +
                                                         sizeof(int8_t) +
                                                         (sizeof(GLuint) * w * h)
                        )
                        );
                        assert(ashmem_valid(GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA.fd));
                        LOG_INFO("sending id %d, size: %zu", GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA.fd,
                                 GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA.size);
                        assert(GLIS_shared_memory_malloc(GLIS_INTERNAL_SHARED_MEMORY_PARAMETER,
                                                         sizeof(size_t) + sizeof(int8_t) +
                                                         (sizeof(int8_t) * 4)));
                        assert(ashmem_valid(GLIS_INTERNAL_SHARED_MEMORY_PARAMETER.fd));
                        LOG_INFO("sending id %d, size: %zu", GLIS_INTERNAL_SHARED_MEMORY_PARAMETER.fd,
                                 GLIS_INTERNAL_SHARED_MEMORY_PARAMETER.size);
                        SOCKET_CLIENT client;
                        serializer cmd1;
                        serializer id;
                        GLIS_shared_memory_increase_reference(GLIS_INTERNAL_SHARED_MEMORY_PARAMETER);
                        GLIS_shared_memory_increase_reference(GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA);
                        cmd1.add<int>(GLIS_SERVER_COMMANDS.shm_params);
                        cmd1.add<size_t>(GLIS_client_id);
                        cmd1.add<size_t>(GLIS_INTERNAL_SHARED_MEMORY_PARAMETER.size);
                        cmd1.add<size_t>(GLIS_INTERNAL_SHARED_MEMORY_PARAMETER.reference_count);
                        cmd1.add<size_t>(GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA.size);
                        cmd1.add<size_t>(GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA.reference_count);
                        if (client.connect_to_server()) {
                            if (client.socket_put_serial(cmd1)) {
                                client.socket_put_fd(GLIS_INTERNAL_SHARED_MEMORY_PARAMETER.fd);
                                client.socket_put_fd(GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA.fd);
                                if (client.socket_get_serial(id)) {
                                    if (client.disconnect_from_server()) {
                                        bool ret;
                                        id.get<bool>(&ret);
                                        if (ret == true) {
                                            GLIS_SHARED_MEMORY_INITIALIZED = true;
                                            return true;
                                        } else
                                            LOG_ERROR("failed to initialize shared memory");
                                    } else
                                        LOG_ERROR("failed to disconnect from the server");
                                } else
                                    LOG_ERROR("failed to get serial from the server");
                            } else
                                LOG_ERROR("failed to send serial to the server");
                        } else
                            LOG_ERROR("failed to connect to server");
                    } else
                        LOG_ERROR("failed to connect to the server");
                } else
                    LOG_ERROR("failed to disconnect from the server");
            } else
                LOG_ERROR("failed to get serial from the server");
        } else
            LOG_ERROR("failed to send command to the server");
    } else
        LOG_ERROR("failed to connect to server");
    return false;
}

size_t GLIS_new_window(int x, int y, int w, int h) {
    serializer window;
    serializer id;
    int win[4] = {x, y, x + w, y + h};
    window.add<int>(GLIS_SERVER_COMMANDS.new_window);
    window.add<size_t>(GLIS_client_id);
    window.add_pointer<int>(win, 4);
    SOCKET_CLIENT client;
    if (client.connect_to_server()) {
        if (client.socket_put_serial(window)) {
            if (client.socket_get_serial(id)) {
                if (client.disconnect_from_server()) {
                    size_t window_id;
                    id.get<size_t>(&window_id);
                    return window_id;
                } else
                    LOG_ERROR("failed to disconnect from the server");
            } else
                LOG_ERROR("failed to get serial from the server");
        } else
            LOG_ERROR("failed to send command to the server");
    } else
        LOG_ERROR("failed to connect to server");
    return static_cast<size_t>(-1);
}

bool GLIS_modify_window(size_t window_id, int x, int y, int w, int h) {
    serializer window;
    int win[4] = {x, y, x + w, y + h};
    window.add<int>(GLIS_SERVER_COMMANDS.modify_window);
    window.add<size_t>(window_id);
    window.add_pointer<int>(win, 4);
    SOCKET_CLIENT client;
    if (client.connect_to_server()) {
        if (client.socket_put_serial(window)) {
            if (client.disconnect_from_server()) return true;
            else
                LOG_ERROR("failed to disconnect from the server");
        } else
            LOG_ERROR("failed to send command to the server");
    } else
        LOG_ERROR("failed to connect to server");
    return false;
}

bool GLIS_close_window(size_t window_id) {
    serializer window;
    window.add<int>(GLIS_SERVER_COMMANDS.close_window);
    window.add<size_t>(window_id);
    SOCKET_CLIENT client;
    if (client.connect_to_server()) {
        if (client.socket_put_serial(window)) {
            if (client.disconnect_from_server()) return true;
            else
                LOG_ERROR("failed to disconnect from the server");
        } else
            LOG_ERROR("failed to send command to the server");
    } else
        LOG_ERROR("failed to connect to server");
    return false;
}

void
GLIS_upload_texture_resize(GLIS_CLASS &GLIS, size_t &window_id, GLuint &texture_id,
                           GLint texture_width,
                           GLint texture_height, GLint texture_width_to,
                           GLint texture_height_to) {
    LOG_INFO("uploading texture");
    GLIS_SwapBuffers(GLIS);
    GLIS_Sync_GPU();
    if (IPC == IPC_MODE.socket || IPC == IPC_MODE.shared_memory) {
        if (texture_width_to != 0 && texture_height_to != 0) {
            LOG_ERROR("resizing from %dx%d to %dx%d",
                      texture_width, texture_height, texture_width_to, texture_height_to);
            GLIS_resize(&TEXDATA, TEXDATA_LEN, texture_width, texture_height, texture_width_to,
                        texture_height_to);
            LOG_ERROR("resized from %dx%d to %dx%d",
                      texture_width, texture_height, texture_width_to, texture_height_to);
            assert(TEXDATA != nullptr);
        } else {
            TEXDATA_LEN = texture_width * texture_height * sizeof(GLuint);
            TEXDATA = new GLuint[TEXDATA_LEN];
            memset(TEXDATA, 0, TEXDATA_LEN);

                glReadPixels(0, 0, texture_width, texture_height, GL_RGBA, GL_UNSIGNED_BYTE,
                             TEXDATA)
            ;
        }
        serializer tex;
        tex.add<int>(GLIS_SERVER_COMMANDS.texture);
        tex.add<size_t>(GLIS_client_id);
        tex.add<size_t>(window_id);
        GLint tex_dimens[2] = {
            texture_width_to != 0 ? texture_width_to : texture_width,
            texture_height_to != 0 ? texture_height_to : texture_height
        };
        tex.add_pointer<GLint>(tex_dimens, 2);
        if (IPC == IPC_MODE.shared_memory) {
            LOG_INFO("writing");
            SOCKET_CLIENT client;
            if (client.connect_to_server()) {
                if (client.socket_put_serial(tex)) {
                    if (!client.disconnect_from_server())
                        LOG_ERROR("failed to disconnect from server");
                } else
                    LOG_ERROR("failed to send texture to server");
            } else
                LOG_ERROR("failed to connect to server");
            LOG_INFO("wrote");
            LOG_INFO("writing");
            GLIS_shared_memory_write_texture(GLIS_INTERNAL_SHARED_MEMORY_TEXTURE_DATA,
                                             reinterpret_cast<int8_t *>(TEXDATA), TEXDATA_LEN);
            LOG_INFO("wrote");
        } else if (IPC == IPC_MODE.socket) {
            tex.add_pointer<GLuint>(TEXDATA, TEXDATA_LEN);
            SOCKET_CLIENT client;
            if (client.connect_to_server()) {
                if (client.socket_put_serial(tex)) {
                    if (!client.disconnect_from_server())
                        LOG_ERROR("failed to disconnect from server");
                } else
                    LOG_ERROR("failed to send texture to server");
            } else
                LOG_ERROR("failed to connect to server");
        }
        delete TEXDATA;
        LOG_INFO("uploaded texture");
        return;
    } else {
        while (SYNC_STATE != STATE.request_upload) {}
        SYNC_STATE = STATE.response_uploading;
        if (IPC == IPC_MODE.thread) {
            GLIS_current_texture = texture_id;
            SYNC_STATE = STATE.response_uploaded;
        } else if (IPC == IPC_MODE.texture) {
            TEXDATA_LEN = texture_width * texture_height * sizeof(GLuint);
            TEXDATA = new GLuint[TEXDATA_LEN];

                glReadPixels(0, 0, texture_width, texture_height, GL_RGBA, GL_UNSIGNED_BYTE,
                             TEXDATA);
            SYNC_STATE = STATE.response_uploaded;
        }
        LOG_INFO("uploaded texture");
        LOG_INFO("requesting SERVER to render");
        SYNC_STATE = STATE.request_render;
        while (SYNC_STATE != STATE.response_rendered) {}
        LOG_INFO("SERVER has rendered");
        return;
    }
}

void
GLIS_upload_texture(GLIS_CLASS &GLIS, size_t &window_id, GLuint &texture_id, GLint texture_width,
                    GLint texture_height) {
    GLIS_upload_texture_resize(GLIS, window_id, texture_id, texture_width, texture_height, 0, 0);
}

#endif //GLNE_GLIS_COMMANDS_H
