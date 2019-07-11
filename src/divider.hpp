#ifndef DIVIDER_HPP
#define DIVIDER_HPP

#include "chunk.hpp"
#include "file.hpp"

using std::min;
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
    int nextpos = min(pos + chunksize, filesize);
    nextpos = advancePastNewline(r, nextpos);

    chunks.emplace_back(filename, pos, nextpos);
    pos = nextpos;
  }
  assert(chunks.back().end == filesize);

  return chunks;
}

#endif
