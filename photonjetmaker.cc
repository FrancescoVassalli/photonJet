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
#include "TROOT.h"

float deltaPhi(Photon p, Jet j);
float deltaR(Parton,Jet);

template<class T>
void swapPointer(T* a, T* b){
	T* t=a;
	a=b;
	b=t;
}

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

int fillTreebyEvent(Event e, int* status,int* id,float* pT,float* eT,float* eta,float* phi,int* mother1,int* mother2, float* energy,int* position){
	int arrcount=0;
	int originalpositon=*position;
	for (int i = 0; i < e.size(); ++i)
	{
	  if(e[i].isFinal()){
		status[arrcount]=e[i].status();
  		id[arrcount]=e[i].id();
  		pT[arrcount]=e[i].pT();
  		eT[arrcount]=e[i].eT();
  		eta[arrcount]=e[i].eta();
  		phi[arrcount]=e[i].phi();
  		energy[arrcount]=e[i].e();
  		mother1[arrcount]=e[i].mother1();
  		mother2[arrcount]=e[i].mother2();
  		arrcount++;
	  }
	  else{
	  	if(i<originalpositon) *position=*position-1;
	  }
	}
	status[arrcount]=e[5].status();
  	id[arrcount]=e[5].id();
  	pT[arrcount]=e[5].pT();
  	eT[arrcount]=e[5].eT();
  	eta[arrcount]=e[5].eta();
  	phi[arrcount]=e[5].phi();
  	energy[arrcount]=e[5].e();
  	mother1[arrcount]=e[5].mother1();
  	mother2[arrcount]=e[5].mother2();
  	status[arrcount]=e[5].status();
  	id[arrcount+1]=e[6].id();
  	pT[arrcount+1]=e[6].pT();
  	eT[arrcount+1]=e[6].eT();
  	eta[arrcount+1]=e[6].eta();
  	energy[arrcount+1]=e[6].e();
  	phi[arrcount+1]=e[6].phi();
  	mother1[arrcount+1]=e[6].mother1();
  	mother2[arrcount+1]=e[6].mother2();
	return --arrcount;
}

int fillTreebySlowJet(SlowJet* a1, SlowJet* a2,SlowJet* a3,int* mult, float* y, float* phi, float* pT,float* r,float* m,float* pz){
	int arrcount=0;
	for (int i = 0; i < a1->sizeJet(); ++i)
	{
		mult[arrcount]=a1->multiplicity(i);
		y[arrcount]=a1->y(i);
		phi[arrcount]=a1->phi(i);
		pT[arrcount]=a1->pT(i);
		r[arrcount]=0.2;
		m[arrcount]=a1->m(i);
		pz[arrcount]=(a1->p(i)).pz();
		arrcount++;
	}
	for (int i = 0; i < a2->sizeJet(); ++i)
	{
	    mult[arrcount]=a2->multiplicity(i);
	    y[arrcount]=a2->y(i);
	    phi[arrcount]=a2->phi(i);
	    pT[arrcount]=a2->pT(i);
	    r[arrcount]=0.3;
	    m[arrcount]=a2->m(i);
	    pz[arrcount]=(a2->p(i)).pz();
		arrcount++;
	}
	for (int i = 0; i < a3->sizeJet(); ++i)
    {
        mult[arrcount]=a3->multiplicity(i);
        y[arrcount]=a3->y(i);
        phi[arrcount]=a3->phi(i);
	    pT[arrcount]=a3->pT(i);
        r[arrcount]=0.4;
        m[arrcount]=a3->m(i);
        pz[arrcount]=(a3->p(i)).pz();
        arrcount++;
    }
    return --arrcount;

}


inline bool piZeroFilter(Event e, int position){ //return true if either mother is a piZero
	int mother1 = e[position].mother1();
	int mother2 = e[position].mother2();
	return e[mother1].id()==111 || e[mother2].id()==111;
}

inline bool isQuark(int ID){
		return TMath::Abs(ID)>0&&TMath::Abs(ID)<9;
}

inline bool bothParentQuarkORGluon(Event e, int position){ //returns true if both parents are either a quark or gluon
	int mother1 = e[position].mother1();
	int mother2 = e[position].mother2();
	return (isQuark(e[mother1].id())||e[mother1].id()==21) && (e[mother2].id()==21||isQuark(e[mother2].id()));
}

inline bool nonHadronParent(Event e, int position){ //returns true if both parents are not hadrons
	int mother1 = e[position].mother1();
	int mother2 = e[position].mother2();
	return ( TMath::Abs(e[mother1].id())<100 && TMath::Abs(e[mother1].id())<100 );
}

bool bothParentQuarkORGluon_v2(Event e, int position) //returns true if both parents are quarks or gluons, trying because above always returns false
{ 
  int mother1 = e[position].mother1();
  int mother2 = e[position].mother2();
  bool mother1check = false;
  bool mother2check = false;
  ////////everything below this is FRAN's last return statement//
  cout<<"mother1: ID: "<<e[mother1].id()<<endl; 
  if(isQuark(e[mother1].id()) or e[mother1].id() == 21) 
    { 
      mother1check = true; 
      cout<<"Good mother!"<<endl; 
    } 
  cout<<"mother1: ID: "<<e[mother1].id()<<endl; 
  cout<<endl; 
  if(isQuark(e[mother2].id()) or e[mother2].id() == 21) 
    { 
      mother2check = true; 
      cout<<"Good mother!"<<endl; 
    } 
  if(mother1check == true and mother2check == true) 
    { 
      return true; 
    } 
  else 
    { 
      return false; 
    } 
  ///////////////////////////////////////////////////////////////
}

void makeData(std::string filename, long nEvents, string pTHat, float gammaCut, bool genHEP)
{
  	using namespace HepMC;
	string hepName = filename+".dat";    //filenames
	filename+=".root";

	TFile* f = new TFile(filename.c_str(),"RECREATE");
  	TTree* interestXj = new TTree("interest","interest");
  	interestXj->SetAutoSave(3000);

    HepMC::Pythia8ToHepMC ToHepMC;    // Interface for conversion from Pythia8::Event to HepMC event.
    HepMC::IO_GenEvent ascii_io_direct(hepName, std::ios::out); //file where HepMC events will be stored.


	/*pythia set up*/
    Pythia pythiaengine;
    pythiaengine.readString("Beams:eCM = 200."); //LHC VS RHIC
    /* ATLAS photon generation functions 
"HardQCD:gg2gg = on",
"HardQCD:gg2qqbar = on",
"HardQCD:qg2qg = on",
"HardQCD:qq2qq = on",
"HardQCD:qqbar2gg = on",
"HardQCD:qqbar2qqbarNew = on",
"PromptPhoton:qg2qgamma = on",
"PromptPhoton:qqbar2ggamma = on",
"PhaseSpace:pTHatMin = 25"*/
  	pythiaengine.readString("HardQCD:gg2gg = on");
  	pythiaengine.readString("HardQCD:gg2qqbar = on");
  	pythiaengine.readString("HardQCD:qg2qg = on");
  	pythiaengine.readString("HardQCD:qq2qq = on");
  	pythiaengine.readString("HardQCD:qqbar2gg = on");
  	pythiaengine.readString("HardQCD:qqbar2qqbarNew = on");
  	pythiaengine.readString("PromptPhoton:qg2qgamma = on");
  	pythiaengine.readString("PromptPhoton:qqbar2ggamma = on");
  	pythiaengine.readString("Random::setSeed = on");
  	pythiaengine.readString("Random::seed =0");
  	pTHat = "PhaseSpace:pTHatMin = "+pTHat+".";
  	pythiaengine.readString(pTHat);
  	pythiaengine.init();
  	/* Tbranching  */
  	SlowJet *antikT2 = new SlowJet(-1,.4,10,2,2,1); 
  	SlowJet *antikT3 = new SlowJet(-1,.4,10,3,2,1); 
  	SlowJet *antikT4 = new SlowJet(-1,.4,10,4,2,1); 
  	/*int for the TTree*/
  	int status[300];
  	int id[300];
  	int mother1[300];
  	int mother2[300];

  	float pT[300];
  	float eta[300];
  	float phi[300];
	float eT[300];
	float e[300];
  	
  	int jetmult[200];
  	float jety[200];
  	float jetphi[200];
  	float jetpT[200];
  	float jetR[200];
  	float jetpz[200];
  	float jetm[200];
  	/* setting up the branches*/
  	interestXj->Branch("Status",&status,"status[300]/I");
  	interestXj->Branch("ID",&id,"id[300]/I");
  	interestXj->Branch("pT",pT,"pT[300]/F");
  	interestXj->Branch("eta",eta,"eta[300]/F");
  	interestXj->Branch("phi",phi,"phi[300]/F");
	interestXj->Branch("eT",eT,"eT[300]/F");
	interestXj->Branch("e",e,"e[300]/F");
  	interestXj->Branch("jety",jety,"jety[200]/F");
  	interestXj->Branch("jetphi",jetphi,"jetphi[200]/F");
  	interestXj->Branch("jetpT", jetpT,"jetpT[200]/F");
  	interestXj->Branch("jetmult",&jetmult,"jetmult[200]/I");
  	interestXj->Branch("jetR",jetR,"jetR[200]/F");
  	interestXj->Branch("jetm",jetm,"jetm[200]/F");
  	interestXj->Branch("jetpz",jetpz,"jetpz[200]/F");
  	/* varibles for the TTree*/
  	int position,end,jetend;
  	int jetquark;
	float pthat;
  	interestXj->Branch("photonPosition",&position);
  	interestXj->Branch("infocode", &jetquark);
	interestXj->Branch("pTHat", &pthat);
  	interestXj->Branch("end",&end);
  	interestXj->Branch("jetend",&jetend);
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
    		int finalcount=0;
    		if (quickPhotonCheck(pythiaengine.event[i],gammaCut)&&nonHadronParent(pythiaengine.event,i)) //eta, pT, and photon cut then parent cut
    		{
    			
    			HepMC::GenEvent* hepmcevtfrag = new HepMC::GenEvent(); //create HepMC "event" for frag photons
          		ToHepMC_frag.fill_next_event( pythiaengine, hepmcevtfrag ); //convert event from pythia to HepMC
          		ascii_io_frag << hepmcevtfrag;//write event to file
          		delete hepmcevtfrag; //delete event so it can be redeclared next time
    			antikT2->analyze(pythiaengine.event);
    			antikT3->analyze(pythiaengine.event);
    			antikT4->analyze(pythiaengine.event);
    			/*fill the tree*/ 
  				position=i;
    			jetquark=pythiaengine.info.code();
  				end=fillTreebyEvent(pythiaengine.event,status,id,pT,eT,eta,phi,mother1,mother2,e,&position);
  				/*fill the jet vectors*/
  				jetend=fillTreebySlowJet(antikT2,antikT3,antikT4,jetmult,jety,jetphi,jetpT,jetR,jetm,jetpz);
  				/* fill the non vector*/
				pthat=pythiaengine.info.pTHat();
  				interestXj->Fill();
     			break;
    		}
    	}
  	}
  	delete antikT2; //clear the mem
  	delete antikT3;
  	delete antikT4;
  	interestXj->Write();
  	f->Close();
  	delete f;
  	f=NULL;
}

int main(int argc, char const *argv[] )
{
	string fileOut = string(argv[1]);
	string pTHat = string(argv[2]);
	float gammaCut= strtod(argv[3],NULL);
	long nEvents =strtol(argv[4],NULL,10);  // 5000000;
	bool genHEP=true;
	makeData(fileOut,nEvents, pTHat, gammaCut,genHEP);
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
