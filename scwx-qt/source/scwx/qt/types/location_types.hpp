#pragma once

#include <scwx/util/iterator.hpp>

#include <string>

namespace scwx
{
namespace qt
{
namespace types
{

enum class LocationMethod
{
   Fixed,
   Track,
   RadarSite,
   County,
   WFO,
   All,
   Unknown
};
typedef scwx::util::
   Iterator<LocationMethod, LocationMethod::Fixed, LocationMethod::All>
      LocationMethodIterator;

enum class PositioningPlugin
{
   Default,
   Nmea,
   Unknown
};
typedef scwx::util::Iterator<PositioningPlugin,
                             PositioningPlugin::Default,
                             PositioningPlugin::Nmea>
   PositioningPluginIterator;

LocationMethod     GetLocationMethod(const std::string& name);
const std::string& GetLocationMethodName(LocationMethod locationMethod);

PositioningPlugin GetPositioningPlugin(const std::string& name);
const std::string&
GetPositioningPluginName(PositioningPlugin positioningPlugin);

} // namespace types
} // namespace qt
} // namespace scwx
