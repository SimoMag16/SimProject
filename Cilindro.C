#include <Riostream.h>
#include <TMath.h>
#include <TString.h>
#include "Cilindro.h" //NB tra coppia di apici

ClassImp(Cilindro) //solo per root 
 Cilindro::Cilindro(): TObject(),  //il primo è lo scope operator e non è omettibile
  fR(0.),
  fH(0.),
  fG(0.)
{
  //default constructor
}

Cilindro::Cilindro(double R, double H, double G): TObject(),  
  fR(R),
  fH(H),
  fG(G)
{
  //standard constructor
}


Cilindro::Cilindro(const Cilindro &source):
TObject(source),  
  fR(source.fR),
  fH(source.fH),
  fG(source.fG)
{
  cout << "copy constructor" << endl;
}

Cilindro::~Cilindro()
{
  //default destructor
  cout << " destructor " << endl; 
}
//scrittura standard dell'assignment operator

Cilindro& Cilindro::operator=(const Cilindro& source)
{
  //assignment operator
  if (this == &source) return *this;
  cout << " assignment operator " << this << endl;
  this->~Cilindro();
  new(this) Cilindro(source);
  cout << " assignment operator 2 " << this << endl;
  return *this;
}


double Cilindro::getR() const
{
return fR;
}
double Cilindro::getH() const
{
return fH;
}
double Cilindro::getG() const
{
return fG;
}
void Cilindro::print() const
{
  cout << "Cilindro di raggio: " << fR << ", estensione: " << fH << ", e spessore: "  << fG <<  endl; 
 return;
}
