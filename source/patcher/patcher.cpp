/**************************************************************************/
/* Copyright (c) 2024-present Craig Stoneham.                             */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "patcher.h"
#include "uuid.h"

void Patcher::add_module(const std::string &p_module) {
    for (const std::string module : modules) {
        if (module == p_module) {
            return;
        }
    }
    modules.push_back(p_module);
}

void Patcher::patch() {

    std::string solution_path;

    if (find_solution(fs::current_path(), solution_name, solution_path)) {
        std::cout << "Found solution \'" << solution_path << "\'\n";
    } else {
        std::cout << "Unable to find solution \'" << solution_name << "\'\n";
        return;
    }

    if (!document.load_file(solution_path.c_str())) {
        std::cout << "Unable to load solution\n";
        return;
    }

    patch_solution();

    if (!document.save_file(solution_path.c_str())) {
        std::cout << "Unable to save solution\n";
        return;
    }
}

bool Patcher::find_solution(const fs::path &p_path, const std::string &p_solution_name, std::string &p_result) const {
    const std::string solution_path = p_path.string() + "/" + p_solution_name;

    if (fs::exists(solution_path)) {
        p_result = solution_path;
        return true;
    }

    for (auto &path : fs::directory_iterator(p_path)) {
        if (fs::is_directory(path)) {
            if (find_solution(path, p_solution_name, p_result)) {
                return true;
            }
        }
    }
    return false;
}

void Patcher::add_filter(pugi::xml_node p_node, const std::string &p_name) {
    std::string filter_name = p_name + "\\godot";
    for (pugi::xml_node sibling : p_node.children()) {
        if (sibling.first_attribute().value() == filter_name) {
            return;
        }
    }
    for (pugi::xml_node sibling : p_node.children()) {
        if (sibling.first_attribute().value() == p_name) {
            pugi::xml_node filter = sibling.parent().insert_child_after("Filter", sibling);
            filter.append_attribute("Include").set_value(filter_name.c_str());
            filter.append_child("UniqueIdentifier").append_child(pugi::node_pcdata).set_value(generate_uuid().c_str());
            break;
        }
    }
}

bool Patcher::patch_name(std::string &p_name) const {
    bool patched = false;

    for (const std::string &module : modules) {
        // Iterate through all of the modules as some may be a submodule
        size_t module_pos = p_name.find(module);
        if (module_pos != std::string::npos) {
            const std::string header_files = "Header Files\\";
            const std::string source_files = "Source Files\\";
            const size_t header_size = header_files.size();
            const size_t source_size = source_files.size();
            size_t insert_at = std::string::npos;

            if (p_name.find(header_files) != std::string::npos) {
                insert_at = header_size;
            } else if (p_name.find(source_files) != std::string::npos) {
                insert_at = source_size;
            }

            if (insert_at != std::string::npos) {
                p_name.erase(insert_at, module_pos - insert_at);
                patched = true;
            }
        }
    }

    for (const std::string &directory : directories) {
        const std::string header_path = "Header Files\\" + directory;
        const std::string source_path = "Source Files\\" + directory;

        if (p_name.find(header_path) != std::string::npos) {
            p_name.replace(0, header_path.size(), "Header Files\\godot\\" + directory);
            patched = true;
        }

        if (p_name.find(source_path) != std::string::npos) {
            p_name.replace(0, source_path.size(), "Source Files\\godot\\" + directory);
            patched = true;
        }
    }

    return patched;
}

void Patcher::patch_solution() {

    pugi::xml_node filters = document.first_child().first_child();

    add_filter(filters, "Header Files");
    add_filter(filters, "Source Files");

    for (pugi::xml_node item_group = filters; item_group; item_group = item_group.next_sibling()) {
        for (pugi::xml_node node : item_group.children()) {
            const std::string name = node.name();
            std::string path;
            if (name == "Filter") {
                path = node.first_attribute().value();
                if (patch_name(path)) {
                    node.first_attribute().set_value(path.c_str());
                    std::cout << "Patched " << path << "\n";
                }
            } else if (name == "ClInclude" || name == "ClCompile") {
                path = node.first_child().first_child().value();
                if (patch_name(path)) {
                    node.first_child().first_child().set_value(path.c_str());
                    std::cout << "Patched " << path << "\n";
                }
            }
        }
    }
}
