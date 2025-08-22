#pragma once

#include <string>
#include <vector>

namespace Macro {
	std::vector<std::string> process(const std::vector<std::string>& lines);
	std::vector<std::string> ProcessFromFile(std::string path);
}