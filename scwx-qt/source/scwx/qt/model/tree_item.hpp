#pragma once

#include <memory>
#include <vector>

#include <QVariant>

namespace scwx
{
namespace qt
{
namespace model
{

class TreeModel;

class TreeItem
{
public:
   explicit TreeItem(const std::vector<QVariant>& data,
                     TreeItem*                    parent = nullptr);
   explicit TreeItem(std::initializer_list<QVariant> data,
                     TreeItem*                       parent = nullptr);
   virtual ~TreeItem();

   TreeItem(const TreeItem&)            = delete;
   TreeItem& operator=(const TreeItem&) = delete;

   TreeItem(TreeItem&&) noexcept;
   TreeItem& operator=(TreeItem&&) noexcept;

   const TreeItem*        child(int row) const;
   TreeItem*              child(int row);
   std::vector<TreeItem*> children();
   int                    child_count() const;
   int                    column_count() const;
   QVariant               data(int column) const;
   int                    row() const;
   const TreeItem*        parent_item() const;

   TreeItem* FindChild(int column, const QVariant& data);

protected:
   void AppendChild(TreeItem* child);
   bool InsertChildren(int position, int count, int columns);
   bool SetData(int column, const QVariant& value);

private:
   class Impl;
   std::unique_ptr<Impl> p;

   friend class TreeModel;
};

} // namespace model
} // namespace qt
} // namespace scwx
