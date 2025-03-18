#include <scwx/qt/types/time_types.hpp>
#include <scwx/util/enum.hpp>

#include <unordered_map>

#include <boost/algorithm/string.hpp>

namespace scwx
{
namespace qt
{
namespace types
{

static const std::unordered_map<DefaultTimeZone, std::string>
   defaultTimeZoneName_ {{DefaultTimeZone::Local, "Local"},
                         {DefaultTimeZone::Radar, "Radar"},
                         {DefaultTimeZone::UTC, "UTC"},
                         {DefaultTimeZone::Unknown, "?"}};

SCWX_GET_ENUM(DefaultTimeZone, GetDefaultTimeZone, defaultTimeZoneName_)

const std::string& GetDefaultTimeZoneName(DefaultTimeZone timeZone)
{
   return defaultTimeZoneName_.at(timeZone);
}

} // namespace types
} // namespace qt
} // namespace scwx
