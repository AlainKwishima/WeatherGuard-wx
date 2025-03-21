#pragma once

#include <scwx/qt/settings/settings_variable_base.hpp>

#include <memory>
#include <string>

#include <boost/json/object.hpp>
#include <boost/signals2/signal.hpp>

namespace scwx::qt::settings
{

class SettingsCategory
{
public:
   explicit SettingsCategory(const std::string& name);
   virtual ~SettingsCategory();

   SettingsCategory(const SettingsCategory&)            = delete;
   SettingsCategory& operator=(const SettingsCategory&) = delete;

   SettingsCategory(SettingsCategory&&) noexcept;
   SettingsCategory& operator=(SettingsCategory&&) noexcept;

   [[nodiscard]] std::string name() const;

   /**
    * Gets the signal invoked when a variable within the category is changed.
    *
    * @return Changed signal
    */
   boost::signals2::signal<void()>& changed_signal();

   /**
    * Gets the signal invoked when a variable within the category is staged.
    *
    * @return Staged signal
    */
   boost::signals2::signal<void()>& staged_signal();

   /**
    * Gets whether or not all settings variables are currently set to default
    * values.
    *
    * @return true if all settings variables are currently set to default
    * values, otherwise false.
    */
   [[nodiscard]] bool IsDefault() const;

   /**
    * Gets whether or not all settings variables currently have staged values
    * set to default.
    *
    * @return true if all settings variables currently have staged values set
    * to default, otherwise false.
    */
   [[nodiscard]] bool IsDefaultStaged() const;

   /**
    * Set all variables to their defaults.
    */
   void SetDefaults();

   /**
    * Stage all variables to their defaults.
    */
   void StageDefaults();

   /**
    * Sets the current value of all variables to the staged value.
    *
    * @return true if any staged value was committed, false if no staged values
    * are present.
    */
   bool Commit();

   /**
    * Clears the staged value of all variables.
    */
   void Reset();

   /**
    * Reads the variables from the JSON object.
    *
    * @param json JSON object to read
    *
    * @return true if the values read are valid, false if any values were
    * modified.
    */
   virtual bool ReadJson(const boost::json::object& json);

   /**
    * Writes the variables to the JSON object.
    *
    * @param json JSON object to write
    */
   virtual void WriteJson(boost::json::object& json) const;

   void RegisterSubcategory(SettingsCategory& subcategory);
   void RegisterSubcategoryArray(const std::string&             name,
                                 std::vector<SettingsCategory>& subcategories);
   void RegisterSubcategoryArray(const std::string&              name,
                                 std::vector<SettingsCategory*>& subcategories);
   void
   RegisterVariables(std::initializer_list<SettingsVariableBase*> variables);
   void RegisterVariables(std::vector<SettingsVariableBase*> variables);

protected:
   void set_block_signals(bool blockSignals);

private:
   class Impl;
   std::unique_ptr<Impl> p;
};

} // namespace scwx::qt::settings
