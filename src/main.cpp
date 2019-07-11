#include "divider.hpp"
#include "chunk.hpp"
#include "sorter.hpp"
#include "merger.hpp"
#include "config.hpp"

#include <string>
#include <thread>
#include <utility>

using std::string;
using std::swap;

Chunk mergeAndSaveToTemp(Chunks chunks_to_merge, string out_path) {
  int start = chunks_to_merge.front().start;
  int end = chunks_to_merge.back().end;

  Chunk out_chunk(out_path, start, end);
  merge(chunks_to_merge, out_chunk);

  return out_chunk;
}

// returns chunk with the same position
// but in <filename> file.
Chunk substituteFilename(Chunk chunk, const string& filename) {
  return Chunk(filename, chunk.start, chunk.end);
}

void run(Config& cfg) {
  auto chunks = divide(cfg.input_path, cfg.initial_chunksize);

  string from_temp_path = File::createTemporary(chunks.back().end);
  string to_temp_path = File::createTemporary(chunks.back().end);

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
      for (int i = 0; chunk_id < chunks.size() && i < cfg.merge_concurrency; ++i) {
        chunks_to_merge.push_back(chunks[chunk_id]);
        ++chunk_id;
      }

      Chunk merged_chunk = mergeAndSaveToTemp(chunks_to_merge, to_temp_path);
      new_chunks.push_back(merged_chunk);
    }

    // 2 temporary files are constantly alternating
    // in terms of who outputs results of merges into who
    swap(chunks, new_chunks);
    swap(from_temp_path, to_temp_path);
  }

  File::copy(from_temp_path, cfg.output_path);
}

int main(int argc, char* argv[]) {
  auto cfg = parseOptions(argc, argv);
  run(cfg);
  return 0;
}
