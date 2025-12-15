#include "list.h"
#include <regex>

class List8 : public ListBase {
public:
    List8() { REGISTER_TASKS(5); }
    std::string_view Name() const override { return "Lista 8"; }

private:
    static void TestRegex(const std::regex& re,
                          const vec<std::string>& ok,
                          const vec<std::string>& bad) {
        std::cout << "== MATCH ==" << endl;
        for (const auto& s : ok) {
            std::cout << (std::regex_match(s, re) ? "TAK " : "NIE ") << " | " << s << endl;
        }
        std::cout << "== NO MATCH ==" << endl;
        for (const auto& s : bad) {
            std::cout << (std::regex_match(s, re) ? "TAK " : "NIE ") << " | " << s << endl;
        }
    }

    static void RunLineMatcherFromCin(const std::regex& re, const std::string_view ex) {
        std::cout << "Podaj tekst do zadania " << ex << ": ";
        std::string s;
        std::getline(std::cin, s);
        std::cout << (std::regex_match(s, re) ? "TAK" : "NIE") << endl;
    }




    void Ex_1() {
        const std::regex re(R"(^(?:[01]\d|2[0-3]):[0-5]\d(?::[0-5]\d)?$)");
        vec<std::string> ok = {
            "00:00", "09:05", "23:59", "12:34:56", "00:00:00", "23:59:59"
        };
        vec<std::string> bad = {
            "24:00", "12:60", "12:34:60", "3:05", "03:5", "03:05:5", "12-34", "12:34:"
        };
        TestRegex(re, ok, bad);

        //RunLineMatcherFromCin(re, "1");
    }

    void Ex_2() {
        const std::regex re(
            R"(^(?:(?:0[1-9]|[12]\d|3[01])-(?:01|03|05|07|08|10|12)|(?:0[1-9]|[12]\d|30)-(?:04|06|09|11)|(?:0[1-9]|1\d|2[0-8])-02)-\d{4}$)"
        );
        vec<std::string> ok = {
            "01-01-2000", "31-01-1999", "30-04-2020", "28-02-2023", "29-02-2023" /* NIE, ale regex ma 28 max, więc tu nie przejdzie */
        };
        vec<std::string> bad = {
            "00-01-2000", "32-01-2000", "31-04-2020", "29-02-2023", "30-02-2020",
            "1-01-2000", "01-1-2000", "01/01/2000", "01-01-20"
        };
        TestRegex(re, ok, bad);
        //RunLineMatcherFromCin(re, "2");
    }

    void Ex_3() {
        const std::regex re(
            R"(^\s*[A-Z][a-z]*(?:-[A-Z][a-z]*)*(?:\s+[A-Z][a-z]*(?:-[A-Z][a-z]*)*)*\s*$)"
        );
        vec<std::string> ok = {
            "Wroclaw",
            "Nowa Huta",
            "Bielsko-Biala",
            "Nowy Targ",
            "  Gora Kalwaria  ",
            "Zimna Wodka  ",
        };
        vec<std::string> bad = {
            "wroclaw",
            "ZIMNA WODKA",
            "ZiMNa WOOOODKa",
            "Nowa-Huta-",
            "-Nowa Huta",
            "Nowa_huta",
            "Nowa123",
            "Nowa huta",
            "zimna-wodka-07"
        };
        TestRegex(re, ok, bad);
       // RunLineMatcherFromCin(re, "3");
    }

    void Ex_4() {
        const std::regex re(
            R"(^\((-?\d+(?:\.\d+)?)[+-](\d+(?:\.\d+)?)[iI]\)$)"
        );
        vec<std::string> ok = {
            "(1+2i)",
            "(-3-4i)",
            "(0+0i)",
            "(12.5-0.25i)",
            "(-0.5+10I)"
        };
        vec<std::string> bad = {
            "1+2i",
            "(1+2)",
            "(1+i)",
            "(+1+2i)",
            "(1+2j)",
            "(1 +2i)",
            "(1+ 2i)",
            "(1+2i))"
        };
        TestRegex(re, ok, bad);
        //RunLineMatcherFromCin(re, "4");
    }

    void Ex_5() {
        const std::regex re(
            R"re(<a\b[^>]*\bhref\s*=\s*(?:"([^"<>]*)"|'([^'<>]*)')[^>]*>)re",
            std::regex::icase
        );


        vec<std::string> ok = {
            R"(<a href="https://example.com">x</a>)",
            R"(<a class="btn" href="page.html">link</a>)",
            R"(<a id='x' href='a/b/c'>ok</a>)",
            R"(<A HREF="UPPER.html">ok</A>)",
            R"(<a data-x="1" href="x" target="_blank">ok</a>)"
        };
        vec<std::string> bad = {
            R"(<link href="style.css">)",
            R"(<a>no href</a>)",
            R"(<a href=>bad</a>)",
            R"(<a href="unterminated>bad</a>)",
            R"(<a name="x" >nope</a>)"
        };

        std::cout << "== MATCH + WYCIAGNIECIE href ==" << endl;
        for (const auto& s : ok) {
            std::smatch m;
            const bool match = std::regex_search(s, m, re);
            std::cout << (match ? "TAK " : "NIE ") << " | " << s << endl;
            if (match) {
                if (m[1].matched) std::cout << "href = " << m[1].str() << endl;
                else              std::cout << "href = " << m[2].str() << endl;
            }
        }

        std::cout << "== NO MATCH ==" << endl;
        for (const auto& s : bad) {
            std::smatch m;
            const bool match = std::regex_search(s, m, re);
            std::cout << (match ? "TAK " : "NIE ") << " | " << s << endl;
        }

        std::string_view sv = Arg(1);
        if (sv.empty()) {
            std::cout << "Brak pliku w Arg(1)" << endl;
            return;
        }

        const std::string html = ReadFileFromArg(1);
        std::cout << "== HREFY Z PLIKU " << sv << "==" << endl;
        for (auto it = std::sregex_iterator(html.begin(), html.end(), re); it != std::sregex_iterator(); ++it) {
            const std::smatch& m = *it;
            if (m[1].matched)
                std::cout << m[1].str() << endl;
            else
                std::cout << m[2].str() << endl;
        }
    }
};

REGISTER_LIST(List8)
