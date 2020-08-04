#include <iostream>
#include <iomanip>

using namespace std;

#define print(x) cout << #x << " = " << x << endl << flush

enum GLIS_WRAP_MODE {
    /*
     * 1 2 3 4 1 2 3 4
     */
    LOOP,

    /*
     * 1 2 3 4 3 2 1 2
     */
    PING_PONG
};

template<typename TYPE>
TYPE GLIS_wrap_to_range(GLIS_WRAP_MODE mode, TYPE value, TYPE min, TYPE max, TYPE representation_of_zero, TYPE representation_of_addition_or_subtraction_by_one) {
    if (value < min) return min;
    else if (value > max) {
        TYPE counter = representation_of_zero;
        TYPE tmp = representation_of_zero;
        if (mode == GLIS_WRAP_MODE::LOOP) {
            while (counter < value) {
                // LOOP
                tmp += representation_of_addition_or_subtraction_by_one;
                if (tmp > max) tmp = min;
                counter += representation_of_addition_or_subtraction_by_one;
            }
        } else {
            bool reverse = false;
            while (counter < value) {
                // PING PONG
                if (!reverse) {
                    tmp += representation_of_addition_or_subtraction_by_one;
                    if (tmp > max) {
                        tmp = max-representation_of_addition_or_subtraction_by_one;
                        reverse = true;
                    }
                } else {
                    tmp -= representation_of_addition_or_subtraction_by_one;
                    if (tmp < min) {
                        tmp = min+representation_of_addition_or_subtraction_by_one;
                        reverse = false;
                    }
                }
                counter += representation_of_addition_or_subtraction_by_one;
            }
        }
        return tmp;
    } else return value;
}

int main() {
    // LOOP
    cout << "LOOP" << endl << flush;
    {
        auto max = 10;
        auto m = 1;
        auto M = 3;
        for (auto i = 0; i < max; i += 1) {
            auto x = GLIS_wrap_to_range(GLIS_WRAP_MODE::LOOP, i, m, M, 0, 1);
            cout << "wrap " << setw(4) << i << " " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
        }
    }
    {
        auto max = 1.0f;
        auto m = 0.1f;
        auto M = 0.3f;
        for (auto i = 0.0f; i < max; i += 0.1f) {
            auto x = GLIS_wrap_to_range(GLIS_WRAP_MODE::LOOP, i, m, M, 0.0f, 0.1f);
            if (i == 0.0f) cout << "wrap  0.0 " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
            else cout << "wrap " << setw(4) << i << " " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
        }
    }
    {
        auto max = 10;
        auto m = -2;
        auto M = 2;
        for (auto i = -2; i < max; i += 1) {
            auto x = GLIS_wrap_to_range(GLIS_WRAP_MODE::LOOP, i, m, M, 0, 1);
            cout << "wrap " << setw(4) << i << " " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
        }
    }
    {
        auto max = 1.0f;
        auto m = -0.2f;
        auto M = 0.2f;
        for (auto i = -0.2f; i < max; i += 0.1f) {
            auto x = GLIS_wrap_to_range(GLIS_WRAP_MODE::LOOP, i, m, M, 0.0f, 0.1f);
            if (i == 0.0f) cout << "wrap  0.0 " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
            else cout << "wrap " << setw(4) << i << " " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
        }
    }
    
    // PING-PONG
    cout << "PING-PONG" << endl << flush;
    {
        auto max = 10;
        auto m = 1;
        auto M = 3;
        for (auto i = 0; i < max; i += 1) {
            auto x = GLIS_wrap_to_range(GLIS_WRAP_MODE::PING_PONG, i, m, M, 0, 1);
            cout << "wrap " << setw(4) << i << " " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
        }
    }
    {
        auto max = 1.0f;
        auto m = 0.1f;
        auto M = 0.3f;
        for (auto i = 0.0f; i < max; i += 0.1f) {
            auto x = GLIS_wrap_to_range(GLIS_WRAP_MODE::PING_PONG, i, m, M, 0.0f, 0.1f);
            if (i == 0.0f) cout << "wrap  0.0 " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
            else cout << "wrap " << setw(4) << i << " " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
        }
    }
    {
        auto max = 10;
        auto m = -2;
        auto M = 2;
        for (auto i = -2; i < max; i += 1) {
            auto x = GLIS_wrap_to_range(GLIS_WRAP_MODE::PING_PONG, i, m, M, 0, 1);
            cout << "wrap " << setw(4) << i << " " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
        }
    }
    {
        auto max = 1.0f;
        auto m = -0.2f;
        auto M = 0.2f;
        for (auto i = -0.2f; i < max; i += 0.1f) {
            auto x = GLIS_wrap_to_range(GLIS_WRAP_MODE::PING_PONG, i, m, M, 0.0f, 0.1f);
            if (i == 0.0f) cout << "wrap  0.0 " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
            else cout << "wrap " << setw(4) << i << " " << setw(4) << m << " " << setw(4) << M << " = " << setw(4) << x << endl << flush;
        }
    }
    exit(0);
}