/*
	for the generation of photon-jet observables through Pythia pp collisions 
	by Francesco Vassalli
*/

#include <sstream>
#include <queue>
#include <fstream>
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
	bool isJetQuark(){
		return jet.isJetQuark();
	}
	void addEvent(Event e){
		pythiaEvent=e;
	}
	Event getPythiaEvent(){
		return pythiaEvent;
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
	Event pythiaEvent;
	
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
	return p.id()==22&&p.isFinal()&&p.pT()>20&&TMath::Abs(p.eta())<1.1;
}

/* list of "problem" events that I am still getting
	1) MonoJet Events 
	2) Jet pair is not in in leading or subleading / jets to one side
*/

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
/*
stringstream eventToStream(Event e){
	stringstream ss;
	ss<<"_______________________________________________________________________________________________________________________"<<'\n';
    ss<<"||                                   Events that contain isophoton                                                   ||"<<'\n';
    ss<<"-----------------------------------------------------------------------------------------------------------------------"<<'\n';
    ss<<"|| Particle  |  Status   |   ID   |   pT  |   eta   |   phi   |  mother 1  |  mother 2  |  daughter 1  |  daughter 2 ||"<<'\n';
    ss<<"-----------------------------------------------------------------------------------------------------------------------"<<'\n';
    for(int i = 0; i < e.size(); i++)
    {
     	ss<<"||   "<<i<<"    |    "<<e[i].status()<<"    |    "<<e[i].id()<<"    |    "<<e[i].pT()<<"    |    "<< e[i].eta()<<"    |    "
     	<<e[i].phi()<<"    |    "<< e[i].mother1()<<"    |    "<< e[i].mother2()<<"    |    "<<e[i].daughter1()<<"    |    "<<e[i].daughter2()<<"    |    "<<'\n';
    }
    ss<<"||_____________________________________________________________________________________________________________________" <<'\n';
    ss<<'\n';
    return ss;
}*/

void makeData(std::string filename, int nEvents){
	string interestName = filename+".txt";
	filename+=".root";
	TFile* f = new TFile(filename.c_str(),"RECREATE");
  	TTree* directTree=new TTree("tree100","direct");
  	TTree* fragTree = new TTree("tree200","frag");
  	TTree* fragTreeISO = new TTree("tree300","fragISO");
  	TTree* directTreeISO = new TTree("tree400","directISO");
  	TTree* interestXj
  	std::ofstream ofs;
  	ofs.open(interestName.c_str(),ofstream::out);
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
  	SlowJet *antikT = new SlowJet(-1,.4,10,4,2,1); // what should be my minimum jet pT
  	 // are the monojets low pT photons with match 
  	//Jet *temp =NULL;
  	int finalGammaCount=0;
  	//stringstream ss;
  	//stringstream interest;
  	stringstream xjInterest;
  	queue<PhotonJet> dmap;
  	queue<PhotonJet> fmap;
  	//queue<float> monoJetEventPhis;
  	PhotonJet tempXj;
  	/* generation loop*/
  	string nullInterest="Interest=0";
  	bool interestHit=false;
  	int interestC=0;

  	/*
	TTree *tempEvent= new TTree("event","event");
  	tempEvent->Branch("Particles",&Particles);
  	tempEvent->Branch("photonType",&photonType);
  	tempEvent->Branch("Status",Status,"Status[Particles]/I");
  	tempEvent->Branch("ID",ID,"ID[Particles]/I");
  	tempEvent->Branch("pT",pT,"pT[Particles]/F");
  	tempEvent->Branch("eta",eta,"eta[Particles]/F");
  	tempEvent->Branch("phi",phi,"phi[Particles]/F");
  	tempEvent->Branch("Et",Et,"Et[Particles]/F");
  	tempEvent->Branch("mother1", mother1,"mother1[Particles]/F");
  	tempEvent->Branch("mother2", mother2,"mother2[Particles]/F");
  	tempEvent->Branch("daughter1", daughter1,"daughter1[Particles]/F");
  	tempEvent->Branch("daughter2", daughter2,"daughter2[Particles]/F");
  	*/

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
    			Photon myPhoton = Photon(pythiaengine.event[i].pT(),positivePhi(pythiaengine.event[i].phi()),pythiaengine.event[i].eta(),isDirect(pythiaengine.info.code()),EventToQueue(pythiaengine.event));
    			antikT->analyze(pythiaengine.event);
    			//ss<<finalGammaCount<<'\n';
    			if(antikT->sizeJet()>1){
    				//biasing by only looking at first 2?
    				tempXj=PhotonJet(myPhoton,Jet(antikT->pT(1),positivePhi(antikT->phi(1)),positivePhi(antikT->y(1))),Jet(antikT->pT(0),positivePhi(antikT->phi(0)),positivePhi(antikT->y(0))));
    				if (tempXj.getphi()>7*TMath::Pi()/8)
    				{
    					tempXj.matchPartons(p1,p2);
    					tempXj.addEvent(pythiaengine.event);
    					if (tempXj.isDirect())
    					{
    						dmap.push(tempXj);
    					}
    					else{
    						fmap.push(tempXj);
    						if (tempXj.getXj()>1.5&&tempXj.getXj()<1.7)
    						{
    							xjInterest<<eventToStream(pythiaengine.event).str();
    						}
    					}
    					
    				}//do I want an else?
    			}
    			/*else{ // to handle monojet 
    				tempXj=PhotonJet(myPhoton);
    				monoJetEventPhis.push(myPhoton.getphi().value);
    				map.push(tempXj);
    			}*/
    			//this if is an error check process to see which events the XjPhi is screwing up
    			/*if (tempXj.getphi()<7*TMath::Pi()/8)
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
     			*/
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
  	float phitemp,monPhitemp,pTtemp;
  	directTree->Branch("xj",&xjtemp);
  	fragTree->Branch("xj",&xjtemp);
  	directTree->Branch("phi",&phitemp);
  	fragTree->Branch("phi",&phitemp);
  	fragTree->Branch("gpT",&pTtemp);
  	directTree->Branch("gpT",&pTtemp);
  	directTreeISO->Branch("xj",&xjtemp);
  	fragTreeISO->Branch("xj",&xjtemp);
  	directTreeISO->Branch("phi",&phitemp);
  	fragTreeISO->Branch("phi",&phitemp);
  	fragTreeISO->Branch("gpT",&pTtemp);
  	directTreeISO->Branch("gpT",&pTtemp);

  	//t->Branch("monPhi",&monPhitemp);
  	bool jetquark;
  	fragTree->Branch("jetquark",&jetquark);
  	directTree->Branch("jetquark",&jetquark);
  	fragTreeISO->Branch("jetquark",&jetquark);
  	directTreeISO->Branch("jetquark",&jetquark);

  	/*fragTree->Branch("FullEvent",&tempEvent);
  	directTree->Branch("FullEvent",&tempEvent);
  	fragTreeISO->Branch("FullEvent",&tempEvent);
  	directTreeISO->Branch("FullEvent",&tempEvent);*/
  	//cout<<ss.str();
  	//cout<<"Map:"<<finalGammaCount<<endl;
  	//interest<<interestC;
  	//int counter=0;
  	/*while(!monoJetEventPhis.empty()){
  		phitemp=map.front().getphi().value;
  		jetquark=map.front().isJetQuark();
  		monPhitemp=monoJetEventPhis.front();
  		monoJetEventPhis.pop();
  		t->Fill();
  		map.pop();
  	}*/
  	while(!dmap.empty()){
  		xjtemp=dmap.front().getXj().value;
  		pTtemp=dmap.front().getPhoton().getpT().value;
  		phitemp=dmap.front().getphi().value;
  		jetquark=dmap.front().isJetQuark();
  		//tempEvent=dmap.front().getPythiaEvent();
  		//monPhitemp=-2*TMath::Pi();
  		directTree->Fill();
  		if (dmap.front().getPhoton().getIsoEt()<3)
  		{
  			directTreeISO->Fill();
  		}
  		dmap.pop();
  	}
  	while(!fmap.empty()){
  		xjtemp=fmap.front().getXj().value;
  		pTtemp=fmap.front().getPhoton().getpT().value;
  		phitemp=fmap.front().getphi().value;
  		//tempEvent=fmap.front().getPythiaEvent();
  		jetquark=fmap.front().isJetQuark();
  		//monPhitemp=-2*TMath::Pi();
  		fragTree->Fill();
  		if (fmap.front().getPhoton().getIsoEt()<3)
  		{
  			fragTreeISO->Fill();
  		}
  		fmap.pop();
  	}
  	/*if (!interestHit)
  	{
  		cout<<nullInterest;
  	}
  	else{
  		cout<<interest.str();
  	}*/
  	ofs<<xjInterest.rdbuf()<<std::endl;
  	ofs.close();
  	directTree->Write();
  	fragTree->Write();
  	f->Write();
  	f->Close();
}

int main(int argc, char const *argv[] )
{
	string fileOut = string(argv[1]);
	int nEvents = 300000;
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