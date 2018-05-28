/*
	for the generation of photon-jet observables through Pythia pp collisions 
	by Francesco Vassalli
*/

#include <sstream>
#include <queue>
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

//float** qXjPhiTo2DArray(queue<XjPhi> in); //in utils for now

float deltaPhi(Photon p, Jet j);
float deltaR(Parton,Jet);

template<class T>
void swapPointer(T* a, T* b){
	T* t=a;
	a=b;
	b=t;
}

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
	PhotonJet(bool f){ //place holder for NULL events
		xjphi= XjPhi(0,2*TMath::Pi());
	}
	PhotonJet(Photon p, DiJet d){
		//get the Jet with the bigger difference in phi
		if (deltaPhi(p,d.getleading())>deltaPhi(p,d.getsubleading()))
		{
			jet = d.getleading();
		}
		else{
			jet = d.getsubleading();
		}
		photon=p;
		makeXjPhi();
	}
	PhotonJet(Photon p, Jet j1, Jet j2){
		photon=p;
		if (deltaPhi(p,j1)>deltaPhi(p,j2))
		{
			jet=j1;
		}
		else{
			jet=j2;
		}
		makeXjPhi();
	}
	PhotonJet(Photon p){//for events with no paired jet
		photon=p;
		xjphi=XjPhi(0,0);
	}
	~PhotonJet(){}
	Scalar getXj(){
		return xjphi.getXj();
	}
	Scalar getphi(){
		return xjphi.getphi();
	}
	Photon getPhoton(){
		return photon;
	}
	Jet getJet(){
		return jet;
	}
	bool isDirect(){
		return photon.isDirect();
	}
	void matchPartons(Parton p1, Parton p2){
		if (deltaR(p1,jet)<deltaR(p2,jet))
		{
			jet.setParton(p1);
			photon.setParton(p2);
		}
		else{
			jet.setParton(p2);
			photon.setParton(p1);
		}
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

template<class T>
T positivePhi(T in){
	if (in<0)
	{
		in = in+2*TMath::Pi();
	}
	return in;
}

inline bool quickPhotonCheck(Particle p){
	return p.id()==22&&p.isFinal()&&p.pT()>10&&TMath::Abs(p.eta())<1;
}

/* list of "problem" events that I am still getting
	1) MonoJet Events 
	2) Jet pair is not in in leading or subleading / jets to one side
*/

void makeData(std::string filename, int nEvents){
	TFile* f = new TFile(filename.c_str(),"RECREATE");
  	TTree* t=new TTree("tree100","events");
  	/*pythia set up*/
  	Pythia pythiaengine;
  	pythiaengine.readString("Beams:eCM = 200.");
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
  	queue<float> monoJetEventPhis;
  	PhotonJet tempXj;
  	/* generation loop*/
  	string nullInterest="Interest=0";
  	bool interestHit=false;
  	int interestC=0;
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
    		if (quickPhotonCheck(pythiaengine.event[i]))
    		{
    			finalGammaCount++;
    			Parton p1 = Parton(pythiaengine.event[5].id(),positivePhi(pythiaengine.event[5].phi()),positivePhi(pythiaengine.event[5].eta()),pythiaengine.event[5].px(),pythiaengine.event[5].py());
    			Parton p2 = Parton(pythiaengine.event[6].id(),positivePhi(pythiaengine.event[6].phi()),positivePhi(pythiaengine.event[6].eta()),pythiaengine.event[6].px(),pythiaengine.event[6].py());
    			Photon myPhoton = Photon(pythiaengine.event[i].pT(),positivePhi(pythiaengine.event[i].phi()),pythiaengine.event[i].eta(),isDirect(pythiaengine.info.code()));
    			antikT->analyze(pythiaengine.event);
    			//ss<<finalGammaCount<<'\n';
    			if(antikT->sizeJet()>1){
    				//biasing by only looking at first 2?
    				tempXj=PhotonJet(myPhoton,Jet(antikT->pT(1),positivePhi(antikT->phi(1)),positivePhi(antikT->y(1))),Jet(antikT->pT(0),positivePhi(antikT->phi(0)),positivePhi(antikT->y(0))));
    				if (tempXj.getphi()>7*TMath::Pi()/8)
    				{
    					tempXj.matchPartons(p1,p2);
    					map.push(tempXj);
    				}//do I want an else?
    			}
    			else{
    				tempXj=PhotonJet(myPhoton);
    				monoJetEventPhis.push(myPhoton.getphi().value);
    				map.push(tempXj);
    			}
    			//this if is an error check process to see which events the XjPhi is screwing up
    			if (tempXj.getphi()<7*TMath::Pi()/8)
    			{
    				interestC++;
    				if (!interestHit)
    				{
    					interestHit=true;
    				}
    				interest<<"Number:"<<finalGammaCount<<'\n';
    				interest<<"Photon:"<<tempXj.getPhoton().getpT().value<<","<<tempXj.getPhoton().getphi().value<<'\n'<<"Select:"<<tempXj.getJet().getpT().value;
    				// get all significant jets 
    				queue<Jet> significant = getSignificantJet(antikT,10,.4);
    				int printcount=0;
    				while(!significant.empty()){
    					interest<<"Jet"<<printcount<<": "<<significant.front().getpT().value<<","<<positivePhi(significant.front().getphi()).value<<'\n';
    					significant.pop();
    					printcount++;
    				}
    			}
    			ss<<"Photon phi:"<<pythiaengine.event[i].phi()<<" Jet1 phi:"<<antikT->phi(0)<<" Jet2 phi:"<<antikT->phi(1)<<"\n";
    			ss<<"Photon pT:"<<pythiaengine.event[i].pT()<<" Jet1 pT:"<<antikT->pT(0)<<" Jet2 pT:"<<antikT->pT(1)<<"\n";
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
  	float xjdtemp;
  	float xjftemp;
  	float phitemp,monPhitemp;
  	t->Branch("xjd",&xjdtemp);
  	t->Branch("xjf",&xjftemp);
  	t->Branch("phi",&phitemp);
  	t->Branch("monPhi",&monPhitemp);
  	bool directTemp;
  	t->Branch("direct", &directTemp);
  	cout<<ss.str();
  	cout<<"Map:"<<finalGammaCount<<endl;
  	interest<<interestC;
  	int counter=0;
  	while(!monoJetEventPhis.empty()){
  		phitemp=map.front().getphi().value;
  		directTemp=map.front().isDirect();
  		if (directTemp)
  		{
  			xjdtemp=map.front().getXj().value;
  			xjftemp=-1;
  		}
  		else{
  			xjftemp=map.front().getXj().value;
  			xjdtemp=-1;
  		}
  		monPhitemp=monoJetEventPhis.front();
  		monoJetEventPhis.pop();
  		t->Fill();
  		map.pop();
  	}
  	while(!map.empty()){
  		/*cout<<map.front();*/
  		phitemp=map.front().getphi().value;
  		directTemp=map.front().isDirect();
  		if (directTemp)
  		{
  			xjdtemp=map.front().getXj().value;
  			xjftemp=-1;
  		}
  		else{
  			xjftemp=map.front().getXj().value;
  			xjdtemp=-1;
  		}
  		monPhitemp=-2*TMath::Pi();
  		t->Fill();
  		//out the file 
  		map.pop();
  	}
  	if (!interestHit)
  	{
  		cout<<nullInterest;
  	}
  	else{
  		cout<<interest.str();
  	}
  	t->Write();
  	f->Write();
  	f->Close();
}

int main(int argc, char const *argv[] )
{
	string fileOut = string(argv[1]);
	int nEvents = 1000;
	makeData(fileOut,nEvents);
	return 0;
}

float deltaPhi(Photon p, Jet j){
	Scalar r= Scalar(TMath::Abs((p.getphi()-j.getphi()).value));
	if (r>TMath::Pi())
	{
		r= r*(-1)+2*TMath::Pi();
	}
	return r.value;
}
inline float deltaR(Parton p, Jet j){
	return TMath::Power(TMath::Power(TMath::Abs(p.getphi()-j.getphi().value),2)+TMath::Power(TMath::Abs(p.gety()-j.gety().value),2),.5);
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