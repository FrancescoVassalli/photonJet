
/* to find the number of gammajets that fall in all ranges of eta vs a specific range*/
#include "Pythia8/Pythia.h"
using namespace Pythia8;
using namespace std;
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TMath.h"
#include "Utils.C"



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
	
};

template<class T>
T positivePhi(T in){
	if (in<0)
	{
		in = in+2*TMath::Pi();
	}
	return in;
}

inline bool quickPhotonCheck(Particle p){
	return p.id()==22&&p.isFinal()&&p.pT()>10;
}

queue<myParticle> EventToQueue(Event e){
	myParticle temp;
	queue<myParticle> r;
	for (int i = 0; i < e.size(); ++i)
	{
		temp = myParticle(e[i].id(),e[i].pT(),e[i].phi(),e[i].y());
		r.push(temp);
	}
	return r;
}


void makeData(std::string filename, int nEvents){
	/*TFile* f = new TFile(filename.c_str(),"RECREATE");
  	TTree* directTree=new TTree("tree100","direct");*/
  	Pythia pythiaengine;

  	pythiaengine.readString("Beams:eCM = 200.");
 	pythiaengine.readString("promptphoton:all = on");
 	pythiaengine.readString("HardQCD:all = on");
 	pythiaengine.readString("PhaseSpace:pTHatMin = 10.");
 	pythiaengine.readString("Random::setSeed = on");
  	pythiaengine.readString("Random::seed =0");
  	pythiaengine.init();

  	SlowJet *antikT = new SlowJet(-1,.4,10,4,2,1);
  	int total=0;
  	int inside=0;
  	int insideLowEng
  	PhotonJet tempXj;

  	for (int iEvent = 0; iEvent < nEvents; ++iEvent)
  	{
  		if (!pythiaengine.next()){
      		cout<<"pythia.next() failed"<<"\n";
      		iEvent--;
      		continue;
    	}
    	for (int i = 0; i < pythiaengine.event.size(); ++i)
    	{
    		if (quickPhotonCheck(pythiaengine.event[i]))
    		{
    			total++;
    			Photon myPhoton = Photon(pythiaengine.event[i].pT(),positivePhi(pythiaengine.event[i].phi()),pythiaengine.event[i].eta(),EventToQueue(pythiaengine.event));
    			antikT->analyze(pythiaengine.event);
    			if(antikT->sizeJet()>1){
    				inside++;
    				if (myPhoton.getIsoEt()<3)
    				{
    					insideLowEng++;
    				}
    			}
    		}
    	}
    }
    cout<<"Total:"<<total<<" Inside:"<<inside<<" Inside and LowEt:"<<insideLowEng<<endl;

  }

int main(int argc, char const *argv[] )
{
	string fileOut = string(argv[1]);
	int nEvents = 10000;
	makeData(fileOut,nEvents);
	return 0;
}