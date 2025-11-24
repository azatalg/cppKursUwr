#include "list.h"
#include "./lexList5.h"
#include "./WeightedGraph.h"

class List5 : public ListBase {
public:
    List5() {
        REGISTER_TASKS(3);
    }

    std::string_view Name() const override { return "Lista 5"; }

private:
    void Ex_1() {
        std::string input = "int x = a + 42; if (x >= 10) x++;";

        std::map<std::string, double> constants;
        std::unordered_map<std::string, double> variables;
        std::deque<std::string> functions;

        auto tokens = LexSymbols(input, constants, variables, functions);

        for (const Symbol &tok: tokens) {
            std::cout << "[";
            std::cout << tok.text;
            std::cout << "]" << " ";
        }
    }

    void Ex_2() {
        std::string input = "sin(pi / 2) + x * 3";

        std::map<std::string, double> constants = {
            {"pi", 3.141592653589793}
        };
        std::unordered_map<std::string, double> variables = {
            {"x", 2.0}
        };
        std::deque<std::string> functions = {
            "sin"
        };

        auto infix = LexSymbols(input, constants, variables, functions);
        std::cout << "Infix: \n";
        for (const Symbol &tok: infix) {
            std::cout << "[" << tok.text << "]" << " ";
        }
        auto postfix = InfixToPostfix(infix);
        std::cout << "\nPostfix: \n";
        for (const Symbol &tok: postfix) {
            std::cout << "[" << tok.text << "]" << " ";
        }
    }

    void TestUnionFind() {
        UnionFind uf(5);
        ASSERT(uf.Find(0) == uf.Find(0));
        ASSERT(uf.Unite(0, 1));
        ASSERT(uf.Find(0) == uf.Find(1));
        ASSERT(!uf.Unite(0, 1));
        ASSERT(uf.Unite(2, 3));
        ASSERT(uf.Unite(1, 2));
        ASSERT(uf.Find(0) == uf.Find(3));
    }

    void TestDynamicGraphBasic() {
        DynamicGraph g;

        g.AddVertex("A");
        g.AddVertex("B");
        ASSERT(g.HasVertex("A"));
        ASSERT(g.HasVertex("B"));
        ASSERT(!g.HasVertex("X"));

        g.AddEdge("A", "B", 5.0);
        ASSERT(g.HasEdge("A", "B"));
        ASSERT(g.HasEdge("B", "A"));

        g.AddOrUpdateEdge("A", "B", 3.0);
        {
            const auto &adj = g.Adjacency();
            auto it = adj.find("A");
            ASSERT(it != adj.end());
            bool ok = false;
            for (const auto &e: it->second) {
                if (e.to == "B" && e.w == 3.0) ok = true;
            }
            ASSERT(ok);
        }

        g.ChangeWeight("A", "B", 2.0);
        {
            const auto &adj = g.Adjacency();
            auto it = adj.find("A");
            ASSERT(it != adj.end());
            bool ok = false;
            for (const auto &e: it->second) {
                if (e.to == "B" && e.w == 2.0) ok = true;
            }
            ASSERT(ok);
        }

        g.RemoveEdge("A", "B");
        ASSERT(!g.HasEdge("A", "B"));
        ASSERT(!g.HasEdge("B", "A"));

        g.RemoveVertex("A");
        ASSERT(!g.HasVertex("A"));
    }

    void TestDynamicGraphMST() {
        DynamicGraph g;
        g.AddEdge("A", "B", 4.0);
        g.AddEdge("A", "C", 2.0);
        g.AddEdge("B", "C", 1.0);
        g.AddEdge("B", "D", 5.0);
        g.AddEdge("C", "D", 8.0);
        g.AddEdge("C", "E", 10.0);
        g.AddEdge("D", "E", 2.0);

        auto mstPrim = g.PrimMST();
        auto mstKruskal = g.KruskalMST();

        std::cout << "Original graph:" << endl << g;
        std::cout << "MST Prim:" << endl << mstPrim;
        std::cout << "MST Kruskal:" << endl << mstKruskal;
    }

    void TestDynamicGraphRemoveVertexEffect() {
        DynamicGraph g;
        g.AddEdge("A", "B", 1.0);
        g.AddEdge("B", "C", 2.0);
        g.AddEdge("C", "A", 3.0);

        ASSERT(g.HasVertex("B"));
        ASSERT(g.HasEdge("A", "B"));
        ASSERT(g.HasEdge("B", "C"));

        g.RemoveVertex("B");

        ASSERT(!g.HasVertex("B"));
        ASSERT(!g.HasEdge("A", "B"));
        ASSERT(!g.HasEdge("B", "C"));

        std::cout << "Graph after removing B:" << endl << g;
    }

    void Ex_3() {
        TestUnionFind();
        TestDynamicGraphBasic();
        TestDynamicGraphMST();
        TestDynamicGraphRemoveVertexEffect();
    }
};

REGISTER_LIST(List5)
