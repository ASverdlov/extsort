#include "divider.hpp"
#include "chunk.hpp"
#include "sorter.hpp"
#include "merger.hpp"

#include <iostream>
#include <string>
#include <cstdio> // TODO delete
#include <thread>
#include <utility>

#include <getopt.h>

using std::string;
using std::swap;

string from_temp_path;
string to_temp_path;

string input_path;
string output_path;
int initial_chunksize = 32768;
int merge_concurrency = 8192;

Chunk mergeAndSaveToTemp(Chunks chunks_to_merge) {
  int start = chunks_to_merge.front().start;
  int end = chunks_to_merge.back().end;

  Chunk out_chunk(to_temp_path, start, end);
  merge(chunks_to_merge, out_chunk);

  return out_chunk;
}

// returns chunk with the same position
// but in `from_temp_path` file.
Chunk substituteFilename(Chunk chunk, const string& filename) {
  return Chunk(filename, chunk.start, chunk.end);
}

void run() {
  auto chunks = divide(input_path, initial_chunksize);

  {
    from_temp_path = tmpnam(nullptr); // TODO move to file.cpp
    File tmp(from_temp_path, "w");
    tmp.truncate(chunks.back().end);
  }

  {
    to_temp_path = tmpnam(nullptr); // TODO move to file.cpp
    File tmp(to_temp_path, "w");
    tmp.truncate(chunks.back().end);
  }

  // sort stage
  for (auto& chunk : chunks) {
    auto to_chunk = substituteFilename(chunk, from_temp_path);
    sort(chunk, to_chunk);
    chunk = to_chunk;
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

      Chunk merged_chunk = mergeAndSaveToTemp(chunks_to_merge);
      new_chunks.push_back(merged_chunk);
    }

    // 2 temporary files are constantly alternating
    // in terms of who outputs results of merges into who
    swap(chunks, new_chunks);
    swap(from_temp_path, to_temp_path);
  }

  File::copy(from_temp_path, output_path);
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

  if (output_path == "") {
    output_path = input_path;
  }
}

int main(int argc, char* argv[]) {
  parse_options(argc, argv);
  run();
  return 0;
}
