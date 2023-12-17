#ifndef PUNTO_H
#define PUNTO_H

#include "TObject.h"

class Punto: public TObject
{
  
public:
  
  Punto();
  Punto(double phi, double Z);
  Punto(const Punto& source);  
  virtual ~Punto();

  Punto& operator=(const Punto& source);
  
  double getPhi() const;
  double getZ() const;
  void smear(double, double, double, double *) const;
  void setCoord(double, double);
  
  
private:
  
  double fphi;
  double fZ;
  
  ClassDef(Punto,1)
};

#endif 
