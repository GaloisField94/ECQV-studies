#include "ECpoint.h"

using namespace NTL;

ECpoint::ECpoint() { //point in infinity
  this->x = 0;
  this->y = 0;
  this->z = 0;
}

ECpoint::ECpoint(ZZ a, ZZ b) {
  this->x = a;
  this->y = b;
  this->z = 1;
}

ECpoint::ECpoint(ZZ a, ZZ b, ZZ c) {
  this->x = a;
  this->y = b;
  this->z = c;
}

void ECpoint::printPoint() {
  if(this->isInfinite()) std::cout << "inf";
  else std::cout << "(" << this->x << "," << this->y << ")";
}

void ECpoint::printlnPoint() {
  if(this->isInfinite()) std::cout << "inf\n";
  else std::cout << "(" << this->x << "," << this->y << ")\n";
}

ZZ ECpoint::getX() {
  return this->x;
}

ZZ ECpoint::getY() {
  return this->y;
}

ZZ ECpoint::getZ() {
  return this->z;
}

bool ECpoint::isInfinite() {
  if(this->z == 0) return true;
  else return false;
}

bool ECpoint::isSpecial() {
  if(this->isInfinite()) return false;
  else if(this->y == 0) return true;
  else return false;
}

bool ECpoint::isOppositeTo(ECpoint *a){
  if(this->isInfinite())
    if(a->isInfinite()) return true;
    else return false;
  else
    if(a->isInfinite()) return false;
    else
      if(this->x != a->getX()) return false;
      else
        if(!(this->isEqualTo(a))) return true;
        else return false;
}

bool ECpoint::isEqualTo(ECpoint *a) {
  if(this->isInfinite())
    if(a->isInfinite()) return true;
    else return false;
  else
    if(a->isInfinite()) return false;
    else
      if(this->x == a->getX())
        if(this->y == a->getY()) return true;
        else return false;
      else return false;
}

ECpoint * ECpoint::copy() {
  return new ECpoint(this->x, this->y, this->z);
}
