#include <scwx/qt/util/json.hpp>
#include <scwx/util/logger.hpp>

#include <fstream>

#include <boost/json.hpp>
#include <fmt/ranges.h>
#include <QFile>
#include <QTextStream>

namespace scwx
{
namespace qt
{
namespace util
{
namespace json
{

static const std::string logPrefix_ = "scwx::qt::util::json";
static const auto        logger_    = scwx::util::Logger::Create(logPrefix_);

/* Adapted from:
 * https://www.boost.org/doc/libs/1_77_0/libs/json/doc/html/json/examples.html#json.examples.pretty
 *
 * Copyright (c) 2019, 2020 Vinnie Falco
 * Copyright (c) 2020 Krystian Stasiowski
 * Distributed under the Boost Software License, Version 1.0. (See
 * http://www.boost.org/LICENSE_1_0.txt)
 */
static void PrettyPrintJson(std::ostream&             os,
                            boost::json::value const& jv,
                            std::string*              indent = nullptr);

static boost::json::value ReadJsonFile(QFile& file);
static boost::json::value ReadJsonStream(std::istream& is);

boost::json::value ReadJsonFile(const std::string& path)
{
   boost::json::value json;

   if (path.starts_with(":"))
   {
      QFile file(path.c_str());
      json = ReadJsonFile(file);
   }
   else
   {
      std::ifstream ifs {path};
      json = ReadJsonStream(ifs);
   }

   return json;
}

static boost::json::value ReadJsonFile(QFile& file)
{
   boost::json::value json;

   if (file.open(QIODevice::ReadOnly))
   {
      QTextStream jsonStream(&file);
      jsonStream.setEncoding(QStringConverter::Utf8);

      std::string        jsonSource = jsonStream.readAll().toStdString();
      std::istringstream is {jsonSource};

      json = ReadJsonStream(is);

      file.close();
   }
   else
   {
      logger_->warn("Could not open file for reading: \"{}\"",
                    file.fileName().toStdString());
   }

   return json;
}

static boost::json::value ReadJsonStream(std::istream& is)
{
   std::string line;

   boost::json::stream_parser p;
   boost::system::error_code  ec;

   while (std::getline(is, line))
   {
      p.write(line, ec);
      if (ec)
      {
         logger_->warn("{}", ec.message());
         return nullptr;
      }
   }

   p.finish(ec);
   if (ec)
   {
      logger_->warn("{}", ec.message());
      return nullptr;
   }

   return p.release();
}

void WriteJsonFile(const std::string&        path,
                   const boost::json::value& json,
                   bool                      prettyPrint)
{
   std::ofstream ofs {path};

   if (!ofs.is_open())
   {
      logger_->warn("Cannot write JSON file: \"{}\"", path);
   }
   else
   {
      if (prettyPrint)
      {
         PrettyPrintJson(ofs, json);
      }
      else
      {
         ofs << json;
      }
      ofs.close();
   }
}

static void PrettyPrintJson(std::ostream&             os,
                            boost::json::value const& jv,
                            std::string*              indent)
{
   std::string indent_;
   if (!indent)
      indent = &indent_;
   switch (jv.kind())
   {
   case boost::json::kind::object:
   {
      os << "{\n";
      indent->append(4, ' ');
      auto const& obj = jv.get_object();
      if (!obj.empty())
      {
         auto it = obj.begin();
         for (;;)
         {
            os << *indent << boost::json::serialize(it->key()) << " : ";
            PrettyPrintJson(os, it->value(), indent);
            if (++it == obj.end())
               break;
            os << ",\n";
         }
      }
      os << "\n";
      indent->resize(indent->size() - 4);
      os << *indent << "}";
      break;
   }

   case boost::json::kind::array:
   {
      os << "[\n";
      indent->append(4, ' ');
      auto const& arr = jv.get_array();
      if (!arr.empty())
      {
         auto it = arr.begin();
         for (;;)
         {
            os << *indent;
            PrettyPrintJson(os, *it, indent);
            if (++it == arr.end())
               break;
            os << ",\n";
         }
      }
      os << "\n";
      indent->resize(indent->size() - 4);
      os << *indent << "]";
      break;
   }

   case boost::json::kind::string:
   {
      os << boost::json::serialize(jv.get_string());
      break;
   }

   case boost::json::kind::uint64:
      os << jv.get_uint64();
      break;

   case boost::json::kind::int64:
      os << jv.get_int64();
      break;

   case boost::json::kind::double_:
      os << jv.get_double();
      break;

   case boost::json::kind::bool_:
      if (jv.get_bool())
         os << "true";
      else
         os << "false";
      break;

   case boost::json::kind::null:
      os << "null";
      break;
   }

   if (indent->empty())
      os << "\n";
}

} // namespace json
} // namespace util
} // namespace qt
} // namespace scwx
