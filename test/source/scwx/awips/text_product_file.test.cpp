#include <scwx/awips/text_product_file.hpp>

#include <gtest/gtest.h>

namespace scwx
{
namespace awips
{

static const std::string logPrefix_ = "scwx::awips::text_product_file.test";

class TextProductValidFileTest : public testing::TestWithParam<std::string>
{
};

TEST_P(TextProductValidFileTest, ValidFile)
{
   TextProductFile file;

   auto param = GetParam();

   const std::string filename {std::string(SCWX_TEST_DATA_DIR) + param};
   file.LoadFile(filename);

   EXPECT_GT(file.message_count(), 0);
}

INSTANTIATE_TEST_SUITE_P(
   TextProductFile,
   TextProductValidFileTest,
   testing::Values("/warnings/warnings_20210604_21.txt",
                   "/warnings/warnings_20210606_15.txt",
                   "/warnings/warnings_20210606_22-59.txt",
                   "/nexrad/level3/KLSX_NOUS63_FTMLSX_202201041404",
                   "/text/PGUM_WHPQ41_CFWPQ1_202201231710.nids",
                   "/text/PGUM_WHPQ42_CFWPQ2_202201231924.nids"));

TEST(TextProductFile, Update)
{
   const std::string filename1 {std::string(SCWX_TEST_DATA_DIR) +
                                "/warnings/warnings_20210606_22-08.txt"};
   const std::string filename2 {std::string(SCWX_TEST_DATA_DIR) +
                                "/warnings/warnings_20210606_22-19.txt"};
   const std::string filename3 {std::string(SCWX_TEST_DATA_DIR) +
                                "/warnings/warnings_20210606_22-59.txt"};

   TextProductFile file;

   file.LoadFile(filename1);
   EXPECT_EQ(file.message_count(), 2);

   file.LoadFile(filename2);
   EXPECT_EQ(file.message_count(), 4);

   file.LoadFile(filename3);
   EXPECT_EQ(file.message_count(), 13);
}

} // namespace awips
} // namespace scwx
