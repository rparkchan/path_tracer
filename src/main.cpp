/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * main.cpp
 *
 * a brute force implementation of cpu-based path tracing:
 * largely derived from Iñigo Quilez
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "cvec.h"
#include "sdEstimators.h"

using namespace std;
using namespace std::chrono;

const float WIDTH = 1024;
const float HEIGHT = 512;
const float AR = WIDTH/HEIGHT;

const Cvec3 sunp = Cvec3(15,15,-15);
const Cvec3 suncol = Cvec3(255., 255., 255.)* 1.9;

float randf() {
  return static_cast<float> (rand() / static_cast<float>((RAND_MAX)));
}

Cvec3 lambDirection(Cvec3 n) { // from "http://www.amietia.com/lambertnotangent.html"
  float theta = randf()*6.283185;
  float y = randf()*2. - 1.;
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

float map(Cvec3 p) {
  return sdSphere(Cvec3(fmod(p[0],3.),fmod(p[1],4.),p[2]), 1.);
}

Cvec3 eNormal(Cvec3 p) {
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

float shadow(Cvec3 ro, Cvec3 rd) { //0 in shadow
  float shad = 0;
  
  float tmax = 16.0;
  float t = 0.01;
  int max_iter = 128;
  
  for (int i = 0; i < max_iter; i++ ) {
    float h = map(ro + rd*t);
    if (h<0.0001 || t>tmax) break;
    t += h;
  }
  
  if (t > tmax) shad = 1.;
  return shad;
}

Cvec3 applyLighting(Cvec3 p, Cvec3 n) {
  Cvec3 lightray = normalize(sunp - p); //point light
  float ndl = max(dot(lightray, n), 0.0);
  float inShadow = shadow(p, lightray);
  
  return suncol * ndl * inShadow;
}

Cvec3 renderColor(Ray r, float nb) {
  Cvec3 tc = Cvec3(0.);
  Cvec3 mask = Cvec3(1.0);
  float atten = 1. ; //value < 1.0 for i.e. Cvec3(255.)
  
  for (int b = 0; b < nb; ++b) {
    float march = intersect(r.o_, r.d_);
    
    if (march < 0) {
      if (b == 0) return Cvec3(255.);
      else break;
    }
    
    Cvec3 pos = r.o_ + r.d_ * march;
    Cvec3 nor = eNormal(pos);
    
    Cvec3 scol = Cvec3(.48, .48, .48)*255.;
    Cvec3 dcol = applyLighting(pos, nor);
    
    mask *= (scol * atten/255.);
    tc += (mask * dcol);
    
    r.o_ = pos;
    r.d_ = lambDirection(nor);
  }
  
  return min(tc, Cvec3(255.)); //stupid hack for blinding light!
}

Cvec3 calcPixelColor(int x, int y) {
  float nRays = 1.; //256.
  float nBounces = 3.;
  
  Cvec3 col = Cvec3(0.);
  for (int i = 0; i < nRays; ++i) { //upper-right antialias
    Cvec2 pcoord = Cvec2((2.*(x+randf())/WIDTH - 1.) * AR, -(2.*(y+randf())/HEIGHT - 1.)); //[-1., 1.]
    Cvec3 ro = Cvec3(0., 5., 5.); // origin
    Cvec3 la = Cvec3(0, 0, 0); // lookat
    float zoom = 1.;

    Ray ray = cRay(ro, la, pcoord, zoom);
    Cvec3 tempcol = renderColor(ray, nBounces);

    col += tempcol;
  }
  
  return col / nRays;
}

int main(int argc, const char * argv[]) {
  srand(0);
  ofstream out("out.ppm");
  out << "P3\n" << WIDTH << ' ' << HEIGHT << ' ' << "255\n";
  
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  
  int i_report = 10;
  for (int i = 0; i < HEIGHT; ++i) {
    if (i % i_report == 0) cout << "row #" << i << " out of " << HEIGHT << "\n";
    for (int j = 0; j < WIDTH; ++j) {
      Cvec3 pix_col = calcPixelColor(i, j);
      out << pix_col[0] << ' ' << pix_col[1] << ' ' << pix_col[2] << '\n';
    }
  }
  
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  cout << "time elapsed: " << duration_cast<milliseconds>( t2 - t1 ).count() << " milliseconds!\n";
}
