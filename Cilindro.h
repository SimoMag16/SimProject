#ifndef Cilindro_h
#define Cilindro_h

#include "TObject.h"

class Cilindro : public TObject
{
public:
  Cilindro(); //primo costruttore di default
  Cilindro(double R, double H, double G); //secondo costruttore
  Cilindro(const Cilindro& source);
  virtual ~Cilindro();

  Cilindro& operator=(const Cilindro& source);
  
//metodi
  double getR() const;
  double getH() const;
  double getG() const;
  void print() const;

private:
  double fR;
  double fH;
  double fG;

  ClassDef(Cilindro,1) 
};

#endif
