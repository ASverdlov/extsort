#ifndef DIVIDER_HPP
#define DIVIDER_HPP

#include "chunk.hpp"
#include "file.hpp"

#include <iostream> // TODO: delete

using std::string;
using Chunks = std::vector<Chunk>;

inline int advancePastNewline(File& r, int startpos) {
  r.seek(startpos);
  r.advancePast('\n');
  return r.pos;
}

inline Chunks divide(const string& filename, int chunksize) {
  File r(filename, "r");
  const int filesize = r.size(); // in bytes

  Chunks chunks;

  int pos = 0;
  while (pos < filesize) {
    int nextpos = advancePastNewline(r, pos + chunksize);
    chunks.emplace_back(filename, pos, nextpos);
    pos = nextpos;
  }

  return chunks;
}

#endif
