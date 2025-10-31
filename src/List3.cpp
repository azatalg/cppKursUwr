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
        using nl = std::numeric_limits<long long int>;
        std::cout<<nl::digits << endl;
        std::cout<<"according to the standard, at least 64 bit\n";
        std::cout<<"["<<nl::min()<<";"<<nl::max()<<"] Basically (+/-2^63) - 1 \n";
        std::cout<<nl::digits;
    }

    void Ex_2() {
        using nl = std::numeric_limits<long double>;
        std::cout << nl::min() << " " << nl::max()<<endl;
        std::cout << nl::digits<<endl;
        std::cout<<nl::has_infinity<<" "<<nl::epsilon();
    }


    void Ex_3() {
        std::cout << "Harmonic numbers computed via std::ratio<>\n";

        using H1 = Harmonic<1>::type;
        using H2 = Harmonic<2>::type;
        using H3 = Harmonic<3>::type;
        using H4 = Harmonic<4>::type;
        using H5 = Harmonic<5>::type;
        using H6 = Harmonic<6>::type;
        using H46 = Harmonic<46>::type;
        std::cout << "H1 = " << H1::num << "/" << H1::den << "\n";
        std::cout << "H2 = " << H2::num << "/" << H2::den << "\n";
        std::cout << "H3 = " << H3::num << "/" << H3::den << "\n";
        std::cout << "H4 = " << H4::num << "/" << H4::den << "\n";
        std::cout << "H5 = " << H5::num << "/" << H5::den << "\n";
        std::cout << "H6 = " << H6::num << "/" << H6::den << "\n";

        std::cout << "\nH46 (highest) = "
                  << H46::num << "/" << H46::den << "\n";
    }

    template<typename Src, typename Dest>
    void transfer_value(Src&& src, Dest& dest) {
        using SrcType = std::remove_pointer_t<std::decay_t<Src>>;

        if constexpr (std::is_pointer_v<std::decay_t<Src>>) {
            if (src) {
                if constexpr (std::is_convertible_v<SrcType, Dest>) {
                    dest = std::move(*src);
                } else {
                    //std::cerr << "Can't convert *src -> dest\n";
                    throw std::runtime_error("Can't convert *src -> dest");
                }
            } else {
                throw std::runtime_error("nullptr");
            }
        } else {
            if constexpr (std::is_convertible_v<Src, Dest>) {
                dest = std::move(src);
            } else {
                //std::cerr << "Can't convert src -> dest\n";
                throw std::runtime_error("Can't convert *src -> dest");
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
       // transfer_value(s, x); // Error
    }
    class Matrix {
    public:
        int n, m;
        vec<vec<f32>> data;
        Matrix(i32 x, i32 y) : n(x), m(y) {
            data = vec<vec<f32>> (x , vec<f32>());
            for (auto& i: data) {
                for (int j=0; j<y; j++)
                i.push_back(RandomValue(0.9, 1.1));
            }
        }

        static Matrix Zero(i32 rows, i32 cols) {
            Matrix z(rows, cols);
            for (i32 i = 0; i < rows; ++i)
                std::fill(z.data[i].begin(), z.data[i].end(), 0.0f);
            return z;
        }

        friend Matrix operator*(const Matrix& a, const Matrix& b) {
            if (a.m != b.n) {
                throw std::invalid_argument("Matrix::operator*: dimension mismatch");
            }

            Matrix c = Matrix::Zero(a.n, b.m);

            for (i32 i = 0; i < a.n; ++i) {
                const vec<f32>& Ai = a.data[i];
                for (i32 k = 0; k < a.m; ++k) {
                    const f32 aik = Ai[k];
                    const vec<f32>& Bk = b.data[k];
                    vec<f32>& Ci = c.data[i];
                    for (i32 j = 0; j < b.m; ++j) {
                        Ci[j] += aik * Bk[j];
                    }
                }
            }
            return c;
        }

        friend std::ostream& operator<<(std::ostream& out, const Matrix& a) {
            for (auto& i: a.data) {
                for (auto& j: i) {
                    out << j << ' ';
                }
                out << endl;
            }
            return out;
        }
    };
    void Ex_5() {
        using clock = std::chrono::high_resolution_clock;

        struct Task { int n; int reps; };

        vec<Task> tasks = {
            {  50, 2000 },
            { 100,  600 },
            { 200,  100 },
        };

        std::cout << std::fixed << std::setprecision(12);

        volatile f64 blackhole = 0.0; // to not have some optimizations

        for (auto [sz, reps] : tasks) {
            Matrix A(sz, sz);

            auto t0 = clock::now();
            for (int r = 0; r < reps; ++r) {
                Matrix C = A * A;
                blackhole += C.data[(r % sz)][(r * 7) % sz];
            }
            auto t1 = clock::now();

            // Average time
            auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            f64 avg_ms = (total_ns / 1e6) / static_cast<f64>(reps);
            i32 avg_ms_int = static_cast<i32>(avg_ms + 0.5);

            double minutes = avg_ms / (1000.0 * 60.0);

            std::cout
                << "N=" << sz << "  reps=" << reps
                << "  avg: " << avg_ms_int << " ms"
                << "  (" << minutes << " min)\n";
        }

        // to not optimize
        if (blackhole < -1e300) std::cerr << blackhole << "\n";

    }


};

REGISTER_LIST(List3)