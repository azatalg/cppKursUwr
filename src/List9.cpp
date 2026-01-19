#include <iterator>

#include "list.h"
#include <filesystem>
namespace fs = std::filesystem;

class List9 : public ListBase {
public:
    List9() {
        REGISTER_TASKS(7);
    }

    std::string_view Name() const override { return "Lista 9"; }

private:
    void Ex_1() {
        std::ifstream file("liczby.txt");
        if (!file) return;

        vec<f64> data{std::istream_iterator<f64>{file}, 
                      std::istream_iterator<f64>{}};

        if (data.empty()) return;

        f64 sum = std::accumulate(data.begin(), data.end(), 0.0);
        f64 mean = sum / static_cast<f64>(data.size());

        f64 sq_sum = std::transform_reduce(data.begin(), data.end(), 0.0, std::plus<>(),
            [mean](f64 x) { return std::pow(x - mean, 2); });
        
        f64 stdev = std::sqrt(sq_sum / static_cast<f64>(data.size()));

        std::cout << std::format("Srednia: {:.3f}\nOdchylenie: {:.3f}", mean, stdev) << endl;
    }

    void Ex_2() {
        i32 k;
        std::cout << "Podaj k: ";
        if (!(std::cin >> k)) return;

        auto phi = [](i32 n) -> i32 {
            i32 result = n;
            for (i32 i = 2; i * i <= n; i++) {
                if (n % i == 0) {
                    while (n % i == 0) n /= i;
                    result -= result / i;
                }
            }
            if (n > 1) result -= result / n;
            return result;
        };

        std::ofstream out("phi.txt");
        auto values = std::views::iota(1, k + 1) 
                    | std::views::transform(phi);

        std::ranges::copy(values, std::ostream_iterator<i32>(out, "; "));
        std::cout << "Wyniki zapisano do phi.txt" << endl;
    }

    void Ex_3() {
        std::string path_str;
        std::cout << "Podaj sciezke: ";
        std::cin >> path_str;

        fs::path p(path_str);
        if (fs::exists(p)) {
            std::cout << std::format("Kanoniczna: {}\n", fs::canonical(p).string());
            
            auto ftime = fs::last_write_time(p);
            auto sytm = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
            
            std::cout << std::format("Modyfikacja: {:%F %T}\n", sytm);

            if (fs::is_regular_file(p))
                std::cout << std::format("Rozmiar: {} bajtow", fs::file_size(p)) << endl;
        } else {
            std::cerr << "Blad: Sciezka nie istnieje" << endl;
        }
    }

    void Ex_4() {
        std::string dir_path;
        std::cout << "Podaj katalog: ";
        std::cin >> dir_path;

        if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
            std::cerr << "Katalog nie istnieje!" << endl;
            return;
        }

        vec<fs::directory_entry> items;
        std::ranges::copy(fs::directory_iterator(dir_path), std::back_inserter(items));

        std::ranges::sort(items, [](const auto& a, const auto& b) {
            if (a.is_directory() != b.is_directory()) return a.is_directory();
            return a.path().filename() < b.path().filename();
        });

        for (const auto& e : items) {
            std::cout << std::format("{} {}\n", e.is_directory() ? "[DIR]" : "[FILE]", e.path().filename().string());
        }
    }

    void Ex_5() {
        std::string dir_path;
        std::cout << "Podaj katalog: ";
        std::cin >> dir_path;

        if (!fs::is_directory(dir_path)) {
            std::cerr << "To nie jest katalog!" << endl;
            return;
        }

        u64 total_size = 0;
        u32 count = 0;

        for (const auto& entry : fs::recursive_directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                total_size += entry.file_size();
                count++;
            }
        }

        std::cout << std::format("Znaleziono {} plikow\nLaczny rozmiar: {} bajtow", count, total_size) << endl;
    }

    void Ex_6() {
        std::string root_path_str;
        std::cout << "Podaj sciezke do przeszukania projektow: ";
        if (!(std::cin >> root_path_str)) return;

        fs::path root_path(root_path_str);

        if (!fs::exists(root_path) || !fs::is_directory(root_path)) {
            std::cerr << "Blad: Podana sciezka nie jest poprawnym katalogiem." << endl;
            return;
        }

        try {
            for (const auto& entry : fs::recursive_directory_iterator(root_path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".cbp") {

                    fs::path project_dir = entry.path().parent_path();
                    fs::path bin_dir = project_dir / "bin";

                    if (fs::exists(bin_dir) && fs::is_directory(bin_dir)) {
                        std::cerr << std::format("Usuwanie katalogu bin w: {}", bin_dir.string()) << endl;

                        fs::remove_all(bin_dir);
                    }
                }
            }
            std::cout << "Przeszukiwanie i czyszczenie zakonczone." << endl;
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << std::format("Blad systemu plikow: {}", e.what()) << endl;
        }
    }

    #pragma region Ex7
    template<typename T>
    void serialize(std::ostream& os, const T& val) {
        os.write(reinterpret_cast<const char*>(&val), sizeof(T));
    }

    template<typename T>
    void deserialize(std::istream& is, T& val) {
        is.read(reinterpret_cast<char*>(&val), sizeof(T));
    }

    void serialize_str(std::ostream& os, const std::string& s) {
        usize size = s.size();
        serialize(os, size);
        os.write(s.data(), size);
    }

    void deserialize_str(std::istream& is, std::string& s) {
        usize size;
        deserialize(is, size);
        s.resize(size);
        is.read(s.data(), size);
    }

    template<typename T>
    void serialize_arr(std::ostream& os, T* arr, usize size) {
        serialize(os, size);
        os.write(reinterpret_cast<const char*>(arr), size * sizeof(T));
    }

    template<typename T>
    auto deserialize_arr(std::istream& is) -> std::pair<T*, usize> {
        usize size;
        deserialize(is, size);
        T* arr = new T[size];
        is.read(reinterpret_cast<char*>(arr), size * sizeof(T));
        return {arr, size};
    }

    void Ex_7() {
        std::string filename = "data.bin";

        {
            std::ofstream ofs(filename, std::ios::binary);
            i32 magic = 42;
            f64 pi = 3.14159;
            std::string name = "Arigato";
            i32* numbers = new i32[3]{10, 20, 30};

            serialize(ofs, magic);
            serialize(ofs, pi);
            serialize_str(ofs, name);
            serialize_arr(ofs, numbers, 3);

            delete[] numbers;
            std::cout << "Dane zserializowane do data.bin" << endl;
        }

        {
            std::ifstream ifs(filename, std::ios::binary);
            i32 magic;
            f64 pi;
            std::string name;

            deserialize(ifs, magic);
            deserialize(ifs, pi);
            deserialize_str(ifs, name);
            auto [ptr, sz] = deserialize_arr<i32>(ifs);

            std::cout << std::format("Odebrano: {}, {}, {}, tablica size: {}\n", magic, pi, name, sz);

            delete[] ptr;
        }
    }
#pragma endregion
};

REGISTER_LIST(List9)