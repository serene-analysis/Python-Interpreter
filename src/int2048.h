#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER
#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include<cassert>

// 请不要使用 using namespace std;

namespace sjtu {
class int2048 {
  // todo
public:
  std::vector<int>pol;
  bool neg;
  void sizen(int);
  int& operator[](int);
  void ntt(int, int, const int);
  int2048 mul(int2048, int2048, const int);
  friend int2048 lmove(int2048, int);
  friend int2048 rmove(int2048, int);
  friend int2048 div(int2048, int2048);
  int length();
  int2048 newton_inv(int);
  friend int2048 div(int2048, int);
  // 构造函数
  int2048();
  int2048(bool);
  int2048(int);
  int2048(long long);
  int2048(const std::string &);
  int2048(const int2048 &);
  double toDouble();
  std::string toString();

  // 以下给定函数的形式参数类型仅供参考，可自行选择使用常量引用或者不使用引用
  // 如果需要，可以自行增加其他所需的函数
  // ===================================
  // Integer1
  // ===================================

  // 读入一个大整数
  void read(const std::string &);
  // 输出储存的大整数，无需换行
  void print();

  // 加上一个大整数
  int2048 &add(const int2048 &);
  // 返回两个大整数之和
  friend int2048 add(int2048, const int2048 &);

  // 减去一个大整数
  int2048 &minus(const int2048 &);
  // 返回两个大整数之差
  friend int2048 minus(int2048, const int2048 &);

  // ===================================
  // Integer2
  // ===================================

  int2048 operator+() const;
  int2048 operator-() const;

  int2048 &operator=(const int2048 &);

  int2048 &operator+=(const int2048 &);
  friend int2048 operator+(int2048, const int2048 &);

  int2048 &operator-=(const int2048 &);
  friend int2048 operator-(int2048, const int2048 &);

  int2048 &operator*=(const int2048 &);
  friend int2048 operator*(int2048, const int2048 &);

  int2048 &operator/=(const int2048 &);
  friend int2048 operator/(int2048, const int2048 &);

  int2048 &operator%=(const int2048 &);
  friend int2048 operator%(int2048, const int2048 &);

  friend std::istream &operator>>(std::istream &, int2048 &);
  friend std::ostream &operator<<(std::ostream &, const int2048 &);

  friend bool operator==(const int2048 &, const int2048 &);
  friend bool operator!=(const int2048 &, const int2048 &);
  friend bool operator<(const int2048 &, const int2048 &);
  friend bool operator>(const int2048 &, const int2048 &);
  friend bool operator<=(const int2048 &, const int2048 &);
  friend bool operator>=(const int2048 &, const int2048 &);
};
}
// your cpp source code

#endif