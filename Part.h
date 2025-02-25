#ifndef Part_h
#define Part_h

#include "TObject.h"

class Part : public TObject
{
public:
  Part();
  Part(double x, double y, double z, double th, double ph, double P, double C, int i, int j);
  Part(const Part& source);
  virtual ~Part();
  Part& operator=(const Part& source);
  
//metodi
  double getTh() const;
  double getPh() const;
  void setPos(double, double, double);
  void setDir(double, double);
  void Hit(double, double *) const;
  void scat(double, double, double *) const;
  void printPart() const;

private:
  double fX; 
  double fY;
  double fZ;
  double fTh;
  double fPh;
  double fP;
  double fC;
  int fInd;
  int fIndEv;
  
  ClassDef(Part,1)
};

#endif
