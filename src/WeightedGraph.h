#pragma once
#include "defines.h"

#include <map>
#include <queue>
#include <string>
#include <algorithm>

struct AdjEdge {
    std::string to;
    f64 w{};
};

class UnionFind {
public:
    UnionFind() = default;

    explicit UnionFind(usize n) {
        Reset(n);
    }

    void Reset(usize n) {
        parent.resize(n);
        rank.assign(n, 0);
        for (usize i = 0; i < n; ++i) parent[i] = static_cast<i32>(i);
    }

    i32 Find(i32 x) {
        if (parent[x] == x) return x;
        return parent[x] = Find(parent[x]);
    }

    bool Unite(i32 a, i32 b) {
        a = Find(a);
        b = Find(b);
        if (a == b) return false;
        if (rank[a] < rank[b]) std::swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b]) ++rank[a];
        return true;
    }

private:
    vec<i32> parent;
    vec<i32> rank;
};

class DynamicGraph {
public:
    void AddVertex(const std::string& id) {
        adj_[id];
    }

    void RemoveVertex(const std::string& id) {
        auto it = adj_.find(id);
        if (it == adj_.end()) return;
        for (auto& [u, nbrs] : adj_) {
            if (u == id) continue;
            RemoveEdgeFromList(nbrs, id);
        }
        adj_.erase(it);
    }

    void AddOrUpdateEdge(const std::string& u,
                         const std::string& v,
                         f64 w) {
        ASSERT(u != v);
        AddVertex(u);
        AddVertex(v);
        AddOrUpdateEdgeOneSide(u, v, w);
        AddOrUpdateEdgeOneSide(v, u, w);
    }

    void AddEdge(const std::string& u,
                 const std::string& v,
                 f64 w) {
        AddOrUpdateEdge(u, v, w);
    }

    void RemoveEdge(const std::string& u,
                    const std::string& v) {
        auto itU = adj_.find(u);
        auto itV = adj_.find(v);
        if (itU == adj_.end() || itV == adj_.end()) return;
        RemoveEdgeFromList(itU->second, v);
        RemoveEdgeFromList(itV->second, u);
    }

    void ChangeWeight(const std::string& u,
                      const std::string& v,
                      f64 newW) {
        AddOrUpdateEdge(u, v, newW);
    }

    NO_DISCARD bool HasVertex(const std::string& id) const {
        return adj_.find(id) != adj_.end();
    }

    NO_DISCARD bool HasEdge(const std::string& u,
                            const std::string& v) const {
        auto it = adj_.find(u);
        if (it == adj_.end()) return false;
        for (const auto& e : it->second) {
            if (e.to == v) return true;
        }
        return false;
    }

    NO_DISCARD const std::map<std::string, vec<AdjEdge>>& Adjacency() const {
        return adj_;
    }

    DynamicGraph PrimMST() const {
        DynamicGraph mst;
        if (adj_.empty()) return mst;

        std::set<std::string> inMst;

        struct Node {
            f64 w;
            std::string from;
            std::string to;
        };

        struct Cmp {
            bool operator()(const Node& a, const Node& b) const {
                return a.w > b.w;
            }
        };

        std::priority_queue<Node, vec<Node>, Cmp> pq;

        auto startIt = adj_.begin();
        const std::string start = startIt->first;

        inMst.insert(start);
        mst.AddVertex(start);

        for (const auto& e : startIt->second) {
            pq.push(Node{ e.w, start, e.to });
        }

        while (!pq.empty()) {
            Node cur = pq.top();
            pq.pop();
            if (inMst.count(cur.to)) continue;

            inMst.insert(cur.to);
            mst.AddVertex(cur.to);
            mst.AddEdge(cur.from, cur.to, cur.w);

            auto it = adj_.find(cur.to);
            if (it == adj_.end()) continue;

            for (const auto& e : it->second) {
                if (!inMst.count(e.to)) {
                    pq.push(Node{ e.w, cur.to, e.to });
                }
            }
        }

        return mst;
    }

    DynamicGraph KruskalMST() const {
        DynamicGraph mst;
        if (adj_.empty()) return mst;

        std::map<std::string, i32> id;
        vec<std::string> verts;
        for (const auto& [u, _] : adj_) {
            id[u] = static_cast<i32>(verts.size());
            verts.push_back(u);
        }

        struct Edge {
            i32 u;
            i32 v;
            f64 w;
        };

        vec<Edge> edges;
        for (const auto& [u, nbrs] : adj_) {
            i32 iu = id[u];
            for (const auto& e : nbrs) {
                i32 iv = id[e.to];
                if (iu < iv) {
                    edges.push_back(Edge{ iu, iv, e.w });
                }
            }
        }

        std::sort(edges.begin(), edges.end(),
                  [](const Edge& a, const Edge& b) {
                      return a.w < b.w;
                  });

        for (const auto& v : verts) mst.AddVertex(v);

        UnionFind dsu(verts.size());
        for (const auto& e : edges) {
            if (dsu.Unite(e.u, e.v)) {
                mst.AddEdge(verts[e.u], verts[e.v], e.w);
            }
        }

        return mst;
    }

private:
    std::map<std::string, vec<AdjEdge>> adj_;

    static void RemoveEdgeFromList(vec<AdjEdge>& list,
                                   const std::string& v) {
        list.erase(
            std::remove_if(list.begin(), list.end(),
                           [&v](const AdjEdge& e) { return e.to == v; }),
            list.end()
        );
    }

    void AddOrUpdateEdgeOneSide(const std::string& u,
                                const std::string& v,
                                f64 w) {
        auto& nbrs = adj_[u];
        for (auto& e : nbrs) {
            if (e.to == v) {
                e.w = w;
                return;
            }
        }
        nbrs.push_back(AdjEdge{ v, w });
    }
};

inline std::ostream& operator<<(std::ostream& os, const DynamicGraph& g) {
    for (const auto& [u, nbrs] : g.Adjacency()) {
        for (const auto& e : nbrs) {
            if (u < e.to) {
                os << u << " - " << e.to << "  " << e.w << endl;
            }
        }
    }
    return os;
}

