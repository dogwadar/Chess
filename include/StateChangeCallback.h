#pragma once

#include <functional>
#include "GameState.h"

using StateChangeCallback = std::function<void(GameState)>;
