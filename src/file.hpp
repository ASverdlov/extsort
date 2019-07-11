#ifndef FILE_HPP
#define FILE_HPP

#include <string>

using std::string;

struct File {
  File(const string& filename, const char* mode);
  ~File();

  string readLine();
  char read();
  void flush();
  bool eof();
  void seek(int pos);
  void advancePast(char c);
  int size();
  void truncate(int size);

  void write(const string& s);

  void throw_on_libc_error(const char* errmsg);

  static const int READAHEAD = 4096;

  FILE* file;
  int filesize;

  int pos;

  char buffer[READAHEAD];
  int buffer_pos;
  int buffer_len;
};

#endif
