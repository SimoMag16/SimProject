#ifndef Track_h
#define Track_h

#include "TObject.h"

class Track : public TObject
{
public:
  Track();
  Track(double z1, double z2, int i, int j);
  Track(const Track& source);
  virtual ~Track();
  Track& operator=(const Track& source);
  
//metodi

  void GetPar(double *) const;
  void printTrack() const;

private:
  double fZ1;
  double fZ2;
  int fInd;
  int fIndEv;
  ClassDef(Track,1)
};

#endif
