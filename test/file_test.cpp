#include "gtest/gtest.h"

#include "file.hpp"

#include <string>

using std::string;

namespace {
TEST(FileTest, Read) {
  File r("./test/samplefile.txt", "r");
  EXPECT_EQ(r.read(), 'I');
  EXPECT_EQ(r.read(), 't');
  EXPECT_EQ(r.read(), ' ');
}

TEST(FileTest, ReadLine) {
  File r("./test/samplefile.txt", "r");

  string first_line = r.readLine();
  EXPECT_EQ(first_line, "It is a long established fact that a reader will be distracted by the readable\n");

  string second_line = r.readLine();
  EXPECT_EQ(second_line, "content of a page when looking at its layout. The point of using Lorem Ipsum\n");

  for (int i = 0; i < 6; ++i)
    r.readLine();

  string last_line = r.readLine();
  EXPECT_EQ(last_line, "like).\n");
}

TEST(FileTest, AdvanceUntil) {
  File r("./test/samplefile.txt", "r");
  r.advancePast('\n');
  EXPECT_EQ(r.pos, 79);
  EXPECT_EQ(r.read(), 'c');
  EXPECT_EQ(r.read(), 'o');
  r.advancePast('\n');
  EXPECT_EQ(r.pos, 156);
  EXPECT_EQ(r.read(), 'i');
  EXPECT_EQ(r.read(), 's');
}

TEST(FileTest, Size) {
  File r("./test/samplefile.txt", "r");
  EXPECT_EQ(r.size(), 614);
}

TEST(FileTest, Seek) {
  File r("./test/samplefile.txt", "r");
  r.seek(611);
  EXPECT_EQ(r.read(), ')');
  EXPECT_EQ(r.read(), '.');
  EXPECT_EQ(r.read(), '\n');
}
}
