#pragma once

#include <scwx/qt/settings/settings_interface_base.hpp>

#include <functional>
#include <memory>
#include <string>

class QLabel;

namespace scwx::qt::settings
{

template<class T>
class SettingsVariable;

template<class T>
class SettingsInterface : public SettingsInterfaceBase
{
public:
   explicit SettingsInterface();
   ~SettingsInterface();

   SettingsInterface(const SettingsInterface&)            = delete;
   SettingsInterface& operator=(const SettingsInterface&) = delete;

   SettingsInterface(SettingsInterface&&) noexcept;
   SettingsInterface& operator=(SettingsInterface&&) noexcept;

   /**
    * Sets the settings variable associated with the interface. This must be
    * set prior to calling any other functions.
    *
    * @param variable Settings variable
    */
   void SetSettingsVariable(SettingsVariable<T>& variable);

   /**
    * Gets the settings variable associated with the interface.
    *
    * @return Settings variable
    */
   SettingsVariable<T>* GetSettingsVariable() const;

   /**
    * Gets whether the staged value (or current value, if none staged) is
    * set to the default value.
    *
    * @return true if the settings variable is set to default, otherwise false.
    */
   bool IsDefault() override;

   /**
    * Sets the current value of the associated settings variable to the staged
    * value.
    *
    * @return true if the staged value was committed, false if no staged value
    * is present.
    */
   bool Commit() override;

   /**
    * Clears the staged value of the associated settings variable.
    */
   void Reset() override;

   /**
    * Stages the default value of the associated settings variable.
    */
   void StageDefault() override;

   /**
    * Stages a value to the associated settings variable.
    */
   void StageValue(const T& value);

   /**
    * Sets the edit widget from the settings dialog.
    *
    * @param widget Edit widget
    */
   void SetEditWidget(QWidget* widget) override;

   /**
    * Sets the reset button from the settings dialog.
    *
    * @param button Reset button
    */
   void SetResetButton(QAbstractButton* button) override;

   /**
    * Sets the label for units from the settings dialog.
    *
    * @param label Unit label
    */
   void SetUnitLabel(QLabel* label);

   /**
    * If the edit widget displays a different value than what is stored in the
    * settings variable, a mapping function must be provided in order to convert
    * the value used by the edit widget from the settings value.
    *
    * @param function Map from settings value function
    */
   void SetMapFromValueFunction(std::function<std::string(const T&)> function);

   /**
    * If the edit widget displays a different value than what is stored in the
    * settings variable, a mapping function must be provided in order to convert
    * the value used by the edit widget to the settings value.
    *
    * @param function Map to settings value function
    */
   void SetMapToValueFunction(std::function<T(const std::string&)> function);

   /**
    * Sets the unit to be used by this setting.
    *
    * @param scale The ratio of the current unit to the base unit
    * @param abbreviation The abbreviation to be displayed
    */
   void SetUnit(const double& scale, const std::string& abbreviation);

   /**
    * Enables or disables whitespace trimming of text input.
    * Removes whitespace ('\t', '\n', '\v', '\f', '\r', and ' ') at the
    * beginning and end of the string from a QLineEdit.
    *
    * @param trimmingEnabled If trimming should be enabled.
    */
   void EnableTrimming(bool trimmingEnabled = true);

private:
   class Impl;
   std::unique_ptr<Impl> p;
};

} // namespace scwx::qt::settings
