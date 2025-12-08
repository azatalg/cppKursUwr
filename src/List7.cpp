#include "defines.h"
#include "list.h"
#include <complex>
#include <fstream>
#include <iomanip>


constexpr f64 EM_GAMMA = 0.577215664901532860606512090082402431;
static std::mt19937_64 g_rng(std::random_device{}());

static std::discrete_distribution<i32> MakeLetterDist() {
    vec<f64> w = {
        8.37,
        1.93,
        3.89,
        3.35,
        8.68,
        0.26,
        1.46,
        1.25,
        8.83,
        2.28,
        3.01,
        2.24,
        2.81,
        5.69,
        7.53,
        2.87,
        0.00,
        4.15,
        4.13,
        3.85,
        2.06,
        0.00,
        4.11,
        0.00,
        4.03,
        5.33
    };
    return std::discrete_distribution<i32>(w.begin(), w.end());
}

static std::discrete_distribution<i32> g_letterDist = MakeLetterDist();
static std::binomial_distribution<i32> g_wordLenDist(11, 0.5);

static std::string GenerateWord(u32 len) {
    std::string s(len, ' ');
    for (u32 i = 0; i < len; ++i) {
        s[i] = static_cast<char>('a' + g_letterDist(g_rng));
    }
    return s;
}

static void GenerateText(std::ofstream& file, u32 len) {
    u32 generated = 0;

    while (len - generated > 12u) {
        u32 word_len = static_cast<u32>(g_wordLenDist(g_rng)) + 1u;
        std::string word = GenerateWord(word_len);
        file << word << ' ';
        generated += word_len + 1u;
    }

    u32 last_len = len - generated;
    if (last_len > 0u) {
        std::string word = GenerateWord(last_len);
        file << word << endl;
    }
}

cf64 GammaEuler(cf64 z, i32 iterations) {
    cf64 log_gamma = -EM_GAMMA * z - std::log(z);
    for (i32 n = 1; n <= iterations; ++n) {
        f64 nld = static_cast<f64>(n);
        cf64 term = z / nld;
        log_gamma += term - std::log(cf64(1.0, 0.0) + term);
    }
    return std::exp(log_gamma);
}

cf64 InvGammaEuler(cf64 z, i32 iterations) {
    return cf64(1.0, 0.0) / GammaEuler(z, iterations);
}

cf64 ZetaDirichlet(cf64 z, i32 iterations) {
    cf64 sum = cf64(0.0, 0.0);
    for (i32 n = 1; n <= iterations; ++n) {
        sum += cf64(1.0, 0.0) / std::pow(static_cast<f64>(n), z);
    }
    return sum;
}

cf64 ZetaAlternating(cf64 z, i32 iterations) {
    cf64 eta = cf64(0.0, 0.0);
    i32 sign = 1;
    for (i32 n = 1; n <= iterations; ++n) {
        cf64 term = cf64(1.0, 0.0) / std::pow(static_cast<f64>(n), z);
        if (sign > 0) eta += term;
        else eta -= term;
        sign = -sign;
    }
    cf64 factor = cf64(1.0, 0.0) /
        (cf64(1.0, 0.0) - std::pow(cf64(2.0, 0.0), cf64(1.0, 0.0) - z));
    return factor * eta;
}

cf64 Zeta(cf64 z, i32 iterations) {
    if (z.real() > 1.0) return ZetaDirichlet(z, iterations);
    return ZetaAlternating(z, iterations);
}

void ExportZetaCriticalLine(f64 t_min, f64 t_max, f64 step,
                            i32 iterations, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) return;

    out << std::setprecision(18);
    out << "t,real,imag\n";

    for (f64 t = t_min; t <= t_max + 1e-12; t += step) {
        cf64 z(0.5, t);
        cf64 val = ZetaAlternating(z, iterations);
        out << t << "," << val.real() << "," << val.imag() << "\n";
    }
}

class List7 : public ListBase {
public:
    List7() {
        REGISTER_TASKS(4);
    }

    std::string_view Name() const override { return "Lista 7"; }

private:
    void Ex_1() {
        std::cout << std::fixed << std::setprecision(18);

        cf64 z_gamma(0.5, 0.7);
        i32 it_gamma = 20000;

        cf64 g = GammaEuler(z_gamma, it_gamma);
        cf64 ig = InvGammaEuler(z_gamma, it_gamma);

        std::cout << "z = " << z_gamma << ", iteracje = " << it_gamma << endl;
        std::cout << "Gamma(z)      = " << g  << endl;
        std::cout << "1 / Gamma(z)  = " << ig << endl << endl;
    }

    void Ex_2() {
        std::cout << std::setprecision(18);

        cf64 z_zeta(2.0, 1.0);
        i32 it_zeta = 20000;

        cf64 zval = Zeta(z_zeta, it_zeta);

        std::cout << "z = " << z_zeta << ", iteracje = " << it_zeta << endl;
        std::cout << "zeta(z)       = " << zval << endl << endl;

        f64 t_min = -30.0;
        f64 t_max = 30.0;
        f64 step  = 0.05;
        i32 it_crit = 10000;

        std::string filename = "zeta_critical_line.csv";
        ExportZetaCriticalLine(t_min, t_max, step, it_crit, filename);

        std::cout << "zeta(0.5 + i t) zapisano do: "
                  << filename << endl;
    }
    void Ex_3() {
        u32 totalLen = 1000u;
        std::string filename = "tekst_zad3.txt";

        std::ofstream file(filename);
        if (!file) {
            std::cout << "Nie moge otworzyc pliku: " << filename << endl;
            return;
        }

        GenerateText(file, totalLen);
        std::cout << "Zadanie 3" << endl;
        std::cout << "Dluzosc tekstu: " << totalLen << endl;
        std::cout << "Zapisano do pliku: " << filename << endl;
    }
    void Ex_4() {
        constexpr i32 N = 1000;

        std::mt19937_64 rng(std::random_device{}());
        std::uniform_real_distribution<f64> distUniform(0.0, 1.0);
        std::binomial_distribution<i32> distBinom(10, 0.5);
        std::normal_distribution<f64> distNormal(0.0, 1.0);

        std::ofstream fUni("zad4_uniform.csv");
        std::ofstream fBin("zad4_binomial.csv");
        std::ofstream fNorm("zad4_normal.csv");


        for (i32 i = 0; i < N; ++i) {
            fUni << distUniform(rng) << endl;
            fBin << distBinom(rng)   << endl;
            fNorm << distNormal(rng) << endl;
        }

        std::cout << "Zadanie 4: zapisano " << N
                  << " prob do zad4_uniform.csv, zad4_binomial.csv, zad4_normal.csv"
                  << endl;
    }

};
REGISTER_LIST(List7);
