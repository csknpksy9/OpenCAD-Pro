#include "opencadpro/geometry.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>

namespace opencadpro {
namespace {

double cross(const Vec2& a, const Vec2& b, const Vec2& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

double signed_area(const std::vector<Vec2>& points) {
    double area = 0.0;
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto& a = points[i];
        const auto& b = points[(i + 1) % points.size()];
        area += a.x * b.y - b.x * a.y;
    }
    return area * 0.5;
}

bool inside_triangle(const Vec2& p, const Vec2& a, const Vec2& b,
                     const Vec2& c) {
    constexpr double eps = 1e-10;
    const double c1 = cross(a, b, p);
    const double c2 = cross(b, c, p);
    const double c3 = cross(c, a, p);
    return c1 >= -eps && c2 >= -eps && c3 >= -eps;
}

std::vector<std::array<std::size_t, 3>> triangulate(
    const std::vector<Vec2>& points) {
    if (points.size() < 3) {
        throw std::invalid_argument("A polygon needs at least three vertices.");
    }
    if (std::abs(signed_area(points)) < 1e-10) {
        throw std::invalid_argument("The polygon has zero area.");
    }

    std::vector<std::size_t> remaining(points.size());
    for (std::size_t i = 0; i < points.size(); ++i) remaining[i] = i;
    if (signed_area(points) < 0.0) {
        std::reverse(remaining.begin(), remaining.end());
    }

    std::vector<std::array<std::size_t, 3>> result;
    result.reserve(points.size() - 2);
    while (remaining.size() > 3) {
        bool clipped = false;
        for (std::size_t i = 0; i < remaining.size(); ++i) {
            const auto prev = remaining[(i + remaining.size() - 1) % remaining.size()];
            const auto curr = remaining[i];
            const auto next = remaining[(i + 1) % remaining.size()];
            if (cross(points[prev], points[curr], points[next]) <= 1e-10) {
                continue;
            }
            bool contains_vertex = false;
            for (const auto candidate : remaining) {
                if (candidate == prev || candidate == curr || candidate == next) continue;
                if (inside_triangle(points[candidate], points[prev],
                                    points[curr], points[next])) {
                    contains_vertex = true;
                    break;
                }
            }
            if (contains_vertex) continue;
            result.push_back({prev, curr, next});
            remaining.erase(remaining.begin() + static_cast<std::ptrdiff_t>(i));
            clipped = true;
            break;
        }
        if (!clipped) {
            throw std::invalid_argument(
                "Could not triangulate polygon; check for self-intersections or duplicate vertices.");
        }
    }
    result.push_back({remaining[0], remaining[1], remaining[2]});
    return result;
}

}  // namespace

Mesh extrude_polygon(const std::vector<Vec2>& polygon, double depth) {
    if (!std::isfinite(depth) || depth <= 0.0) {
        throw std::invalid_argument("Extrusion depth must be a finite positive number.");
    }
    const auto faces = triangulate(polygon);
    Mesh mesh;
    mesh.triangles.reserve(faces.size() * 2 + polygon.size() * 2);

    const auto at_z = [](const Vec2& point, double z) {
        return Vec3{point.x, point.y, z};
    };
    for (const auto& face : faces) {
        // Bottom faces point outward (-Z), top faces point outward (+Z).
        mesh.triangles.push_back({at_z(polygon[face[2]], 0.0),
                                  at_z(polygon[face[1]], 0.0),
                                  at_z(polygon[face[0]], 0.0)});
        mesh.triangles.push_back({at_z(polygon[face[0]], depth),
                                  at_z(polygon[face[1]], depth),
                                  at_z(polygon[face[2]], depth)});
    }
    for (std::size_t i = 0; i < polygon.size(); ++i) {
        const auto j = (i + 1) % polygon.size();
        const auto a0 = at_z(polygon[i], 0.0);
        const auto b0 = at_z(polygon[j], 0.0);
        const auto a1 = at_z(polygon[i], depth);
        const auto b1 = at_z(polygon[j], depth);
        mesh.triangles.push_back({a0, b0, b1});
        mesh.triangles.push_back({a0, b1, a1});
    }
    return mesh;
}

void write_obj(const Mesh& mesh, const std::filesystem::path& path) {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("Cannot write OBJ file: " + path.string());
    out << "# OpenCAD Pro MVP - triangle mesh\n" << std::fixed << std::setprecision(6);
    std::size_t index = 1;
    for (const auto& triangle : mesh.triangles) {
        for (const auto& vertex : triangle) {
            out << "v " << vertex.x << ' ' << vertex.y << ' ' << vertex.z << '\n';
        }
    }
    for (std::size_t i = 0; i < mesh.triangles.size(); ++i) {
        out << "f " << index << ' ' << index + 1 << ' ' << index + 2 << '\n';
        index += 3;
    }
    if (!out) throw std::runtime_error("Failed while writing OBJ file: " + path.string());
}

void write_svg(const std::vector<Vec2>& polygon, const std::filesystem::path& path) {
    if (polygon.size() < 3) throw std::invalid_argument("A sketch needs at least three vertices.");
    double min_x = polygon.front().x, max_x = min_x;
    double min_y = polygon.front().y, max_y = min_y;
    for (const auto& p : polygon) {
        min_x = std::min(min_x, p.x); max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y); max_y = std::max(max_y, p.y);
    }
    const double width = std::max(max_x - min_x, 1.0);
    const double height = std::max(max_y - min_y, 1.0);
    constexpr double margin = 10.0;
    std::ofstream out(path);
    if (!out) throw std::runtime_error("Cannot write SVG file: " + path.string());
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\""
        << width + 2 * margin << "mm\" height=\"" << height + 2 * margin
        << "mm\" viewBox=\"0 0 " << width + 2 * margin << ' '
        << height + 2 * margin << "\">\n"
        << "  <rect width=\"100%\" height=\"100%\" fill=\"#111827\"/>\n"
        << "  <polygon points=\"";
    for (const auto& p : polygon) {
        out << (p.x - min_x + margin) << ',' << (max_y - p.y + margin) << ' ';
    }
    out << "\" fill=\"#2563eb\" fill-opacity=\"0.18\" stroke=\"#60a5fa\" stroke-width=\"0.8\"/>\n</svg>\n";
    if (!out) throw std::runtime_error("Failed while writing SVG file: " + path.string());
}

}  // namespace opencadpro
