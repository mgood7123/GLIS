#include <array>
#include <algorithm>
#include <iostream>

// https://godbolt.org/ to verify compile-time execution

template <typename T, size_t N>
struct array {
    T elems[N];
    int index = 0;

    constexpr T * begin() noexcept { return elems; }
    constexpr T const* begin() const noexcept { return elems; }
    constexpr T const* cbegin() const noexcept { return elems; }
    constexpr T * end() noexcept { return &elems[N]; }
    constexpr T const* end() const noexcept { return &elems[N]; }
    constexpr T const* cend() const noexcept { return &elems[N]; }
    constexpr size_t size() const noexcept { return N; }
    constexpr T & operator[] (size_t i) noexcept { return elems[i]; }
    constexpr void push(const T & v) noexcept { elems[index++] = v; }
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

constexpr static int draw_commands_draw_nothing = 0;
constexpr static int draw_commands_draw_line = 1;
constexpr static int draw_commands_draw_lines = 2;

template <size_t stack>
struct Instance {
    
    array<int, stack> memory {0};
    
    constexpr void nothing() {
        memory.push(draw_commands_draw_nothing);
    }
    
    constexpr void line(int x, int y) noexcept {
        memory.push(draw_commands_draw_line);
        memory.push(x);
        memory.push(y);
    }
    
    constexpr void optimize() noexcept {
        auto start = memory.begin();
        auto end = memory.end();
        auto N = memory.size();
        for (int i = 0; i < N-1; i++) {
            if (start[i] == draw_commands_draw_line && start[i+3] == draw_commands_draw_line) {
                start[i] = draw_commands_draw_lines;
                start[i+3] = start[i+4];
                start[i+4] = start[i+5];
                start[i+5] = draw_commands_draw_nothing;
            }
        }
    }
};

Instance<3*2> r;

int main(int argc, char ** argv) {
    if (argc == 2) r.line(1, 2);
    r.line(50, 60);
    std::cout << "original" << std::endl;
    printArray(r.memory);
    std::cout << std::endl;
    return 0;
}
