#include <Riostream.h>
#include "Track.h" //NB tra coppia di apici

ClassImp(Track) //solo per root

//_________________________________________________________________
     Track::Track(): TObject(),  //il primo è lo scope operator e non è omettibile
  fZ1(0.),
  fZ2(0.),
  fInd(0),
  fIndEv(0)
{
  //default constructor
}

//_________________________________________________________________
Track::Track(double z1, double z2, int i, int j):
  TObject(),
  fZ1(z1),
  fZ2(z2),
  fInd(i),
  fIndEv(j)
{
  //standard constructor
}

//_________________________________________________________________
Track::Track(const Track &source):
  TObject(source),  
  fZ1(source.fZ1),
  fZ2(source.fZ2),
  fInd(source.fInd),
  fIndEv(source.fIndEv)
{
  //copy constructor
}
Track::~Track()
{
  // Default destructor
}

//_________________________________________________________________
Track& Track::operator=(const Track& source)
{
  //assignment operator
  if (this == &source) return *this;
  cout << " assignment operator " << this << endl;
  this->~Track();
  new(this) Track(source);
  cout << " assignment operator 2 " << this << endl;
  return *this;
}

//_________________________________________________________________
//_________________________________________________________________
void Track::GetPar(double *par) const
{
  //parametri della retta nel piano z-r
  //a partire dai raggi dei rivelatori
  par[0] = 3./(fZ2-fZ1);
  par[1] = 4-par[0]*fZ1;
  return;
}

//_________________________________________________________________
void Track::printTrack() const
{
  cout << " che passa per " << fZ1 << " e " << fZ2 << endl;
  cout << " indice: " << fInd << endl;
  cout << " e indice evento: " << fIndEv << endl;
  return;
}
