#include "fmt/core.h"
#include "glm/glm.hpp"
#include <fmt/format.h>

template <class T>
requires requires{
    glm::vec<3,T>{};
}
struct fmt::formatter<glm::vec<3,T>> {
    using Tvec = glm::vec<3,T>;
  // Presentation format: 'f' - fixed, 'e' - exponential, 's' - polar.
  char presentation = 'f';

  // Parses format specifications of the form ['f' | 'e' | 's'].
  constexpr auto parse(format_parse_context& ctx) {
     // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e' || *it == 's')) presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}')
      throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the vec using the parsed format specification (presentation)
  // stored in this formatter.
  template <typename FormatContext>
  auto format(const Tvec& vec, FormatContext& ctx) {
    // ctx.out() is an output iterator to write to.
    const char* fmtstring = [&](){
        if(presentation == 'f' || presentation == 's'){
            return "({:.1f}, {:.1f}, {:.1f})";
        }else{
            return "({:.1e}, {:.1e}, {:.1e})";
        }
    }();

    if(presentation == 's'){
        Tvec svec = glm::normalize(vec);
        svec = Tvec{
            glm::degrees(glm::acos(glm::dot(Tvec(0,1,0),svec))),
            glm::degrees(glm::atan(svec.z / svec.x)),
            glm::length(vec)
            };

        return format_to(
        ctx.out(),
        fmtstring,
        svec.x, svec.y, svec.z);
    }
    return format_to(
        ctx.out(),
        fmtstring,
        vec.x, vec.y, vec.z);
  }
};