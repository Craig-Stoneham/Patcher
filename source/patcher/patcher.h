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

#pragma once

#include "typedefs.h"

class Patcher {
public:
	Patcher() = default;
	~Patcher() = default;

    void add_module(const std::string &p_module);

    void patch();

private:
    bool find_solution(const fs::path &p_path, const std::string &p_solution_name, std::string &p_result) const;
    void add_filter(pugi::xml_node p_node, const std::string &p_name);
    bool patch_name(std::string &p_name) const;
    void patch_solution();

private:
    const std::vector<std::string> directories = { "core", "drivers", "editor", "main", "misc", "modules", "platform", "scene", "servers", "tests", "thirdparty" };
    const std::string solution_name = "godot.vcxproj.filters";
    std::vector<std::string> modules;
    pugi::xml_document document;
};