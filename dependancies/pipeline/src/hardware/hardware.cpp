#define ELPP_STL_LOGGING
#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP
#include <rigtorp/SPSCQueue.h>
#include <deque>

#define HardwarePrintModifiersPrintValue(value) #value << ": " << value
#define HardwarePrintModifiersAlphaBool(boolean) #boolean << ": " << (boolean ? "true" : "false")
#define HardwarePrint CLOG(INFO, "hardware")
#define HardwarePrintIf(condition) CLOG_IF(condition, INFO, "hardware")

template <typename T, int CAPACITY = 1>
struct Flop {
    rigtorp::SPSCQueue<T> * input = nullptr;
    rigtorp::SPSCQueue<T> * output = nullptr;
    bool hasOutput = false;
    bool hasInput = false;
    bool debug_output = false;
    
    Flop() {
        input = new rigtorp::SPSCQueue<T>(CAPACITY);
        output = new rigtorp::SPSCQueue<T>(CAPACITY);
    }
    
    Flop(const bool & debug_output) {
        input = new rigtorp::SPSCQueue<T>(CAPACITY);
        output = new rigtorp::SPSCQueue<T>(CAPACITY);
        this->debug_output = debug_output;
    }
    
    Flop(const Flop & flop) {
        // copy constructor
        // input can only be moved
        std::swap(input, const_cast<Flop&>(flop).input);
        // output can only be moved
        std::swap(output, const_cast<Flop&>(flop).output);
        debug_output = flop.debug_output;
    }
    
    Flop(Flop && flop) {
        // move constructor
        std::swap(input, flop.input);
        std::swap(output, flop.output);
        std::swap(debug_output, flop.debug_output);
    }
    
    Flop & operator=(const Flop & flop) {
        // copy assign
        // input can only be moved
        std::swap(input, const_cast<Flop&>(flop).input);
        // output can only be moved
        std::swap(output, const_cast<Flop&>(flop).output);
        debug_output = flop.debug_output;
        return *this;
    }
    
    Flop & operator=(Flop && flop) {
        // move assign
        std::swap(input, flop.input);
        std::swap(output, flop.output);
        std::swap(debug_output, flop.debug_output);
        return *this;
    }
    
    ~Flop() {
        delete input;
        delete output;
    }
    
    bool has_input() {
        return input->front() != nullptr;
    }
    
    bool has_output() {
        return output->front() != nullptr;
    }
    
    void push_input(T && in) {
        HardwarePrintIf(debug_output) << "[FLOP   ] " << "pushing input";
        HardwarePrintIf(debug_output) << "[FLOP   ] " << HardwarePrintModifiersPrintValue(input->size());
        input->push(std::move(in));
        HardwarePrintIf(debug_output) << "[FLOP   ] " << "pushed input";
        HardwarePrintIf(debug_output) << "[FLOP   ] " << HardwarePrintModifiersPrintValue(input->size());
        hasInput = true;
    }
    
    void push_output(T && out) {
        HardwarePrintIf(debug_output) << "[FLOP   ] " << "pushing output";
        HardwarePrintIf(debug_output) << "[FLOP   ] " << HardwarePrintModifiersPrintValue(output->size());
        output->push(std::move(out));
        HardwarePrintIf(debug_output) << "[FLOP   ] " << "pushed ouput";
        HardwarePrintIf(debug_output) << "[FLOP   ] " << HardwarePrintModifiersPrintValue(output->size());
        hasOutput = true;
    }
    
    T && pull_input() {
        HardwarePrintIf(debug_output) << "[FLOP   ] " << "pulling input";
        HardwarePrintIf(debug_output) << "[FLOP   ] " << HardwarePrintModifiersPrintValue(input->size());
        T && in = std::move(*input->front());
        input->pop();
        HardwarePrintIf(debug_output) << "[FLOP   ] " << "pulled input";
        HardwarePrintIf(debug_output) << "[FLOP   ] " << HardwarePrintModifiersPrintValue(input->size());
        hasInput = false;
        return std::move(in);
    }
    
    T && pull_output() {
        HardwarePrintIf(debug_output) << "[FLOP   ] " << "pulling output";
        HardwarePrintIf(debug_output) << "[FLOP   ] " << HardwarePrintModifiersPrintValue(output->size());
        T && out = std::move(*output->front());
        output->pop();
        HardwarePrintIf(debug_output) << "[FLOP   ] " << "pulled output";
        HardwarePrintIf(debug_output) << "[FLOP   ] " << HardwarePrintModifiersPrintValue(output->size());
        hasOutput = false;
        return std::move(out);
    }
    
    void exec() {
        HardwarePrintIf(debug_output) << "[FLOP   ] " << HardwarePrintModifiersPrintValue(input->front());
        if (input->front()) {
            push_output(std::move(pull_input()));
        }
    }
};

template <typename T>
struct Wire {
    std::string id = "Undefined";
    Flop<T, 1> data;
    std::deque<std::string> outputs;
    
    Wire() {}
    
    Wire(const std::string & id) {
        this->id = id;
    }
    
    Wire(const Wire & wire) {
        id = wire.id;
        data = wire.data;
        outputs = wire.outputs;
    }
    
    Wire(Wire && wire) {
        std::swap(id, wire.id);
        std::swap(data, wire.data);
        std::swap(outputs, wire.outputs);
    }
    
    Wire & operator=(const Wire & wire) {
        id = wire.id;
        data = wire.data;
        outputs = wire.outputs;
        return *this;
    }
    
    Wire & operator=(Wire && wire) {
        std::swap(id, wire.id);
        std::swap(data, wire.data);
        std::swap(outputs, wire.outputs);
        return *this;
    }
    
    void push(T & input) {
        data.push_input(std::move(input));
        data.exec();
    }
    
    void push(T && input) {
        data.push_input(std::move(input));
        data.exec();
    }
    
    T && pull() {
        return std::move(data.pull_output());
    }
};

struct ComponentTypes {
    int Undefined = -1;
    int Wire = 0;
}

struct Component {
    int type = Undefined;
    std::variant<Wire> component;
}

template <typename T>
struct Hardware {
    el::Logger* logger = nullptr;
    el::Configurations config;
    
    Hardware() {
        logger = el::Loggers::getLogger("hardware");
        config.setToDefault();
        config.setGlobally(el::ConfigurationType::Format, "[%logger:%level] %msg");
        el::Loggers::reconfigureLogger("hardware", config);
    }
    
    std::deque<Wire<T>> wires;
    
    void addWire(const char * wireName) {
        wires.push_back(Wire<T>(wireName));
    }
    
    Wire<T> * findWire(const std::string & id) {
        const char * wire_id = id.c_str();
        const size_t wire_id_length = id.length();
        for (Wire<T> & wire: wires) {
            const char * wire_id_cstr = wire.id.c_str();
            if (memcmp(wire_id, wire_id_cstr, wire_id_length) == 0) {
                return &wire;
            }
        }
        return nullptr;
    }
    
    void connectWires(const std::string & wire_1, const std::string & wire_2) {
        Wire<T> * in = findWire(wire_1);
        CHECK_NE(in, nullptr);
        CHECK_NE(findWire(wire_2), nullptr);
        in->outputs.push_back(wire_2);
    }
    
    void run(const std::string & id, T input) {
        // TODO: upgrade to Component types
        Wire<T> * start = findWire(id);
        CHECK_NE(start, nullptr);
        start->push(std::move(input));
        T && copy = std::move(start->pull());
        CHECK_EQ(copy, input);
        HardwarePrint << HardwarePrintModifiersPrintValue(copy);
        HardwarePrint << HardwarePrintModifiersPrintValue(input);
        HardwarePrint << HardwarePrintModifiersPrintValue(start->outputs);
        if (!start->outputs.empty()) {
            for (std::string & out_id : start->outputs) {
                HardwarePrint << HardwarePrintModifiersPrintValue(out_id);
                Wire<T> * out = findWire(out_id);
                CHECK_NE(out, nullptr);
                out->push(copy);
                {
                    T && output = std::move(out->pull());
                    CHECK_EQ(output, input);
                    HardwarePrint << HardwarePrintModifiersPrintValue(copy);
                    HardwarePrint << HardwarePrintModifiersPrintValue(output);
                    HardwarePrint << HardwarePrintModifiersPrintValue(input);
                }
            }
        }
    }
};

int main() {
    Hardware<bool> hardware;
    hardware.addWire("A");
    hardware.addWire("B");
    hardware.addWire("C");
    hardware.connectWires("A", "B");
    hardware.connectWires("A", "C");
    hardware.run("A", 1);
    return 0;
}
