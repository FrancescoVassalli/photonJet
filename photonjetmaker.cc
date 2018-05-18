/*
	for the generation of photon-jet observables through Pythia pp collisions 
	by Francesco Vassalli
*/

#include "Pythia8/Pythia.h"
using namespace Pythia8;
using namespace std;
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TMath.h"
#include "Utils.C" //split this into the three classes below needs like #IFNDEF
/*#include "XjPhi.C"
#include "CollisionClasses.C"
#include "SimpleMath.C"*/
#include <sstream>
#include <queue>

//float** qXjPhiTo2DArray(queue<XjPhi> in); //in utils for now

class DiJet
{
public:
	DiJet(Jet j1, Jet j2){
		leading = bigger(j1,j2);
		subleading=smaller(j1,j2);
		
	}
	DiJet(double pt1, double phi1, double pt2,double phi2){
		if (pt1>pt2)
		{
			leading=Jet(pt1,phi1,0,0);
			subleading=Jet(pt2,phi2,0,0);
		}
		else{
			subleading=Jet(pt1,phi1,0,0);
			leading=Jet(pt2,phi2,0,0);
		}
		makeXjPhi();
	}
	DiJet(){}

	~DiJet(){}
	Jet getleading(){
		return leading;
	}
	Jet getsubleading(){
		return subleading;
	}
private:
	void makeXjPhi(){
		xjphi=XjPhi(leading,subleading);
	}
	Jet leading;
	Jet subleading;
	XjPhi xjphi;
	
};

class PhotonJet
{
public:
	PhotonJet(){}
	PhotonJet(Photon p, Jet j){
		photon = p;
		jet = j;
		makeXjPhi();
	}
	PhotonJet(Photon p, DiJet d){
		//get the Jet with the bigger difference in phi
		if (TMath::Abs(Scalar(p.getphi()-d.getleading().getphi()).value)>TMath::Abs(Scalar(p.getphi()-d.getsubleading().getphi()).value))
		{
			jet = d.getleading();
		}
		else{
			jet = d.getsubleading();
		}
		this->photon=p;
		makeXjPhi();
	}
	PhotonJet(Photon p, Jet j1, Jet j2){
		photon=p;
		if (TMath::Abs(Scalar(p.getphi()-j1.getphi()).value)>TMath::Abs(Scalar(p.getphi()-j2.getphi()).value))
		{
			jet=j1;
		}
		else{
			jet=j2;
		}
		makeXjPhi();
	}
	~PhotonJet(){}
	Scalar getXj(){
		return xjphi.getXj();
	}
	Scalar getphi(){
		return xjphi.getphi();
	}
	friend ostream& operator<<(ostream& os, PhotonJet const & tc) {
        return os << tc.xjphi;
    }
private:
	void makeXjPhi(){
		xjphi = XjPhi(photon,jet);
	}
	Photon photon;
	Jet jet;
	XjPhi xjphi;
	
};

//inclusive
queue<Jet> getSignificantJet(SlowJet* antikT, float minGeV, float rad){
	queue<Jet> r;
	int i=0;
	while (antikT->pT(i)>=minGeV)
	{
		r.push(Jet(antikT->pT(i),antikT->phi(i),antikT->y(i),rad));
		i++;
	}
	return r;
}

void makeData(std::string filename, int nEvents){
	TFile* f = new TFile(filename.c_str(),"RECREATE");
  	TTree* t=new TTree("tree100","events");

  	/*pythia set up*/
  	Pythia pythiaengine;
  	pythiaengine.readString("Beams:eCM = 2760.");
 	pythiaengine.readString("promptphoton:all = on");
 	pythiaengine.readString("HardQCD:all = on");
 	pythiaengine.readString("PhaseSpace:pTHatMin = 10.");
 	pythiaengine.readString("Random::setSeed = on");
  	pythiaengine.readString("Random::seed =0");
  	pythiaengine.init();

  	/* Tbranching  */
  	SlowJet *antikT = new SlowJet(-1,.4,10,4,2,1);
  	//Jet *temp =NULL;
  	int finalGammaCount=0;
  	stringstream ss;
  	stringstream interest;
  	queue<PhotonJet> map;
  	PhotonJet tempXj;
  	/* generation loop*/
  	for (int iEvent = 0; iEvent < nEvents; ++iEvent)
  	{
  		if (!pythiaengine.next()){
      		cout<<"pythia.next() failed"<<"\n";
      		iEvent--;
      		continue;
    	}
    	/*if(iEvent%300==0) {
  			cout<<"Event N: "<<iEvent<<'\n';
    	}
    	/* zero out */
    	for (int i = 0; i < pythiaengine.event.size(); ++i)
    	{
    		if (pythiaengine.event[i].id()==22&&pythiaengine.event[i].isFinal()&&pythiaengine.event[i].pT()>10&&TMath::Abs(pythiaengine.event[i].eta())<1)
    		{
    			finalGammaCount++;
    			//get the generation process 
    			//cout<<pythiaengine.process[i].pT()<<'\n';
    			Photon myPhoton = Photon(pythiaengine.event[i].pT(),pythiaengine.event[i].phi(),pythiaengine.event[i].eta());
    			antikT->analyze(pythiaengine.event);
    			ss<<finalGammaCount<<'\n';
    			tempXj=PhotonJet(Photon(pythiaengine.event[i].pT(),pythiaengine.event[i].phi()),Jet(antikT->pT(1),antikT->phi(1)),Jet(antikT->pT(0),antikT->phi(0)));
    			if (tempXj.getphi()<.8)
    			{
    				interest<<"Number:"<<finalGammaCount<<'\n';
    				// get all significant jets 
    				queue<Jet> significant = getSignificantJet(antikT, 10, .4);
    				int printcount=0;
    				while(!significant.empty()){
    					interest<<"Jet"<<printcount<<": "<<significant.front().getpT().value<<","<<significant.front().getphi().value<<'\n';
    					printcount++;
    				}
    			}
    			ss<<"Photon phi:"<<pythiaengine.event[i].phi()<<" Jet1 phi:"<<antikT->phi(0)<<" Jet2 phi:"<<antikT->phi(1)<<"\n";
    			ss<<"Photon pT:"<<pythiaengine.event[i].pT()<<" Jet1 pT:"<<antikT->pT(0)<<" Jet2 pT:"<<antikT->pT(1)<<"\n";
    			map.push(tempXj);
    			//create a map of XjPhi and output that data to TFile or txt or something 
     			
    		}
    	}
    	/*
    	std::vector<Jet> myJets(0);
    	for (int i = 0; i < antikT->sizeJet(); ++i)
    	{
    		temp = new Jet(antikT->pT(i),antikT->phi(i),antikT->y(i),.4);
    		// if mult is needed add here
    		myJets.push_back(temp);

    	}*/
  	}
  	//data out 
  	float xjtemp;
  	float phitemp;
  	t->Branch("xj",&xjtemp);
  	t->Branch("phi",&phitemp);
  	cout<<ss.str();
  	cout<<"Map:"<<finalGammaCount<<endl;
  	cout<<interest.str();
  	int counter=0;
  	while(!map.empty()){
  		cout<<map.front();
  		xjtemp=map.front().getXj().value;
  		phitemp=map.front().getphi().value;
  		t->Fill();
  		//out the file 
  		map.pop();
  	}
  	t->Write();
  	f->Write();
  	f->Close();
}


int main()
{
	string fileOut = "XjgP.root";
	int nEvents = 50000;
	makeData(fileOut,nEvents);
	return 0;
}

/*float** qXjPhiTo2DArray(queue<XjPhi> in){
	float** out = new float*[2];
	float *o1 = new float[in.size()];
	float *o2 = new float[in.size()];
	int i=0;
	while(!in.empty()){
		o1[i]=in.front().getXj().value;
		o2[i]=in.front().getphi().value;
		i++;
		in.pop();
	}
	out[0]=o1;
	out[1]=o2;
	return out;
}*/