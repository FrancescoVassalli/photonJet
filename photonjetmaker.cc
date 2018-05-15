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
#include "Utils.C"
#include <sstream>
#include <queue>

class Photon
{
public:
	Photon(){}
	Photon(float _pT){
		this->pT = Scalar(_pT);
	}
	Photon(float _pT,float _phi){
		this->pT = Scalar(_pT);
		this->phi= Scalar(_phi);
	}
	Photon(float _pT,float _phi, float _eta){
		this->pT = Scalar(_pT);
		this->phi= Scalar(_phi);
		this->eta= Scalar(_eta);
	}
	~Photon(){}
	Scalar getpT(){
		return pT;
	}
	void setpT(float _pT){
		this->pT = Scalar(_pT);
	}
	Scalar getphi(){
		return phi;
	}
	void setphi(float _phi){
		this->phi = Scalar(_phi);
	}
	Scalar geteta(){
		return eta;
	}
	void seteta(float _eta){
		this->eta = Scalar(_eta);
	}
	Point getAngle(){
		Point r;
		r.x=phi;
		r.y=eta;
		return r;
	}

private:
	Scalar pT;
	Scalar phi;
	Scalar eta;

};

class Jet
{
public:
	Jet(){}
	Jet(float _pT, float _phi, float _y, float _r){
		this->pT =_pT;
		this->phi = _phi;
		this->y = _y;
		this->r = _r;
	}
	~Jet(){
		if (next!=NULL)
		{
			delete next;
			next =NULL;
		}
		if (pair!=NULL)
		{
			delete pair;
			pair=NULL;
		}
	}
	void setMult(int m){
		mult=m;
	}
	int getmult(){
		return mult;
	}
	float getpT(){
		return pT;
	}
	float getphi(){
		return phi;
	}
	float gety(){
		return y;
	}
	float getr(){
		return r;
	}
private:
	float pT=-1;
	float phi =7;
	float y=0;
	float r=-1;
	int mult=0;
	Jet* next=NULL;
	Jet* pair=NULL;
	
};

class XjPhi
{
public:
	XjPhi(float pt1, float phi1, float pt2, float phi2){
		//calculate scalar Xjs from parameters 
		xj=Scalar(pt1/pt2);
		phi=Scalar(phi2-phi1);
	}
	~XjPhi(){}
	friend ostream& operator<<(ostream& os, XjPhi const & tc) {
        return os << tc.xj.value <<','<<tc.phi.value<<'\n';
    }
	
private:
	Scalar xj;
	Scalar phi;
};

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
  	Jet *temp =NULL;
  	int finalGammaCount=0;
  	stringstream ss;
  	queue<XjPhi> map;
  	/* generation loop*/
  	for (int iEvent = 0; iEvent < nEvents; ++iEvent)
  	{
  		if (!pythiaengine.next()){
      		cout<<"pythia.next() failed"<<"\n";
      		iEvent--;
      		continue;
    	}
    	if(iEvent%300==0) {
  			cout<<"Event N: "<<iEvent<<'\n';
    	}
    	/* zero out */
    	for (int i = 0; i < pythiaengine.event.size(); ++i)
    	{
    		if (pythiaengine.event[i].id()==22&&pythiaengine.event[i].isFinal()&&pythiaengine.event[i].pT()>10&&TMath::Abs(pythiaengine.event[i].eta())<1)
    		{
    			//get the generation process 
    			//cout<<pythiaengine.process[i].pT()<<'\n';
    			Photon myPhoton = Photon(pythiaengine.event[i].pT(),pythiaengine.event[i].phi(),pythiaengine.event[i].eta());
    			antikT->analyze(pythiaengine.event);
    			ss<<"Photon phi:"<<pythiaengine.event[i].phi()<<" Jet1 phi:"<<antikT->phi(0)<<" Jet2 phi:"<<antikT->phi(1)<<"\n";
    			ss<<"Photon pT:"<<pythiaengine.event[i].pT()<<" Jet1 pT:"<<antikT->pT(0)<<" Jet2 pT:"<<antikT->pT(1)<<"\n";
    			map.push(XjPhi(pythiaengine.event[i].pT(),pythiaengine.event[i].phi(),antikT->pT(1),antikT->phi(1)));
    			//create a map of XjPhi and output that data to TFile or txt or something 
     			finalGammaCount++;
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
  	cout<<ss.str();
  	cout<<"Map:"<<finalGammaCount<<endl;
  	while(!map.empty()){
  		cout<<map.front()<<'\n';
  		map.pop();
  	}
}


int main()
{
	string fileOut = "";
	int nEvents = 10000;
	makeData(fileOut,nEvents);
	return 0;
}