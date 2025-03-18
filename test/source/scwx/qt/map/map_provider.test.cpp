#include <scwx/qt/map/map_provider.hpp>
#include <scwx/qt/settings/general_settings.hpp>
#include <scwx/qt/util/maplibre.hpp>
#include <scwx/util/environment.hpp>
#include <scwx/util/logger.hpp>

#include <QCoreApplication>
#include <QTimer>

#include <gtest/gtest.h>
#include <qmaplibre.hpp>
#include <re2/re2.h>

namespace scwx
{
namespace qt
{
namespace map
{

static const std::string logPrefix_ {"scwx::qt::map::map_provider.test"};
static const auto        logger_ = scwx::util::Logger::Create(logPrefix_);

class ByMapProviderTest :
    public testing::TestWithParam<std::pair<MapProvider, std::string>>
{
};

TEST_P(ByMapProviderTest, MapProviderLayers)
{
   auto& [mapProvider, apiKeyName] = GetParam();

   // Configure API key
   std::string apiKey = scwx::util::GetEnvironment(apiKeyName);
   if (apiKey.empty())
   {
      logger_->info("API key not set, skipping test");
      EXPECT_EQ(true, true);
      return;
   }

   // Setup QCoreApplication
   int              argc   = 1;
   const char*      argv[] = {"arg", nullptr};
   QCoreApplication application(argc, const_cast<char**>(argv));

   // Configure map provider
   const MapProviderInfo& mapProviderInfo = GetMapProviderInfo(mapProvider);

   // Configure QMapLibre
   QMapLibre::Settings mapSettings {};
   if (mapProvider == map::MapProvider::Mapbox)
   {
      mapSettings.setProviderTemplate(mapProviderInfo.providerTemplate_);
      mapSettings.setApiKey(QString::fromStdString(apiKey));
   }
   else if (mapProvider == map::MapProvider::MapTiler)
   {
      settings::GeneralSettings::Instance().maptiler_api_key().SetValue(apiKey);
   }

   std::shared_ptr<map::MapContext> mapContext =
      std::make_shared<map::MapContext>();
   std::shared_ptr<QMapLibre::Map> map =
      std::make_shared<QMapLibre::Map>(nullptr, mapSettings, QSize(1, 1));
   mapContext->set_map(map);
   mapContext->set_map_provider(mapProvider);
   application.processEvents();

   // Connect style load completion signal
   QObject::connect(
      map.get(),
      &QMapLibre::Map::mapChanged,
      [&](QMapLibre::Map::MapChange mapChange)
      {
         if (mapChange ==
             QMapLibre::Map::MapChange::MapChangeDidFinishLoadingStyle)
         {
            application.exit();
         }
      });

   // Connect timeout timer
   bool   timeout = false;
   QTimer timeoutTimer {};
   timeoutTimer.setSingleShot(true);
   QObject::connect(&timeoutTimer,
                    &QTimer::timeout,
                    [&]()
                    {
                       // Reached timeout
                       logger_->warn("Timed out waiting for style change");
                       timeout = true;

                       application.exit();
                    });

   // Iterate through each style
   for (const auto& mapStyle : mapProviderInfo.mapStyles_)
   {
      using namespace std::chrono_literals;

      // Load style
      timeout = false;
      util::maplibre::SetMapStyleUrl(mapContext, mapStyle.url_);
      timeoutTimer.start(5000ms);
      application.exec();
      timeoutTimer.stop();

      // Check result
      if (!timeout)
      {
         // Print layer names for debug
         std::string layerIdsString = map->layerIds().join(", ").toStdString();
         logger_->debug("{} Layers: [{}]", mapStyle.name_, layerIdsString);

         // Search layer list
         bool foundMatch = false;
         for (const QString& qlayer : map->layerIds())
         {
            const std::string layer = qlayer.toStdString();

            // Draw below layers defined in map style
            auto it =
               std::find_if(mapStyle.drawBelow_.cbegin(),
                            mapStyle.drawBelow_.cend(),
                            [&layer](const std::string& styleLayer) -> bool
                            {
                               // Perform case insensitive matching
                               RE2 re {"(?i)" + styleLayer};
                               return RE2::FullMatch(layer, re);
                            });

            if (it != mapStyle.drawBelow_.cend())
            {
               foundMatch = true;
               break;
            }
         }

         // Check match
         EXPECT_EQ(foundMatch, true);

         if (!foundMatch)
         {
            logger_->error("Could not find drawBelow in style {}",
                           mapStyle.name_);
         }
      }
      else
      {
         EXPECT_EQ(timeout, false);
      }
   }

   EXPECT_EQ(false, false);
}

INSTANTIATE_TEST_SUITE_P(
   MapProviderTest,
   ByMapProviderTest,
   testing::Values(std::make_pair(MapProvider::Mapbox, "MAPBOX_API_KEY"),
                   std::make_pair(MapProvider::MapTiler, "MAPTILER_API_KEY")));

} // namespace map
} // namespace qt
} // namespace scwx
