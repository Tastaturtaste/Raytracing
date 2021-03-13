#include "fmt/core.h"
#include <fmt/format.h>
#include "glm/glm.hpp"
#include <span>
#include <numeric>
#include <array>
#include <compare>

struct Plane{
    // A point on a plane and a normal on that plane
    glm::vec3 point;
    glm::vec3 normal;
};

constexpr inline std::weak_ordering cmpToPlane(Plane const& plane, glm::vec3 vertex) noexcept {
    // Returns a weak ordering of the vertex to the plane (above, on, below)
    auto const res = glm::dot(vertex - plane.point,plane.normal);
    return res > 0.f ? std::weak_ordering::greater: res < 0.f ? std::weak_ordering::less : std::weak_ordering::equivalent;
}

constexpr inline std::partial_ordering cmpToPlane(Plane const& plane, std::span<glm::vec3> vertices){
    // Returns a partial ordering of the vertices in relation to the plane (above, on, below, unordered)
    if (vertices.size() == 0){
        throw std::runtime_error("Number of vertices has to be greater than zero to determine a ordering!");
    }
    // return std::transform_reduce(
    //     std::next(vertices.begin()), std::end(vertices),
    //     static_cast<std::partial_ordering>(cmpToPlane(plane,vertices[0])),
    //     [](std::partial_ordering const& d1, std::partial_ordering const& d2) -> std::partial_ordering { 
    //         return d1 == d2 ? d1 : std::partial_ordering::unordered; },
    //     [&plane](glm::vec3 const& vec) -> std::partial_ordering { return cmpToPlane(plane,vec); }
    // );
    auto d{cmpToPlane(plane,vertices[0])};
    for(auto it = std::next(vertices.begin()); it != vertices.end(); ++it){
        if(cmpToPlane(plane,*it) != d){
            return std::partial_ordering::unordered;
        }
    }
    return d;
}

int main(){
    std::array<glm::vec3,4> vertices{
        glm::vec3(1,2,3), glm::vec3(3,2,1), glm::vec3(4,2,4), glm::vec3(1,1,1)
        };
    Plane plane{glm::vec3(0,0,0), glm::vec3(1,0,0)};
    auto res = cmpToPlane(plane,vertices);
    fmt::print("{}", res == 0 ? 0 : res < 0 ? -1 : 1);
}