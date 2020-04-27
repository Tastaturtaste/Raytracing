#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "ctre.hpp"

namespace Tracer {
	static constexpr ctll::fixed_string tokenRe{ R"(\s*((#.*)|[^ \t\n\r#]+))" };

    template <typename F>
    void parse(std::istream& in, F&& handler) {
        std::string lineAsString;
        int lineNumber = 0;
        while (std::getline(in, lineAsString)) {
            lineNumber++;
            std::string_view line(lineAsString);

            std::vector<std::string_view> fields;
            for (auto match : ctre::range<tokenRe>(line)) {
                if (!match)
                    break;
                auto view = match.get<1>().to_view();
                if (view[0] != '#')
                    fields.emplace_back(view);
            }

            if (fields.empty())
                continue;

            auto command = fields.front();
            fields.erase(fields.begin());
            if (!handler(command, fields)) {
                throw std::runtime_error("Unknown directive '" + std::string(command)
                    + "' on line " + std::to_string(lineNumber));
            }
        }
    }
}
