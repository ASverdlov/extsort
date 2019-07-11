#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>
#include <string>

// Represents [start, end) range in a file.
struct Chunk {
  Chunk(const std::string& filename, int start, int end) {
    this->filename = filename;
    this->start = start;
    this->end = end;
  }

  int start;
  int end;
  std::string filename;
};

using Chunks = std::vector<Chunk>;

#endif
