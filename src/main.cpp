/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * a brute force implementation of cpu-based path tracing:
 * largely inspired by research from Iñigo Quilez
 *
 * cvec.h adapted from Steven J. Gortler (Harvard CS175)
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <fstream>
#include <vector>

#include "cvec.h"

using namespace std;

float WIDTH = 512.;
float HEIGHT = 512.;
float AR = WIDTH/HEIGHT;
Cvec3 sunp = Cvec3(15,10,-15);

float randf() {
  return (rand() % 10000) * 1./10000;
}

Cvec3 lambDirection(Cvec3 n) {
  float theta = 6.283185 * randf();
  float y = randf();
  y = 2.0 * y - 1.0;
  
  return normalize( n + Cvec3(Cvec2(cos(theta), sin(theta)) * sqrt(1.0-y*y), y) );
}

struct Ray {
  Cvec3 o_, d_;
  Ray(Cvec3 o, Cvec3 d){
    o_ = o, d_ = d;
  }
};

Ray cRay(Cvec3 ro, Cvec3 la, Cvec2 pc, float zm) {
  Cvec3 cf = normalize(la - ro);
  Cvec3 cr = cross(Cvec3(0., 1., 0.), cf);
  Cvec3 cu = cross(cf, cr);
  Cvec3 rd = normalize(cf*zm + cr*pc[0] + cu*pc[1]);
  
  return Ray(ro, rd);
}

float sdBox(Cvec3 p, Cvec3 s) {
  return max(abs(p) - s);
}

float sdSphere(Cvec3 p, Cvec3 c, float r) {
  return norm(p - c) - r;
}

float sdPlane(Cvec3 p, float y) {
  return norm(Cvec3(p[0], y, p[2]) - p);
}

float map(Cvec3 p) {
  float box_a = sdBox(p, Cvec3(.5));
  float sp_a = sdSphere(p, Cvec3(0.), 1.);
  float sp_b = sdSphere(p, Cvec3(1.8,0.,-1.8), 1.);
  float pl_a = sdPlane(p, -1.);
  
//  return box_a;
  return min(sp_a, min(pl_a, sp_b)); //union
//  return max(sp_a, -sp_b); //subtraction
//  return max(sp_a, sp_b); //intersection
}

Cvec3 eNormal(Cvec3 p) { //so cool!
  float e = 0.0001;
  return normalize(Cvec3(
    map(Cvec3(p[0] + e, p[1], p[2])) - map(Cvec3(p[0] - e, p[1], p[2])),
    map(Cvec3(p[0], p[1] + e, p[2])) - map(Cvec3(p[0], p[1] - e, p[2])),
    map(Cvec3(p[0], p[1], p[2] + e)) - map(Cvec3(p[0], p[1], p[2] - e))
  ));
}

float intersect(Cvec3 ro, Cvec3 rd) {
  float dist = -1.0;
  
  float tmax = 16.0;
  float t = 0.01;
  int max_iter = 128;
  
  for (int i = 0; i < max_iter; i++ ) {
    float h = map(ro + rd*t);
    if (h<0.0001 || t>tmax) break;
    t += h;
  }
  
  if (t < tmax) dist = t;
  return dist;
}

bool shadow(Cvec3 ro, Cvec3 rd) { //0 in shadow
  bool shad = 0;
  
  float tmax = 16.0;
  float t = 0.01;
  int max_iter = 128;
  
  for (int i = 0; i < max_iter; i++ ) {
    float h = map(ro + rd*t);
    if (h<0.0001 || t>tmax) break;
    t += h;
  }
  
  if (t > tmax) shad = 1;
  return shad;
}

Cvec3 applyLighting(Cvec3 p, Cvec3 n) {
  Cvec3 scol = Cvec3(255.);
  Cvec3 lightray = normalize(sunp - p);
  float ndl = max(dot(lightray, n), 0.0);
  float inShadow = shadow(p, lightray);
  
  return scol * ndl * inShadow;
}

Cvec3 renderColor(Ray r, float nb) {
  Cvec3 tc = Cvec3(0.);
  Cvec3 mask = Cvec3(1.0);
  float atten = .8 ; //value < 1.0 for i.e. a white surface color
  
  for (int b = 0; b < nb; ++b) {
    float march = intersect(r.o_, r.d_);
    
    if (march < 0) {
      if (b == 0) return Cvec3(255.);
      else break;
    }
    
    Cvec3 pos = r.o_ + r.d_ * march;
    Cvec3 nor = eNormal(pos);
    
    Cvec3 cs = Cvec3(255.);
    Cvec3 cd = applyLighting(pos, nor);
    for (int i = 0 ; i < 3; ++i) {
      mask[i] *= atten * cs[i] / 255.;
      tc[i] += mask[i] * cd[i];
    }
    
    r.o_ = pos;
    r.d_ = lambDirection(nor);
  }
  
  return tc;
}

Cvec3 calcPixelColor(int x, int y) {
  float nRays = 1.; //256.
  float nBounces = 4.;
  
  Cvec3 col = Cvec3();
  for (int i = 0; i < nRays; ++i) { //upper-right antialias
    Cvec2 pc = Cvec2((2.*(x+randf())/WIDTH - 1.) * AR, -(2.*(y+randf())/HEIGHT - 1.)); //[-1., 1.]
    Cvec3 ro = Cvec3(0., 1., -3.5);
    Cvec3 la = Cvec3(0., 0., 0.);
    float zoom = 1.;

    Ray ray = cRay(ro, la, pc, zoom);
    Cvec3 tempcol = renderColor(ray, nBounces);

    col += tempcol;
  }
  
  return col / nRays;
}

int main(int argc, const char * argv[]) {
  srand(0);
  ofstream out("out.ppm");
  out << "P3\n" << WIDTH << ' ' << HEIGHT << ' ' << "255\n";
  
  int i_report = 10;
  for (int i = 0; i < HEIGHT; ++i) {
    if (i % i_report == 0) cout << "row #" << i << " out of " << HEIGHT << "\n";
    for (int j = 0; j < WIDTH; ++j) {
      Cvec3 col = calcPixelColor(j, i);
      out << col[0] << ' ' << col[1] << ' ' << col[2] << '\n';
    }
  }
}