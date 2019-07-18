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

//put your cuts in here
namespace {
	int kGAMMAPTCUT=10;
	int kGAMMAETACUT=1;
}

float deltaPhi(Photon p, Jet j);

/* list of "problem" events that I am still getting
	 1) MonoJet Events 
	 2) Jet pair is not in in leading or subleading / jets to one side
	 */

void makeData(std::string filename, int nEvents){
	TFile* f = new TFile(filename.c_str(),"RECREATE");
	TTree* t=new TTree("tree100","events");

  float xjtemp;
  float phitemp;
  t->Branch("xj",&xjtemp);
  t->Branch("phi",&phitemp);


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
	//stringstream ss;
	/* generation loop*/
	for (int iEvent = 0; iEvent < nEvents; ++iEvent)
	{
		if (!pythiaengine.next()){
			cout<<"pythia.next() failed"<<"\n";
			iEvent--;
			continue;
		}
		/* zero out */
		for (int i = 0; i < pythiaengine.event.size(); ++i)
		{
			if (pythiaengine.event[i].id()==22&&pythiaengine.event[i].isFinal()&&pythiaengine.event[i].pT()>kGAMMAPTCUT&&TMath::Abs(pythiaengine.event[i].eta())<kGAMMAETACUT)
			{
				finalGammaCount++;
				//get the generation process 
				//cout<<pythiaengine.process[i].pT()<<'\n'; 
				Photon myPhoton = Photon(pythiaengine.event[i].pT(),positivePhi(pythiaengine.event[i].phi()),pythiaengine.event[i].eta(),isDirect(pythiaengine.info.code()));
				antikT->analyze(pythiaengine.event);
				//ss<<finalGammaCount<<'\n';
				//put your code here

				//ss<<"Photon phi:"<<pythiaengine.event[i].phi()<<" Jet1 phi:"<<antikT->phi(0)<<" Jet2 phi:"<<antikT->phi(1)<<"\n";
				//ss<<"Photon pT:"<<pythiaengine.event[i].pT()<<" Jet1 pT:"<<antikT->pT(0)<<" Jet2 pT:"<<antikT->pT(1)<<"\n";
				//create a map of XjPhi and output that data to TFile or txt or something 

			}
		} // particle loop
	} //pythia loop
	//data out 
	//cout<<ss.str();
	cout<<"Map:"<<finalGammaCount<<endl;
	int counter=0;
	
	t->Write();
	f->Write();
	f->Close();
}


int main()
{
	string fileOut = "XjgP1.root";
	int nEvents = 100000;
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
