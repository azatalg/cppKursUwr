#include "list.h"
#include <memory>
#include <random>
#include <fstream>
#include <filesystem>
#include <vector>
#include <set>
#include <optional>

class List2 : public ListBase {
public:
    List2() {
        REGISTER_TASKS(3);
    }

    std::string_view Name() const override { return "Lista 2"; }

private:
    #pragma region ex1
    class Counter {
    public:
        u64 cnt = 1;
        virtual ~Counter() {
            std::cerr << cnt << endl;
        }
    };
    std::unique_ptr<Counter[]> recursive_modify(std::unique_ptr<Counter[]>&& arr, u32 n, u32 m, u64 depth = 1) {
        if (depth > m) return std::move(arr);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<u32> dist(0, n - 1);

        u32 to_increase = n / 10;
        for (u32 i = 0; i < to_increase; ++i) {
            u32 idx = dist(gen);
            arr[idx].cnt += depth;
        }

        return recursive_modify(std::move(arr), n, m, depth + 1);
    }
    void Ex_1() {
        u32 n = 40;
        u32 m = 20;
        auto arr = std::make_unique<Counter[]>(n);
        arr = recursive_modify(std::move(arr), n, m);
        std::cout << "\nWartosci licznikow po rekurencji:\n";
        for (u32 i = 0; i < n; ++i) {
            std::cout << i << ": " << arr[i].cnt << endl;
        }

    }
    #pragma endregion

    #pragma region ex2
    class line_writer {
        std::ofstream file_;

    public:
        explicit line_writer(const std::filesystem::path& path)
            : file_(path, std::ios::out | std::ios::trunc)
        {
            if (!file_) throw std::runtime_error("line_writer: blad otwierania pliku: " + path.string());
        }

        line_writer(const line_writer&) = delete;
        line_writer& operator=(const line_writer&) = delete;

        line_writer(line_writer&&) = default;
        line_writer& operator=(line_writer&&) = default;

        void write_line(std::string_view line) {
            file_ << line << '\n';
            file_.flush();
            if (!file_) throw std::runtime_error("line_writer: blad zapisywania");
        }

        ~line_writer() noexcept {
            if (file_.is_open())
                file_.close();
            std::cout<<"Line_writer zginal!\n";
        }
    };


    void Ex_2() {
        using std::make_shared;
        using std::shared_ptr;

        shared_ptr<line_writer> writer = make_shared<line_writer>("wynik.txt");

        {
            auto w1 = writer;
            w1->write_line("[A] Rybki robia BOOL BOOL");
            w1->write_line("[A] BOOL BOOL TRWA");
        }

        {
            auto w2 = writer;
            w2->write_line("[B] UwU Ara Ara");
            {
                auto w3 = w2;
                w3->write_line("[C] Yamete Kudasai");
            }
        }

        writer->write_line("[MAIN] BOOL BOOL!");

        std::cout<<"Zapisano do pliku wynik.txt\n";
    }
    #pragma endregion

    #pragma region ex3
    class llama {
    public:
        enum class sex { Male, Female, Unknown };

        inline static std::shared_ptr<llama> NONAME =
            std::make_shared<llama>("__NONAME__", sex::Unknown, nullptr, nullptr, true);

    private:
        std::string name_;
        sex sex_ = sex::Unknown;

        std::shared_ptr<llama> father_;
        std::shared_ptr<llama> mother_;
        std::vector<std::weak_ptr<llama>> children_;

        bool is_noname_ = false;

    public:
        llama(std::string name,
              sex s,
              std::shared_ptr<llama> father = nullptr,
              std::shared_ptr<llama> mother = nullptr,
              bool is_noname = false)
            : name_(std::move(name)), sex_(s), is_noname_(is_noname)
        {
            if (!is_noname_) {
                father_ = father ? father : NONAME;
                mother_ = mother ? mother : NONAME;
                std::cout << "[birth] Llama \"" << name_ << "\" ("
                          << (s == sex::Male ? "M" : s == sex::Female ? "F" : "?")
                          << ") appears in the herd.\n";
            } else {
                father_ = nullptr;
                mother_ = nullptr;
                std::cout << "[init] Creating sentinel parent: NONAME.\n";
            }
        }

        ~llama() {
            if (!is_noname_) {
                std::cout << "[destruct] Llama \"" << name_
                          << "\" leaves the herd (last shared_ptr gone).\n";
            }
        }

        const std::string& name()  const { return name_; }
        sex                gender() const { return sex_; }

        std::shared_ptr<llama> father() const { return father_; }
        std::shared_ptr<llama> mother() const { return mother_; }

        void add_child(const std::shared_ptr<llama>& child) {
            children_.push_back(child);
        }

        void list_children() const {
            std::cout << "  Children of \"" << name_ << "\": ";
            bool any = false;
            for (auto const& w : children_) {
                if (auto s = w.lock()) {
                    std::cout << s->name() << " ";
                    any = true;
                }
            }
            if (!any) std::cout << "(none or expired)";
            std::cout << "\n";
        }

        void sweep_expired_children() {
            children_.erase(
                std::remove_if(children_.begin(), children_.end(),
                               [](const std::weak_ptr<llama>& w){ return w.expired(); }),
                children_.end()
            );
        }
    };

    struct ByName {
        using is_transparent = void;
        bool operator()(const std::shared_ptr<llama>& a,
                        const std::shared_ptr<llama>& b) const {
            return a->name() < b->name();
        }
        bool operator()(const std::shared_ptr<llama>& a,
                        const std::string& b) const {
            return a->name() < b;
        }
        bool operator()(const std::string& a,
                        const std::shared_ptr<llama>& b) const {
            return a < b->name();
        }
    };

    class herd {
        std::set<std::shared_ptr<llama>, ByName> flock_;

    public:
        std::shared_ptr<llama> get(const std::string& name) const {
            auto it = flock_.find(name);
            return (it == flock_.end()) ? nullptr : *it;
        }

        bool buy(const std::string& name, llama::sex s) {
            if (get(name)) return false;
            auto l = std::make_shared<llama>(name, s, nullptr, nullptr);
            flock_.insert(l);
            return true;
        }

        bool birth(const std::string& child_name,
                   const std::string& mother_name,
                   std::optional<std::string> father_name = std::nullopt,
                   llama::sex child_sex = llama::sex::Unknown)
        {
            if (get(child_name)) return false;
            auto mother = get(mother_name);
            if (!mother) return false;

            std::shared_ptr<llama> father =
                father_name ? get(*father_name) : llama::NONAME;

            auto child = std::make_shared<llama>(child_name, child_sex, father, mother);
            flock_.insert(child);

            mother->add_child(child);
            if (father && father != llama::NONAME) father->add_child(child);

            return true;
        }

        bool sell(const std::string& name) {
            auto it = flock_.find(name);
            if (it == flock_.end()) return false;
            flock_.erase(it);
            return true;
        }

        void list() const {
            std::cout << "[herd] ";
            if (flock_.empty()) { std::cout << "(empty)\n"; return; }
            for (auto const& l : flock_) std::cout << l->name() << " ";
            std::cout << "\n";
        }

        void debug_children_of(const std::string& who) const {
            auto l = get(who);
            if (!l) { std::cout << who << " does not exist.\n"; return; }
            l->list_children();
        }

        void sweep_all_expired_children() {
            for (auto const& l : flock_) l->sweep_expired_children();
        }
    };

    void Ex_3() {
        herd h;

        h.buy("Asuna",  llama::sex::Female);
        h.buy("Hinata", llama::sex::Female);

        h.buy("Kirito", llama::sex::Male);
        h.buy("Levi",   llama::sex::Male);

        h.list();

        h.birth("Sakura",  "Asuna",  std::optional<std::string>{"Kirito"}, llama::sex::Female);
        h.birth("Tanjiro", "Hinata", std::nullopt,                          llama::sex::Male);

        h.list();
        h.debug_children_of("Asuna");
        h.debug_children_of("Kirito");
        h.debug_children_of("Hinata");

        std::cout << "\nselling Kirito (father of Sakura)\n";
        h.sell("Kirito");

        h.list();
        if (auto sakura = h.get("Sakura")) {
            std::cout << "Sakura: mother=" << sakura->mother()->name()
                      << ", father=" << (sakura->father() ? sakura->father()->name() : "(null)") << "\n";
        }

        std::cout << "\nselling Sakura\n";
        h.sell("Sakura");

        h.debug_children_of("Asuna");
        std::cout << "(after sweeping expired weak_ptrs)\n";
        h.sweep_all_expired_children();
        h.debug_children_of("Asuna");

        h.list();
        std::cout << "End of herd simulation.\n";
    }
    #pragma endregion

};

REGISTER_LIST(List2)