#ifndef _EC_POINT_H
#define _EC_POINT_H

#include <NTL/ZZ.h>
using namespace NTL;

class ECpoint {
private:
   ZZ x, y, z;

public:
  ECpoint();
  ECpoint(ZZ, ZZ);
  ECpoint(ZZ, ZZ, ZZ);
  void printPoint();
  void printlnPoint();
  ZZ getX();
  ZZ getY();
  ZZ getZ();
  bool isInfinite();
  bool isSpecial();
  bool isOppositeTo(ECpoint *);
  bool isEqualTo(ECpoint *);
  ECpoint * copy();
};

#endif
