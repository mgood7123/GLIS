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

template<typename T>
constexpr void swap(T *a, T *b) noexcept {
    const T & c = *a;
    *a = *b;
    *b = c;
}

template<size_t N>
constexpr auto sort(const array<int, N>& array_) noexcept {
    auto result = duplicate(array_);

    auto start = result.begin();
    auto end = result.end();

    for (int i = 0; i < N-1; i++) {
        if (start[i] > start[i+1]) {
            int s = start[i+1];
            start[i+1] = start[i];
            start[i] = s;
        }
    }

    return result;
}

constexpr auto result = append(10);
constexpr auto result_ = append(result, 5);
constexpr auto sorted1 = sort(result_);

constexpr auto sorted2 = sort(append(append(10), 5));

template <typename T>
void printArray(T & a) {
    for (auto& x : a ) std::cout << x << " ";
    std::cout << std::endl;
}

int main() {
    printArray(sorted1);
    printArray(sorted2);
    return 0;
}
