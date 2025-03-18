#include "placefile_settings_widget.hpp"
#include "ui_placefile_settings_widget.h"

#include <scwx/qt/manager/placefile_manager.hpp>
#include <scwx/qt/model/placefile_model.hpp>
#include <scwx/qt/types/qt_types.hpp>
#include <scwx/qt/ui/open_url_dialog.hpp>
#include <scwx/util/logger.hpp>

#include <QSortFilterProxyModel>

namespace scwx
{
namespace qt
{
namespace ui
{

static const std::string logPrefix_ = "scwx::qt::ui::placefile_settings_widget";
static const auto        logger_    = scwx::util::Logger::Create(logPrefix_);

class PlacefileSettingsWidgetImpl
{
public:
   explicit PlacefileSettingsWidgetImpl(PlacefileSettingsWidget* self) :
       self_ {self},
       openUrlDialog_ {new OpenUrlDialog(QObject::tr("Add Placefile"), self_)},
       placefileModel_ {new model::PlacefileModel(self_)},
       placefileProxyModel_ {new QSortFilterProxyModel(self_)}
   {
      placefileProxyModel_->setSourceModel(placefileModel_);
      placefileProxyModel_->setSortRole(types::ItemDataRole::SortRole);
      placefileProxyModel_->setFilterCaseSensitivity(
         Qt::CaseSensitivity::CaseInsensitive);
      placefileProxyModel_->setFilterKeyColumn(-1);
   }
   ~PlacefileSettingsWidgetImpl() = default;

   void ConnectSignals();

   PlacefileSettingsWidget* self_;
   OpenUrlDialog*           openUrlDialog_;

   std::shared_ptr<manager::PlacefileManager> placefileManager_ {
      manager::PlacefileManager::Instance()};

   model::PlacefileModel* placefileModel_;
   QSortFilterProxyModel* placefileProxyModel_;
};

PlacefileSettingsWidget::PlacefileSettingsWidget(QWidget* parent) :
    QFrame(parent),
    p {std::make_unique<PlacefileSettingsWidgetImpl>(this)},
    ui(new Ui::PlacefileSettingsWidget)
{
   ui->setupUi(this);

   ui->removeButton->setEnabled(false);
   ui->refreshButton->setEnabled(false);

   ui->placefileView->setModel(p->placefileProxyModel_);

   auto placefileViewHeader = ui->placefileView->header();

   placefileViewHeader->setMinimumSectionSize(10);
   placefileViewHeader->setSortIndicator(
      static_cast<int>(model::PlacefileModel::Column::Placefile),
      Qt::AscendingOrder);

   // Enabled and Thresholds columns have a fixed size (checkbox)
   placefileViewHeader->setSectionResizeMode(
      static_cast<int>(model::PlacefileModel::Column::Enabled),
      QHeaderView::ResizeMode::ResizeToContents);
   placefileViewHeader->setSectionResizeMode(
      static_cast<int>(model::PlacefileModel::Column::Thresholds),
      QHeaderView::ResizeMode::ResizeToContents);

   p->ConnectSignals();
}

PlacefileSettingsWidget::~PlacefileSettingsWidget()
{
   delete ui;
}

void PlacefileSettingsWidgetImpl::ConnectSignals()
{
   QObject::connect(self_->ui->addButton,
                    &QPushButton::clicked,
                    self_,
                    [this]() { openUrlDialog_->open(); });

   QObject::connect(self_->ui->removeButton,
                    &QPushButton::clicked,
                    self_,
                    [this]()
                    {
                       auto selectionModel =
                          self_->ui->placefileView->selectionModel();

                       // Get selected URL string
                       QModelIndex selected =
                          selectionModel
                             ->selectedRows(static_cast<int>(
                                model::PlacefileModel::Column::Placefile))
                             .first();
                       QVariant data = self_->ui->placefileView->model()->data(
                          selected, types::ItemDataRole::SortRole);
                       std::string urlString = data.toString().toStdString();

                       // Remove Placefile
                       if (!urlString.empty())
                       {
                          placefileManager_->RemoveUrl(urlString);
                       }
                    });

   QObject::connect(self_->ui->refreshButton,
                    &QPushButton::clicked,
                    self_,
                    [this]()
                    {
                       auto selectionModel =
                          self_->ui->placefileView->selectionModel();

                       // Get selected URL string
                       QModelIndex selected =
                          selectionModel
                             ->selectedRows(static_cast<int>(
                                model::PlacefileModel::Column::Placefile))
                             .first();
                       QVariant data = self_->ui->placefileView->model()->data(
                          selected, types::ItemDataRole::SortRole);
                       std::string urlString = data.toString().toStdString();

                       // Refresh placefile
                       if (!urlString.empty())
                       {
                          placefileManager_->Refresh(urlString);
                       }
                    });

   QObject::connect(
      openUrlDialog_,
      &OpenUrlDialog::accepted,
      self_,
      [this]()
      { placefileManager_->AddUrl(openUrlDialog_->url().toStdString()); });

   QObject::connect(self_->ui->placefileFilter,
                    &QLineEdit::textChanged,
                    placefileProxyModel_,
                    &QSortFilterProxyModel::setFilterWildcard);

   QObject::connect(
      self_->ui->placefileView->selectionModel(),
      &QItemSelectionModel::selectionChanged,
      self_,
      [this](const QItemSelection& selected, const QItemSelection& deselected)
      {
         if (selected.size() == 0 && deselected.size() == 0)
         {
            // Items which stay selected but change their index are not
            // included in selected and deselected. Thus, this signal might
            // be emitted with both selected and deselected empty, if only
            // the indices of selected items change.
            return;
         }

         bool itemSelected = selected.size() > 0;
         self_->ui->removeButton->setEnabled(itemSelected);
         self_->ui->refreshButton->setEnabled(itemSelected);
      });
}

} // namespace ui
} // namespace qt
} // namespace scwx
