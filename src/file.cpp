#include "file.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

using std::string;

void File::throw_on_libc_error(const char* errmsg) {
  if (!this->file || ferror(this->file)) {
    throw std::runtime_error(errmsg);
  }
}

File::File(const string& filename, const char* mode) {
  this->file = fopen(filename.c_str(), mode);
  this->throw_on_libc_error("File: error during fopen()");

  this->pos = 0;
  this->buffer_len = 0;
  this->buffer_pos = 0;

  // cache file size
  fseek(this->file, (off_t)pos, SEEK_END);
  this->throw_on_libc_error("File::size(): error during seek()");
  this->filesize = ftell(this->file);

  // restore position in file
  this->seek(0);
}

File::~File() {
  if (this->file) {
    fclose(this->file);
  }
}

string File::createTemporary(int size) {
    string filepath = tmpnam(nullptr);
    File tmp(filepath, "w");
    tmp.truncate(size);
    return filepath;
}

void File::copy(const string& oldfilename, const string& newfilename) {
  File oldfile(oldfilename, "r");
  File newfile(newfilename, "w");
  while (!oldfile.eof()) {
    string line = oldfile.readLine();
    newfile.write(line);
  }
}

// We use fseek here, which won't be able to go above files > 1GB
// To fix, we can use open+lseek instead of fopen+fseek.
void File::seek(int pos) {
  fseek(this->file, (off_t)pos, SEEK_SET);
  this->throw_on_libc_error("File::seek(): error during seek()");

  this->pos = pos;
  this->buffer_len = 0;
  this->buffer_pos = 0;
}

void File::truncate(int size) {
  File::seek(size - 1);
  fputc('\0', this->file);
  this->throw_on_libc_error("File::truncate(): error during fputc()");
}

int File::position() const {
    return this->pos;
}

int File::size() const {
  return this->filesize;
}

bool File::eof() const {
  return this->pos >= this->size();
}

char File::read() {
  if (this->buffer_pos == this->buffer_len) {
    int newlen = (int)fread(
      (void*)(this->buffer), sizeof(char), File::READAHEAD, this->file
    );
    this->throw_on_libc_error("File::read(): error during fread()");

    this->buffer_pos = 0;
    this->buffer_len = newlen;
  }

  assert(this->buffer_len > 0);
  assert(this->buffer_pos < this->buffer_len);

  char c = buffer[this->buffer_pos];
  ++this->pos;
  ++this->buffer_pos;

  return c;
}

void File::flush() {
  fflush(this->file);
  this->throw_on_libc_error("File::flush(): error during fflush()");
}

string File::readLine() {
  string s = "";

  while (!this->eof()) {
    char c = this->read();
    s += c;
    if (c == '\n') {
      break;
    }
  }

  return s;
}

void File::write(const string& s) {
  int pos = 0;
  while (pos < s.length()) {
    void* ptr = (char*)s.c_str() + pos;
    int remainder = s.length() - pos;

    int written = fwrite(ptr, sizeof(char), remainder, this->file);
    this->throw_on_libc_error("File::write(): error during fwrite()");

    pos += written;
  }
  this->pos += s.length();
}

void File::advancePast(char c) {
  while (!this->eof()) {
    if (this->read() == c)
      break;
  }
}
