#include <array>
#include <algorithm>
#include <iostream>

template <typename T, size_t N>
struct array {
    T elems[N];

    constexpr T * begin() const noexcept { return const_cast<T*>(elems); }
    constexpr T const* cbegin() const noexcept { return elems; }
    constexpr T * end() const noexcept { return const_cast<T*>(&elems[N]); }
    constexpr T const* cend() const noexcept { return &elems[N]; }
    constexpr size_t size() const noexcept { return N; }
};

constexpr auto add(int value) noexcept {
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

template<int N>
constexpr auto add(const array<int, N>& ar1, int value) noexcept {
    array<int, N+1> result {0};
    copy (ar1.cbegin(), ar1.cend(), result.begin());

    auto one = add(value);
    copy(one.cbegin(), one.cend(), result.end()-1);

    return result;
}

#define add_(array, value) add<array.size()>(array, value)

int main()
{
    constexpr auto result = add(5);
    constexpr auto result_ = add_(result, 10);
    for (auto& x : result_) std::cout << x << " ";
    std::cout << std::endl;
    return 0;
}