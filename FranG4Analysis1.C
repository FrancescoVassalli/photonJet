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
		bool r = inRange(reconstructedMass,(float).15,(float).13) || inRange(reconstructedMass,(float).538,(float).578);
		if (r)
		{
			cout<<reconstructedMass<<'\n';
		}
		return r;
	}
	else return false;
}

 TH2F* getClusterpT(TChain *all,queue<TH1*> plots){
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


	TH1F *p_exclusive = new TH1F(getNextPlotName(&plotcount).c_str(),"",32,5,45);
	TH1F *p_inclusive = new TH1F(getNextPlotName(&plotcount).c_str(),"",32,5,45);
	//cannot currently find direct vs frag in reco
	/*TH1F *p_dircExclusive = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,10,50);
	TH1F *p_fragExclusive = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,10,50);
	TH1F *p_fragInclusive = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,10,50);
	TH1F *p_dircInclusive = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,10,50);*/
	TH1F *pTRatio = new TH1F(getNextPlotName(&plotcount).c_str(),"",80,0,2);
	TH1F *etaRis = new TH1F(getNextPlotName(&plotcount).c_str(),"",20,-1,1);
	TH2F *pTRatio2 = new TH2F(getNextPlotName(&plotcount).c_str(),"",200,10,30,100,0,2);
	TH2F *etaRis2 = new TH2F(getNextPlotName(&plotcount).c_str(),"",320,10,20,150,-.11,.11);
	TH2F *recopT = new TH2F(getNextPlotName(&plotcount).c_str(),"",25,10,30,22,0,30);
	int passCluster=0;
	int passIso=0;
	for (int i = 0; i < all->GetEntries(); ++i)
	{
		all->GetEntry(i);
		if (Ncluster>0)
		{
			passCluster++;
			int photonPosition= -1;
			/* check to make sure there isn't a second photon*/
			int j=0;
			while(photonPosition==-1&&j<Nparticle){//find the photon
				if (!isneg99(emcal[j]))
				{
					photonPosition=j;
					TLorentzVector pVec;
					pVec.SetPtEtaPhiM(eT[photonPosition],eta[photonPosition],phi[photonPosition],0);

					for (int k = 0; k < Nparticle; ++k)
					{
						if(k!=photonPosition&&makesPion(pVec,eT[k],eta[k],phi[k],id[k])){
							cout<<photonPosition<<','<<k<<'\n';
							photonPosition=-1;
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
				cout<<"Warning photon not found"<<endl;
				continue;
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
			ssanl<<truthIso<<", "<<pT[photonPosition]<<'\n';
			if(truthIso<1){ // truth isolation CUT
				passIso++;
				pTRatio->Fill(c1.getpT()/pT[photonPosition]);
				etaRis->Fill(c1.geteta()-eta[photonPosition]);
				pTRatio2->Fill(pT[photonPosition],c1.getpT()/pT[photonPosition]);
				etaRis2->Fill(pT[photonPosition],c1.geteta()-eta[photonPosition]);
				if (c1.getpT()/pT[photonPosition]>1.8)
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


				}
			}
			if (Ncluster==1 && (recoIso(emcal[0],clusterpT[0],innerHcal[0],outerHcal[0])<3)) //exclude the events that trigger multiple clusters and then isoCut
			{
				p_exclusive->Fill(clusterpT[0]);
			}
		}
	}
	plots.push(p_inclusive); //1
	plots.push(p_exclusive); //2
	plots.push(pTRatio);   	 //3
	plots.push(etaRis); 	 //4
	plots.push(etaRis2); 	 //6
	plots.push(recopT); 	 //7
	ssanl<<passCluster<<":"<<passIso<<'\n';
	return pTRatio2;
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

void drawPlotRecopT2(TH1* plot){
	TCanvas *tc= new TCanvas();
	tc->SetRightMargin(.15);
	plot->Scale(1/plot->Integral());
	gPad->SetLogz();
	plot->Draw("colz");
	axisTitles(plot,"pT#gamma truth","pT#gamma reco");
}

TH2F* handleG4File(string name, string extension, int filecount,queue<TH1*> reco){
	TChain *all = new TChain("ttree");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		//if(i==401||i==435) continue;
		temp = name+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	TH2F* plots=getClusterpT(all,reco);
	delete all;
	return plots;
}

//needs particle ID to calculate truth ISO
/*void getTruthpT(TChain *chain, TH1F* dirc, TH1F * frag){
	int photonPosition,end;
	bool direct;
	float eT[300];
	float phi[300];
	float eta[300];
	chain->SetBranchAddress("photonPosition",&photonPosition);
	chain->SetBranchAddress("end",&end);
	chain->SetBranchAddress("direct",&direct);
	chain->SetBranchAddress("eT",&eT);
	chain->SetBranchAddress("phi",&phi);
	chain->SetBranchAddress("eta",&eta);
	for (int i = 0; i < chain->GetEntries(); ++i)
	{
		chain->GetEntry(i);
		end++;
		Photon pTemp =Photon(end,photonPosition,eT,phi,eta,direct,.3);
		if (pTemp.getIsoEt()>3){
			continue; // isoEt cut 
		} 
		if (pTemp.isDirect())
		{
			dirc->Fill(eT[photonPosition]);
		}
		else{
			frag->Fill(eT[photonPosition]);
		}
	}
}*/

Scalar getResolution(TH1* plot){
	TF1 *fit = new TF1(getNextPlotName(&plotcount).c_str(),"gaus",plot->GetBinContent(1),plot->GetBinContent(plot->GetNbinsX()));
	plot->Scale(1/plot->Integral());
	plot->Fit(fit);
	float gausData[2];
	gausData[0]= fit->GetParameter(1);
	gausData[1]= fit->GetParameter(2);
	recursiveGaus(plot,fit,gausData,1.5,90);
	Scalar mean(gausData[0],fit->GetParError(1));
	Scalar sigma(gausData[1],fit->GetParError(2));
	//ssanl<<"Mean:"<<gausData[0]<<", "<<gausData[1]<<"="<<gausData[1]/gausData[0]<<'\n';
	return sigma/mean;
}

void plotWithGaus(TH1* plot){
	TCanvas *tc =new TCanvas();
	TF1 *fit = new TF1(getNextPlotName(&plotcount).c_str(),"gaus",plot->GetBinContent(1),plot->GetBinContent(plot->GetNbinsX()));
	plot->Scale(1/plot->Integral());
	plot->Fit(fit);
	float gausData[2];
	gausData[0]= fit->GetParameter(1);
	gausData[1]= fit->GetParameter(2);
	recursiveGaus(plot,fit,gausData,1.5,90);
	string sigma = "#sigma:"+to_string(gausData[1]);
	plot->Draw();
	fit->SetRange(gausData[0]-gausData[1],0,gausData[0]+gausData[1],1);
	fit->Draw("same");
	myText(.2,.3,kBlack,sigma.c_str());
}

void drawRes(TH1F* res){
	TCanvas *tc = new TCanvas();
	gStyle->SetErrorX(0);
	res->Draw("p0");
}

void makeResolution(TH2F* data){
	const int NVALUES = 7;
	float values[NVALUES] = {10.0,12.0,14.0,16.0,18.0,20.0,22.0};
	queue<TH1D*> plots = makeYProjections(data,getBinsFromValues(data,values,NVALUES),NVALUES-1,&plotcount);
	int i=0;
	TH1F* res = new TH1F(getNextPlotName(&plotcount).c_str(),"",NVALUES-1,values);
	while(!plots.empty()){
		plotWithGaus(plots.front());
		Scalar tempRes = getResolution(plots.front());
		res->SetBinContent(i+1,tempRes.value);
		res->SetBinError(i+1,tempRes.uncertainty);
		plots.pop();
		i++;
	}
	drawRes(res);
}

//fix getTruthpT before using this 
/*void handlePYTHIAFile(string name, string extension, int filecount,TH1F* dirc, TH1F* frag){
	TChain *all = new TChain("interest");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = name+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	getTruthpT(all,dirc,frag);
	delete all;
}*/

void FranG4Analysis1(){
	string fileLocation = "/home/user/Droptemp/G4OutInitial/";
	string filename = "XjPhi1_pT5_output_";
	string extension = ".root";
	string temp = fileLocation+filename;
	queue<TH1*> reco;
	TH2F* pTRatio2=handleG4File(temp,extension,1000,reco);
	TH1* p_inclusive = reco.front();
	reco.pop();
	TH1* p_exclusive = reco.front();
	reco.pop();
	TH1* pTRatio = reco.front();
	reco.pop();
	TH1* etaRis = reco.front();
	reco.pop();
	TH1* etaRis2 = reco.front();
	reco.pop();
	TH1* recopT2 = reco.front();

	/*fileLocation="/home/user/Droptemp/XjPhi3/";
	filename = "XjPhi1_pT5_";
	temp = fileLocation+filename;
	TH1F *truthdirc=new TH1F(getNextPlotName(&plotcount).c_str(),"",32,5,45);
	TH1F *truthfrag=new TH1F(getNextPlotName(&plotcount).c_str(),"",32,5,45);
	handlePYTHIAFile(temp,extension,500,truthdirc,truthfrag);*/
	/*printHist(truthdirc);
	printHist(truthfrag);
	printHist(reco.front());*/
	//drawPlotRecovsTruthpT(reco.front(),truthdirc,truthfrag);
	//drawPlotRecopTResid(pTRatio);
	//drawPlotRecopTResid2D(pTRatio2);
	//drawPlotRecoetaResid(etaRis);
	//drawPlotRecoetaResid2d(etaRis2);
	//drawPlotRecopT2(recopT2);
	makeResolution(pTRatio2);
	cout<<ssanl.str();
}
