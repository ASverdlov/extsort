#ifndef FILE_HPP
#define FILE_HPP

#include <string>

using std::string;

class File {
public:
  File(const string& filename, const char* mode);
  ~File();

  static void copy(const string& oldfilename, const string& newfilename);
  static string createTemporary(int size);

  string readLine();
  char read();
  void flush();
  void seek(int pos);
  void advancePast(char c);
  void truncate(int size);
  void write(const string& s);

  bool eof() const;
  int position() const;
  int size() const;

  static const int READAHEAD = 4096;

private:
  void throw_on_libc_error(const char* errmsg);

  FILE* file;
  int filesize;

  int pos;

  char buffer[READAHEAD];
  int buffer_pos;
  int buffer_len;
};

#endif
