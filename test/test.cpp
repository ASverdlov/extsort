#include "gtest/gtest.h"

#include "chunk.hpp"
#include "file.hpp"
#include "sorter.hpp"
#include "merger.hpp"

#include <string>
#include <libgen.h>

using std::string;

string source_dir;

namespace {
TEST(SorterTest, Sort) {
  auto chunk = Chunk(source_dir + "/assets/samplefile.txt", 0, 614);
  auto outchunk = Chunk(source_dir + "/assets/outfile.txt", 0, 614);
  sort(chunk, outchunk);

  File expected(source_dir + "/assets/samplefile.sorted.txt", "r");
  File got(source_dir + "/assets/outfile.txt", "r");

  EXPECT_EQ(got.size(), expected.size());
  while (!got.eof()) {
    string expected_line = expected.readLine();
    string got_line = got.readLine();
    EXPECT_EQ(got_line, expected_line);
  }
}

TEST(MergerTest, Merge) {
  vector<Chunk> chunks;
  chunks.emplace_back(source_dir + "/assets/a.txt", 0, 8);
  chunks.emplace_back(source_dir + "/assets/b.txt", 0, 8);

  Chunk out_chunk(source_dir + "/assets/merged.txt", 0, 16);

  merge(chunks, out_chunk);

  File outfile(source_dir + "/assets/merged.txt", "r");

  vector<string> lines_expected = {
    "a\n",
    "b\n",
    "c\n",
    "c\n",
    "d\n",
    "t\n",
    "z\n",
    "z\n"
  };

  for (string expected : lines_expected) {
    string line = outfile.readLine();
    ASSERT_EQ(line, expected);
  }
}
TEST(FileTest, Read) {
  File r(source_dir + "/assets/samplefile.txt", "r");
  EXPECT_EQ(r.read(), 'I');
  EXPECT_EQ(r.read(), 't');
  EXPECT_EQ(r.read(), ' ');
}

TEST(FileTest, ReadLine) {
  File r(source_dir + "/assets/samplefile.txt", "r");

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
  File r(source_dir + "/assets/samplefile.txt", "r");
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
  File r(source_dir + "/assets/samplefile.txt", "r");
  EXPECT_EQ(r.size(), 614);
}

TEST(FileTest, Seek) {
  File r(source_dir + "/assets/samplefile.txt", "r");
  r.seek(611);
  EXPECT_EQ(r.read(), ')');
  EXPECT_EQ(r.read(), '.');
  EXPECT_EQ(r.read(), '\n');
}
}

int main(int argc, char** argv)
{
  source_dir = dirname(argv[0]);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

