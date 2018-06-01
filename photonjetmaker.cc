/*
	for the generation of photon-jet observables through Pythia pp collisions 
	by Francesco Vassalli
*/
/* stop trying to make one file do so much.  Make test files */
#include <sstream>
#include <queue>
#include <fstream>
#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h" //added plugin for HepMC, think we will need some new library in pythia for this
using namespace Pythia8;
using namespace std;
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TMath.h"
#include "Utils.C" 
#ifdef __MAKECINT__
#pragma link C++ class vector<float>+;
#endif
//gROOT->ProcessLine("#include<vector>");

 class myEvent
 {
 public:
 	myEvent(){}
 	~myEvent(){}
 	myEvent(int particle,bool direct, int status, int id, float pt, float eta, float phi, float mother1, float mother2,float daughter1, float daughter2){
		this->particle=particle;
 		this->direct=direct;
 		this->status=status;
 		this->id=id;
 		this->pt=pt;
 		this->eta=eta;
 		this->phi=phi;
 		this->mother1=mother1;
 		this->mother2=mother2;	 
 		this->daughter1=daughter1;
 		this->daughter2=daughter2;
 	}
 private:
 	int particle;
 	bool direct;
 	int status;
 	int id;
 	float pt;
 	float eta;
 	float phi;
 	float mother1;
 	float mother2; 	 
 	float daughter1; 
 	float daughter2;
 };

float deltaPhi(Photon p, Jet j);
float deltaR(Parton,Jet);

template<class T>
void swapPointer(T* a, T* b){
	T* t=a;
	a=b;
	b=t;
}

/*inline bool isDirect(int i)
{
  if(i > 200 and i < 267)
  {
    return true;
  }
  else
  {
    return false;
  }
}*/

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
	bool isJetQuark(){
		return jet.isJetQuark();
	}
	/*void addEvent(Event e){
		pythiaEvent=e;
	}*/
	/*Event getPythiaEvent(){
		return pythiaEvent;
	}*/
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
	//Event pythiaEvent;
	
};
struct MyPair
{
	Event e;
	PhotonJet pj;
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

inline bool quickPhotonCheck(Particle p, float gammaCut){
	return p.id()==22&&p.isFinal()&&p.pT()>gammaCut&&TMath::Abs(p.eta())<1.1;
}

queue<myParticle> EventToQueue(Event e){
	myParticle temp;
	queue<myParticle> r;
	for (int i = 0; i < e.size(); ++i)
	{
		temp = myParticle(e[i].id(),e[i].eT(),e[i].phi(),e[i].y());
		r.push(temp);
	}
	return r;
}

void fillTreebyEvent(Event e, vector<int> status,vector<int> id,vector<float> pT,vector<float> eT,vector<float> eta,vector<float> phi,vector<int> mother1,vector<int> mother2,vector<int> daughter1,vector<int> daughter2){
	for (int i = 0; i < e.size(); ++i)
	{
		status.push_back(e[i].status());
  		id.push_back(e[i].id());
  		pT.push_back(e[i].pT());
  		eT.push_back(e[i].eT());
  		eta.push_back(e[i].eta());
  		phi.push_back(e[i].phi());
  		mother1.push_back(e[i].mother1());
  		mother2.push_back(e[i].mother2());
  		daughter1.push_back(e[i].daughter1());
  		daughter2.push_back(e[i].daughter2());
	}
}

void fillTreebySlowJet(SlowJet* antikT, float R,vector<int> mult, vector<float> y, vector<float> phi, vector<float> pT,vector<float> r){
	for (int i = 0; i < antikT->sizeJet(); ++i)
	{
		mult.push_back(antikT->multiplicity(i));
		y.push_back(antikT->y(i));
		phi.push_back(antikT->phi(i));
		pT.push_back(antikT->pT(i));
		r.push_back(R);
	}
}
void makeData(std::string filename, int nEvents, string pTHat, float gammaCut){
	string hepName = filename+".dat";
	filename+=".root";
	TFile* f = new TFile(filename.c_str(),"RECREATE");
  	TTree* interestXj = new TTree("interest","interest");
  	using namespace HepMC;
    HepMC::Pythia8ToHepMC ToHepMC;    // Interface for conversion from Pythia8::Event to HepMC event.
    HepMC::IO_GenEvent ascii_io(hepName, std::ios::out); //file where HepMC events will be stored.
	/*pythia set up*/
    Pythia pythiaengine;
    pythiaengine.readString("Beams:eCM = 200.");
  	pythiaengine.readString("promptphoton:all = on");
 	  pythiaengine.readString("HardQCD:all = on");
  	pythiaengine.readString("Random::setSeed = on");
  	pythiaengine.readString("Random::seed =0");
  	pTHat = "PhaseSpace:pTHatMin = "+pTHat+".";
  	pythiaengine.readString(pTHat);
  	pythiaengine.init();
  	/* Tbranching  */
  	SlowJet *antikT2 = new SlowJet(-1,.4,10,2,2,1); 
  	SlowJet *antikT3 = new SlowJet(-1,.4,10,3,2,1); 
  	SlowJet *antikT4 = new SlowJet(-1,.4,10,4,2,1); 
  	/*vectors for the TTree*/
  	vector<int> status;
  	vector<int> id;
  	vector<float> pT ;
  	vector<float> eta ;
  	vector<float> phi ;
  	vector<int> mother1;
  	vector<int> mother2;
  	vector<int> daughter1;
  	vector<int> daughter2;
  	std::vector<float> eT ;
  	std::vector<float> jety ;
  	std::vector<float> jetphi ;
  	std::vector<float> jetpT ;
  	std::vector<int> jetmult;
  	std::vector<float> jetR ;
  	/* setting up the vector branches*/
  	interestXj->Branch("Status",&status);
  	interestXj->Branch("ID",&id);
  	interestXj->Branch("pT",pT);
  	interestXj->Branch("eta",eta);
  	interestXj->Branch("phi",phi);
  	interestXj->Branch("mother1",&mother1);
  	interestXj->Branch("mother2",&mother2);
  	interestXj->Branch("daughter1",&daughter1);
  	interestXj->Branch("daughter2",&daughter2);
  	interestXj->Branch("particleET",eT);
  	interestXj->Branch("jety",jety);
  	interestXj->Branch("jetphi",jetphi);
  	interestXj->Branch("jetpT", jetpT);
  	interestXj->Branch("jetmult",jetmult);
  	interestXj->Branch("jetR",jetR);
  	/* varibles for the TTree*/
  	int position;
  	bool jetquark;
  	interestXj->Branch("photonPosition",&position);
  	interestXj->Branch("direct", &jetquark);
  	/* generation loop*/
    for (int iEvent = 0; iEvent < nEvents; ++iEvent)
  	{
  		if (!pythiaengine.next()){
      		cout<<"pythia.next() failed"<<"\n";
      		iEvent--;
      		continue;
    	}

    	for (int i = 0; i < pythiaengine.event.size(); ++i)
    	{
    		if (quickPhotonCheck(pythiaengine.event[i],gammaCut)) //eta, pT, and photon cut
    		{
          		HepMC::GenEvent* hepmcevt = new HepMC::GenEvent(); //create HepMC "event"
          		ToHepMC.fill_next_event( pythiaengine, hepmcevt ); //convert event from pythia to HepMC
          		ascii_io << hepmcevt; //write event to file 
          		delete hepmcevt; //delete event so it can be redeclared next time

    			//Photon myPhoton = Photon(i,pythiaengine.event[i].pT(),positivePhi(pythiaengine.event[i].phi()),pythiaengine.event[i].eta(),isDirect(pythiaengine.info.code()));
    			antikT2->analyze(pythiaengine.event);
    			antikT3->analyze(pythiaengine.event);
    			antikT4->analyze(pythiaengine.event);
    			/*fill the tree*/ 
    			status.clear();
  				id.clear();
  				pT .clear();
  				eta.clear() ;
  				phi .clear();
  				mother1.clear();
  				mother2.clear();
  				daughter1.clear();
  				daughter2.clear();
  				eT.clear();
  				jety .clear();
  				jetphi.clear() ;
  				jetpT.clear() ;
  				jetmult.clear() ;
  				jetR .clear();
  				/*fill the particle vectors*/
  				fillTreebyEvent(pythiaengine.event,status,id,pT,eT,eta,phi,mother1,mother2,daughter1,daughter2);
  				/*fill the jet vectors*/
  				fillTreebySlowJet(antikT2,.2,jetmult,jety,jetphi,jetpT,jetR);
  				fillTreebySlowJet(antikT2,.3,jetmult,jety,jetphi,jetpT,jetR);
  				fillTreebySlowJet(antikT2,.4,jetmult,jety,jetphi,jetpT,jetR);
  				/* fill the non vector*/
  				position=i;
  				jetquark=isDirect(pythiaengine.info.code());
  				interestXj->Fill();
     			break;
    		}
    	}
  	}
  	interestXj->Write();
  	f->Write();
  	f->Close();
}

int main(int argc, char const *argv[] )
{
	string fileOut = string(argv[1]);
	string pTHat = string(argv[2]);
	float gammaCut= strtod(argv[3],NULL);
	int nEvents = 10000;
	makeData(fileOut,nEvents, pTHat, gammaCut);
	return 0;
}

inline float deltaPhi(Photon p, Jet j){
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
