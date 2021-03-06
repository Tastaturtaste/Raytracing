#pragma once
#include <array>
#include "norm3.h"

namespace Samples {

	norm3 hemispheresampleCosWeighted(const std::array<norm3,3>& basis, double u, double v) noexcept;

	//norm3 hemispheresampleUniform(norm3 normal, double aroundX, double aroundZ) noexcept;

	norm3 conesampleCosWeighted(const std::array<norm3, 3>& basis, double coneAngle, double u, double v) noexcept;
}