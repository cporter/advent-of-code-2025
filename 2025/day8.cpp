#include <set>

#include "prelude/prelude.hpp"
namespace rv = std::ranges::views;

struct vec3 {
    int x, y, z;
};

std::ostream &operator<<(std::ostream &out, vec3 &v) {
    out << "<" << v.x << ", " << v.y << ", " << v.z << ">";
    return out;
}

long distance(const vec3 &a, const vec3 &b) {
    long dx = a.x - b.x;
    long dy = a.y - b.y;
    long dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

struct edge {
    size_t a, b;
    long distance;
};

struct DSU {
    std::vector<size_t> parent, size;

    DSU(size_t n) {
        parent.resize(n);
        size.resize(n);
        for (size_t i = 0; i < n; i++) {
            parent[i] = i;
            size[i] = 1;
        }
    }

    size_t find(size_t x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    bool unite(size_t a, size_t b) {
        size_t ar = find(a);
        size_t br = find(b);
        if (ar == br) {
            return false;
        }

        if (size[ar] < size[br]) {
            std::swap(ar, br);
        }
        size[ar] += size[br];
        parent[br] = ar;
        return true;
    }

    auto subgraph_sizes() {
        std::map<size_t, size_t> sizes;
        for (size_t i = 0; i < parent.size(); ++i) {
            auto root = find(i);
            sizes[root] = size[root];
        }

        std::vector<size_t> ret;
        for (auto &[_, s] : sizes) {
            ret.push_back(s);
        }
        std::sort(ret.begin(), ret.end());
        std::reverse(ret.begin(), ret.end());

        return ret;
    }
};

auto calc_part1(const std::vector<vec3> &verts, const std::vector<edge> &es) {
    DSU dsu(verts.size());
    const size_t N = verts.size() < 100 ? 10 : 1000;
    for (auto &e : es | rv::take(N)) {
        dsu.unite(e.a, e.b);
    }
    auto subsizes = dsu.subgraph_sizes();

    return subsizes | rv::take(3) | prelude::product;
}

int main(int, char **) {
    auto points = prelude::line_view(std::cin) | rv::transform([](const std::string &line) {
                      auto parts = line | rv::split(',') | rv::transform([](auto &&r) {
                                       return std::stoi(std::string(r.begin(), r.end()));
                                   });
                      vec3 v;
                      auto ints = parts.begin();
                      v.x = *ints++;
                      v.y = *ints++;
                      v.z = *ints++;
                      return v;
                  })
                  | prelude::collect<std::vector>;

    std::vector<edge> edges;
    edges.reserve(points.size() * points.size());

    for (size_t a = 0; a < points.size(); ++a) {
        for (size_t b = 1 + a; b < points.size(); ++b) {
            edges.emplace_back(a, b, distance(points[a], points[b]));
        }
    }

    std::sort(edges.begin(), edges.end(),
              [](const edge &a, const edge &b) { return a.distance < b.distance; });

    auto part1 = calc_part1(points, edges);

    fmt::print("part 1 = {}\n", part1);

    return 0;
}