#pragma once

#include <vector>

#include "body.h"

class CollisionResolver {
public:
  CollisionResolver();


private:
  std::vector<bool[2]> marked;
}
