/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * sdEstimators.h
 *
 * (signed) distance estimation functions for geometries
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef path_tracer_sdEstimators_h
#define path_tracer_sdEstimators_h

using namespace std;

float sdBox(Cvec3 p, Cvec3 s) {
  return max(abs(p) - s);
}

float sdSphere(Cvec3 p, float r) {
  return norm(p) - r;
}

float sdPlane(Cvec3 p, float y) {
  return norm(Cvec3(p[0], y, p[2]) - p);
}

#endif