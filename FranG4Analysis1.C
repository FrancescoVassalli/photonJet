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

void drawPlotRecopTResid2D(TH1* plot){
	TCanvas *tc= new TCanvas();
	tc->SetRightMargin(.15);
	plot->Scale(1/plot->Integral());
	gPad->SetLogz();
	plot->Draw("colz");
	axisTitles(plot,"pT truth","pT reco/truth");
}

void drawRes2Eta(TH1* plot, TProfile* prof){
	TCanvas *tc= new TCanvas();
	tc->SetRightMargin(.15);
	plot->Scale(1/plot->Integral());
	//gPad->SetLogz();
	plot->Draw("colz");
	axisTitles(plot,"eta","resolution");
	prof->Draw("same");
}

void drawPlotRecovsTruthpT(TH1F* p_excluspT, TH1F* dirc, TH1F *frag){
	TCanvas *tc= new TCanvas();
	double truthsum = dirc->Integral()+frag->Integral();
	//cout<<truthsum<<":"<<p_excluspT->Integral()<<endl;
	dirc->Scale(1/p_excluspT->Integral());
	frag->Scale(1/p_excluspT->Integral());
	p_excluspT->Scale(1/p_excluspT->Integral());
	p_excluspT->Draw();
	dirc->Draw("same");
	frag->Draw("same");
	//doubleZero(p_excluspT,.5,1);
	makeDifferent(dirc,1);
	makeDifferent(frag,2);
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_excluspT,"reco","p");
	tl->AddEntry(dirc, "truth direct","p");
	tl->AddEntry(frag,"truth frag","p");
	axisTitles(p_excluspT,"pT#gamma","count");
	tl->Draw();
}

void drawPlotRecopTResid(TH1* plot){
	TCanvas *tc= new TCanvas();
	TF1 *fit = new TF1(getNextPlotName(&plotcount).c_str(),"gaus",.4,1.6);
	cout<<plot->Integral()<<endl;
	plot->Scale(1/plot->Integral());
	plot->Fit(fit);
	float gausData[2];
	gausData[0]= fit->GetParameter(1);
	gausData[1]= fit->GetParameter(2);
	recursiveGaus(plot,fit,gausData,1.5,90);
	string sigma = "sigma: "+to_string(gausData[1]);
	plot->Draw();
	fit->SetRange(gausData[0]-gausData[1],0,gausData[0]+gausData[1],1);
	fit->Draw("same");
	myText(.2,.3,kBlack,sigma.c_str());
	axisTitles(plot,"pT reco/truth","count");
}
void drawPlotRecoetaResid(TH1* plot){
	TCanvas *tc= new TCanvas();
	plot->Scale(1/plot->Integral());
	plot->Draw();
	axisTitles(plot,"eta reco-truth","count");
}

void drawPlotRecopT2(TH1* plot){
	TCanvas *tc= new TCanvas();
	tc->SetRightMargin(.15);
	plot->Scale(1/plot->Integral());
	gPad->SetLogz();
	plot->Draw("colz");
	axisTitles(plot,"pT#gamma truth","pT#gamma reco");
}

 TH2F* getClusterpT(TChain *all){
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

	TH1F *p_inclusive = new TH1F(getNextPlotName(&plotcount).c_str(),"",32,5,45);

	TH1F *pTRatio = new TH1F(getNextPlotName(&plotcount).c_str(),"",80,0,2);
	TH1F *etaRis = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,-1,1);
	TH2F *pTRatio2 = new TH2F(getNextPlotName(&plotcount).c_str(),"",200,10,30,100,0,2);
	TH2F *etaRis2 = new TH2F(getNextPlotName(&plotcount).c_str(),"",320,10,20,150,-.11,.11);
	TH2F *recopT = new TH2F(getNextPlotName(&plotcount).c_str(),"",25,10,30,22,0,30);
	TH2F *res2eta = new TH2F(getNextPlotName(&plotcount).c_str(),"",20,-.7,.7,20,0,1.5);
	TProfile* profResEta = new TProfile(getNextPlotName(&plotcount).c_str(),"",20,-.7,.7,0,1.5);
	
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
			while(photonPosition==-1&&j<Nparticle){//find the photon
				if (!isneg99(emcal[j]))
				{
					photonPosition=j;
					if (firstPhoton==-1)
					{
						firstPhoton=photonPosition;
					}
					TLorentzVector pVec;
					pVec.SetPtEtaPhiM(eT[photonPosition],eta[photonPosition],phi[photonPosition],0);

					for (int k = 0; k < Nparticle; ++k)
					{
						if(k!=photonPosition&&makesPion(pVec,eT[k],eta[k],phi[k],id[k])){
							ssanl<<"At pion:"<<photonPosition<<','<<k<<'\n';
							photonPosition=-1;
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
				ssanl<<"Warning photon"<<i<<" not found"<<endl;		
				noPhoton++;		
				continue;
				//photonPosition=firstPhoton;
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

			p_inclusive->Fill(c1.getpT());
			recopT->Fill(pT[photonPosition],c1.getpT());
			//ssanl<<truthIso<<", "<<pT[photonPosition]<<'\n';
			if(truthIso<1){ // truth isolation CUT 
				passIso++;
				pTRatio->Fill(c1.getpT()/pT[photonPosition]);
				etaRis->Fill(c1.geteta()-eta[photonPosition]);
				pTRatio2->Fill(pT[photonPosition],c1.getpT()/pT[photonPosition]);
				etaRis2->Fill(pT[photonPosition],c1.geteta()-eta[photonPosition]);
				res2eta->Fill(eta[photonPosition],c1.getpT()/pT[photonPosition]);
				profResEta->Fill(eta[photonPosition],c1.getpT()/pT[photonPosition]);
				/*if (c1.getpT()/pT[photonPosition]<.8)
				{
					for (int j = 0; j < Ncluster; ++j)
					{
						cout<<"Cluster"<<j<<": "<<clusterpT[j]<<", "<<clusterphi[j]<<", "<<clustereta[j]<<'\n';
					}
					cout<<"Photon: "<<pT[photonPosition]<<", "<<phi[photonPosition]<<", "<<eta[photonPosition]<<'\n';
					cout<<"Match:"<<c1.getIndex()<<", "<<truthIso<<'\n';
					for (int j = 0; j < Nparticle; ++j)
					{
						cout<<"Truth"<<j<<": "<<pT[j]<<", "<<phi[j]<<", "<<eta[j]<<", "<<'\n';
					}
					cout<<"\n \n";


				}*/
			}
		}
	}
	int NVALUES = 11;
	float values[]= {-.7,-.5,-.3,-.2,-.1,-.0,.1,.2,.3,.5,.7};
	drawRes2Eta(res2eta,profResEta);
	drawPlotRecopTResid2D(pTRatio2);
	//drawPlotRecovsTruthpT(reco.front(),truthdirc,truthfrag);
	//drawPlotRecopTResid(pTRatio);
	//drawPlotRecopTResid2D(pTRatio2);
	//drawPlotRecoetaResid(etaRis);
	//drawPlotRecoetaResid2d(etaRis2);
	//drawPlotRecopT2(recopT2);
	ssanl<<passCluster<<":"<<passIso<<'\n';
	ssanl<<"pion CUt:"<<pionCut<<" No Photon:"<<noPhoton<<'\n';
	return pTRatio2;
}

TH2F* handleG4File(string name, string extension, int filecount){
	TChain *all = new TChain("ttree");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		//if(i==401||i==435) continue;
		temp = name+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	TH2F* plots=getClusterpT(all);
	delete all;
	return plots;
}

void drawRes(TH1F* res,string title){
	TCanvas *tc = new TCanvas();
	gStyle->SetErrorX(0);
	res->Draw("p0");
	axisTitles(res,title.c_str(),"resolution");
	
}

void makeResolution(TH2F* data, float* values, int NVALUES,string title){
	queue<TH1D*> plots = makeYProjections(data,getBinsFromValues(data,true,values,NVALUES),NVALUES-1,&plotcount);
	int i=0;
	TH1F* res = new TH1F(getNextPlotName(&plotcount).c_str(),"",NVALUES-1,values);
	Scalar average=0;
	while(!plots.empty()){
		plotWithGaus(plots.front());
		Scalar tempRes = getResolution(plots.front());
		ssanl<<tempRes;
		res->SetBinContent(i+1,tempRes.value);
		average+=tempRes;
		res->SetBinError(i+1,tempRes.uncertainty);
		plots.pop();
		i++;
	}
	drawRes(res,title);
	average/=i;
	ssanl<<average;
}

void FranG4Analysis1(){
	string fileLocation = "/home/user/Droptemp/directseperatedOverflow/";
	string filename = "XjPhi_pT5_output_direct_";
	string extension = ".root";
	string temp = fileLocation+filename;
	TH2F* pTRatio2=handleG4File(temp,extension,10000);
	
	/*printHist(truthdirc);
	printHist(truthfrag);
	printHist(reco.front());*/
	
	const int NVALUES = 5;
	float values[NVALUES] = {10.0,12.0,14.0,16.0,22.0};
	makeResolution(pTRatio2,values,NVALUES,"pT truth");
	//cout<<ssanl.str();
}
