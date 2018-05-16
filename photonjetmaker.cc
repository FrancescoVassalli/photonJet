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
	Photon(double _pT,double _phi){
		this->pT = Scalar((float)_pT);
		this->phi= Scalar((float)_phi);
	}
	Photon(float _pT,float _phi, float _eta){
		this->pT = Scalar(_pT);
		this->phi= Scalar(_phi);
		this->eta= Scalar(_eta);
	}
	Photon(double _pT,double _phi, double _eta){
		this->pT = Scalar((float)_pT);
		this->phi= Scalar((float)_phi);
		this->eta= Scalar((float)_eta);
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
		this->pT =Scalar(_pT);
		this->phi = Scalar(_phi);
		this->y = Scalar(_y);
		this->r = Scalar(_r);
	}
	Jet(double _pT, double _phi){
		pT=Scalar(_pT);
		phi=Scalar(_phi);
		y=Scalar(0);
		r=0;
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
	Scalar getpT(){
		return pT;
	}
	Scalar getphi(){
		return phi;
	}
	Scalar gety(){
		return y;
	}
	Scalar getr(){
		return r;
	}
	Scalar operator/(float s){ 
		return pT/s;
	}
	Scalar operator/(Jet s){ 
		return pT/s.getpT();
	}
	bool operator==(Jet s){
		return pT==s.pT&&phi==s.getphi();
	}
	bool operator>(Jet s){
		return pT>s.getpT();
	}
	bool operator<(Jet s){
		return pT<s.getpT();
	}
	bool operator>=(Jet s){
		return pT>=s.getpT();
	}
	bool operator<=(Jet s){
		return pT<=s.getpT();
	}
	bool operator!=(Jet s){
		return pT!=s.getpT();
	}
private:
	Scalar pT=-1;
	Scalar phi =7;
	Scalar y=0;
	Scalar r=-1;
	int mult=0;
	Jet* next=NULL;
	Jet* pair=NULL;
	
};

class XjPhi
{
public:
	XjPhi(float pt1, float phi1, float pt2, float phi2){
		//calculate scalar Xjs from parameters 
		xj=Scalar(smaller(pt1,pt2)/bigger(pt1,pt2));
		phi=Scalar(TMath::Abs(Scalar(phi2-phi1).value));
	}
	XjPhi(Jet leading, Jet subleading){
		xj=Scalar(subleading/leading);
		phi = Scalar(TMath::Abs(Scalar(subleading.getphi()-leading.getphi()).value));
	}
	XjPhi(Photon p, Jet j){
		xj=(smaller(p.getpT(),j.getpT())/bigger(p.getpT(),j.getpT()));
		phi = TMath::Abs(Scalar(p.getphi()-j.getphi()).value);
	}
	XjPhi(){};
	~XjPhi(){}
	friend ostream& operator<<(ostream& os, XjPhi const & tc) {
        return os << tc.xj.value <<','<<tc.phi.value<<'\n';
    }
	
private:
	Scalar xj;
	Scalar phi;
};

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
  	queue<PhotonJet> map;
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
    			//get the generation process 
    			//cout<<pythiaengine.process[i].pT()<<'\n';
    			Photon myPhoton = Photon(pythiaengine.event[i].pT(),pythiaengine.event[i].phi(),pythiaengine.event[i].eta());
    			antikT->analyze(pythiaengine.event);
    			ss<<finalGammaCount<<'\n';
    			ss<<"Photon phi:"<<pythiaengine.event[i].phi()<<" Jet1 phi:"<<antikT->phi(0)<<" Jet2 phi:"<<antikT->phi(1)<<"\n";
    			ss<<"Photon pT:"<<pythiaengine.event[i].pT()<<" Jet1 pT:"<<antikT->pT(0)<<" Jet2 pT:"<<antikT->pT(1)<<"\n";
    			map.push(PhotonJet(Photon(pythiaengine.event[i].pT(),pythiaengine.event[i].phi()),Jet(antikT->pT(1),antikT->phi(1)),Jet(antikT->pT(0),antikT->phi(0))));
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
  	int counter=0;
  	while(!map.empty()){
  		cout<<map.front();
  		map.pop();
  	}
}


int main()
{
	string fileOut = "";
	int nEvents = 100000;
	makeData(fileOut,nEvents);
	return 0;
}