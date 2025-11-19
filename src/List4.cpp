#include "list.h"

template<typename T>
using fun = T (*)(T);

template<typename T>
void transform(T tab[], int n, fun<T> f) {
    for (int i = 0; i < n; ++i) {
        tab[i] = f(tab[i]);
    }
}

i32 square_i32(i32 x) { return x * x; }
i32 plus_one(i32 x)   { return x + 1; }

f64 half_f64(f64 x)   { return x / 2.0; }

std::string shout(std::string s) { return s + "!"; }

template<typename F1, typename F2>
struct PoKolei {
    F1 f1;
    F2 f2;

    template<typename... Args>
    void operator()(Args&&... args) const {
        std::invoke(f1, std::forward<Args>(args)...);
        std::invoke(f2, std::forward<Args>(args)...);
    }
};

template<typename F1, typename F2>
auto po_kolei(F1 f1, F2 f2) {
    return PoKolei<F1, F2>{std::move(f1), std::move(f2)};
}

void print_value(int x) {
    std::cout << "value = " << x << endl;
}
void print_square(int x) {
    std::cout << "value^2 = " << (x * x) << endl;
}
void print_cube(int x) {
    std::cout << "value^3 = " << (x * x * x) << endl;
}

template<typename T>
struct PrintRange {
    T a;
    T b;

    void operator()(const T& value) const {
        if (value > a && value < b) {
            std::cout << value << ' ';
        }
    }
};

template<typename T>
struct PrintKthFromP {
    usize k;
    usize p;
    usize index = 0;

    void operator()(const T& value) {
        if (index >= p && ((index - p) % k == 0)) {
            std::cout << value << ' ';
        }
        ++index;
    }
};

template<typename T>
struct MinMax {
    bool first = true;
    T min{};
    T max{};

    void operator()(const T& value) {
        if (first) {
            min = max = value;
            first = false;
        } else {
            if (value < min) min = value;
            if (max < value) max = value;
        }
    }

    std::pair<T, T> result() const {
        return { min, max };
    }
};

template<typename T>
struct SumAll {
    T acc{};

    void operator()(const T& value) {
        acc += value;
    }

    T result() const { return acc; }
};



class List4 : public ListBase {
public:
    List4() {
        REGISTER_TASKS(5);
    }

    std::string_view Name() const override { return "Lista 4"; }
private:
    void Ex_1() {
        i32 ints[] = {1, 2, 3, 4, 5};
        constexpr int n_ints = static_cast<int>(std::size(ints));
        ::transform(ints, n_ints, square_i32);
        std::cout << "Kwadraty intow: ";
        for (int i = 0; i < n_ints; ++i) std::cout << ints[i] << ' ';
        std::cout << endl;

        f64 ds[] = {2.0, 4.0, 8.0, 16.0};
        constexpr int n_ds = static_cast<int>(std::size(ds));
        ::transform(ds, n_ds, half_f64);
        std::cout << "Po polowieniu double: ";
        for (int i = 0; i < n_ds; ++i) std::cout << ds[i] << ' ';
        std::cout << endl;

        std::string ss[] = {"hello", "world"};
        constexpr int n_ss = static_cast<int>(std::size(ss));
        ::transform(ss, n_ss, shout);
        std::cout << "Stringi po shout: ";
        for (int i = 0; i < n_ss; ++i) std::cout << ss[i] << ' ';
        std::cout << endl;
    }

    void Ex_2() {
        auto f12 = po_kolei(print_value, print_square);
        auto f123 = po_kolei(f12, print_cube);

        std::cout << "Wywolanie po_kolei(print_value, print_square, print_cube) dla x = 3:" << endl;
        f123(3);
    }

    void Ex_3() {
        vec<double> vd{ -1.0, 0.5, 1.2, 3.14, 7.0, 10.0 };
        std::list<int> li{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::set<std::string> ss{ "kot", "pies", "aaa", "zombie", "xyz" };

        std::cout << "Wartosci z vd z przedzialu (0, 5): ";
        std::for_each(vd.begin(), vd.end(), PrintRange<double>{0.0, 5.0});
        std::cout << endl;

        std::cout << "Co 2-ga wartosc z listy liczb calkowitych, zaczynajac od indeksu 1: ";
        std::for_each(li.begin(), li.end(), PrintKthFromP<int>{2, 1});
        std::cout << endl;

        MinMax<double> mm_d;
        std::for_each(vd.begin(), vd.end(), std::ref(mm_d));
        auto [min_d, max_d] = mm_d.result();
        std::cout << "Min i max w vd: " << min_d << "  " << max_d << endl;

        MinMax<int> mm_i;
        std::for_each(li.begin(), li.end(), std::ref(mm_i));
        auto [min_i, max_i] = mm_i.result();
        std::cout << "Min i max w liscie intow: " << min_i << "  " << max_i << endl;

        MinMax<std::string> mm_s;
        std::for_each(ss.begin(), ss.end(), std::ref(mm_s));
        auto [min_s, max_s] = mm_s.result();
        std::cout << "Min i max w secie stringow (leksykograficznie): "
                  << min_s << "  " << max_s << endl;

        SumAll<double> sum_d;
        std::for_each(vd.begin(), vd.end(), std::ref(sum_d));
        std::cout << "Suma wszystkich double: " << sum_d.result() << endl;

        SumAll<int> sum_i;
        std::for_each(li.begin(), li.end(), std::ref(sum_i));
        std::cout << "Suma wszystkich intow: " << sum_i.result() << endl;

        SumAll<std::string> sum_s;
        std::for_each(ss.begin(), ss.end(), std::ref(sum_s));
        std::cout << "Konkatenacja wszystkich stringow z seta: " << sum_s.result() << endl;
    }

    void Ex_4() {
        std::function<long long(int, int)> binom;

        binom = [&binom](int n, int k) -> long long {
            if (n < 0 || k < 0) {
                throw std::invalid_argument("n i k musza byc naturalne");
            }
            if (k == 0 || k == n) return 1;
            if (k > n) return 0;
            return binom(n - 1, k - 1) + binom(n - 1, k);
        };

        vec<std::pair<int, int>> tests {
            {5, 2},
            {6, 3},
            {10, 5},
            {5, -1}
        };

        for (const auto& [n, k] : tests) {
            try {
                std::cout << "C(" << n << ", " << k << ") = " << binom(n, k) << endl;
            } catch (const std::exception& e) {
                std::cout << "C(" << n << ", " << k << ") -> EXCEPTION: " << e.what() << endl;
            }
        }
    }

    void Ex_5() {
        auto sum_collection = []<typename T>(const vec<T>& collection) -> T {
            T acc{};
            for (const T& x : collection) {
                acc += x;
            }
            return acc;
        };

        vec<int> ints{ 1, 2, 3, 4, 5 };
        vec<double> doubles{ 0.5, 1.5, 2.5 };

        int sum_i = sum_collection(ints);
        double sum_d = sum_collection(doubles);

        std::cout << "sum_collection(ints) = " << sum_i << endl;
        std::cout << "sum_collection(doubles) = " << sum_d << endl;
        std::cout << "Ta sama lambda dziala dla dowolnego vec<T>." << endl;
    }
};

REGISTER_LIST(List4)
