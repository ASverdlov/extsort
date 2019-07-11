#ifndef MERGER_HPP
#define MERGER_HPP

#include "chunk.hpp"
#include "file.hpp"

#include <cassert>
#include <set>
#include <vector>
#include <unordered_map>
#include <utility>

using Chunks = std::vector<Chunk>;
using ChunkLine = std::pair<std::string, int>;

using FilePointer = std::shared_ptr<File>;
using FilePointers = std::vector<FilePointer>;

inline void merge(Chunks& chunks, Chunk& out_chunk) {
  assert(chunks.size() > 0);

  FilePointers file_ptrs;
  for (const auto& c : chunks) {
    file_ptrs.emplace_back(new File(c.filename, "r+"));
    file_ptrs.back()->seek(c.start);
  }

  File outfile(out_chunk.filename, "r+");
  outfile.seek(out_chunk.start);

  // take first line of every chunk
  std::set<ChunkLine> chunklines;
  for (int id = 0; id < (int)chunks.size(); ++id) {
    if (chunks[id].start >= chunks[id].end)
      continue;

    std::string line = file_ptrs[id]->readLine();
    ChunkLine chunkline = ChunkLine(line, id);
    chunklines.insert(chunkline);
  }

  while (!chunklines.empty()) {
    auto smallest_iter = chunklines.begin();

    std::string line = smallest_iter->first;
    int id = smallest_iter->second;
    FilePointer file_ptr = file_ptrs[id];
    Chunk c = chunks[id];

    outfile.write(line);

    // take next line in this chunk
    chunklines.erase(smallest_iter);
    if (!file_ptr->eof() && file_ptr->position() < c.end) {
      std::string line = file_ptr->readLine();
      ChunkLine chunkline = ChunkLine(line, id);
      chunklines.insert(chunkline);
    }
  }
  assert(outfile.position() <= out_chunk.end);
}

#endif
