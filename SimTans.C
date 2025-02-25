#include <Riostream.h>
#include <TStopwatch.h>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TClonesArray.h>

#include <TH1D.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TString.h>

#include "Punto.h"
#include "Cilindro.h"
#include "Part.h"

double theta(double eta);

/*
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//BOOLEANI
bool doScat = true;
bool doIf = true; //se è true, si tiene conto dell'accettanza del rivelatore
bool multUniform = true; //se è true, la molteplicità è uniforme
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*/


void SimTans(int kNEv, TString fSimName, bool doScat, bool doIf, bool multUniform, bool zUniform, unsigned int seed = 347){

    TStopwatch timer;
    timer.Start();
  
    //costanti
    const double kRBP = 3; //cm
    const double kR1 = 4; //cm
    const double kR2 = 7; //cm
    const double kH = 27; //cm
    const double kSx = 0.01; //cm
    const double kSy = 0.01; //cm
    const double kSz = 5.3; //cm
    const double kWBP = 0.08; //cm
    const double kWRiv = 0.02; //cm
    const double kLRbe = 35.28; //cm
    const double kLRsi = 9.37; //cm
  
    gRandom->SetSeed(seed);
  
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Apertura file di input: cinematica
    TFile hfileIn("kinem.root", "READ"); 
    TH1D *hm = (TH1D*) hfileIn.Get("hm");
    TH1D *heta2 = (TH1D*) hfileIn.Get("heta2");

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Apertura del file di output
    TFile hfile(fSimName,"RECREATE"); //Tree che contiene le hits

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //dichiarazione del TTree
    TTree *tree = new TTree("T","TTree Simulazione");

    //il layer 0 è la beampipe, non è sensibile, per cui non salvo i dati
    //Dichiarazione di un TClonesArray per il layer 1
    TClonesArray *ptrhits1 = new TClonesArray("Punto",100); //la dimensione deve riflettere la molteplicità media
    TClonesArray &hits1 = *ptrhits1; //definisco un alias
    //Dichiarazione di un TClonesArray per il layer 2
    TClonesArray *ptrhits2 = new TClonesArray("Punto",100); //la dimensione deve riflettere la molteplicità media
    TClonesArray &hits2 = *ptrhits2; //definisco un alias

    //Definizione di una struct per il vertice simulato
    typedef struct{
        double X,Y,Z;
        int mult;} VTX;

    static VTX Vertex;
    
    //dichiarazione dei branch del tree
    tree->Branch("VertMult",&Vertex.X,"X/D:Y:Z:mult/I"); //contiene il vertice e la molteplicità
    tree->Branch("Hits1",&ptrhits1); //contiene le hits sul layer 1
    tree->Branch("Hits2",&ptrhits2); //contiene le hits sul layer 2

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Definizione istogrammi per l'analisi
    TH1D *hmulSim = new TH1D("hmulSim", "molteplicita' simulata", 60, 0, 60);
    TH1D *hVertSZ = new TH1D("hVertSZ", "", 10, -20, 20);

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //creazione apparato sperimentale
    Cilindro *BP = new Cilindro(kRBP, 1e10, kWBP); //la BP ha estensione infinita
    Cilindro *Riv1 = new Cilindro(kR1, kH, kWRiv);
    Cilindro *Riv2 = new Cilindro(kR2, kH, kWRiv);

    BP->print();
    Riv1->print();
    Riv2->print();

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Simulazione: loop sugli eventi

    cout<<"+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"<<endl;
    for (int iev=0; iev<kNEv; iev++){

        cout<<"---------------------evento "<<iev<<"------------------------------"<<endl;
        cout<<" "<<endl;
    
        //generazione molteplicità
        double N;
        if (multUniform) N = gRandom->Uniform(0, 60);
        else N = hm->GetRandom();

        double n = round(N); //o floor
        int np = static_cast<int>(n);
        Vertex.mult=np;
        cout<<"molteplicita' "<<np<<endl;
        hmulSim->Fill(np);

        double z0;
    
        if (zUniform) {
            //generazione uniforme del vertice
            z0 = gRandom->Uniform(-4*kSz,4*kSz); // cm
        }
        else {
            //generazione Gaussiana del vertice
            z0 = gRandom->Gaus(0,kSz); // cm
        }

        double x0 = gRandom->Gaus(0,kSx); // cm
        double y0 = gRandom->Gaus(0,kSy); // cm

        Vertex.X=x0;
        Vertex.Y=y0;
        Vertex.Z=z0;

        hVertSZ->Fill(z0);

        //loop sulle particelle in un evento
        for (int i=0; i<np; i++){

            //genero le caratteristiche della particella:
            //impulso di una particella
            double P = gRandom->Uniform(0.6, 0.8); //GeV/c
            //direzione iniziale
            double ph = gRandom->Uniform(2*TMath::Pi());
            double th = theta(heta2->GetRandom()); //converte la pseudorapidita' in angolo
      
            //definisco la particella
            Part *Part0 = new Part(x0, y0, z0, th, ph, P, 1, i, iev);

            //Collisione con BP
            double Hit0[3]; //creato nello stack
            Part0->Hit(kRBP, Hit0); //trasporto la particella alla  BP
            //Part0->setPos(Hit0[0], Hit0[1], Hit0[2]);
            
            double ang0[2];
            if (doScat){
	            Part0->scat(kWBP, kLRbe, ang0); //multiple scattering
	            Part0->setDir(ang0[0], ang0[1]);
            }
     
            //Collisione con Riv1
            double Hit1[3];
            Part0->Hit(kR1, Hit1); //trasporto la particella a Riv1
            if (TMath::Abs(Hit1[2]) > kH/2 && doIf) {
	            double x1 = 1e10;
	            double y1 = 1e10;
    	        double z1 = 1e10;
                double phi1 = 1e10;
	            new(hits1[i])Punto(phi1, z1); //salvo la hit errata col layer
	            //Part0->setPos(Hit1[0], Hit1[1], Hit1[2]);
            }
            else{
	            double z1 = Hit1[2];
	            double phi1 = Part0->getPh();
	            new(hits1[i])Punto(phi1, z1); //salvo la hit col layer
            	//Part0->setPos(Hit1[0], Hit1[1], Hit1[2]);
            }
       
            double ang1[2];
            if (doScat){
	            Part0->scat(kWRiv, kLRsi, ang1);
	            Part0->setDir(ang1[0], ang1[1]);
            }
           
            //Collisione con Riv2
            double Hit2[3];
            Part0->Hit(kR2, Hit2);
            if (TMath::Abs(Hit2[2]) > kH/2 && doIf) {
	            double z2 = 1e10;
	            double phi2 = 1e10;
	            new(hits2[i])Punto(phi2, z2);
	            //Part0->setPos(Hit2[0], Hit2[1], Hit2[2]);
            }
            else{
	            double z2 = Hit2[2];
            	double phi2 = Part0->getPh();
	            new(hits2[i])Punto(phi2, z2);
	            //Part0->setPos(Hit2[0], Hit2[1], Hit2[2]);
            }

            delete Part0;
            Part0=nullptr;
        }

        cout<<"________________________________________________________________"<<endl;
        cout<<" "<<endl;
    
        tree->Fill();
        ptrhits1->Clear();
        ptrhits2->Clear();
    }

    delete BP;
    delete Riv1;
    delete Riv2;
  
    // Save all objects in this file
    hfile.Write();
  
    // Close the file. 
    hfile.Close();


    //Stampa tempo di esecuzione
    cout<<"+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"<<endl;
    cout<<"Tempo necessario a simulare "<<kNEv<<" eventi:"<<endl;
    timer.Stop();
    timer.Print();
    cout << "Size: " << hfile.GetBytesWritten()*1e-6 << " MB" << endl;
}
  
double theta(double eta){
    return 2*atan(exp(-eta));  
}
