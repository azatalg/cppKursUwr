#include "list.h"
#include <set>
#include <chrono>
#include <algorithm>
#include <cmath>
class List1 : public ListBase {
public:
    List1() {
        REGISTER_TASKS(8);
    }

    std::string_view Name() const override { return "Lista 1"; }

private:
    void Ex_1() {
        std::cout << R"s(Instytut Informatyki Uniwersytetu Wrocławskiego
ul. Fryderyka Joliot-Curie 15
50-383 Wrocław

C:\Windows\System32\

Sekwencje znaków:
" ( ) (" )" () "()"
)s" ;
    }

    void Ex_2() {
        using StrSet = std::set<std::string>;
        StrSet words{ "UwU", "Ara", "Ara", "Senpai" };
        for (const auto& s : words)
            std::cout << s << '\n';
    }

    void Ex_3() {
        using namespace std::literals;
        enum class Name : std::uint16_t {
            Hatsume, Rei, Mikasa, Hinata,
            Levi, Itachi
        };
        auto send_message = [](std::string_view msg, Name who) {
            std::string_view n{};
            switch (who) {
            case Name::Hatsume:n = "Hatsume"sv;  break;
            case Name::Rei:    n = "Rei"sv;    break;
            case Name::Mikasa: n = "Mikasa"sv; break;
            case Name::Hinata: n = "Hinata"sv; break;
            case Name::Levi:   n = "Levi"sv;   break;
            case Name::Itachi: n = "Itachi"sv; break;
            }
            std::cout << "To " << n << ": " << msg << '\n';
        };

        send_message("Konnichiwa!"sv, Name::Hatsume);
        send_message("Konnichiwa!"sv, Name::Itachi);
    }

    #pragma region Ex4
    NO_DISCARD static auto lucas(auto n) {
        if (n == 0) return u64{ 2 };
        if (n == 1) return u64{ 1 };
        return lucas(n - 1) + lucas(n - 2);
    };

    void Ex_4() {
        std::cout << "L(0) = " << lucas(0) << endl;
        std::cout << "L(1) = " << lucas(1) << endl;
        std::cout << "L(10)= " << lucas(10) << endl;
    }
    #pragma endregion

    #pragma region Ex5
    static void solve_quadratic(f64 a, f64 b, f64 c) {
        std::cout << "Rozwiazanie rownania: " << a << " * x^2 + " << b << " * x + " << c << ":\n";
        if (a == 0.0) {
            if (b == 0.0) { std::cout << "Nie ma rozwiazan: (a=b=0)\n"; return; }
            std::cout << "Rownanie liniowe: x = " << (-c / b) << "\n";
            return;
        }

        if (const f64 D = std::fma(b, b, -4.0 * a * c); D < 0.0) {
            std::cout << "Delta = " << D << " < 0 Brak rozwiazan w Rzeczywistych\n";
        }
        else if (D == 0.0) {
            const f64 x = (-0.5 * b) / a;
            std::cout << "Delta = 0 \nx = " << x << "\n";
        }
        else {
            const f64 sqrtD = std::sqrt(D);

            const f64 q = -0.5 * (b + std::copysign(sqrtD, b));
            const f64 x1 = q / a;
            const f64 x2 = c / q;

            f64 r1 = x1, r2 = x2;
            if (r2 < r1) std::swap(r1, r2);

            std::cout << "Delta = " << D << " > 0\nx1 = " << r1 << "\nx2 = " << r2 << "\n";
        }
    }

    void Ex_5() {
        solve_quadratic(1.0, -3.0, 2.0);
        solve_quadratic(1.0, 2.0, 1.0);
        solve_quadratic(1e8, 1e8, 1.0);
    }
    #pragma endregion

    void Ex_6() {
        using namespace std::chrono;
        year_month_day ymd{ year{2025}, month{10}, day{8} };

        std::string_view mon;
        switch (u32 m = u32(ymd.month()); m) {
            case 1:  mon = "stycznia";     break;
            case 2:  mon = "lutego";       break;
            case 3:  mon = "marca";        break;
            case 4:  mon = "kwietnia";     break;
            case 5:  mon = "maja";         break;
            case 6:  mon = "czerwca";      break;
            case 7:  mon = "lipca";        break;
            case 8:  mon = "sierpnia";     break;
            case 9:  mon = "wrzesnia";     break;
            case 10: mon = "pazdziernika"; break;
            case 11: mon = "listopada";    break;
            case 12: mon = "grudnia";      break;
            default: ASSERT(false);
        }

        std::cout << u32(ymd.day()) << ' ' << mon << ' ' << i32(ymd.year()) << '\n';
    }

    #pragma region Ex7
    class osoba {
        std::string nazwisko_;
        std::string imie_;
        int rok_urodzenia_{};

    public:
        osoba(std::string_view nazwisko, std::string_view imie, int rok)
            : nazwisko_(nazwisko),
            imie_(imie),
            rok_urodzenia_(rok) {}

        NO_DISCARD friend bool operator<(const osoba& a, const osoba& b) {
            using std::tie;
            return tie(a.nazwisko_, a.imie_, a.rok_urodzenia_) <
                tie(b.nazwisko_, b.imie_, b.rok_urodzenia_);
        }

        NO_DISCARD friend std::ostream& operator<<(std::ostream& os, const osoba& o) {
            return os << o.nazwisko_ << " " << o.imie_ << " (" << o.rok_urodzenia_ << ")";
        }
    };
    void Ex_7() {
        vec<osoba> v{
            {"Hatsune",  "Miku",    2007},
            {"Ayanami",  "Rei",     2001},
            {"Shikinami","Asuka",   2001},
            {"Ackerman", "Mikasa",  1999}
        };
        std::sort(v.begin(), v.end());
        for (const auto& x : v)
            std::cout << x << '\n';
    }
    #pragma endregion

    #pragma region Ex8
    template<class It, class T>
    std::pair<It, It> equal_range_3way(It first, It last, const T& value) {
        It lo = first, hi = last;
        while (lo < hi) {
            It mid = lo + (std::distance(lo, hi) / 2);
            if (auto ord = (*mid <=> value); ord < 0)
                lo = mid + 1;
            else
                hi = mid;
        }
        It lb = lo;

        lo = lb; hi = last;
        while (lo < hi) {
            It mid = lo + (std::distance(lo, hi) / 2);
            if (auto ord = (value <=> *mid); ord < 0)
                hi = mid;
            else
                lo = mid + 1;
        }
        return { lb, lo };
    }
    void Ex_8() {
        vec<int> v{ 1,2,3,3,4,5,5,5,6,7,7,7,7,8,9 };
        i32 key = 7;

        auto [beg, end] = equal_range_3way(v.begin(), v.end(), key);

        std::cout << "value=" << key
            << " count=" << std::distance(beg, end)
            << " range=[" << std::distance(v.begin(), beg)
            << "," << std::distance(v.begin(), end) << ")\n";

        for (auto it = beg; it != end; ++it) std::cout << *it << ' ';
        std::cout << '\n';
    }
    #pragma endregion
};

REGISTER_LIST(List1)