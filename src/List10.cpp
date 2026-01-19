#include <iostream>
#include <locale>
#include <codecvt>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include "list.h"
class List10 : public ListBase {
public:
    List10() {
        REGISTER_TASKS(4);
    }

    std::string_view Name() const override { return "Lista 10"; }

private:
    void Ex_1() {
        std::wcout.imbue(std::locale("pl_PL.UTF-8"));

        std::wstring ws = L"Pchnąć w tę łódź jeża lub ośm skrzyń fig. Zażółć gęślą jaźń.";
        std::wcout << ws << endl;
    }

    void Ex_2() {
        auto print_data = [](const std::string& loc_name) {
            try {
                std::locale loc(loc_name);
                std::cout.imbue(loc);

                std::cout << "Lokalizacja: " << loc_name << endl;

                std::cout << "Liczby: " << 1234567.89 << " | " << -500 << endl;

                std::cout << "Waluta: " << std::showbase << std::put_money(1234567LL) << endl;

                auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::cout << "Data: " << std::put_time(std::localtime(&now), "%c") << endl;

                std::cout << "-------------------------------" << endl;
            } catch (...) {
                std::cout << "Lokalizacja " << loc_name << " niedostepna." << endl;
            }
        };

        print_data("en_US.UTF-8");
        print_data("de_DE.UTF-8");
        print_data("fr_FR.UTF-8");
    }

    void Ex_3() {
        std::ifstream in("test_utf8.txt", std::ios::binary);
        if (!in) {
            std::cerr << "Brak pliku test_utf8.txt" << endl;
            return;
        }

        std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        in.close();

        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv32;
        std::u32string u32str = conv32.from_bytes(content);

        std::ofstream out32("out_utf32.bin", std::ios::binary);
        out32.write(reinterpret_cast<const char*>(u32str.data()), u32str.size() * sizeof(char32_t));

        try {
            std::ofstream out_iso("out_iso.txt");
            out_iso.imbue(std::locale("pl_PL.ISO88592"));

            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv_wide;
            std::wstring wstr = conv_wide.from_bytes(content);

            for(wchar_t c : wstr) out_iso.put(static_cast<char>(c)); // prymitywne mapowanie

            std::cout << "Konwersja zakonczona (UTF-32 i ISO-8859-2)." << endl;
        } catch (...) {
            std::cerr << "Blad konwersji ISO - brak zainstalowanej lokalizacji." << endl;
        }
    }

    void Ex_4() {
        vec<std::wstring> animals = {
            L"Łoś", L"Łasica", L"Ametyst", L"Ącki", L"Czapla",
            L"Ćma", L"Zebra", L"Źrebak", L"Żyrafa", L"Słoń", L"Świstak"
        };

        try {
            std::locale pol("pl_PL.UTF-8");
            const auto& coll = std::use_facet<std::collate<wchar_t>>(pol);

            std::sort(animals.begin(), animals.end(), [&](const std::wstring& a, const std::wstring& b) {
                return coll.compare(a.data(), a.data() + a.size(), b.data(), b.data() + b.size()) < 0;
            });

            std::wcout.imbue(pol);
            std::wcout << L"Posortowane zwierzęta i rośliny:" << endl;
            for (const auto& s : animals) {
                std::wcout << s << endl;
            }
        } catch (...) {
            std::cerr << "Blad: Nie mozna zaladować polskiego kolatora." << endl;
        }
    }
};

REGISTER_LIST(List10)