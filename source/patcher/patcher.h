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

    void patch() const;

private:
    bool find_solution(const fs::path &p_path, const std::string &p_solution_name, std::string &p_result) const;
    void insert_filter(pugi::xml_node p_node, const std::string &p_name) const;
    bool patch_path(std::string &p_name, const std::string &dir_type, const std::vector<std::string> &directories) const;
    bool patch_name(std::string &p_name, const std::vector<std::string> &directories) const;
    bool patch_module_name(std::string &p_name, const std::string &module_name) const;
    void patch_solution(pugi::xml_document &xml, const std::vector<std::string> &directories, const std::string &module_name) const;
};