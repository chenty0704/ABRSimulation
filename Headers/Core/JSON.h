#pragma once

#include <boost/json.hpp>

#include "Core/Core.h"

namespace JSON {
    [[nodiscard]] boost::json::value Import(std::string_view fileName);
}
