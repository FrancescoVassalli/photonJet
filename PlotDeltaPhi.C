using namespace std;

#include "Utils.C"
#include <sstream>
#include <iostream>


queue<Jet> makeJets(float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, float* pz,int SIZE){
	queue<Jet> r;
	
	for (int i = 0; i < SIZE; ++i)
	{
		r.push(Jet(jetpT[i],jetphi[i],jety[i],jetR[i],pz[i]));
	}

	return r;
}

queue<Jet> getRJets(float cut,float r,queue<Jet> q){
	queue<Jet> rQ;
	while(!q.empty()){
		//if (q.front().getr().value==r&&q.front().getpT().value>=cut)
		if (q.front().getr().value==r)
		{
			cout<<q.front().getpT().value-cut<<'\n';
			rQ.push(q.front());
		}
		q.pop();
	}	
	return rQ;
}
//this cut does nothing
queue<Jet> getRJets(float cut,float r,float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, float* jetpz, int SIZE){
	return getRJets(cut,r,makeJets(photonPhi,jetphi,jety,jetpT,jetR,jetpz,SIZE));
}

inline float deltaPhi(Photon p, Jet j){
	float r= TMath::Abs((p.getphi()-j.getphi()).value);
	if (r>TMath::Pi())
	{
		r= r*(-1)+2*TMath::Pi();
	}
	return r;
}

void getDeltaPhi(TChain* tree){
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
	int isocutCount=tree->GetEntries();
	TCanvas *tc =new TCanvas();
	TH1F *p_dircQ = new TH1F("plot1","",18,.5,TMath::Pi()); 
	TH1F *p_fragQ = new TH1F("plot3","",18,.5,TMath::Pi());
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	makeDifferent(p_fragQ,1);
	tl->AddEntry(p_fragQ,"frag","p");
	tl->AddEntry(p_dircQ,"direct","p");
	cout<<"here"<<endl;
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		tree->GetEntry(i);
		end++; // ends are measured inclusive I want them exclusive
		jetend++;
		Photon pTemp=Photon(end,photonPosition,eT,phi,eta,direct,.3); //make a photon
		if (pTemp.getIsoEt()>3){ //isoEt cut
			isocutCount--;
			continue;
		}
		queue<Jet> jTQ;
		float cut=0;
		jTQ= getRJets(cut,.4,pTemp.getphi().value,jetphi,jety,jetpT,jetR,jetpz,jetend); //make all the jets then match
		while (!jTQ.empty()) //for all the jets of the radius
		{
			if (direct)
			{
				p_dircQ->Fill(deltaPhi(pTemp,jTQ.front()));
			}
			else p_fragQ->Fill(deltaPhi(pTemp,jTQ.front()));
			jTQ.pop();
		}
	}
	gPad->SetLogy();
	p_dircQ->Draw();
	p_dircQ->SetTitle(";#Delta#phi;relative count");
	p_fragQ->Draw("same");
	tl->Draw();
	tree->ResetBranchAddresses();
}

void handleFile(string name, string extension, int filecount){
	TChain *all = new TChain("interest");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = name+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	getDeltaPhi(all);
}

void PlotDeltaPhi(){
string fileLocation = "";
	string filename = "XjPhi3_pT5_";
	string extension = ".root";
	string temp = fileLocation+filename;
	//queue<XjPhi> mainQ1 = handleFile(temp,extension,500,10);

	filename = "XjPhi3_pT15_";
	temp = fileLocation+filename;
	handleFile(temp,extension,100);

	/*filename = "XjPhi3_pT25_";
	temp = fileLocation+filename;
	queue<XjPhi> mainQ3 = handleFile(temp,extension,50,30);

	filename = "XjPhi3_pT35_";
	temp = fileLocation+filename;
	queue<XjPhi> mainQ4 = handleFile(temp,extension,50,40);*/
}