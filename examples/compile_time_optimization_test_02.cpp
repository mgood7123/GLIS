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




int main(int argc, char ** argv) {
    if (argc == 1) {
        constexpr array<int, 3> cmd1 = r.line(1, 2);
        constexpr array<int, 6> cmd2 = r.line(cmd1, 3, 4);
    } else {
        constexpr array<int, 3> cmd2 = r.line(33, 44);
    }
    if (argc == 2) {
        constexpr array<int, 3> cmd3 = r.line(5, 6);
        constexpr array<int, 6> cmd4 = r.line(cmd3, 7, 8);
    } else {
        constexpr array<int, 3> cmd4 = r.line(77, 88);
    }
    
    constexpr array<int, 12> cmd5 = concat(cmd2, cmd4);
    std::cout << "original" << std::endl;
    printArray(cmd5);
    std::cout << std::endl << "optimized" << std::endl;
    printArray(r.optimize(cmd5));
    return 0;
}
