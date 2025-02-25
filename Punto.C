#include "TObject.h"
#include "Punto.h"
#include "TMath.h"
#include "TRandom3.h"
#include <Riostream.h>

ClassImp(Punto)

//_________________________________________________________________
Punto::Punto():TObject(),
  fphi(0.),
  fZ(0.)
{
   // default constructor
 }

//_________________________________________________________________
Punto::Punto(double phi, double Z):TObject(),
 fphi(phi),
 fZ(Z){
  //standard constructor 
}	     

//_________________________________________________________________
Punto::Punto(const Punto &source):
TObject(source),
  fphi(source.fphi),
  fZ(source.fZ)
{
  cout << "copy constructor" << endl;
}

//_________________________________________________________________
Punto::~Punto()	 {
  // destructor
}

Punto& Punto::operator=(const Punto& source)
{
  //assignment operator                                                  
  if (this == &source) return *this;
  cout << " assignment operator " << this << endl;
  this->~Punto();
  new(this) Punto(source);
  cout << " assignment operator 2 " << this << endl;
  return *this;
}

//_________________________________________________________________
//_________________________________________________________________
double Punto::getPhi() const
{
  return fphi;
}

//_________________________________________________________________
double Punto::getZ() const
{
  return fZ;
}

//_________________________________________________________________
void Punto::setCoord(double phi, double z)
{
  fphi = phi;
  fZ = z;
  return;
}

//_________________________________________________________________
void Punto::smear(double sigZ, double sigPh, double R, double *Prec)  const
{
  double a = gRandom->Gaus(0, sigPh);
  Prec[1] = fZ + gRandom->Gaus(0, sigZ);
  Prec[0] = fphi + a/R;
  if (Prec[0] > 2*TMath::Pi()) Prec[0]-=2*TMath::Pi();
  if (Prec[0] < 0) Prec[0]+=2*TMath::Pi();
  
  return;	
}
