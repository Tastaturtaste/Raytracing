#pragma once
#include <fstream>
#include "Scene.h"

namespace Tracer {
Scene loadScene(std::istream& obj_file);
} // Tracer
