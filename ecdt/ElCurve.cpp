#include "ElCurve.h"
#include <iostream>

using namespace NTL;

ElCurve::ElCurve(ZZ p, ZZ a, ZZ b) {
  this->p = p;
  this->a = a;
  this->b = b;
  this->generatePoints();
}

void ElCurve::generatePoints() {
  ZZ x = ZZ(0), y;
  this->points.push_back(new ECpoint());
  for(x=0; x < p; x++) {
    AddMod(y, AddMod(PowerMod(x, 3, p), MulMod(x, this->a, p), p), this->b, p);
    if(Jacobi(y, p) == 1) {
      SqrRootMod(y, y, p);
      this->points.push_back(new ECpoint(x, y));
      y = p - y;
      this->points.push_back(new ECpoint(x, y));
    } else if(Jacobi(y, p) == 0) {
      SqrRootMod(y, y, p);
      this->points.push_back(new ECpoint(x, y));
    }
  }
}

void ElCurve::printPoints() {
  size_t n = this->points.size();
  std::cout << "{";
  for(int i=0; i < n; i++) {
    this->points[i]->printPoint();
    std::cout << ", ";
  }
  std::cout << "\b\b}\n";
}

ZZ ElCurve::getP() {
  return this->p;
}

ZZ ElCurve::getA() {
  return this->a;
}

ZZ ElCurve::getB() {
  return this->b;
}

std::vector<ECpoint *> ElCurve::getPoints() {
  return this->points;
}

ECpoint * ElCurve::doublePoint(ECpoint *a) {

  if(a->isInfinite()) return new ECpoint(a->getX(), a->getY());
  else
    if(a->isSpecial()) return new ECpoint();
    else
      if(a->isOppositeTo(a)) return new ECpoint();
      else {
        ZZ alfa, x, y;

        MulMod(alfa, AddMod(MulMod(3, PowerMod(a->getX(), 2, p), p), this->a, p), InvMod(MulMod(a->getY(), 2, p), p), p);
        SubMod(x, PowerMod(alfa, 2, p), AddMod(a->getX(), a->getX(), p), p); //x3 = alfa^2-x1-x2
        SubMod(y, MulMod(alfa, SubMod(a->getX(), x, p), p), a->getY(), p); //y3 = alfa(x1-x3)-y1

        return new ECpoint(x, y);
      }
}

ECpoint * ElCurve::addPoints(ECpoint *a, ECpoint *b) {

  if(a->isInfinite()) return new ECpoint(b->getX(), b->getY());
  else
    if(b->isInfinite()) return new ECpoint(a->getX(), a->getY());
    else
      if(a->isOppositeTo(b)) return new ECpoint();
      else
        if(a->isEqualTo(b)) return doublePoint(a);
        else {
          ZZ alfa, x, y;

          MulMod(alfa, SubMod(b->getY(), a->getY(), p), InvMod(SubMod(b->getX(), a->getX(), p), p), p);
          SubMod(x, PowerMod(alfa, 2, p), AddMod(a->getX(), b->getX(), p), p); //x3 = alfa^2-x1-x2
          SubMod(y, MulMod(alfa, SubMod(a->getX(), x, p), p), a->getY(), p); //y3 = alfa(x1-x3)-y1

          return new ECpoint(x, y);
      }
}

ECpoint * ElCurve::mulPoint(ECpoint *a, ZZ n) {
  ECpoint *result = new ECpoint();
  ECpoint *powered = new ECpoint(a->getX(), a->getY(), a->getZ());
  long bitsInN = NumBits(n);

  for(long i=0; i<bitsInN; i++) {
    if(bit(n, i)==1) result = addPoints(result, powered);
    powered = doublePoint(powered);

  }

  return result;
}
