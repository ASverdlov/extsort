#ifndef SORTER_HPP
#define SORTER_HPP

#include "chunk.hpp"
#include "file.hpp"

#include <iostream> // TODO: delete
#include <cassert>
#include <vector>

using std::vector;

inline void sort(Chunk from_chunk, Chunk to_chunk) {
  vector<string> lines;

  File r(from_chunk.filename, "r");
  r.seek(from_chunk.start);
  while (!r.eof() && r.pos < from_chunk.end) {
    string s = r.readLine();
    lines.push_back(s);
  }
  // TODO delete
  if (r.pos != from_chunk.end) {
    std::cerr << "r.pos " << r.pos << ", from_chunk.end: " << from_chunk.end << std::endl;
    std::cerr << "r.eof(): " << (r.eof() ? "YES": "NO") << std::endl;
  }
  assert(r.pos == from_chunk.end);

  sort(lines.begin(), lines.end());

  File w(to_chunk.filename, "r+");
  w.seek(to_chunk.start);
  for (const string& line : lines) {
    w.write(line.c_str());
  }
}

#endif
