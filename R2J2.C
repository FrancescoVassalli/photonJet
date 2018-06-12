using namespace std;

#include "Utils.C"
#include <iostream>

int plotCount=0;
inline string getNextPlotName(){
	return "plot"+string(plotCount++);
}

queue<Jet> makeJets(float radius,float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, float* pz, float* jetm,int SIZE){
	queue<Jet> r;
	
	for (int i = 0; i < SIZE; ++i)
	{
		if(jetR[i]==radius&&deltaPhi(photonPhi,jetphi[i])>=TMath::Pi()/2){ //make the jets that are the radius I want and far enough in phi space from the photon
			r.push(Jet(jetpT[i],jetphi[i],jety[i],jetR[i],pz[i]));
		}
	}

	return r;
}

Dijet makeDijet(queue<Jet> jQ){
	if (jQ.size()<2)
	{
		return Dijet(false);
	}
	else{
		Jet j1, j2;
		while(!jQ.empty()){ //get the two highest energy jets 
			if (jQ.front().getEnergy().value>=j1.getEnergy().value)
			{
				j2=j1;
				j1=jQ.front();
			}
			else if (jQ.front().getEnergy().value>j2.getEnergy().value)
			{
				j2=jQ.front();
			}
			jQ.pop();
		}
		return Dijet(j1,j2,true);
	}
}

inline float deltaPhi(Photon p, Jet j){
	float r= TMath::Abs((p.getphi()-j.getphi()).value);
	if (r>TMath::Pi())
	{
		r= r*(-1)+2*TMath::Pi();
	}
	return r;
}

void plot(TH2F *plot){

}

void pickR2J2(TChain* tree){
	float eT[300];
	float phi[300];
	float eta[300];
	float e[300];
	int id[300];
	float jetphi[200];
	float jetpT[200];
	float jety[200];
	float jetR[200];
	float jetm[200];
	float jetpz[200];
	int photonPosition;
	bool direct;
	int end;
	int jetend=0;
	tree->SetBranchAddress("eT",&eT);
	tree->SetBranchAddress("phi",&phi);
	tree->SetBranchAddress("eta",&eta);
	tree->SetBranchAddress("e",&e);
	tree->SetBranchAddress("photonPosition",&photonPosition);
	tree->SetBranchAddress("end",&end);
	tree->SetBranchAddress("direct",&direct);
	tree->SetBranchAddress("ID",&id);
	tree->SetBranchAddress("jetphi",&jetphi);
	tree->SetBranchAddress("jetpT",&jetpT);
	tree->SetBranchAddress("jety",&jety);
	tree->SetBranchAddress("jetR",&jetR);
	tree->SetBranchAddress("jetm",jetm);
	tree->SetBranchAddress("jetpz",jetpz);
	tree->SetBranchAddress("jetend",&jetend);

	TCanvas *tc =new TCanvas();
	TH1F *p_r2j2 = new TH2F(getNextPlotName().c_str(),"",18,0,TMath::Pi(),20,0,2); //work on these bins
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		tree->GetEntry(i);
		end++; // ends are measured inclusive I want them exclusive
		jetend++;
		if(!direct) continue; //directCut
		Photon pTemp=Photon(end,photonPosition,eT,phi,eta,direct,.3); //make a photon
		/*if (pTemp.getIsoEt()>3){ //isoEt cut
			isocutCount--;
			continue;
		}*/
		Dijet dijet = makeDijet(makeJets(.2,pTemp.getphi().value,jetphi,jety,jetpT,jetR,jetpz,jetm,jetend)); //make all the jets then match
		if (dijet) //checks that the event is actually a dijet 
		{
			p_r2j2->Fill(dijet.getDeltaPhi(),dijet.getR2J2());
		}
	}
	plot(p_r2j2);
}

void handleFile(string name, string extension, int filecount){
	TChain *all = new TChain("interest");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = name+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	pickR2J2(all);
}

void R2J2(){
	string fileLocation = "";
	string filename = "XjPhi3_pT5_";
	string extension = ".root";
	string temp = fileLocation+filename;
	handleFile(temp,extension,500,10);

}