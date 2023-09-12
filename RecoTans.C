#include <Riostream.h>
#include <TStopwatch.h>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TClonesArray.h>

#include <TH1D.h>
#include <TRandom3.h>
#include <TString.h>

#include "Punto.h"
#include "Track.h"

void RecoTans(TString fSimName, TString fSmearName, TString fRecoName, double d_phi=0.01){

  TStopwatch timer;
  timer.Start();

  const double kSigmaZ  = 0.012; //cm
  const double kSigmaXY = 0.003; //cm
  
  const double kR1 = 4; //cm
  const double kR2 = 7; //cm

  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  // Definizione di una struct per il vertice simulato
  typedef struct{
    double X,Y,Z;
    int mult;} VTX;

  // Definizione di una struct per il vertice ricostruito
  typedef struct{
    double Z;} VTX_R;
  
  static VTX   VertexS; //simulo le 3 coord e la molteplicità
  static VTX_R VertexR; //ricostruisco solo la Z
    
  //Dichiarazione di un TClonesArray per il layer 1
  TClonesArray *hitsSmeared1 = new TClonesArray("Punto",100); //la dimensione deve riflettere la molteplicità media
  
  //Dichiarazione di un TClonesArray per il layer 2
  TClonesArray *hitsSmeared2 = new TClonesArray("Punto",100); //la dimensione deve riflettere la molteplicità media

  //Apertura file di input: risultato della Simulazione (verità Monte Carlo)
  TFile hfile(fSimName);
  //Lettura TTree  e branch
  TTree *tree = (TTree*)hfile.Get("T");
  TBranch *bSim=tree->GetBranch("VertMult");

  // Definizione degli indirizzi per la lettura dei dati su ttree
  bSim->SetAddress(&VertexS.X);

  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  //Apertura file di input (ovvero, dell'output della smearing)
  TFile hfileSmeared(fSmearName);
  //Lettura TTree  e branch
  TTree *treeS = (TTree*)hfileSmeared.Get("TS");
  TBranch *bL1=treeS->GetBranch("HitsSmeared1");
  TBranch *bL2=treeS->GetBranch("HitsSmeared2");
  // Definizione degli indirizzi per la lettura dei dati su ttree
  bL1->SetAddress(&hitsSmeared1);
  bL2->SetAddress(&hitsSmeared2);

  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  //Apertura del file di output
  TFile hfileReco(fRecoName,"RECREATE"); //Tree che contiene le hits smearate
  //dichiarazione del TTree
  TTree *treeR = new TTree("TR","Ricostruzione");
  
  //dichiarazione dei branch del tree
  treeR->Branch("VertMultReco",&VertexR.Z,"Z/D"); //contiene la Z del vertice ricostruito

  TClonesArray *ptrTracks = new TClonesArray("Track",100); //numero medio di tracce
  TClonesArray &tracks = *ptrTracks; //definisco un alias

  //Definizione istogramma molteplicità ricostruita
  TH1D* hVertRZ = new TH1D("hVertRZ", "Vz ricostruita", 10, -20, 20);
  TH1D* hZmax = new TH1D("hZmax", "", 10, -20, 20);
  TH1D* hmulVertR = new TH1D("hmulVertR", "V ricostruiti vs molt vera", 60, 0, 60);

  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  // loop sugli ingressi nel TTree Smearato
  for(int ev=0;ev<treeS->GetEntries();ev++){
    
    treeS->GetEvent(ev);
    tree->GetEvent(ev);
    
    int num1 = hitsSmeared1->GetEntries();
    int num2 = hitsSmeared2->GetEntries();

    cout<<"---------------------evento "<<ev<<"------------------------------"<<endl;
    cout<<" "<<endl;
    
    int nTrack = 0;
    for (int i=0; i<num1; i++){
      Punto *tst1 = (Punto*)hitsSmeared1->At(i);
      double phi1 = tst1->getPhi();
      double z1 = tst1->getZ();
      if (z1 > 1e9) continue;
      tst1->Clear(); //deallocazione della memoria

      for (int j=0; j<num2; j++){
	Punto *tst2 = (Punto*)hitsSmeared2->At(j);
	double phi2 = tst2->getPhi();
	double z2 = tst2->getZ();
	if (z2 > 1e9) continue;
	tst2->Clear(); //deallocazione della memoria
	
	if (abs(phi1-phi2) > d_phi) continue; //taglio sulla coordinata azimutale
	 new(tracks[nTrack])Track(z1, z2, nTrack, ev);
	 nTrack++;     
      }
    }

    cout<<"numero di tracce costruite "<<nTrack<<endl;
    
    if (nTrack > 1) {
      vector <double> zVcands; //numero medio di candidati vertici
      TH1D hZcomb("hZcomb", "Z candidate", 128, -20, 20);
      int nVcand=0;
      
      for (int k1=1; k1<nTrack; k1++){
	Track *track1 = (Track*)ptrTracks->At(k1);
	double par1[2];
	track1->GetPar(par1);
	double Zinter = -par1[1]/par1[0]; 
	track1->Clear(); //deallocazione della memoria
	
	zVcands.push_back(Zinter);	  
	nVcand++;
	hZcomb.Fill(Zinter);	 
      }     
    
      if (nVcand == 0) {
	VertexR.Z = 1e10;	  
      }
      
      else {
	double Zmax;
	int nOfMax;
	do {
	  nOfMax = 0;
	  int nBinMax = hZcomb.GetMaximumBin();
	  for (int nBin = 0; nBin<hZcomb.GetNbinsX(); nBin++) {
	    if (hZcomb.GetBinContent(nBin) == hZcomb.GetBinContent(nBinMax)) nOfMax++;
	  }
	Zmax = hZcomb.GetXaxis()->GetBinCenter(nBinMax);
	if(nOfMax>1) hZcomb.Rebin(2);
	}while (nOfMax>1);
	
	hZmax->Fill(Zmax);
		
	double ZvDef = 0;
	int nVcandDef = 0;

	double width=hZcomb.GetBinWidth(1);
	for (int k=0; k<nVcand; k++){
          double Vz = zVcands[k];
          if(abs(Vz-Zmax)<width/2.){ 
	    nVcandDef++;
	    ZvDef += Vz;
	  }
        }
	
	if (nVcandDef == 0) {
	  cout << "errore evento "<< ev << endl; //i punti sono troppo separati
	  ZvDef=1e10;
	}
	else ZvDef = ZvDef/nVcandDef;
		
	int m = VertexS.mult;
	hmulVertR->Fill(m, 1); //ogni volta che trovo un vertice, fillo il bin di molteplicita' corrispondente
	VertexR.Z = ZvDef; //e' l'unica coordinata effettivamente ricostruita
	
      }
      if(abs(VertexR.Z)<20) hVertRZ->Fill(VertexR.Z); 
    }
    else { //se abbiamo 0/1 traccia -> non possiamo ricostruire nulla
      VertexR.Z = 1e10;
    }

    cout<<"________________________________________________________________"<<endl;
    cout<<" "<<endl;
    treeR->Fill();
    ptrTracks->Clear();
  }
  
  // Save all objects in this file
  hfileReco.Write();
  
  // Close the file. 
  hfileReco.Close();

  cout<<"+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"<<endl;
  cout<<"Tempo necessario a ricostruire "<<treeS->GetEntries()<<" eventi:"<<endl;
  timer.Stop();
  timer.Print();
  cout << "Size: " << hfileReco.GetBytesWritten()*1e-6 << " MB" << endl;

}
