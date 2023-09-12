#include <Riostream.h>
#include <TRandom3.h>
#include <TMath.h>
#include "Part.h" //NB tra coppia di apici

ClassImp(Part) 

//_________________________________________________________________
     Part::Part(): TObject(),  //il primo è lo scope operator e non è omettibile
  fX(0.),
  fY(0.),
  fZ(0.),
  fTh(0.),
  fPh(0.),
  fP(0.),
  fC(0.),
  fInd(0),
  fIndEv(0)
{
  //default constructor
}

//_________________________________________________________________
Part::Part(double x, double y, double z, double th, double ph, double P, double C, int i, int j): TObject(),
  fX(x),
  fY(y),
  fZ(z),
  fTh(th),
  fPh(ph),
  fP(P),
  fC(C),
  fInd(i),
  fIndEv(j)
{
  //standard constructor
}

//_________________________________________________________________
Part::Part(const Part &source):
  TObject(source),  
  fX(source.fX),
  fY(source.fY),
  fZ(source.fZ),
  fTh(source.fTh),
  fPh(source.fPh),
  fP(source.fP),
  fC(source.fC),
  fInd(source.fInd),
  fIndEv(source.fIndEv)
{
  //copy constructor
}

//_________________________________________________________________
Part::~Part()
{
  // Default destructor
}

//_________________________________________________________________
Part& Part::operator=(const Part& source)
{
  //assignment operator
  if (this == &source) return *this;
  cout << " assignment operator " << this << endl;
  this->~Part();
  new(this) Part(source);
  cout << " assignment operator 2 " << this << endl;
  return *this;
}

//_________________________________________________________________
void Part::setX(double x)
{
  fX = x; //nome data member deve essere diverso dal nome della variabile argomento
  return;
}
double Part::getX() const
{
  return fX;
}
void Part::setY(double y)
{
  fY = y;
  return;
}
double Part::getY() const
{
  return fY;
}
void Part::setZ(double z)
{
  fZ = z;
  return;
}
double Part::getZ() const
{
  return fZ;
}
void Part::setTh(double th)
{
  fTh = th;
  return;
}
double Part::getTh() const
{
  return fTh;
}
void Part::setPh(double ph)
{
  fPh = ph;
  return;
}
double Part::getPh() const
{
  return fPh;
}
void Part::setPos(double x, double y, double z)
{
  fX = x;
  fY = y;
  fZ = z;
  return;
}
void Part::setDir(double th, double ph)
{
  fTh = th;
  fPh = ph;
  return;
}
void Part::Hit(double kRad, double *Hit) const
{
  
  double c1 = TMath::Sin(fTh)*TMath::Cos(fPh);
  double c2 = TMath::Sin(fTh)*TMath::Sin(fPh);
  double c3 = TMath::Cos(fTh);

  double t1 = (-(fX*c1+fY*c2)+sqrt(pow(fX*c1+fY*c2,2)-(c1*c1+c2*c2)*(fX*fX+fY*fY-kRad*kRad)))/(c1*c1+c2*c2);
  double t2 = (-(fX*c1+fY*c2)-sqrt(pow(fX*c1+fY*c2,2)-(c1*c1+c2*c2)*(fX*fX+fY*fY-kRad*kRad)))/(c1*c1+c2*c2);
  double t;
  if (t1>t2) t=t1;
  else t = t2;
  
  Hit[0] = fX+c1*t;
  Hit[1] = fY+c2*t;
  Hit[2] = fZ+c3*t;
  
  return;
}
void Part::scat(double W, double LR, double *ang) const
{
  double cd[3];
  double mat[3][3];

  mat[0][0] = -TMath::Sin(fPh);
  mat[1][0] = TMath::Cos(fPh);
  mat[2][0] = 0;
  mat[0][1] = -TMath::Cos(fPh)*TMath::Cos(fTh);
  mat[1][1] = -TMath::Sin(fPh)*TMath::Cos(fTh);
  mat[2][1] = TMath::Sin(fTh);
  mat[0][2] = TMath::Sin(fTh)*TMath::Cos(fPh);
  mat[1][2] = TMath::Sin(fTh)*TMath::Sin(fPh);
  mat[2][2] = TMath::Cos(fTh);
  
  double Sth1 = sqrt(2)*0.0136*fC*sqrt(W/LR)*(1+0.038*log(W/LR))/fP;
  double ph1 = gRandom->Uniform(2*TMath::Pi());
  double th1 = gRandom->Gaus(0,Sth1);

  double vet[3];
  vet[0] = TMath::Sin(th1)*TMath::Cos(ph1);  
  vet[1] = TMath::Sin(th1)*TMath::Sin(ph1);
  vet[2] = TMath::Cos(th1);

  for (int i=0; i<3; i++){
    cd[i] = 0.;
    for (int j=0; j<3; j++){
      cd[i]+=mat[i][j]*vet[j];
    }
  }

  if (cd[0] != 0) ang[1] = atan(cd[1]/cd[0]);
  else if (cd[1] > 0) ang[1] = TMath::Pi()/2;
  else if (cd[1] < 0) ang[1] = -TMath::Pi()/2;
  else ang[1] = 0.; // indeterminato
  
  double w = gRandom->Uniform();
  if (w < 0.5) ang[1] += TMath::Pi();
  else if (ang[1] < 0) ang[1] += 2*TMath::Pi();

  if (cd[2] > 1.) ang[0] = 0.;
  else if (cd[2] < -1.) ang[0] = TMath::Pi();
  else ang[0] = acos(cd[2]);

  return;
}

void Part::printPart() const
{
  cout << " Particella di carica Z = " << fC << endl;
  cout << " che parte dal punto di coordinate: [" << fX << ", " << fY << ", " << fZ << "] " << endl;
  cout << " procede in direzione theta: " << fTh << " e phi: " << fPh << endl;
  cout << " con momento P = " << fP << endl;
  cout << " indice: " << fInd << endl;
  cout << " e indice evento: " << fIndEv << endl;
  return;
}
