#include "TLegend.h"
#include "TH1F.h"
#include <limits.h>
#include <queue>
#include <iostream>
#include <sstream>
//#include "Utils.C"
namespace {
	int plotcount=0;
	stringstream ssanl;
}

void makeResolution(TH2F* data, float* values, int NVALUES,string title);

inline float recoIso(float em, float cluster, float inner, float outer){
	return em-cluster+inner+outer;
}
inline bool isneg99(float in){
	return in==-99;
}

#ifndef Cluster_h
#define Cluster_h 

class Cluster
{
public:
	Cluster(){}
	Cluster(float pT,float phi, float eta){
		this->pT = pT;
		this->phi = phi;
		this->eta =eta;
	}
	Cluster(float pT,float phi, float eta, int index){
		this->pT = pT;
		this->phi = phi;
		this->eta =eta;
		this->index = index;

	}
	~Cluster(){}
	float setdR(float phi,float eta){
		dR =TMath::Power((TMath::Power(TMath::Abs(eta-this->eta),2)+TMath::Power(deltaPhi(this->phi,phi),2)),.5);
	  	return dR;
	}
	float getdR(){
		return dR;
	}
	float getpT(){
		return pT;
	}
	float geteta(){
		return eta;
	}
	int getIndex(){
		return index;
	}
private:
	float phi;
	float eta;
	float dR;
	float pT;
	int index;
	inline float deltaPhi(float i1, float i2){
		float r = TMath::Abs(i1-i2);
		if (r>TMath::Pi())
		{
			r= 2*TMath::Pi()-r;
		}
		return r;
	}
	
};
#endif

inline bool makesPion(TLorentzVector pVec, float eT, float eta, float phi, int id){
	if (id==22)
	{
		TLorentzVector v2;
		v2.SetPtEtaPhiM(eT,eta,phi,0);
		float reconstructedMass = (pVec+v2).M();
		bool r = inRange(reconstructedMass,(float).1348,(float).135) || inRange(reconstructedMass,(float).54775,(float).5495);
		/*if (r)
		{
			cout<<reconstructedMass<<'\n';
		}*/
		return r;
	}
	else return false;
}

void drawPlotRecoetaResid2d(TH1* plot){
	TCanvas *tc= new TCanvas();
	tc->SetRightMargin(.15);
	plot->Scale(1/plot->Integral());
	gPad->SetLogz();
	plot->Draw("colz");
	axisTitles(plot,"pT truth","eta reco-truth");
}

void drawTruth(TH1F* tight_truth, TH1F* decay_truth){
	TCanvas* tc =new TCanvas();
	TLegend *tl = new TLegend(.25,.7,.4,.85);
	tight_truth->Scale(1/tight_truth->Integral());
	decay_truth->Scale(1/decay_truth->Integral());
	tight_truth->Draw();
	decay_truth->Draw("same");
	tl->AddEntry(tight_truth,"tight","p");
	tl->AddEntry(decay_truth,"decay","p");
	tl->Draw();
	makeDifferent(decay_truth,1);
	axisTitles(tight_truth,"iso et", "truth count");
}

void drawReco(TH1F* tight_reco, TH1F* decay_reco){
	TCanvas* tc =new TCanvas();
	TLegend *tl = new TLegend(.25,.7,.4,.85);
	tight_reco->Scale(1/tight_reco->Integral());
	decay_reco->Scale(1/decay_reco->Integral());
	tight_reco->Draw();
	decay_reco->Draw("same");
	tl->AddEntry(tight_reco,"tight","p");
	tl->AddEntry(decay_reco,"decay","p");
	tl->Draw();
	makeDifferent(decay_reco,1);
	axisTitles(tight_reco,"iso et", "reco count");
}

void analyzeFiles(TChain *all){
	int Ncluster, Nparticle;
	float clusterpT[200];
	float clusterphi[200];
	float clustereta[200];
	float emcal[200];
	float innerHcal[200];
	float outerHcal[200];
	float eta[200];
	float phi[200];
	float pT[200];
	float eT[200];
	int id[200];
	all->SetBranchAddress("cluster_n",&Ncluster);
	all->SetBranchAddress("particle_n",&Nparticle);
	all->SetBranchAddress("cluster_pt",&clusterpT);
	all->SetBranchAddress("cluster_phi",&clusterphi);
	all->SetBranchAddress("cluster_eta",&clustereta);
	all->SetBranchAddress("particle_calo_iso_0",&emcal);
	all->SetBranchAddress("particle_calo_iso_1",&innerHcal);
	all->SetBranchAddress("particle_calo_iso_2",&outerHcal);
	all->SetBranchAddress("particle_eta",&eta);
	all->SetBranchAddress("particle_phi",&phi);
	all->SetBranchAddress("particle_pt",&pT);
	all->SetBranchAddress("particle_et",&eT);
	all->SetBranchAddress("particle_pid",&id);

	TH1F* tight_truth = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,-5,15);
	TH1F* tight_reco = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,-5,15);
	TH1F* decay_truth = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,-5,15);
	TH1F* decay_reco = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,-5,15);

	int passCluster=0;
	int passIso=0;
	int pionCut=0;
	int noPhoton=0;
	for (int i = 0; i < all->GetEntries(); ++i)
	{
		all->GetEntry(i);
		if (Ncluster>0)
		{
			passCluster++;
			int photonPosition= -1;
			/* check to make sure there isn't a second photon*/
			int j=0;
			int firstPhoton=-1;
			bool tight;
			while(photonPosition==-1&&j<Nparticle){//find the photon
				if (!isneg99(emcal[j]))
				{
					photonPosition=j;
					tight=true;
					if (firstPhoton==-1)
					{
						firstPhoton=photonPosition;
					}
					TLorentzVector pVec;
					pVec.SetPtEtaPhiM(eT[photonPosition],eta[photonPosition],phi[photonPosition],0);

					for (int k = 0; k < Nparticle; ++k)
					{
						if(k!=photonPosition&&makesPion(pVec,eT[k],eta[k],phi[k],id[k])){
							//ssanl<<"At pion:"<<photonPosition<<','<<k<<'\n';
							photonPosition=-1;
							tight=false;
							pionCut++;
							/*for (int j = 0; j < Nparticle; ++j)
							{
								cout<<"Truth"<<j<<": "<<pT[j]<<", "<<phi[j]<<", "<<eta[j]<<", "<<'\n';
							}*/
							break;
						}
					}
				}
				j++;
			}
			if (photonPosition==-1)
			{
				//ssanl<<"Warning photon"<<i<<" not found"<<endl;		
				noPhoton++;		
				photonPosition=firstPhoton;
			}
			float truthIso = Photon(photonPosition,eT,phi,eta,id,Nparticle).getIsoEt();
			Cluster c1(clusterpT[0],clusterphi[0],clustereta[0],0);
			c1.setdR(phi[photonPosition],eta[photonPosition]);
			int w=1;
			while(w<Ncluster){
				Cluster cTemp(clusterpT[w],clusterphi[w],clustereta[w],w);
				if (cTemp.setdR(phi[photonPosition],eta[photonPosition])<c1.getdR())
				{
					c1=cTemp;
				}
				w++;
			}
			float recoiso = recoIso(emcal[photonPosition],c1.getpT(),innerHcal[photonPosition],outerHcal[photonPosition]);
			if (tight)
			{
				tight_reco->Fill(recoiso);
				tight_truth->Fill(truthIso);
			}
			else{
				decay_reco->Fill(recoiso);
				decay_truth->Fill(truthIso);
			}
		}
	}
	//makeResolution(res2eta,values,NVALUES,"truth #eta");
	//ssanl<<passCluster<<":"<<passIso<<'\n';
	//ssanl<<"pion CUt:"<<pionCut<<" No Photon:"<<noPhoton<<'\n';
	drawTruth(tight_truth,decay_truth);
	drawReco(tight_reco,decay_reco);
}

void handleG4File(string name, string extension, int filecount){
	TChain *all = new TChain("ttree");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = name+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	analyzeFiles(all);
	delete all;
}

void IsoAnalysis(){
	string fileLocation = "/home/user/Droptemp/G4OutInitial/";
	string filename = "XjPhi1_pT5_output_";
	string extension = ".root";
	string temp = fileLocation+filename;
	handleG4File(temp,extension,1000);
}
