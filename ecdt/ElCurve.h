#ifndef _EL_CURVE_h
#define _EL_CURVE_h

#include "ECpoint.h"
#include <vector>

using namespace NTL;

class ElCurve {
private:
   ZZ p, a, b;
   std::vector<ECpoint *> points;

public:
  ElCurve(ZZ, ZZ, ZZ);
  void generatePoints();
  void printPoints();
  ZZ getP();
  ZZ getA();
  ZZ getB();
  std::vector<ECpoint *> getPoints();
  ECpoint *doublePoint(ECpoint *);
  ECpoint *addPoints(ECpoint *, ECpoint *);
  ECpoint *mulPoint(ECpoint *, ZZ);

};

#endif
