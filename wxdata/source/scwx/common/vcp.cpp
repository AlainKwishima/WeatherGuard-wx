#include <scwx/common/vcp.hpp>

#include <unordered_map>

namespace scwx
{
namespace common
{

static const std::string CLEAR_AIR_MODE     = "Clear Air Mode";
static const std::string PRECIPITATION_MODE = "Precipitation Mode";

std::string GetVcpDescription(uint16_t vcp)
{
   switch (vcp)
   {
   case 31:
   case 32:
   case 35:
   case 90:
      return CLEAR_AIR_MODE;

   case 11:
   case 12:
   case 21:
   case 80:
   case 112:
   case 121:
   case 211:
   case 212:
   case 215:
   case 221:
      return PRECIPITATION_MODE;

   default:
      return "?";
   }
}

} // namespace common
} // namespace scwx
