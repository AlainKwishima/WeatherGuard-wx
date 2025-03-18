#include "imgui_debug_dialog.hpp"
#include "ui_imgui_debug_dialog.h"

#include <scwx/qt/model/imgui_context_model.hpp>
#include <scwx/qt/types/qt_types.hpp>
#include <scwx/qt/ui/imgui_debug_widget.hpp>

namespace scwx
{
namespace qt
{
namespace ui
{

static const std::string logPrefix_ = "scwx::qt::ui::imgui_debug_dialog";

class ImGuiDebugDialogImpl
{
public:
   explicit ImGuiDebugDialogImpl() : imGuiDebugWidget_ {nullptr} {}
   ~ImGuiDebugDialogImpl() = default;

   ImGuiDebugWidget* imGuiDebugWidget_;
};

ImGuiDebugDialog::ImGuiDebugDialog(QWidget* parent) :
    QDialog(parent),
    p {std::make_unique<ImGuiDebugDialogImpl>()},
    ui(new Ui::ImGuiDebugDialog)
{
   ui->setupUi(this);

   // ImGui Debug Widget
   p->imGuiDebugWidget_ = new ImGuiDebugWidget(this);
   p->imGuiDebugWidget_->setSizePolicy(QSizePolicy::Policy::Expanding,
                                       QSizePolicy::Policy::Expanding);
   ui->verticalLayout->insertWidget(0, p->imGuiDebugWidget_);

   // Context Combo Box
   auto& contextModel = model::ImGuiContextModel::Instance();
   auto  index = contextModel.IndexOf(p->imGuiDebugWidget_->context_name());
   ui->contextComboBox->setModel(&contextModel);
   ui->contextComboBox->setCurrentIndex(index.row());

   connect(
      ui->contextComboBox,
      &QComboBox::currentIndexChanged,
      [this](int row)
      {
         auto& contextModel = model::ImGuiContextModel::Instance();
         auto  index        = contextModel.index(row, 0);
         if (index.isValid())
         {
            QVariant data = contextModel.data(index, qt::types::RawDataRole);
            auto     contextInfo = data.value<model::ImGuiContextInfo>();

            p->imGuiDebugWidget_->set_current_context(contextInfo.context_);
         }
      });
}

ImGuiDebugDialog::~ImGuiDebugDialog()
{
   delete ui;
}

} // namespace ui
} // namespace qt
} // namespace scwx
