#include <Riostream.h>
#include <TStopwatch.h>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TClonesArray.h>

#include <TAxis.h>   
#include <TH1D.h>
#include <TString.h>
#include <TEfficiency.h>
#include <TCanvas.h>

void AnalysisTans(TString fSimName, TString fRecoName, TString fOutName){

    TStopwatch timer;
    timer.Start();

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    typedef struct {
        double X,Y,Z;
        int mult;} VTX;

    typedef struct{
        double Z;} VTX_R;
  
    static VTX   VertexS; //ho simulato X,Y,Z,mult
    static VTX_R VertexR; //ho ricostruito solo la Z

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Apertura file di input: risultato della Simulazione (verità Monte Carlo)
    TFile hfile(fSimName);
  
    //Lettura TTree  e branch
    TTree *tree = (TTree*)hfile.Get("T");
    TBranch *bSim=tree->GetBranch("VertMult");

    // Definizione degli indirizzi per la lettura dei dati su ttree
    bSim->SetAddress(&VertexS.X);

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Apertura file di input: risultato della Ricostruzione
    TFile hfileReco(fRecoName);
    //Lettura TTree  e branch
    TTree *treeR = (TTree*)hfileReco.Get("TR");
    TBranch *bReco=treeR->GetBranch("VertMultReco");

    // Definizione degli indirizzi per la lettura dei dati su ttree
    bReco->SetAddress(&VertexR.Z); //ho solo la Z ricostruita

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Apertura del file di output
    TFile hfileAnalysis(fOutName,"RECREATE"); //file con gli istogrammi di output dell'analisi

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Grafici di output
    TH1D *hResid = new TH1D ("hResid","Residui",200, -0.1*1e4, 0.1*1e4); //conversione cm->um, 1e4
    TH1D *hRisM = new TH1D("hRisM","Risoluzione vs molteplicita'", 30, 0, 60);
    TH1D *hRisZ = new TH1D("hRisZ","Risoluzione vs Z vertice", 12, -15, 15);
  
    TH1D *hResidM_4 = new TH1D ("hResidM_4","Residui con mult = 4",100, -0.1*1e4, 0.1*1e4);
    TH1D *hResidM_40 = new TH1D ("hResidM_40","Residui con mult = 40",100, -0.1*1e4, 0.1*1e4);
  
    TEfficiency* pEffM = 0;
    TEfficiency* pEffZ = 0;

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-  
    //Grafici di input
    TH1D *hVertSZ = (TH1D*)hfile.Get("hVertSZ");
    TH1D *hVertRZ = (TH1D*)hfileReco.Get("hVertRZ");
  
    TH1D *hmulSim  = (TH1D*)hfile.Get("hmulSim");
    TH1D *hmulVertR = (TH1D*)hfileReco.Get("hmulVertR");

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Residui
    cout<<"Residui ..."<<endl;
    for(int ev=0;ev<tree->GetEntries();ev++){
        treeR->GetEvent(ev);
        tree ->GetEvent(ev);
        //per ogni evento, calcolo lo scarto tra la Z del vertice ricostruito e la Z del vertice Simulato (vero)
        double zSim = VertexS.Z;
        double zReco = VertexR.Z;
        if (zReco > 1e9) continue;
        double resid = (zReco - zSim)*1e4;
        hResid->Fill(resid);
        if(VertexS.mult==4) hResidM_4->Fill(resid);
        if(VertexS.mult==40) hResidM_40->Fill(resid);
    }

    //TCanvas *Gf1 = new TCanvas("Gf1", "Residui", 500, 5, 500, 600);
    hResid->GetXaxis()->SetTitle("Zrec-Zsim [#mum]");
    hResid->GetYaxis()->SetTitle("Conteggi");
    //Gf1->cd();
    //hResid->Draw();
    //Gf1->Update();

    //TCanvas *Gf2 = new TCanvas("Gf2", "Residui a M=4", 500, 5, 500, 600);
    hResidM_4->GetXaxis()->SetTitle("Zrec-Zsim [#mum]");
    hResidM_4->GetYaxis()->SetTitle("Conteggi");
    hResidM_4->Fit("gaus");
    //Gf2->cd();
    //hResidM_4->Draw();
    //Gf2->Update();

    //TCanvas *Gf3 = new TCanvas("Gf3", "Residui a M=40", 500, 5, 500, 600);
    hResidM_40->GetXaxis()->SetTitle("Zrec-Zsim [#mum]");
    hResidM_40->GetYaxis()->SetTitle("Conteggi");
    hResidM_40->Fit("gaus");
    //Gf3->cd();
    //hResidM_40->Draw();
    //Gf3->Update();
     
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    cout<<"Risoluzione ..."<<endl;
    //Residui per molteplicita' a passi di 2
    //TCanvas *Gf4 = new TCanvas("Gf4", "Ris vs M"); 
    for (int nbin=1; nbin<=hRisM->GetNbinsX(); nbin++){//nbin=0 è l'underflow 
        TH1D hResidM("hResidM","ZReco-ZSim a molt fissata",20, -0.1, 0.1);
 
        for(int ev=0;ev<tree->GetEntries();ev++){
            treeR->GetEvent(ev);
            tree->GetEvent(ev);
            int mev = VertexS.mult;
            int m = hRisM->GetXaxis()->GetBinCenter(nbin);
            double zSim = VertexS.Z;
            double zReco = VertexR.Z;
            if (zReco > 1e9) continue;
            double residM = (zReco - zSim);    
            if (abs(mev-m) < hRisM->GetBinWidth(1)/2.) hResidM.Fill(residM);  //hRisM->GetBinWidth(1)/2.
        }
        double sigma = hResidM.GetStdDev()*1e4;
        double errSigma = hResidM.GetStdDevError()*1e4;
        hRisM->SetBinContent(nbin, sigma);
        hRisM->SetBinError(nbin, errSigma);
    }

    hRisM->SetMarkerSize(0.3);
    hRisM->SetMarkerStyle(8);
    hRisM->SetStats(0);
    hRisM->SetTitle("Risoluzione vs Molteplicita'");
    hRisM->GetXaxis()->SetTitle("Molteplicita'");
    hRisM->GetYaxis()->SetTitle("Risoluzione [#mum]");
    //Gf4->cd();
    //hRisM->Draw();
    //Gf4->Update();

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Residui per Z vertice
    //TCanvas *Gf5 = new TCanvas("Gf5", "Ris vs Z");
    for (int nbin=1; nbin<=hRisZ->GetNbinsX(); nbin++){//nbin=0 è l'underflow
        TH1D hResidZ("hResidZ","ZReco-ZSim a Z fissato",20, -0.1, 0.1);
        for(int ev=0;ev<tree->GetEntries();ev++){
            treeR->GetEvent(ev);
            tree->GetEvent(ev);
            double zSim = VertexS.Z;
            double zReco = VertexR.Z;
            double zCenter = hRisZ->GetXaxis()->GetBinCenter(nbin);
            if (zReco > 1e9) continue;
            double residZ = (zReco - zSim);   
            if (abs(zSim-zCenter) < hRisZ->GetBinWidth(1)/2.) hResidZ.Fill(residZ);   //hRisZ->GetBinWidth(1)/2.
        }
        double sigma = hResidZ.GetStdDev()*1.e4;
        double errSigma = hResidZ.GetStdDevError()*1e4;
        hRisZ->SetBinContent(nbin, sigma);
        hRisZ->SetBinError(nbin, errSigma);
    }

    hRisZ->SetMarkerSize(0.6);
    hRisZ->SetMarkerStyle(8);
    hRisZ->SetStats(0);
    hRisZ->SetTitle("Risoluzione vs Z vertice");
    hRisZ->GetXaxis()->SetTitle("Z vertice [cm]");
    hRisZ->GetYaxis()->SetTitle("Risoluzione [#mu m]");
    //Gf5->cd();
    //hRisZ->Draw();
    //Gf5->Update();

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    cout<<"Efficienza ..."<<endl;
    //TCanvas *Gf6 = new TCanvas("Gf6", "Eff vs M");
    if(TEfficiency::CheckConsistency(*hmulVertR,*hmulSim)){
        pEffM = new TEfficiency(*hmulVertR,*hmulSim);
        pEffM->SetDirectory(gDirectory);
    }

    pEffM->SetMarkerSize(0.3);
    pEffM->SetMarkerStyle(8);
    pEffM->SetTitle("Efficienza vs Molteplicita'; Molteplicita'; Efficienza");
    //Gf6->cd();
    //pEffM->Draw();
    //Gf6->Update();
    
    //TCanvas *Gf7 = new TCanvas("Gf7", "Eff vs Z");
    if(TEfficiency::CheckConsistency(*hVertRZ,*hVertSZ)){
        pEffZ = new TEfficiency(*hVertRZ,*hVertSZ);
        pEffZ->SetDirectory(gDirectory);
    }

    pEffZ->SetMarkerSize(0.6);
    pEffZ->SetMarkerStyle(8);
    pEffZ->SetTitle("Efficienza vs Z vertice ; Z vertice [cm]; Efficienza");
    //Gf7->cd();
    //pEffM->Draw();
    //Gf7->Update();
  
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    //Save all objects in this file
    hfileAnalysis.Write();
   
    // Close the file. 
    hfileAnalysis.Close();

    //Stampa tempo di esecuzione                                
    cout<<"+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"<<endl;
    cout<<"Tempo necessario a fare l'analisi di "<<tree->GetEntries()<<" eventi:"<<endl;
    timer.Stop();
    timer.Print();
    cout << "Size: " << hfileAnalysis.GetBytesWritten()*1e-6 <<" MB" << endl; 
}
