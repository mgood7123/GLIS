#include <array>
#include <algorithm>
#include <iostream>

// https://godbolt.org/ to verify compile-time execution

template <typename T, size_t N>
struct array {
    T elems[N];

    constexpr T * begin() noexcept { return elems; }
    constexpr T const* begin() const noexcept { return elems; }
    constexpr T const* cbegin() const noexcept { return elems; }
    constexpr T * end() noexcept { return &elems[N]; }
    constexpr T const* end() const noexcept { return &elems[N]; }
    constexpr T const* cend() const noexcept { return &elems[N]; }
    constexpr size_t size() const noexcept { return N; }
    constexpr T & operator[] (size_t i) noexcept { return elems[i]; }
};

constexpr auto append(int value) noexcept {
    array<int, 1> one = { value };
    return one;
}

template <typename A, typename B, typename C>
constexpr void copy(const A * start, const B * end, C * start_) noexcept {
    while(start != end) {
        *start_ = *start;
        start++;
        start_++;
    }
}

template<size_t N>
constexpr auto duplicate(const array<int, N>& array_) noexcept {
    array<int, N> result {0};
    copy (array_.cbegin(), array_.cend(), result.begin());
    return result;
}

template<size_t N>
constexpr auto append(const array<int, N>& array_, int value) noexcept {
    array<int, N+1> result {0};
    copy (array_.cbegin(), array_.cend(), result.begin());

    auto one = append(value);
    copy(one.cbegin(), one.cend(), result.end()-1);

    return result;
}

template<size_t N>
constexpr auto replace(const array<int, N>& array_, int value) noexcept {
    array<int, N> result = duplicate(array_);

    auto one = append(value);
    copy(one.cbegin(), one.cend(), result.end()-1);

    return result;
}

template <typename T>
void printArray(const T & a) {
    for (auto& x : a ) std::cout << x << " ";
    std::cout << std::endl;
}

struct commands {
    constexpr static int draw_nothing = 0;
    constexpr static int draw_line = 1;
    constexpr static int draw_lines = 2;
};

struct Instance {
    
    constexpr auto nothing() {
        array<int, 1> array_ { commands::draw_nothing };
        return array_;
    }
    
    template<size_t N>
    constexpr auto line(const array<int, N>& array_, int x, int y) noexcept {
        auto a = append(array_, commands::draw_line);
        auto b = append(a, x);
        auto c = append(b, y);
        return c;
    }

    constexpr auto line(int x, int y) noexcept {
        auto a = replace(nothing(), commands::draw_line);
        auto b = append(a, x);
        auto c = append(b, y);
        return c;
    }
    
    template<size_t N>
    constexpr auto optimize(const array<int, N>& array_) noexcept {
        auto result = duplicate(array_);
        auto start = result.begin();
        auto end = result.end();
        for (int i = 0; i < N-1; i++) {
            if (start[i] == commands::draw_line && start[i+3] == commands::draw_line) {
                start[i] = commands::draw_lines;
                start[i+3] = start[i+4];
                start[i+4] = start[i+5];
                start[i+5] = commands::draw_nothing;
            }
        }
        return result;
    }
};

Instance r;

template<size_t N1, size_t N2>
constexpr auto concat(const array<int, N1>& array_, const array<int, N2>& array__) noexcept {
    array<int, N1+N2> result {0};
    copy (array_.cbegin(), array_.cend(), result.begin());
    copy (array__.cbegin(), array__.cend(), result.end()-N2);
    return result;
}

#include "taskflow/taskflow/taskflow.hpp"

int main(int argc, char ** argv) {
// Z takes 4 seconds
// X takes 1 second
// W is waiting
// E is executing
// execution:  |1|2|3|4|5|6|7|8|
//             |Z|E|E|E|Z|E|E|E|
//             |W|W|W|W|X|W|W|W|
    
    tf::Executor executor;
    tf::Taskflow taskflow, tf;
    
    tf::Task A = taskflow.emplace(
        [] () { std::cout << "Task A\n"; }
    );
    tf::Task B = taskflow.emplace(
        [] () { std::cout << "Task B\n"; }
    );
    tf::Task C = taskflow.emplace(
        [] () { std::cout << "Task C\n"; }
    );
    tf::Task D = taskflow.emplace(
        [] () { std::cout << "Task D\n"; }
    );
    
    A.precede(B);  // A runs before B
    A.precede(C);  // A runs before C
    B.precede(D);  // B runs before D
    D.succeed(B);  // D runs after B
    
    executor.run(taskflow).wait();
    
    tf::Task init = tf.emplace([](){ }).name("init");
    tf::Task stop = tf.emplace([](){ }).name("stop");

    // creates a condition task that returns 0 or 1
    tf::Task cond = tf.emplace([](){
    std::cout << "flipping a coin\n";
    return rand() % 2;
    }).name("cond");

    // creates a feedback loop
    init.precede(cond);
    cond.precede(cond, stop);  // cond--0-->cond, cond--1-->stop

    executor.run(tf).wait();

    tf::Taskflow f1, f2;

    auto [f1A, f1B] = f1.emplace( 
    []() { std::cout << "Task f1A\n"; },
    []() { std::cout << "Task f1B\n"; }
    );
    auto [f2A, f2B, f2C] = f2.emplace( 
    []() { std::cout << "Task f2A\n"; },
    []() { std::cout << "Task f2B\n"; },
    []() { std::cout << "Task f2C\n"; }
    );
    auto f1_module_task = f2.composed_of(f1);

    f1_module_task.succeed(f2A, f2B)
                .precede(f2C);
                
    executor.run(f2).wait();
    
    {

        struct MyObserver : public tf::ObserverInterface {

            MyObserver(const std::string& name) {
                std::cout << "constructing observer " << name << '\n';
            }

            void set_up(size_t num_workers) override final {
                std::cout << "settting up observer with " << num_workers << " workers\n";
            }

            void on_entry(size_t w, tf::TaskView tv) override final {
                std::ostringstream oss;
                oss << "worker " << w << " ready to run " << tv.name() << '\n';
                std::cout << oss.str();
            }

            void on_exit(size_t w, tf::TaskView tv) override final {
                std::ostringstream oss;
                oss << "worker " << w << " finished running " << tv.name() << '\n';
                std::cout << oss.str();
            }
        };

        tf::Executor executor;

        // Create a taskflow of eight tasks
        tf::Taskflow taskflow;

        auto A = taskflow.emplace([] () { std::cout << "1\n"; }).name("A");
        auto B = taskflow.emplace([] () { std::cout << "2\n"; }).name("B");
        B.precede(A);
 
        // create an observer
        std::shared_ptr<MyObserver> observer = executor.make_observer<MyObserver>("MyObserver");

        // run the taskflow
        executor.run(taskflow).get();

        // remove the observer (optional)
        executor.remove_observer(std::move(observer));
        
    }

    constexpr auto cmd1 = r.line(1, 2);
    constexpr auto cmd2 = r.line(cmd1, 3, 4);

    constexpr auto cmd3 = r.line(5, 6);
    constexpr auto cmd4 = r.line(cmd3, 6, 7);
    constexpr auto cmd5 = concat(cmd2, cmd4);
    std::cout << "original" << std::endl;
    printArray(cmd5);
    std::cout << std::endl << "optimized" << std::endl;
    printArray(r.optimize(cmd5));
    return 0;
}
