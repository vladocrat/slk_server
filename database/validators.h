#pragma once

#include <string_view>

namespace slk
{

class EmailValidatior
{
public:
    bool operator()(const std::string_view);
};

} //! slk
