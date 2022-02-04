#pragma once

#include <boost/json.hpp>

#include "Core/Core.h"

namespace JSON {
    boost::json::value Import(std::string_view fileName);
}
