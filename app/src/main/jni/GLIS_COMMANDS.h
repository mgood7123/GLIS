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
            slot_() {} // default constructor required
            class slot__ {
            public:
                slot__() {} // default constructor required
                slot__(GLIS_shared_memory *pMemory) {
                    shared_memory = pMemory;
                    assert(pMemory != 0x0);
                };
                GLIS_shared_memory * shared_memory = nullptr;
                int slot = 0;
                int size = 0;

                void * load_ptr() {
                    return &shared_memory->data[slot];
                }

                void store_int8_t(int8_t value) {
                    shared_memory->data[slot] = value;
                }
                void store_int16_t(int16_t value) {
                    reinterpret_cast<int16_t*>(&shared_memory->data[slot])[0] = value;
                }
                void store_int32_t(int32_t value) {
                    reinterpret_cast<int32_t*>(&shared_memory->data[slot])[0] = value;
                }
                void store_int64_t(int64_t value) {
                    reinterpret_cast<int64_t*>(&shared_memory->data[slot])[0] = value;
                }
                void store_size_t(size_t value) {
                    reinterpret_cast<size_t*>(&shared_memory->data[slot])[0] = value;
                }

                int8_t load_int8_t() {
                    return shared_memory->data[slot];
                }
                int16_t load_int16_t() {
                    return reinterpret_cast<int16_t*>(&shared_memory->data[slot])[0];
                }
                int32_t load_int32_t() {
                    return reinterpret_cast<int32_t*>(&shared_memory->data[slot])[0];
                }
                int64_t load_int64_t() {
                    return reinterpret_cast<int64_t*>(&shared_memory->data[slot])[0];
                }
                size_t load_size_t() {
                    return reinterpret_cast<size_t*>(&shared_memory->data[slot])[0];
                }
            };

            class multi_size {
            public:
                multi_size() {} // default constructor required
                multi_size(GLIS_shared_memory *pMemory) {
                    type_int8_t = slot__(pMemory);
                    type_int16_t = slot__(pMemory);
                    type_int32_t = slot__(pMemory);
                    type_int64_t = slot__(pMemory);
                    type_size_t = slot__(pMemory);
                };
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


void GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS__(GLIS_shared_memory & shared_memory, int w, int h) {
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS(shared_memory.slot.total_size, shared_memory.slot.status, sizeof(int8_t));
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.command, sizeof(int8_t), shared_memory.slot.status);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_int8_t, sizeof(int8_t), shared_memory.slot.command);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_0.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_0.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_0.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_0.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_0.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_1.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_1.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_1.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_1.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_1.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_2.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_2.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_2.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_2.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_2.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_3.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_3.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_3.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_3.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_3.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_4.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_4.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_4.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_4.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_4.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_0.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_0.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_0.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_size_t, sizeof(size_t), shared_memory.slot.result_data_0.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_int8_t, sizeof(int8_t), shared_memory.slot.result_data_0.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_1.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_1.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_1.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_size_t, sizeof(size_t), shared_memory.slot.result_data_1.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_int8_t, sizeof(int8_t), shared_memory.slot.result_data_1.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_2.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_2.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_2.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_size_t, sizeof(size_t), shared_memory.slot.result_data_2.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_int8_t, sizeof(int8_t), shared_memory.slot.result_data_2.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_3.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_3.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_3.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_size_t, sizeof(size_t), shared_memory.slot.result_data_3.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_int8_t, sizeof(int8_t), shared_memory.slot.result_data_3.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_4.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_4.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_4.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_size_t, sizeof(size_t), shared_memory.slot.result_data_4.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.texture, (sizeof(GLuint) * w * h), shared_memory.slot.result_data_4.type_size_t);
}

bool GLIS_shared_memory_open(GLIS_shared_memory &sh) {
    return SHM_open(sh.fd, &sh.data, sh.size);
}

void GLIS_shared_memory_clear(GLIS_shared_memory &sh) {
    memset(sh.data, 0, sh.size);
}

bool GLIS_shared_memory_malloc(GLIS_shared_memory &sh, size_t size) {
    sh.size = size;
    return SHM_create(sh.fd, &sh.data, sh.size);
}

bool GLIS_shared_memory_realloc(GLIS_shared_memory &sh, size_t size) {
    return SHM_resize(sh.fd, &sh.data, size);
}

bool GLIS_shared_memory_free(GLIS_shared_memory &sh) {
    if (SHM_close(sh.fd)) {
        sh.data = nullptr;
        sh.size = 0;
        return true;
    }
    return false;
}

SOCKET_CLIENT KEEP_ALIVE;

// this function gets called whenever a handle is passed
void GLIS_shared_memory_increase_reference(GLIS_shared_memory &shared_memory) {
    shared_memory.reference_count++;
}

void *KEEP_ALIVE_MAIN_NOTIFIER(void *arg) {
    int *ret = new int;
    assert(arg != nullptr);
    class Client {
    public:
        class GLIS_shared_memory shared_memory;
        SOCKET_SERVER * server = nullptr;
        size_t server_id = 0;
        size_t id = -1;
        size_t table_id = 0;
        bool connected = false;
    };
    Client * client = static_cast<Client *>(arg);
    SOCKET_SERVER *server = SERVER_get(client->table_id);
    LOG_ERROR_SERVER("CLIENT ID: %zu, connecting", client->id);
    assert(server->socket_accept());
    LOG_ERROR_SERVER("CLIENT ID: %zu, connected", client->id);
    client->connected = true;
    server->connection_wait_until_disconnect();
    LOG_INFO_SERVER("CLIENT ID: %zu, closed its connection", client->id);
    SERVER_deallocate_server(client->table_id);
    LOG_INFO_SERVER("CLIENT ID: %zu, shared_memory.reference_count = %zu", client->id, client->shared_memory.reference_count);
    client->shared_memory.reference_count--;
    LOG_INFO_SERVER("CLIENT ID: %zu, shared_memory.reference_count = %zu", client->id, client->shared_memory.reference_count);
    *ret = 0;
    return ret;
}

bool GLIS_start_drawing() {
    SERVER_LOG_TRANSFER_INFO = true;
    serializer cmd;
    serializer server;
    cmd.add<int>(GLIS_SERVER_COMMANDS.start_drawing);
    if (GLIS_CLIENT.socket_put_serial(cmd)) {
        if (GLIS_CLIENT.socket_get_serial(server)) {
            bool ret = false;
            server.get<bool>(&ret);
            return ret == true;
        } else
            LOG_ERROR("failed to get serial from the server");
    } else
        LOG_ERROR("failed to send command to the server");
    return false;
}

bool GLIS_stop_drawing() {
    SERVER_LOG_TRANSFER_INFO = true;
    serializer cmd;
    serializer server;
    cmd.add<int>(GLIS_SERVER_COMMANDS.stop_drawing);
    if (GLIS_CLIENT.socket_put_serial(cmd)) {
        if (GLIS_CLIENT.socket_get_serial(server)) {
            bool ret = false;
            server.get<bool>(&ret);
            return ret == true;
        } else
            LOG_ERROR("failed to get serial from the server");
    } else
        LOG_ERROR("failed to send command to the server");
    return false;
}

void GLIS_sync_server(const char * operation, size_t id) {
    LOG_ERROR("waiting for %s (for window id %zu) to complete on server side", operation, id);
    GLIS_INTERNAL_SHARED_MEMORY.slot.status.store_int8_t(GLIS_INTERNAL_SHARED_MEMORY.status.ready_to_be_read);
    while (GLIS_INTERNAL_SHARED_MEMORY.slot.status.load_int8_t() == GLIS_INTERNAL_SHARED_MEMORY.status.ready_to_be_read);
    LOG_ERROR("%s (for window id %zu) has completed on server side", operation, id);
}

bool GLIS_INIT_SHARED_MEMORY(int w, int h) {
    if (GLIS_SHARED_MEMORY_INITIALIZED) return true;

    SERVER_LOG_TRANSFER_INFO = true;
    SOCKET_CLIENT client;
    serializer cmd;
    serializer server;
    cmd.add<int>(GLIS_SERVER_COMMANDS.new_connection);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS__(GLIS_INTERNAL_SHARED_MEMORY, w, h);
    assert(GLIS_shared_memory_malloc(
        GLIS_INTERNAL_SHARED_MEMORY, GLIS_INTERNAL_SHARED_MEMORY.slot.total_size
    ));
    assert(ashmem_valid(GLIS_INTERNAL_SHARED_MEMORY.fd));
    GLIS_shared_memory_increase_reference(GLIS_INTERNAL_SHARED_MEMORY);
    cmd.add<size_t>(GLIS_INTERNAL_SHARED_MEMORY.size);
    cmd.add<size_t>(GLIS_INTERNAL_SHARED_MEMORY.reference_count);
    cmd.add<int>(w);
    cmd.add<int>(h);
    if (client.connect_to_server()) {
        if (client.socket_put_serial(cmd)) {
            client.socket_put_fd(GLIS_INTERNAL_SHARED_MEMORY.fd);
            if (client.socket_get_serial(server)) {
                if (client.disconnect_from_server()) {
                    char *server_name;
                    server.get_raw_pointer<char>(&server_name);
                    KEEP_ALIVE.set_name(server_name);
                    delete[] server_name;
                    LOG_ERROR("connecting to keep alive server");
                    if (KEEP_ALIVE.connect_to_server()) {
                        GLIS_sync_server("GLIS_INIT_SHARED_MEMORY", -1);
                        bool ret = false;
                        server.get<bool>(&ret);
                        if (ret == true) {
                            GLIS_SHARED_MEMORY_INITIALIZED = true;
                            return true;
                        } else
                            LOG_ERROR("failed to initialize shared memory");
                    } else
                        LOG_ERROR("failed to connect to server");
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
    GLIS_INTERNAL_SHARED_MEMORY.slot.command.store_int8_t(GLIS_SERVER_COMMANDS.new_window);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_0.type_int64_t.store_int64_t(x);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_1.type_int64_t.store_int64_t(y);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_2.type_int64_t.store_int64_t(x + w);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_3.type_int64_t.store_int64_t(y + h);
    GLIS_sync_server("GLIS_new_window", -1);
    return GLIS_INTERNAL_SHARED_MEMORY.slot.result_data_0.type_size_t.load_size_t();
}

bool GLIS_modify_window(size_t window_id, int x, int y, int w, int h) {
    GLIS_INTERNAL_SHARED_MEMORY.slot.command.store_int8_t(GLIS_SERVER_COMMANDS.modify_window);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_0.type_int64_t.store_int64_t(x);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_1.type_int64_t.store_int64_t(y);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_2.type_int64_t.store_int64_t(x + w);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_3.type_int64_t.store_int64_t(y + h);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_4.type_size_t.store_size_t(window_id);
    GLIS_sync_server("GLIS_modify_window", window_id);
    return true;
}

bool GLIS_close_window(size_t window_id) {
    GLIS_INTERNAL_SHARED_MEMORY.slot.command.store_int8_t(GLIS_SERVER_COMMANDS.close_window);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_0.type_size_t.store_size_t(window_id);
    GLIS_sync_server("GLIS_close_window", window_id);
    return true;
}

void
GLIS_upload_texture(GLIS_CLASS &GLIS, size_t &window_id, GLuint &texture_id, GLint texture_width,
                    GLint texture_height) {
    LOG_INFO("uploading texture");
    GLIS_SwapBuffers(GLIS);
    GLIS_Sync_GPU();
    GLIS_INTERNAL_SHARED_MEMORY.slot.command.store_int8_t(GLIS_SERVER_COMMANDS.texture);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_0.type_int64_t.store_int64_t(texture_width);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_1.type_int64_t.store_int64_t(texture_height);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_2.type_size_t.store_size_t(window_id);
    glReadPixels(0, 0, texture_width, texture_height, GL_RGBA, GL_UNSIGNED_BYTE, GLIS_INTERNAL_SHARED_MEMORY.slot.texture.load_ptr());
    GLIS_sync_server("GLIS_upload_texture", window_id);
    LOG_INFO("uploaded texture");
}

void
GLIS_upload_texture_resize(GLIS_CLASS &GLIS, size_t &window_id, GLuint &texture_id,
                           GLint texture_width,
                           GLint texture_height, GLint texture_width_to,
                           GLint texture_height_to) {
//    if (IPC == IPC_MODE.socket || IPC == IPC_MODE.shared_memory) {
//        if (texture_width_to != 0 && texture_height_to != 0) {
//            LOG_ERROR("resizing from %dx%d to %dx%d",
//                      texture_width, texture_height, texture_width_to, texture_height_to);
//            GLIS_resize(&TEXDATA, TEXDATA_LEN, texture_width, texture_height, texture_width_to,
//                        texture_height_to);
//            LOG_ERROR("resized from %dx%d to %dx%d",
//                      texture_width, texture_height, texture_width_to, texture_height_to);
//            assert(TEXDATA != nullptr);
//        }
//        } else {
//        LOG_INFO("uploaded texture");
//        return;
//    } else {
//        while (SYNC_STATE != STATE.request_upload) {}
//        SYNC_STATE = STATE.response_uploading;
//        if (IPC == IPC_MODE.thread) {
//            GLIS_current_texture = texture_id;
//            SYNC_STATE = STATE.response_uploaded;
//        } else if (IPC == IPC_MODE.texture) {
//            TEXDATA_LEN = texture_width * texture_height * sizeof(GLuint);
//            TEXDATA = new GLuint[TEXDATA_LEN];
//
//                glReadPixels(0, 0, texture_width, texture_height, GL_RGBA, GL_UNSIGNED_BYTE,
//                             TEXDATA);
//            SYNC_STATE = STATE.response_uploaded;
//        }
//        LOG_INFO("uploaded texture");
//        LOG_INFO("requesting SERVER to render");
//        SYNC_STATE = STATE.request_render;
//        while (SYNC_STATE != STATE.response_rendered) {}
//        LOG_INFO("SERVER has rendered");
//        return;
//    }
    LOG_ERROR("GLIS_upload_texture_resize has been depreciated");
    abort();
}

#endif //GLNE_GLIS_COMMANDS_H
