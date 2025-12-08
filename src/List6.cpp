#include "list.h"
#include <deque>
#include <map>
#include <algorithm>
#include <iomanip>
#include <tuple>
#include <sstream>
#include <iterator>
#include <fstream>

struct Person {
    std::string first_name;
    std::string last_name;
    u32 age;
    u32 weight;
    f64 height;

    f64 get_bmi() const {
        return weight / (height * height);
    }

    std::tuple<std::string, u32, f64> get_details_1() const {
        std::string initials;
        initials += first_name[0];
        initials += last_name[0];
        return { initials, age, get_bmi() };
    }

    std::tuple<u32, u32, f64> get_details_2() const {
        return { age, weight, height };
    }

    std::tuple<std::string, std::string, u32> get_details_3() const {
        return { first_name, last_name, age };
    }

    friend std::ostream& operator<<(std::ostream& out, const Person& p) {
        out << "{ "
            << p.first_name << " " << p.last_name
            << ", age: " << p.age
            << ", weight: " << p.weight
            << ", height: " << p.height
            << ", BMI: " << std::fixed << std::setprecision(2) << p.get_bmi()
            << " }";
        return out;
    }
};

struct Point {
    f64 x, y;
    u8 r, g, b;
    std::string name;

    u8 quadrant() const {
        if (x > 0 && y > 0) return 1;
        if (x <= 0 && y > 0) return 2;
        if (x <= 0 && y <= 0) return 3;
        if (x > 0 && y <= 0) return 4;
        return 0;
    }

    f64 brightness() const {
        return 0.3 * r + 0.59 * g + 0.11 * b;
    }

    friend std::ostream& operator<<(std::ostream& out, const Point& p) {
        out << "{ '" << p.name << "' (" << p.x << ", " << p.y << ") "
            << "RGB[" << (int)p.r << "," << (int)p.g << "," << (int)p.b << "] "
            << "Lum: " << (int)p.brightness() << " }";
        return out;
    }
};

template<typename T, size_t... IDXS, typename F>
void tuple_for_each_impl(T &t, F f, std::index_sequence<IDXS...>) {
    (f(std::get<IDXS>(t)), ...);
}

template<typename T, typename F>
void tuple_for_each(T &t, F f) {
    tuple_for_each_impl(
        t,
        f,
        std::make_index_sequence<std::tuple_size_v<T>>{}
    );
}

class List6 : public ListBase {
public:
    List6() {
        REGISTER_TASKS(6);
    }

    std::string_view Name() const override { return "List 6"; }

private:
    void Ex_1() {
        std::deque<Person> people = {
            {"Asuka", "Langley", 14, 42, 1.57},
            {"Rei", "Ayanami", 14, 38, 1.58},
            {"Mikasa", "Ackerman", 19, 68, 1.76},
            {"Violet", "Evergarden", 14, 45, 1.61},
            {"Zero", "Two", 16, 50, 1.70},
            {"Makise", "Kurisu", 18, 45, 1.60},
            {"Saber", "Pendragon", 25, 102, 1.54},
            {"Rin", "Tohsaka", 17, 47, 1.59},
            {"Holo", "Wisewolf", 600, 45, 1.50},
            {"Megumin", "Crimson", 14, 35, 1.45},
            {"Aqua", "Goddess", 100, 55, 1.65},
            {"Darkness", "Ford", 18, 105, 1.75}
        };

        std::sort(people.begin(), people.end(), [](const Person& a, const Person& b) {
            return a.get_bmi() < b.get_bmi();
        });

        std::cout << "\n[A] Sorted by BMI:" << endl;
        for (const auto& p : people) std::cout << p << endl;

        std::for_each(people.begin(), people.end(), [](Person& p) {
            p.weight = static_cast<u32>(p.weight * 0.9);
        });

        std::cout << "\n[B] After weight loss camp (-10%):" << endl;
        for (const auto& p : people) std::cout << p << endl;

        std::cout << "\n[C] Split groups (Heavy > 100kg, Light <= 100kg):" << endl;

        auto print_filtered = [&](bool heavy) {
            std::cout << (heavy ? "Heavy Group:" : "Light Group:") << endl;
            for (const auto& p : people) {
                bool is_heavy = p.weight > 100;
                if (is_heavy == heavy) {
                    std::cout << "  " << p << endl;
                }
            }
        };

        print_filtered(true);
        print_filtered(false);

        auto min_age_it = std::min_element(people.begin(), people.end(), [](const Person& a, const Person& b) {
            return a.age < b.age;
        });
        auto max_age_it = std::max_element(people.begin(), people.end(), [](const Person& a, const Person& b) {
            return a.age < b.age;
        });

        std::cout << "\n[D] Age extremes:" << endl;
        if (min_age_it != people.end()) std::cout << "Youngest: " << *min_age_it << endl;
        if (max_age_it != people.end()) std::cout << "Oldest: " << *max_age_it << endl;
    }

    void Ex_2() {
        std::list<Point> points = {
            {10.0, 10.0, 255, 0, 0, "Red"},
            {-5.0, 5.0, 0, 255, 0, "Green"},
            {-5.0, -5.0, 0, 0, 255, "Blue"},
            {5.0, -5.0, 255, 255, 255, "White"},
            {1.0, 1.0, 10, 10, 10, "Dark"},
            {-2.0, -2.0, 20, 20, 20, "Darker"},
            {0.0, 0.0, 100, 100, 100, "Center"},
            {3.0, 3.0, 50, 50, 50, "VeryLongName"},
            {-10.0, 2.0, 200, 200, 0, "Yellow"}
        };

        points.remove_if([](const Point& p) {
            return p.name.length() > 5;
        });

        std::cout << "\n[A] Points after removing long names:" << endl;
        for (const auto& p : points) std::cout << p << endl;

        std::cout << "\n[B] Count in quadrants:" << endl;
        for (int i = 1; i <= 4; ++i) {
            auto count = std::count_if(points.begin(), points.end(), [i](const Point& p) {
                return p.quadrant() == i;
            });
            std::cout << "Quadrant " << i << ": " << count << endl;
        }

        points.sort([](const Point& a, const Point& b) {
            return a.brightness() < b.brightness();
        });

        std::cout << "\n[C] Sorted by brightness:" << endl;
        for (const auto& p : points) std::cout << p << endl;

        std::list<Point> dark_points;
        auto it = points.begin();
        while (it != points.end()) {
            if (it->brightness() < 64.0) {
                auto current = it++;
                dark_points.splice(dark_points.end(), points, current);
            } else {
                ++it;
            }
        }

        std::cout << "\n[D] Dark points (moved to new list):" << endl;
        std::cout << "Count: " << dark_points.size() << endl;
        for (const auto& p : dark_points) std::cout << p << endl;
    }

vec<std::pair<i32, i32>> get_most_frequent(vec<i32> data) {
    if (data.empty()) return {};
    std::sort(data.begin(), data.end());

    vec<std::pair<i32, i32>> result;
    i32 max_count = 0;

    i32 current_val = data[0];
    i32 current_count = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == current_val) {
            current_count++;
        } else {
            if (current_count > max_count) {
                max_count = current_count;
                result.clear();
                result.push_back({current_val, current_count});
            } else if (current_count == max_count) {
                result.push_back({current_val, current_count}); // Remis, dopisujemy
            }

            current_val = data[i];
            current_count = 1;
        }
    }

    if (current_count > max_count) {
        result.clear();
        result.push_back({current_val, current_count});
    } else if (current_count == max_count) {
        result.push_back({current_val, current_count});
    }

    return result;
}


void Ex_3() {
    vec<i32> data = {1, 1, 3, 5, 8, 9, 5, 8, 8, 5};

    std::cout << "Input data: ";
    for(auto i : data) std::cout << i << " ";
    std::cout << endl;

    vec<std::pair<i32, i32>> result = get_most_frequent(data);

    std::cout << "\nMost frequent elements:" << endl;
    for (const auto& p : result) {
        std::cout << "Value: " << p.first << ", Occurrences: " << p.second << endl;
    }
}

    void Ex_4() {
        std::string filename = "src/test.txt";

        std::ifstream file(filename);

        if (!file) {
            std::cout << "Error: Cannot open file '" << filename << endl;
            return;
        }

        std::cout << "Analyzing file: " << filename << endl;

        std::istream_iterator<char> it(file);
        std::istream_iterator<char> end;

        std::map<char, int> letter_counts;
        int total_letters = 0;

        std::for_each(it, end, [&](char c) {
            if (std::isalpha(static_cast<unsigned char>(c))) {
                char lower_c = std::tolower(static_cast<unsigned char>(c));
                letter_counts[lower_c]++;
                total_letters++;
            }
        });

        if (total_letters == 0) {
            std::cout << "No letters found in file." << endl;
            return;
        }

        std::cout << "\nLetter Frequency Histogram:" << endl;
        for (auto const& [key, val] : letter_counts) {
            double freq = static_cast<double>(val) / total_letters;
            std::cout << key << " (" << std::fixed << std::setprecision(4) << freq << "): ";

            int bars = static_cast<int>(freq * 100);
            for(int i = 0; i < bars; ++i) std::cout << "*";
            std::cout << endl;
        }
    }
    void Ex_5() {
        std::string s = "waifu";
        std::sort(s.begin(), s.end());

        std::cout << "Permutations of '" << s << "':" << endl;
        do {
            std::cout << s << " ";
        } while (std::next_permutation(s.begin(), s.end()));
        std::cout << endl;
    }

    void Ex_6() {
        Person p = {"Rem", "Rezero", 18, 50, 1.55};
        std::cout << "Person: " << p << endl;

        std::cout << "\n[1] Structured Binding" << endl;
        auto [initials, age, bmi] = p.get_details_1();
        std::cout << "Initials: " << initials << ", Age: " << age << ", BMI: " << bmi << endl;

        std::cout << "\n[2] Tuple Iteration" << endl;
        auto t2 = p.get_details_2();
        tuple_for_each(t2, [](auto& x){
            std::cout << x << " ";
        });
        std::cout << endl;

        std::cout << "\n[3] Tuple Iteration" << endl;
        auto t3 = p.get_details_3();
        tuple_for_each(t3, [](auto& x){
            std::cout << x << " ";
        });
        std::cout << endl;
    }
};

REGISTER_LIST(List6)