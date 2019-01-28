/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * cvec.h
 *
 * an implementation of GLSL-like vector functionality:
 * adapted from Steven J. Gortler's "Computer Graphics" course
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef VEC_H
#define VEC_H

#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>

using namespace std;

template <typename T, int n>
class Cvec {
  T d_[n];

public:
  Cvec() {
    for (int i = 0; i < n; ++i) {
      d_[i] = 0;
    }
  }

  explicit Cvec(const T& t) {
    for (int i = 0; i < n; ++i) {
      d_[i] = t;
    }
  }

  Cvec(const T& t0, const T& t1) {
    assert(n == 2);
    d_[0] = t0, d_[1] = t1;
  }

  Cvec(const T& t0, const T& t1, const T& t2) {
    assert(n == 3);
    d_[0] = t0, d_[1] = t1, d_[2] = t2;
  }

  Cvec(const T& t0, const T& t1, const T& t2, const T& t3) {
    assert(n == 4);
    d_[0] = t0, d_[1] = t1, d_[2] = t2, d_[3] = t3;
  }

  template<int m>
  explicit Cvec(const Cvec<T, m>& v, const T& extendValue = T(0)) {
    for (int i = 0; i < std::min(m, n); ++i) {
      d_[i] = v[i];
    }
    for (int i = std::min(m, n); i < n; ++i) {
      d_[i] = extendValue;
    }
  }
  
  T& operator [] (const int i) {
    return d_[i];
  }

  const T& operator [] (const int i) const {
    return d_[i];
  }

  Cvec operator - () const {
    return Cvec(*this) *= -1;
  }

  Cvec& operator += (const Cvec& v) {
    for (int i = 0; i < n; ++i) {
      d_[i] += v[i];
    }
    return *this;
  }

  Cvec& operator -= (const Cvec& v) {
    for (int i = 0; i < n; ++i) {
      d_[i] -= v[i];
    }
    return *this;
  }

  Cvec& operator *= (const T a) {
    for (int i = 0; i < n; ++i) {
      d_[i] *= a;
    }
    return *this;
  }

  Cvec& operator /= (const T a) {
    const T inva(1/a);
    for (int i = 0; i < n; ++i) {
      d_[i] *= inva;
    }
    return *this;
  }
  
  Cvec operator * (const Cvec& v) { //element-wise
    Cvec<T,n> r = Cvec<T,n>();
    for (int i = 0; i < n; ++i) {
      r[i] = d_[i] * v[i];
    }
    return r;
  }
  
  Cvec operator / (const Cvec& v) { //element-wise
    Cvec<T,n> r = Cvec<T,n>();
    for (int i = 0; i < n; ++i) {
      r[i] = d_[i] / v[i];
    }
    return r;
  }
  
  Cvec& operator *= (const Cvec& v) { //element-wise
    for (int i = 0; i < n; ++i) {
      d_[i] *= v[i];
    }
    return *this;
  }
  
  Cvec& operator /= (const Cvec& v) { //element-wise
    for (int i = 0; i < n; ++i) {
      d_[i] *= 1/v[i];
    }
    return *this;
  }

  Cvec operator + (const Cvec& v) const {
    return Cvec(*this) += v;
  }

  Cvec operator - (const Cvec& v) const {
    return Cvec(*this) -= v;
  }

  Cvec operator * (const T a) const {
    return Cvec(*this) *= a;
  }

  Cvec operator / (const T a) const {
    return Cvec(*this) /= a;
  }

  Cvec& normalize() {
    return *this /= std::sqrt(dot(*this, *this));
  }
};

template<typename T>
inline Cvec<T,3> cross(const Cvec<T,3>& a, const Cvec<T,3>& b) {
  return Cvec<T,3>(a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}

template<typename T, int n>
inline T dot(const Cvec<T,n>& a, const Cvec<T,n>& b) {
  T r(0);
  for (int i = 0; i < n; ++i) {
    r += a[i]*b[i];
  }
  return r;
}

template <typename T, int n> //element-wise
inline Cvec<T,n> sin(const Cvec<T,n> a) {
  Cvec<T,n> r = Cvec<T,n>();
  for (int i = 0; i < n; ++i) {
    r[i] = sin(a[i]);
  }
  return r;
}

template <typename T, int n> //element-wise
inline Cvec<T,n> mod(const Cvec<T,n> a, const T x) {
  Cvec<T,n> r = Cvec<T,n>();
  for (int i = 0; i < n; ++i) {
    r[i] = fmod(a[i], x);
  }
  return r;
}

template <typename T, int n> //element-wise
inline Cvec<T,n> abs(const Cvec<T,n> a) {
  Cvec<T,n> r = Cvec<T,n>();
  for (int i = 0; i < n; ++i) {
    r[i] = fabs(a[i]);
  }
  return r;
}

template <typename T, int n> //element-wise
inline T max(const Cvec<T,n> a) {
  return max(max(a[0],a[1]),a[2]);
}

template <typename T, int n> //element-wise
inline T min(const Cvec<T,n> a) {
  return min(min(a[0],a[1]), a[2]);
}

template <typename T, int n> //element-element
inline Cvec<T,n> max(const Cvec<T,n> a, const Cvec<T,n> b) {
  Cvec<T,n> r = Cvec<T,n>();
  for (int i = 0; i < n; ++i) {
    r[i] = std::max(a[i], b[i]);
  }
  return r;
}

template <typename T, int n> //element-element
inline Cvec<T,n> min(const Cvec<T,n> a, const Cvec<T,n> b) {
  Cvec<T,n> r = Cvec<T,n>();
  for (int i = 0; i < n; ++i) {
    r[i] = std::min(a[i], b[i]);
  }
  return r;
}

template<typename T, int n>
inline T norm2(const Cvec<T, n>& v) {
  return dot(v, v);
}

template<typename T, int n>
inline T norm(const Cvec<T, n>& v) {
  return std::sqrt(dot(v, v));
}

template<typename T, int n> //no modification
inline Cvec<T, n> normalize(const Cvec<T,n>& v) {
  return v / norm(v);
}

template<typename T, int n>
inline void printVec(const Cvec<T,n>& v) {
  for (int i = 0; i < n; ++i) {
    std::cout << v[i] << "  ";
  }
  std::cout << "\n";
}

typedef Cvec <double, 2> Cvec2;
typedef Cvec <double, 3> Cvec3;
typedef Cvec <double, 4> Cvec4;

typedef Cvec <float, 2> Cvec2f;
typedef Cvec <float, 3> Cvec3f;
typedef Cvec <float, 4> Cvec4f;

typedef Cvec <unsigned char, 2> Cvec2ub;
typedef Cvec <unsigned char, 3> Cvec3ub;
typedef Cvec <unsigned char, 4> Cvec4ub;

#endif