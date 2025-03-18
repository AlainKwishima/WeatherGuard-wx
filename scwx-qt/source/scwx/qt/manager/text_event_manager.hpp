#pragma once

#include <scwx/awips/text_product_message.hpp>
#include <scwx/qt/types/text_event_key.hpp>

#include <memory>
#include <string>

#include <QObject>

namespace scwx
{
namespace qt
{
namespace manager
{

class TextEventManager : public QObject
{
   Q_OBJECT

public:
   explicit TextEventManager();
   ~TextEventManager();

   size_t message_count(const types::TextEventKey& key) const;
   std::vector<std::shared_ptr<awips::TextProductMessage>>
   message_list(const types::TextEventKey& key) const;

   void LoadFile(const std::string& filename);

   static std::shared_ptr<TextEventManager> Instance();

signals:
   void AlertUpdated(const types::TextEventKey& key, size_t messageIndex);

private:
   class Impl;
   std::unique_ptr<Impl> p;
};

} // namespace manager
} // namespace qt
} // namespace scwx
