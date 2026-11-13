#include "opencadpro/geometry.hpp"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {
void print_help() {
    std::cout << "OpenCAD Pro MVP 0.1.0\n"
              << "Usage:\n"
              << "  opencadpro --box WIDTH HEIGHT DEPTH --output model.obj [--svg sketch.svg]\n"
              << "  opencadpro --demo [output.obj]\n\n"
              << "Creates an extruded rectangular solid (units: mm), exports an OBJ mesh,\n"
              << "and optionally exports the 2D profile as SVG. This is an early geometry\n"
              << "kernel prototype, not a full parametric CAD application.\n";
}

double parse_positive(const char* text, const char* label) {
    std::size_t used = 0;
    const std::string input(text);
    const double value = std::stod(input, &used);
    if (used != input.size() || value <= 0.0) {
        throw std::invalid_argument(std::string(label) + " must be a positive number.");
    }
    return value;
}

std::filesystem::path with_extension(std::filesystem::path path,
                                     const std::string& extension) {
    path.replace_extension(extension);
    return path;
}
}  // namespace

int main(int argc, char** argv) {
    try {
        if (argc == 1 || std::string(argv[1]) == "--help" ||
            std::string(argv[1]) == "-h") {
            print_help();
            return 0;
        }

        double width = 100.0, height = 60.0, depth = 20.0;
        std::filesystem::path output = "opencadpro-demo.obj";
        std::filesystem::path svg;
        if (std::string(argv[1]) == "--demo") {
            if (argc > 2) output = argv[2];
            svg = with_extension(output, ".svg");
        } else if (std::string(argv[1]) == "--box") {
            if (argc < 7) {
                print_help();
                return 2;
            }
            width = parse_positive(argv[2], "WIDTH");
            height = parse_positive(argv[3], "HEIGHT");
            depth = parse_positive(argv[4], "DEPTH");
            bool has_output = false;
            for (int i = 5; i < argc; ++i) {
                const std::string option(argv[i]);
                if (option == "--output" && i + 1 < argc) {
                    output = argv[++i];
                    has_output = true;
                } else if (option == "--svg" && i + 1 < argc) {
                    svg = argv[++i];
                } else {
                    throw std::invalid_argument("Unknown or incomplete option: " + option);
                }
            }
            if (!has_output) throw std::invalid_argument("--output is required.");
        } else {
            print_help();
            return 2;
        }

        const std::vector<opencadpro::Vec2> profile{
            {0.0, 0.0}, {width, 0.0}, {width, height}, {0.0, height}};
        const auto mesh = opencadpro::extrude_polygon(profile, depth);
        opencadpro::write_obj(mesh, output);
        if (!svg.empty()) opencadpro::write_svg(profile, svg);
        std::cout << "Created " << output << " (" << mesh.triangles.size()
                  << " triangles)";
        if (!svg.empty()) std::cout << " and " << svg;
        std::cout << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "OpenCAD Pro: " << error.what() << '\n';
        return 1;
    }
}
