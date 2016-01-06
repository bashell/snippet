#ifndef _MY_STRING_H_
#define _MY_STRING_H_

#include <iostream>
#include <cstddef>
#include <string.h>

class String {
  friend String operator+(const String &, const String &);
  friend String operator+(const String &, const char *);
  friend String operator+(const char *, const String &);
  friend bool operator==(const String &, const String &);
  friend bool operator!=(const String &, const String &);
  friend bool operator>(const String &, const String &);
  friend bool operator<(const String &, const String &);
  friend bool operator>=(const String &, const String &);
  friend bool operator<=(const String &, const String &);
  friend std::istream &operator>>(std::istream &, String &);
  friend std::ostream &operator<<(std::ostream &, const String &);

 public:
  String();
  String(const char *);
  String(const String &);
  ~String();

  String &operator=(const String &);
  String &operator=(const char *);
  String &operator+=(const String &);
  String &operator+=(const char *);
  char &operator[](size_t index);
  char operator[](size_t index) const;
  
  void debug() const;
  size_t size() const;
  const char *c_str() const;

 private:
  char *str_;
};

inline String operator+(const String &lhs, const String &rhs) {
  String ret(lhs);
  ret += rhs;
  return ret;
}

inline String operator+(const String &lhs, const char *s) {
  return lhs + String(s);
}

inline String operator+(const char *s, const String &rhs) {
  return String(s) + rhs;
}

inline bool operator==(const String &lhs, const String &rhs) {
  return strcmp(lhs.str_, rhs.str_) == 0;
}

inline bool operator!=(const String &lhs, const String &rhs) {
  return !(lhs == rhs);
}

inline bool operator>(const String &lhs, const String &rhs) {
  return strcmp(lhs.str_, rhs.str_) > 0;
}

inline bool operator<(const String &lhs, const String &rhs) {
  return strcmp(lhs.str_, rhs.str_) < 0;
}

inline bool operator>=(const String &lhs, const String &rhs) {
  return !(lhs < rhs);
}

inline bool operator<=(const String &lhs, const String &rhs) {
  return !(lhs > rhs);
}

inline std::istream &operator>>(std::istream &is, String &s) {
  char buf[4096];
  is >> buf;
  if(is) 
    s = buf;
  return is;
}

inline std::ostream &operator<<(std::ostream &os, const String &s) {
  os << s.str_;
  return os;
}

String::String() : str_(new char[1]) {
  *str_ = '\0';
}

String::String(const char *s) : str_(new char[strlen(s) + 1]) {
  strcpy(str_, s);
}

String::String(const String &s) : str_(new char[s.size() + 1]) {
  strcpy(str_, s.str_);
}


String::~String() {
  delete[] str_;
}

String& String::operator=(const String &s) {
  if(this != &s) {
    delete[] str_;
    str_ = new char[s.size() + 1];
    strcpy(str_, s.str_);
  }
  return *this;
}

String& String::operator=(const char *s) {
  if(str_ != s) {
    delete[] str_;
    str_ = new char[strlen(s) + 1];
    strcpy(str_, s);
  }
  return *this;
}

String& String::operator+=(const String &s) {
  char *pt = new char[size() + s.size() + 1];
  strcpy(pt, str_);
  strcat(pt, s.str_);
  delete str_;
  str_ = pt;
  return *this;
}

String& String::operator+=(const char *s) {
  return operator+=(String(s));
}

char& String::operator[](size_t index) {
  return str_[index];
}

char String::operator[](size_t index) const {
  return str_[index];
}

void String::debug() const {
  std::cout << str_ << std::endl;
}

size_t String::size() const {
  return strlen(str_);
}

const char* String::c_str() const {
  return str_;
}


#endif  // _MY_STRING_H_
