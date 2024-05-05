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

#include "version.h"
#include "patcher.h"
#include "uuid.h"

void Patcher::patch() const {
    const std::vector<std::string> dirs = { "core", "drivers", "editor", "main", "misc", "modules", "platform", "scene", "servers", "tests", "thirdparty" };
    const std::string solution_name = SOLUTION_NAME ".vcxproj.filters";
    std::string solution_path;
    
    std::cout << "Running Patcher version " << VERSION_STRING << ".\n";

    if (find_solution(fs::current_path(), solution_name, solution_path)) {
        std::cout << "Unable to find solution \'" << solution_name << "\'\n";
    } else {
        std::cout << "Found solution \'" << solution_path << "\'\n";
    }

    pugi::xml_document xml;
    if (!xml.load_file(solution_path.c_str())) {
        std::cout << "Unable to load solution\n";
        return;
    }

    patch_solution(xml, dirs, MODULE_NAME);

    if (!xml.save_file(solution_path.c_str())) {
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

void Patcher::insert_filter(pugi::xml_node p_node, const std::string &p_name) const {
    std::string filter_name = p_name + "\\godot";
    for (pugi::xml_node sibling : p_node.children()) {
        if (sibling.first_attribute().value() == filter_name) return;
    }
    for (pugi::xml_node sibling : p_node.children()) {
        if (sibling.first_attribute().value() == p_name) {
            auto filter = sibling.parent().insert_child_after("Filter", sibling);
            filter.append_attribute("Include").set_value(filter_name.c_str());
            filter.append_child("UniqueIdentifier").append_child(pugi::node_pcdata).set_value(generate_uuid().c_str());
            break;
        }
    }
}

bool Patcher::patch_path(std::string &p_name, const std::string &p_dir_type, const std::vector<std::string> &p_directories) const {
    for (const auto &dir : p_directories) {
        std::string path = p_dir_type + "\\" + dir;
        if (p_name.find(path) != std::string::npos) {
            p_name.replace(0, path.size(), p_dir_type + "\\godot\\" + dir);
            std::cout << "Patched " << p_name << "\n";
            return true;
        }
    }
    return false;
}

bool Patcher::patch_name(std::string &p_name, const std::vector<std::string> &p_directories) const {
    return patch_path(p_name, "Header Files", p_directories) || patch_path(p_name, "Source Files", p_directories);
}

bool Patcher::patch_module_name(std::string &p_name, const std::string &p_module_name) const {
    size_t module_pos = p_name.find(p_module_name);
    if (module_pos == std::string::npos || p_name.find("modules\\") == std::string::npos) return false;
    size_t insert_at = std::string::npos;
    if (p_name.find("Header Files\\") != std::string::npos) insert_at = std::string("Header Files\\").size();
    else if (p_name.find("Source Files\\") != std::string::npos) insert_at = std::string("Source Files\\").size();
    else return false;
    p_name.erase(insert_at, module_pos - insert_at);
    std::cout << "Patched " << p_name << "\n";
    return true;
}

void Patcher::patch_solution(pugi::xml_document &p_xml, const std::vector<std::string> &p_directories, const std::string &p_module_name) const {
    std::cout << "Patching solution......\n";
    pugi::xml_node filters = p_xml.first_child().first_child();
    insert_filter(filters, "Header Files");
    insert_filter(filters, "Source Files");
    for (pugi::xml_node item_group = filters; item_group; item_group = item_group.next_sibling()) {
        for (pugi::xml_node node : item_group.children()) {
            std::string name = node.name(), path;
            if (name == "Filter") {
                path = node.first_attribute().value();
                if (patch_name(path, p_directories)) node.first_attribute().set_value(path.c_str());
            } else if (name == "ClInclude" || name == "ClCompile") {
                path = node.first_child().first_child().value();
                if (patch_name(path, p_directories)) node.first_child().first_child().set_value(path.c_str());
            }
        }
    }
    for (pugi::xml_node item_group = filters; item_group; item_group = item_group.next_sibling()) {
        for (pugi::xml_node node : item_group.children()) {
            std::string name = node.name(), path;
            if (name == "Filter") {
                path = node.first_attribute().value();
                if (patch_module_name(path, p_module_name)) node.first_attribute().set_value(path.c_str());
            } else if (name == "ClInclude" || name == "ClCompile") {
                path = node.first_child().first_child().value();
                if (patch_module_name(path, p_module_name)) node.first_child().first_child().set_value(path.c_str());
            }
        }
    }
}
