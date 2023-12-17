#include <Riostream.h>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TClonesArray.h>

#include <TMath.h>
#include <TRandom3.h>
#include <TString.h>

#include "Punto.h"

void SmearTans(TString fSimName, TString fSmearName, unsigned int seed = 347){

  TStopwatch timer;
  timer.Start();

  const double kSigmaZ  = 0.012; //cm
  const double kSigmaPh = 0.003; //cm
  
  const double kH = 27; //cm
  const double kR1 = 4; //cm
  const double kR2 = 7; //cm

  gRandom->SetSeed(seed);
  
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  //Lettura dei file dall'output della simulazione
  typedef struct {
    double X,Y,Z;
    int mult;} VTX;
  static VTX Vertex;
  
  //Dichiarazione di un TClonesArray per il layer 1
  TClonesArray *hits1 = new TClonesArray("Punto",100); //la dimensione deve riflettere la molteplicità media
  
  //Dichiarazione di un TClonesArray per il layer 2
  TClonesArray *hits2 = new TClonesArray("Punto",100); //la dimensione deve riflettere la molteplicità media
  
  //Apertura file di input (ovvero, dell'output della simulazione)
  TFile hfile(fSimName);
  //Lettura TTree  e branch
  TTree *tree = (TTree*)hfile.Get("T");
  TBranch *bV=tree->GetBranch("VertMult");
  TBranch *bL1=tree->GetBranch("Hits1");
  TBranch *bL2=tree->GetBranch("Hits2");
  // Definizione degli indirizzi per la lettura dei dati su ttree
  bV->SetAddress(&Vertex.X);
  bL1->SetAddress(&hits1);
  bL2->SetAddress(&hits2);

  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  //Apertura del file di output
  TFile hfileSmeared(fSmearName,"RECREATE"); //Tree che contiene le hits smearate
  //dichiarazione del TTree
  TTree *treeS = new TTree("TS","TTree con 2 branches");

  //Dichiarazione di un TClonesArray per il layer 1
  TClonesArray *ptrhitsSmeared1 = new TClonesArray("Punto",100); //la dimensione deve riflettere la molteplicità media
  TClonesArray &hitsSmeared1 = *ptrhitsSmeared1; //definisco un alias
  //Dichiarazione di un TClonesArray per il layer 2
  TClonesArray *ptrhitsSmeared2 = new TClonesArray("Punto",100); //la dimensione deve riflettere la molteplicità media
  TClonesArray &hitsSmeared2 = *ptrhitsSmeared2; //definisco un alias
    
  //dichiarazione dei branch del tree
  treeS->Branch("HitsSmeared1",&ptrhitsSmeared1); //contiene le hits smearate sul layer 1
  treeS->Branch("HitsSmeared2",&ptrhitsSmeared2); //contiene le hits smearate sul layer 2

  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  // loop sugli ingressi nel TTree
  for(int ev=0;ev<tree->GetEntries();ev++){
    
    tree->GetEvent(ev);
    cout<<"---------------------------------------------------"<<endl;
    cout<<"Evento "<<ev<<"; Molteplicita'= "<<Vertex.mult<<endl;
      
    int num1 = hits1->GetEntries();
    int num2 = hits2->GetEntries();
    cout<<"Numero di hit col primo layer  "  <<num1<<endl;
    cout<<"Numero di hit col secondo layer  "<<num2<<endl;

    //smearing dei punti del layer 1
    cout<<"Smearing layer 1 ..."<<endl;
    for (int j=0; j<num1; j++){ 
      Punto *tst1 = (Punto*)hits1->At(j);
      double SmearCoord1[2];
      if (tst1->getZ() > 1e9) {
	double phi1 = tst1->getPhi();
	double z1 = tst1->getZ();
	new(hitsSmeared1[j])Punto(phi1, z1);
      }
      else {
	tst1->smear(kSigmaZ, kSigmaPh, kR1, SmearCoord1);
	double phi1 = SmearCoord1[0];
	double z1 = SmearCoord1[1];
	new(hitsSmeared1[j])Punto(phi1, z1);
	tst1->setCoord(SmearCoord1[0], SmearCoord1[1]);
      }
      tst1->Clear();
    }

    cout<<"Smearing layer 2 ..."<<endl;
    for (int j=0; j<num2; j++){ 
      Punto *tst2 = (Punto*)hits2->At(j);
      double SmearCoord2[2];
      if (tst2->getZ() > 1e9) {
	double phi2 = tst2->getPhi();
	double z2 = tst2->getZ();
	new(hitsSmeared2[j])Punto(phi2, z2);
      }
      else {
	tst2->smear(kSigmaZ, kSigmaPh, kR2, SmearCoord2);
	double phi2 = SmearCoord2[0];
	double z2 = SmearCoord2[1];
	new(hitsSmeared2[j])Punto(phi2, z2);
	tst2->setCoord(SmearCoord2[0], SmearCoord2[1]);
      }
      tst2->Clear();
    }
    
    //punti spuri
    cout<<"Punti Spuri layer 1 ..."<<endl;
    double N1 = gRandom->Uniform(0., 2);
    double n1 = round(N1); //o floor
    int nSpuri1 = static_cast<int>(n1);;
    for(int s=0; s<nSpuri1; s++){
      new(hitsSmeared1[s])Punto(gRandom->Uniform(0, 2*TMath::Pi()), gRandom->Uniform(-kH/2, kH/2)); 
    }
    
    cout<<"Punti Spuri layer 2 ..."<<endl;
    double N2 = gRandom->Uniform(0., 2)*kR2/kR1;
    double n2 = round(N2); //o floor
    int nSpuri2 = static_cast<int>(n2);;
    for(int s=0; s<nSpuri2; s++){
      new(hitsSmeared2[s])Punto(gRandom->Uniform(0, 2*TMath::Pi()), gRandom->Uniform(-kH/2, kH/2)); 
    }
    
    cout<<" "<<endl;
    
    treeS->Fill();
    ptrhitsSmeared1->Clear();
    ptrhitsSmeared2->Clear();
    
  }
  
  // Save all objects in this file
  hfileSmeared.Write();
  
  // Close the file. 
  hfileSmeared.Close();
  
  //Stampa tempo di esecuzione
  
  cout<<"+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"<<endl;
  cout<<"Tempo necessario a fare lo smearing di "<<tree->GetEntries()<<" eventi:"<<endl;
  timer.Stop();
  timer.Print();
  cout << "Size: " << hfileSmeared.GetBytesWritten()*1e-6 << " MB" << endl;
}
