#include <scwx/qt/types/qt_types.hpp>
#include <scwx/util/enum.hpp>

#include <boost/algorithm/string.hpp>

namespace scwx
{
namespace qt
{
namespace types
{

static const std::unordered_map<UiStyle, std::string> qtStyleName_ {
   {UiStyle::Default, "Default"},
   {UiStyle::Fusion, "Fusion"},
   {UiStyle::FusionLight, "Fusion"},
   {UiStyle::FusionDark, "Fusion"},
   {UiStyle::FusionAiry, "Fusion"},
   {UiStyle::FusionDarker, "Fusion"},
   {UiStyle::FusionDusk, "Fusion"},
   {UiStyle::FusionIaOra, "Fusion"},
   {UiStyle::FusionSand, "Fusion"},
   {UiStyle::FusionWaves, "Fusion"},
   {UiStyle::FusionCustom, "Fusion"},
   {UiStyle::Unknown, "?"}};

static const std::unordered_map<UiStyle, std::string> uiStyleName_ {
   {UiStyle::Default, "Default"},
   {UiStyle::Fusion, "Fusion"},
   {UiStyle::FusionLight, "Fusion Light"},
   {UiStyle::FusionDark, "Fusion Dark"},
   {UiStyle::FusionAiry, "Fusion Airy"},
   {UiStyle::FusionDarker, "Fusion Darker"},
   {UiStyle::FusionDusk, "Fusion Dusk"},
   {UiStyle::FusionIaOra, "Fusion IA Ora"},
   {UiStyle::FusionSand, "Fusion Sand"},
   {UiStyle::FusionWaves, "Fusion Waves"},
   {UiStyle::FusionCustom, "Fusion Custom"},
   {UiStyle::Unknown, "?"}};

static const std::unordered_map<UiStyle, Qt::ColorScheme> qtColorSchemeMap_ {
   {UiStyle::Default, Qt::ColorScheme::Unknown},
   {UiStyle::Fusion, Qt::ColorScheme::Unknown},
   {UiStyle::FusionLight, Qt::ColorScheme::Light},
   {UiStyle::FusionDark, Qt::ColorScheme::Dark},
   {UiStyle::FusionAiry, Qt::ColorScheme::Unknown},
   {UiStyle::FusionDarker, Qt::ColorScheme::Unknown},
   {UiStyle::FusionDusk, Qt::ColorScheme::Unknown},
   {UiStyle::FusionIaOra, Qt::ColorScheme::Unknown},
   {UiStyle::FusionSand, Qt::ColorScheme::Unknown},
   {UiStyle::FusionWaves, Qt::ColorScheme::Unknown},
   {UiStyle::FusionCustom, Qt::ColorScheme::Unknown},
   {UiStyle::Unknown, Qt::ColorScheme::Unknown}};

static const std::unordered_map<UiStyle, std::string> paletteFile_ {
   {UiStyle::FusionAiry, ":res/qt6ct_colors/airy.conf"},
   {UiStyle::FusionDarker, ":res/qt6ct_colors/darker.conf"},
   {UiStyle::FusionDusk, ":res/qt6ct_colors/dusk.conf"},
   {UiStyle::FusionIaOra, ":res/qt6ct_colors/ia_ora.conf"},
   {UiStyle::FusionSand, ":res/qt6ct_colors/sand.conf"},
   {UiStyle::FusionWaves, ":res/qt6ct_colors/waves.conf"}};

SCWX_GET_ENUM(UiStyle, GetUiStyle, uiStyleName_)

Qt::ColorScheme GetQtColorScheme(UiStyle uiStyle)
{
   return qtColorSchemeMap_.at(uiStyle);
}

std::string GetQtStyleName(UiStyle uiStyle)
{
   return qtStyleName_.at(uiStyle);
}

std::optional<std::string> GetQtPaletteFile(UiStyle uiStyle)
{
   if (paletteFile_.contains(uiStyle))
   {
      return paletteFile_.at(uiStyle);
   }

   return std::nullopt;
}

std::string GetUiStyleName(UiStyle uiStyle)
{
   return uiStyleName_.at(uiStyle);
}

} // namespace types
} // namespace qt
} // namespace scwx
