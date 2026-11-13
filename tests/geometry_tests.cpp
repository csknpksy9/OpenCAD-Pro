#include "opencadpro/geometry.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void test_rectangle_extrusion() {
    const std::vector<opencadpro::Vec2> rectangle{
        {0, 0}, {4, 0}, {4, 3}, {0, 3}};
    const auto mesh = opencadpro::extrude_polygon(rectangle, 2.0);
    require(mesh.triangles.size() == 12, "A rectangular prism must have 12 triangles.");
    double max_z = 0.0;
    for (const auto& triangle : mesh.triangles)
        for (const auto& vertex : triangle) max_z = std::max(max_z, vertex.z);
    require(std::abs(max_z - 2.0) < 1e-10, "Extrusion depth should be preserved.");
}

void test_concave_polygon() {
    const std::vector<opencadpro::Vec2> l_shape{
        {0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 3}, {0, 3}};
    const auto mesh = opencadpro::extrude_polygon(l_shape, 5.0);
    require(mesh.triangles.size() == 20, "A six-sided profile should produce 20 triangles.");
}

void test_clockwise_polygon() {
    const std::vector<opencadpro::Vec2> clockwise{
        {0, 3}, {4, 3}, {4, 0}, {0, 0}};
    require(opencadpro::extrude_polygon(clockwise, 1.0).triangles.size() == 12,
            "Clockwise input should be normalized.");
}

void test_invalid_depth() {
    bool threw = false;
    try {
        (void)opencadpro::extrude_polygon({{0, 0}, {1, 0}, {0, 1}}, 0.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, "Zero depth must be rejected.");
}

void test_obj_and_svg_export() {
    const auto tmp = std::filesystem::temp_directory_path() / "opencadpro-test";
    std::filesystem::create_directories(tmp);
    const std::vector<opencadpro::Vec2> profile{{0, 0}, {2, 0}, {2, 1}, {0, 1}};
    opencadpro::write_obj(opencadpro::extrude_polygon(profile, 1.0), tmp / "part.obj");
    opencadpro::write_svg(profile, tmp / "sketch.svg");
    std::ifstream obj(tmp / "part.obj");
    std::ifstream svg(tmp / "sketch.svg");
    std::string obj_text((std::istreambuf_iterator<char>(obj)), {});
    std::string svg_text((std::istreambuf_iterator<char>(svg)), {});
    require(obj_text.find("f 1 2 3") != std::string::npos, "OBJ faces should be exported.");
    require(svg_text.find("<polygon") != std::string::npos, "SVG sketch should be exported.");
}
}  // namespace

int main() {
    try {
        test_rectangle_extrusion();
        test_concave_polygon();
        test_clockwise_polygon();
        test_invalid_depth();
        test_obj_and_svg_export();
        std::cout << "All 5 OpenCAD Pro tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << '\n';
        return 1;
    }
}
