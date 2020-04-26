#pragma once
#include "Norm3.h"

namespace samples {

	Norm3 hemispheresampleCosWeighted(Norm3 normal, double u, double v) noexcept;

	Norm3 hemispheresampleUniform(Norm3 normal, double aroundX, double aroundZ) noexcept;

	Norm3 conesampleCosWeighted(Norm3 normal, double coneAngle, double u, double v) noexcept;
}