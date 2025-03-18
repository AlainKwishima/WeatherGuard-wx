#pragma once

#include <scwx/common/products.hpp>
#include <scwx/qt/manager/radar_product_manager.hpp>
#include <scwx/qt/view/radar_product_view.hpp>

#include <string>

namespace scwx
{
namespace qt
{
namespace view
{

class RadarProductViewFactory
{
private:
   explicit RadarProductViewFactory() = delete;
   ~RadarProductViewFactory()         = delete;

   RadarProductViewFactory(const RadarProductViewFactory&) = delete;
   RadarProductViewFactory& operator=(const RadarProductViewFactory&) = delete;

   RadarProductViewFactory(RadarProductViewFactory&&) noexcept = delete;
   RadarProductViewFactory&
   operator=(RadarProductViewFactory&&) noexcept = delete;

public:
   static std::shared_ptr<RadarProductView>
   Create(common::RadarProductGroup                     productGroup,
          const std::string&                            productName,
          int16_t                                       productCode,
          std::shared_ptr<manager::RadarProductManager> radarProductManager);
   static std::shared_ptr<RadarProductView>
   Create(common::Level2Product                         product,
          std::shared_ptr<manager::RadarProductManager> radarProductManager);
};

} // namespace view
} // namespace qt
} // namespace scwx
