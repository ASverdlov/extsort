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
int initial_chunksize = 32768;
int merge_concurrency = 8192;

Chunk merge_and_write_back(Chunks chunks_to_merge) {
  int start = chunks_to_merge.front().start;
  int end = chunks_to_merge.back().end;

  Chunk temp_chunk(temp_path, start, end);
  Chunk out_chunk(input_path, start, end);
  merge(chunks_to_merge, temp_chunk, out_chunk);

  return out_chunk;
}

void run() {
  auto chunks = divide(input_path, initial_chunksize);
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
      for (int i = 0; chunk_id < chunks.size() && i < merge_concurrency; ++i) {
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
    {"input",             required_argument, 0,  'i' },
    {"output",            required_argument, 0,  'o' },
    {"merge-concurrency", required_argument, 0,  'm' },
    {"chunksize",         required_argument, 0,  'c' },
    {"help",              no_argument,       0,  'h' },
    {0,                   0,                 0,  0 }
  };
  static const char *opts = "hi:o:m:c:";

  while ((ch = getopt_long(argc, argv, opts, long_options, NULL)) != -1) {
    switch (ch) {
    case 'h':
      std::cout << "Help:" << std::endl;
      std::cout << "--input, -i:\tinput filename path" << std::endl;
      std::cout << "--output, -o:\tinput filename path" << std::endl;
      std::cout << "--merge-concurrency, -m:\tmaximum number of chunks to be merged in parallel (1024 by default)" << std::endl;
      std::cout << "--chunksize, -c:\tinitial chunks minimum size (1024 bytes by default)" << std::endl;

      exit(0);
      break;

    case 'i':
      std::cout << "-i provided: " << string(optarg) << std::endl;
      input_path = optarg;
      break;

    case 'o':
      std::cout << "-o provided: " << string(optarg) << std::endl;
      output_path = optarg;
      break;

    case 'm':
      std::cout << "-m provided: " << string(optarg) << std::endl;
      merge_concurrency = atoi(optarg);
      break;

    case 'c':
      std::cout << "-c provided: " << string(optarg) << std::endl;
      initial_chunksize = atoi(optarg);
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
