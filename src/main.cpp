#include "divider.hpp"
#include "chunk.hpp"
#include "sorter.hpp"
#include "merger.hpp"

#include <iostream>
#include <string>
#include <cstdio> // TODO delete
#include <thread>

#include <getopt.h>

using std::string;

string temp_path;
string input_path;
string output_path;

static const int INITIAL_CHUNKSIZE = 10;
static const int MERGE_ASSOCIATIVITY = 3;
static const int NUM_THREADS = 8;

Chunk merge_and_write_back(Chunks chunks_to_merge) {
  int start = chunks_to_merge.front().start;
  int end = chunks_to_merge.back().end;

  Chunk temp_chunk(temp_path, start, end);
  Chunk out_chunk(input_path, start, end);
  merge(chunks_to_merge, temp_chunk, out_chunk);

  return out_chunk;
}

void run() {
  auto chunks = divide(input_path, INITIAL_CHUNKSIZE);
  for (auto c : chunks) {
    sort(c, c);
  }

  {
    temp_path = tmpnam(nullptr); // TODO move to file.cpp
    File tempfile(temp_path, "w");
    tempfile.truncate(chunks.back().end);
  }

  // merge stage
  while (chunks.size() > 1) {
    Chunks new_chunks;

    int chunk_id = 0;
    while (chunk_id < chunks.size()) {
      Chunks chunks_to_merge;
      for (int i = 0; chunk_id < chunks.size() && i < MERGE_ASSOCIATIVITY; ++i) {
        chunks_to_merge.push_back(chunks[chunk_id]);
        ++chunk_id;
      }

      Chunk merged_chunk = merge_and_write_back(chunks_to_merge);
      new_chunks.push_back(merged_chunk);
    }

    chunks = new_chunks;
  }
}

void parse_options(int argc, char* argv[]) {
  char ch;

  static option long_options[] = {
    {"input",   required_argument, 0,  'i' },
    {"output",  required_argument, 0,  'o' },
    {"help",    no_argument,       0,  'h' },
    {0,         0,                 0,  0 }
  };
  static const char *opts = "i:o:h";

  while ((ch = getopt_long(argc, argv, opts, long_options, NULL)) != -1) {
    switch (ch) {
    case 'h':
      std::cout << "Help:" << std::endl;
      std::cout << "--input, -i: input filename path" << std::endl;
      std::cout << "--output, -o: input filename path" << std::endl;
      std::cout << "--temp, -t: temporary directory" << std::endl;
      break;

    case 'i':
      std::cout << "-i provided: " << string(optarg) << std::endl;
      input_path = optarg;
      break;

    case 'o':
      std::cout << "-o provided: " << string(optarg) << std::endl;
      output_path = optarg;
      break;

    default:
      std::cout << "invalid option provided" << std::endl;
      break;
    }
  }
}

int main(int argc, char* argv[]) {
  parse_options(argc, argv);
  run();
  return 0;
}
