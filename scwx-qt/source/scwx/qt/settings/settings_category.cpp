#include <scwx/qt/settings/settings_category.hpp>
#include <scwx/qt/util/json.hpp>
#include <scwx/util/logger.hpp>

#include <algorithm>

namespace scwx::qt::settings
{

static const std::string logPrefix_ = "scwx::qt::settings::settings_category";
static const auto        logger_    = scwx::util::Logger::Create(logPrefix_);

class SettingsCategory::Impl
{
public:
   explicit Impl(const std::string& name) : name_ {name} {}

   ~Impl()                       = default;
   Impl(const Impl&)             = delete;
   Impl& operator=(const Impl&)  = delete;
   Impl(const Impl&&)            = delete;
   Impl& operator=(const Impl&&) = delete;

   void ConnectSubcategory(SettingsCategory& category);
   void ConnectVariable(SettingsVariableBase* variable);

   const std::string name_;

   std::vector<std::pair<std::string, std::vector<SettingsCategory*>>>
                                      subcategoryArrays_;
   std::vector<SettingsCategory*>     subcategories_;
   std::vector<SettingsVariableBase*> variables_;

   boost::signals2::signal<void()> changedSignal_ {};
   boost::signals2::signal<void()> stagedSignal_ {};
   bool                            blockSignals_ {false};

   std::vector<boost::signals2::scoped_connection> connections_ {};
};

SettingsCategory::SettingsCategory(const std::string& name) :
    p(std::make_unique<Impl>(name))
{
}
SettingsCategory::~SettingsCategory() = default;

SettingsCategory::SettingsCategory(SettingsCategory&&) noexcept = default;
SettingsCategory&
SettingsCategory::operator=(SettingsCategory&&) noexcept = default;

bool SettingsCategory::IsDefault() const
{
   bool isDefault = true;

   // Get subcategory array defaults
   for (auto& subcategoryArray : p->subcategoryArrays_)
   {
      for (auto& subcategory : subcategoryArray.second)
      {
         isDefault = isDefault && subcategory->IsDefault();
      }
   }

   // Get subcategory defaults
   for (auto& subcategory : p->subcategories_)
   {
      isDefault = isDefault && subcategory->IsDefault();
   }

   // Get variable defaults
   for (auto& variable : p->variables_)
   {
      isDefault = isDefault && variable->IsDefault();
   }

   return isDefault;
}

bool SettingsCategory::IsDefaultStaged() const
{
   bool isDefaultStaged = true;

   // Get subcategory array defaults
   for (auto& subcategoryArray : p->subcategoryArrays_)
   {
      for (auto& subcategory : subcategoryArray.second)
      {
         isDefaultStaged = isDefaultStaged && subcategory->IsDefaultStaged();
      }
   }

   // Get subcategory defaults
   for (auto& subcategory : p->subcategories_)
   {
      isDefaultStaged = isDefaultStaged && subcategory->IsDefaultStaged();
   }

   // Get variable defaults
   for (auto& variable : p->variables_)
   {
      isDefaultStaged = isDefaultStaged && variable->IsDefaultStaged();
   }

   return isDefaultStaged;
}

std::string SettingsCategory::name() const
{
   return p->name_;
}

boost::signals2::signal<void()>& SettingsCategory::changed_signal()
{
   return p->changedSignal_;
}

boost::signals2::signal<void()>& SettingsCategory::staged_signal()
{
   return p->stagedSignal_;
}

void SettingsCategory::set_block_signals(bool blockSignals)
{
   p->blockSignals_ = blockSignals;
}

void SettingsCategory::SetDefaults()
{
   // Don't allow individual variables to invoke the signal when operating over
   // the entire category
   p->blockSignals_ = true;

   // Set subcategory array defaults
   for (auto& subcategoryArray : p->subcategoryArrays_)
   {
      for (auto& subcategory : subcategoryArray.second)
      {
         subcategory->SetDefaults();
      }
   }

   // Set subcategory defaults
   for (auto& subcategory : p->subcategories_)
   {
      subcategory->SetDefaults();
   }

   // Set variable defaults
   for (auto& variable : p->variables_)
   {
      variable->SetValueToDefault();
   }

   // Unblock signals
   p->blockSignals_ = false;

   p->changedSignal_();
   p->stagedSignal_();
}

void SettingsCategory::StageDefaults()
{
   // Don't allow individual variables to invoke the signal when operating over
   // the entire category
   p->blockSignals_ = true;

   // Stage subcategory array defaults
   for (auto& subcategoryArray : p->subcategoryArrays_)
   {
      for (auto& subcategory : subcategoryArray.second)
      {
         subcategory->StageDefaults();
      }
   }

   // Stage subcategory defaults
   for (auto& subcategory : p->subcategories_)
   {
      subcategory->StageDefaults();
   }

   // Stage variable defaults
   for (auto& variable : p->variables_)
   {
      variable->StageDefault();
   }

   // Unblock signals
   p->blockSignals_ = false;

   p->stagedSignal_();
}

bool SettingsCategory::Commit()
{
   bool committed = false;

   // Don't allow individual variables to invoke the signal when operating over
   // the entire category
   p->blockSignals_ = true;

   // Commit subcategory arrays
   for (auto& subcategoryArray : p->subcategoryArrays_)
   {
      for (auto& subcategory : subcategoryArray.second)
      {
         committed |= subcategory->Commit();
      }
   }

   // Commit subcategories
   for (auto& subcategory : p->subcategories_)
   {
      committed |= subcategory->Commit();
   }

   // Commit variables
   for (auto& variable : p->variables_)
   {
      committed |= variable->Commit();
   }

   // Unblock signals
   p->blockSignals_ = false;

   if (committed)
   {
      p->changedSignal_();
   }

   return committed;
}

void SettingsCategory::Reset()
{
   // Don't allow individual variables to invoke the signal when operating over
   // the entire category
   p->blockSignals_ = true;

   // Reset subcategory arrays
   for (auto& subcategoryArray : p->subcategoryArrays_)
   {
      for (auto& subcategory : subcategoryArray.second)
      {
         subcategory->Reset();
      }
   }

   // Reset subcategories
   for (auto& subcategory : p->subcategories_)
   {
      subcategory->Reset();
   }

   // Reset variables
   for (auto& variable : p->variables_)
   {
      variable->Reset();
   }

   // Unblock signals
   p->blockSignals_ = false;

   p->stagedSignal_();
}

bool SettingsCategory::ReadJson(const boost::json::object& json)
{
   bool validated = true;

   const boost::json::value* value = json.if_contains(p->name_);

   if (value != nullptr && value->is_object())
   {
      const boost::json::object& object = value->as_object();

      // Read subcategory arrays
      for (auto& subcategoryArray : p->subcategoryArrays_)
      {
         const boost::json::value* arrayValue =
            object.if_contains(subcategoryArray.first);

         if (arrayValue != nullptr && arrayValue->is_object())
         {
            const boost::json::object& arrayObject = arrayValue->as_object();

            for (auto& subcategory : subcategoryArray.second)
            {
               validated &= subcategory->ReadJson(arrayObject);
            }
         }
         else
         {
            if (arrayValue == nullptr)
            {
               logger_->debug(
                  "Subcategory array key {} is not present, resetting to "
                  "defaults",
                  subcategoryArray.first);
            }
            else if (!arrayValue->is_object())
            {
               logger_->warn(
                  "Invalid json for subcategory array key {}, resetting to "
                  "defaults",
                  p->name_);
            }

            for (auto& subcategory : subcategoryArray.second)
            {
               subcategory->SetDefaults();
            }
            validated = false;
         }
      }

      // Read subcategories
      for (auto& subcategory : p->subcategories_)
      {
         validated &= subcategory->ReadJson(object);
      }

      // Read variables
      for (auto& variable : p->variables_)
      {
         validated &= variable->ReadValue(object);
      }
   }
   else
   {
      if (value == nullptr)
      {
         logger_->debug("Key {} is not present, resetting to defaults",
                        p->name_);
      }
      else if (!value->is_object())
      {
         logger_->warn("Invalid json for key {}, resetting to defaults",
                       p->name_);
      }

      SetDefaults();
      validated = false;
   }

   return validated;
}

void SettingsCategory::WriteJson(boost::json::object& json) const
{
   boost::json::object object;

   // Write subcategory arrays
   for (auto& subcategoryArray : p->subcategoryArrays_)
   {
      boost::json::object arrayObject;

      for (auto& subcategory : subcategoryArray.second)
      {
         subcategory->WriteJson(arrayObject);
      }

      object.insert_or_assign(subcategoryArray.first, arrayObject);
   }

   // Write subcategories
   for (auto& subcategory : p->subcategories_)
   {
      subcategory->WriteJson(object);
   }

   // Write variables
   for (auto& variable : p->variables_)
   {
      variable->WriteValue(object);
   }

   json.insert_or_assign(p->name_, object);
}

void SettingsCategory::RegisterSubcategory(SettingsCategory& subcategory)
{
   p->ConnectSubcategory(subcategory);
   p->subcategories_.push_back(&subcategory);
}

void SettingsCategory::RegisterSubcategoryArray(
   const std::string& name, std::vector<SettingsCategory>& subcategories)
{
   auto& newSubcategories = p->subcategoryArrays_.emplace_back(
      name, std::vector<SettingsCategory*> {});

   std::transform(subcategories.begin(),
                  subcategories.end(),
                  std::back_inserter(newSubcategories.second),
                  [this](SettingsCategory& subcategory)
                  {
                     p->ConnectSubcategory(subcategory);
                     return &subcategory;
                  });
}

void SettingsCategory::RegisterSubcategoryArray(
   const std::string& name, std::vector<SettingsCategory*>& subcategories)
{
   auto& newSubcategories = p->subcategoryArrays_.emplace_back(
      name, std::vector<SettingsCategory*> {});

   std::transform(subcategories.begin(),
                  subcategories.end(),
                  std::back_inserter(newSubcategories.second),
                  [this](SettingsCategory* subcategory)
                  {
                     p->ConnectSubcategory(*subcategory);
                     return subcategory;
                  });
}

void SettingsCategory::RegisterVariables(
   std::initializer_list<SettingsVariableBase*> variables)
{
   for (auto& variable : variables)
   {
      p->ConnectVariable(variable);
   }
   p->variables_.insert(p->variables_.end(), variables);
}

void SettingsCategory::RegisterVariables(
   std::vector<SettingsVariableBase*> variables)
{
   for (auto& variable : variables)
   {
      p->ConnectVariable(variable);
   }
   p->variables_.insert(
      p->variables_.end(), variables.cbegin(), variables.cend());
}

void SettingsCategory::Impl::ConnectSubcategory(SettingsCategory& category)
{
   connections_.emplace_back(category.changed_signal().connect(
      [this]()
      {
         if (!blockSignals_)
         {
            changedSignal_();
         }
      }));

   connections_.emplace_back(category.staged_signal().connect(
      [this]()
      {
         if (!blockSignals_)
         {
            stagedSignal_();
         }
      }));
}

void SettingsCategory::Impl::ConnectVariable(SettingsVariableBase* variable)
{
   connections_.emplace_back(variable->changed_signal().connect(
      [this]()
      {
         if (!blockSignals_)
         {
            changedSignal_();
         }
      }));

   connections_.emplace_back(variable->staged_signal().connect(
      [this]()
      {
         if (!blockSignals_)
         {
            stagedSignal_();
         }
      }));
}

} // namespace scwx::qt::settings
