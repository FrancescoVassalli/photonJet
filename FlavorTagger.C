using namespace std;

#include "Utils.C"
#include <sstream>
#include <iostream>

queue<Jet> makeJets(float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, float* pz,int SIZE){
	queue<Jet> r;
	
	for (int i = 0; i < SIZE; ++i)
	{
		if (TMath::Abs(jetphi[i]-photonPhi)>7.0*TMath::Pi()/8.0)
		{
			r.push(Jet(jetpT[i],jetphi[i],jety[i],jetR[i],pz[i]));
			
		}
	}

	return r;
}

queue<Jet> getRJets(float r,queue<Jet> q){
	queue<Jet> rQ;
	while(!q.empty()){
		if (q.front().getr()==r)
		{
			rQ.push(q.front());
		}
		q.pop();
	}	
	return rQ;
}

queue<Jet> getRJets(float r,float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, float* jetpz, int SIZE){
	return getRJets(r,makeJets(photonPhi,jetphi,jety,jetpT,jetR,jetpz,SIZE));
}

Jet* getMaxJet(queue<Jet> jQ){
	if(jQ.empty()) return NULL;
	else{
		Jet *r = new Jet();
		while(!jQ.empty()){
			if (jQ.front()>*r)
			{
				*r=jQ.front();
			}
			jQ.pop();
		}
		return r;
	}
}

queue<XjPhi> getXjPhi(TChain* tree){
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
	queue<XjPhi> xjPhiQ;
	analysisStream<<"Total:"<<tree->GetEntries()<<'\n';
	int isocutCount=tree->GetEntries();
	int jetCutCount=tree->GetEntries();
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		tree->GetEntry(i);
		end++; // ends are measured inclusive I want them exclusive
		jetend++;
		Photon pTemp=Photon(end,photonPosition,eT,phi,eta,direct,.3); //make a photon
		if (pTemp.getIsoEt()>3){ //isoEt cut
			isocutCount--;
			jetCutCount--;
			continue;
		}
		Jet* maxJet=NULL;
		maxJet= getMaxJet(getRJets(.4,pTemp.getphi().value,jetphi,jety,jetpT,jetR,jetpz,jetend)); //make all the jets then match
		if (maxJet!=NULL) //if there is a matched jet
		{
			maxJet->setParton(Parton(id[end],phi[end],eta[end],eT[end],e[end]),Parton(id[end+1],phi[end+1],eta[end+1],eT[end+1],e[end+1]));
			xjPhiQ.push(XjPhi(pTemp,*maxJet));
			delete maxJet;
			maxJet=NULL;
		}
		else jetCutCount--;
	}
	analysisStream<<"IsoCut:"<<isocutCount<<'\n';
	analysisStream<<"JetCut:"<<jetCutCount<<'\n';
	tree->ResetBranchAddresses();
	return xjPhiQ;
}

void FlavorTagger(){
	string fileLocation = "";
	string filename = "XjPhi3_pT5_";
	string extension = ".root";
	int filecount=500;
	TChain *all = new TChain("interest");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = fileLocation+filename+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	queue<XjPhi> mainQ1 = getXjPhi(all);
	//plot1D(mainQ);
	classifyTypeAndFlavorpTg(mainQ);
	//plotpTMatched(mainQ);
	//xjgpT(mainQ);
	analysisStream<<"end"<<endl;
	cout<<analysisStream.str();
}