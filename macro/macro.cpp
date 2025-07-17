#include "macro.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace {
    struct macro_t {
        std::string name;
        std::vector<std::string> args;
        std::vector<std::string> body;
    };
    
    std::vector<std::string> tokenize_args(const std::string& line) {
        std::vector<std::string> tokens;
        size_t l = line.find('(');
        size_t r = line.find(')');
        if (l == std::string::npos || r == std::string::npos || r <= l) return tokens;

        std::string args = line.substr(l + 1, r - l - 1);
        std::istringstream iss(args);
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

    macro_t* get_macro(const std::string& name, std::vector<macro_t>& macros) { // search macro by name
        for (auto& m : macros) {
            if (m.name == name) return &m;
        }
        return nullptr;
    }

    std::vector<std::string> expand_macro(const std::vector<std::string>& X, const macro_t& macro) {
        std::vector<std::string> n_content = macro.body;
        for (size_t i = 0; i < X.size(); ++i) {
            for (auto& line : n_content) {
                size_t pos = 0;
                while ((pos = line.find(macro.args[i], pos)) != std::string::npos) {
                    line.replace(pos, macro.args[i].length(), X[i]);
                    pos += X[i].length();
                }
            }
        }
        return n_content;
    }

    std::vector<std::string> solve_macro(const std::string& line, std::vector<macro_t>& macros) {
        std::string name = line.substr(0, line.find('('));
        macro_t* m = get_macro(name, macros);
        if (!m) throw std::runtime_error("Macro '" + name + "' not defined");
        return expand_macro(tokenize_args(line), *m);
    }
}

namespace Macro {
    std::vector<std::string> process(const std::vector<std::string>& lines) {
        std::vector<std::string> n_lines;
        std::vector<macro_t> macros;

        macro_t n_macro;
        bool active = false;
        size_t arg_i = 0;

        for (auto line_raw : lines) {
            std::string line = line_raw;

            if (line.empty() || line.substr(0, 2) == "//") continue; // ignore comment

            std::istringstream iss(line);
            std::string first_word;
            iss >> first_word;

            if (active) { // defining macro
                if (first_word == ".end_macro") { // end of macro
                    active = false;

                    // fix argument name
                    for (size_t i = 0; i < n_macro.args.size(); ++i) {
                        std::string old_arg = n_macro.args[i];
                        std::string new_arg = "%" + std::to_string(arg_i);
                        n_macro.args[i] = new_arg;
                        for (auto& b_line : n_macro.body) {
                            size_t pos = 0;
                            while ((pos = b_line.find(old_arg, pos)) != std::string::npos) {
                                b_line.replace(pos, old_arg.length(), new_arg);
                                pos += new_arg.length();
                            }
                        }
                        arg_i++;
                    }

                    // add new macro to macro list
                    macros.push_back(n_macro);

                    // reset n_macro
                    n_macro.body.clear();
                    n_macro.args.clear();
                } else { // macro body
                    if (first_word.find('(') != std::string::npos) { // macro call inside macro body
                        std::vector<std::string> expanded = solve_macro(line, macros);
                        n_macro.body.insert(n_macro.body.end(), expanded.begin(), expanded.end());
                    } else { // normal line
                        n_macro.body.push_back(line);
                    }
                }
            } else { // not defining macro
                if (first_word == ".macro") { // open macro definition
                    active = true;
                    iss >> n_macro.name;
                    std::string arg;
                    while (iss >> arg) {
                        n_macro.args.push_back(arg);
                    }
                } else { // normal line
                    if (first_word.find('(') != std::string::npos) { // macro call
                        auto expanded = solve_macro(line, macros);
                        n_lines.insert(n_lines.end(), expanded.begin(), expanded.end());
                    } else { // normal line
                        n_lines.push_back(line);
                    }
                }
            }
        }

        return n_lines;
    }
}