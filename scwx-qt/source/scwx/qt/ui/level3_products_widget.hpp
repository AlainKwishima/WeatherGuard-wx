#pragma once

#include <scwx/common/products.hpp>

#include <QWidget>

namespace scwx
{
namespace qt
{
namespace ui
{

class Level3ProductsWidgetImpl;

class Level3ProductsWidget : public QWidget
{
   Q_OBJECT

public:
   explicit Level3ProductsWidget(QWidget* parent = nullptr);
   ~Level3ProductsWidget();

   void showEvent(QShowEvent* event) override;

   void UpdateAvailableProducts(
      const common::Level3ProductCategoryMap& updatedCategoryMap);
   void UpdateProductSelection(common::RadarProductGroup group,
                               const std::string&        productName);

signals:
   void RadarProductSelected(common::RadarProductGroup group,
                             const std::string&        productName,
                             int16_t                   productCode);

private:
   std::shared_ptr<Level3ProductsWidgetImpl> p;
};

} // namespace ui
} // namespace qt
} // namespace scwx
