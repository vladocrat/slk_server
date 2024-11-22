#include "validators.h"

namespace slk
{

bool EmailValidatior::operator()(const std::string_view email)
{
    // Locate '@'
    auto at = std::find(email.begin(), email.end(), '@');
    // Locate '.' after '@'
    auto dot = std::find(at, email.end(), '.');
    // make sure both characters are present
    return (at != email.end()) && (dot != email.end());
}

} //! slk
