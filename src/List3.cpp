#include "list.h"
#include <set>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <limits>
template<i32 N>
    struct Harmonic {
    using prev = typename Harmonic<N-1>::type;
    using type = std::ratio_add<prev, std::ratio<1, N>>;
};

template<>
struct Harmonic<1> {
    using type = std::ratio<1, 1>;
};

class List3 : public ListBase {
public:
    List3() {
        REGISTER_TASKS(5);
    }

    std::string_view Name() const override { return "Lista 3"; }

private:
    void Ex_1() {
        std::cout<<sizeof(long long int)<< " bytes " << sizeof(long long int) * CHAR_BIT << "bits\n";
        std::cout<<"according to the standard, at least 64 bit\n";
        std::cout<<"["<<LLONG_MIN<<";"<<LLONG_MAX<<"] Basically (+/-2^63) - 1 \n";
        std::cout<<ceil(log10(LLONG_MAX));
    }

    void Ex_2() {
        std::cout << "sizeof(long double) = " << sizeof(long double)
                  << " bytes (" << sizeof(long double) * CHAR_BIT << " bits)\n";

        std::cout << "LDBL_MIN = " << LDBL_MIN << " (smallest positive normalized)\n";
        std::cout << "LDBL_MAX = " << LDBL_MAX << " (largest finite)\n";
        std::cout << "LDBL_EPSILON = " << LDBL_EPSILON
                  << " (difference between 1 and next representable > 1)\n";

        std::cout << "Infinity representable?  "
                  << (std::isinf(HUGE_VALL) ? "yes" : "no") << "\n";

        std::cout << "HUGE_VALL = " << HUGE_VALL << "\n";
    }


    void Ex_3() {
        std::cout << "Harmonic numbers computed via std::ratio<>\n";

        using H1 = Harmonic<1>::type;
        using H2 = Harmonic<2>::type;
        using H3 = Harmonic<3>::type;
        using H4 = Harmonic<4>::type;
        using H5 = Harmonic<5>::type;
        using H6 = Harmonic<6>::type;
        using H20 = Harmonic<46>::type;
        std::cout << "H1 = " << H1::num << "/" << H1::den << "\n";
        std::cout << "H2 = " << H2::num << "/" << H2::den << "\n";
        std::cout << "H3 = " << H3::num << "/" << H3::den << "\n";
        std::cout << "H4 = " << H4::num << "/" << H4::den << "\n";
        std::cout << "H5 = " << H5::num << "/" << H5::den << "\n";
        std::cout << "H6 = " << H6::num << "/" << H6::den << "\n";

        std::cout << "\nH20 (highest) = "
                  << H20::num << "/" << H20::den << "\n";
    }

    template<typename Src, typename Dest>
    void transfer_value(Src&& src, Dest& dest) {
        using SrcType = std::remove_pointer_t<std::decay_t<Src>>;

        if constexpr (std::is_pointer_v<std::decay_t<Src>>) {
            if (src) {
                if constexpr (std::is_convertible_v<SrcType, Dest>) {
                    dest = *src;
                } else {
                    std::cerr << "Can't convert *src -> dest\n";
                }
            } else {
                std::cerr << "nullptr\n";
            }
        } else {
            if constexpr (std::is_convertible_v<Src, Dest>) {
                dest = src;
            } else {
                std::cerr << "Can't convert src -> dest\n";
            }
        }
    }
    void Ex_4() {
        i32 a = 42;
        f64 b = 0.0;
        transfer_value(a, b); // Ok

        i32* p = &a;
        f32 f = 0.0f;
        transfer_value(p, f); // Ok
        f+=1.4;
        std::cout<<a;
        std::string s = "doom";
        i32 x = 0;
        transfer_value(s, x); // Error
    }

    void Ex_5() {

    }


};

REGISTER_LIST(List3)