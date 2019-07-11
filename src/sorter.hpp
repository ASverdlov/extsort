#ifndef SORTER_HPP
#define SORTER_HPP

#include "chunk.hpp"
#include "file.hpp"

#include <cassert>
#include <vector>

using std::vector;

inline void sort(Chunk from_chunk, Chunk to_chunk) {
  vector<string> lines;

  File r(from_chunk.filename, "r");
  r.seek(from_chunk.start);
  while (!r.eof() && r.position() < from_chunk.end) {
    string s = r.readLine();
    lines.push_back(s);
  }
  assert(r.position() == from_chunk.end);

  sort(lines.begin(), lines.end());

  File w(to_chunk.filename, "r+");
  w.seek(to_chunk.start);
  for (const string& line : lines) {
    w.write(line.c_str());
  }
}

#endif
