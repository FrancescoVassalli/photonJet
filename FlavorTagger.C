using namespace std;

#include "Utils.C"
#include <sstream>
#include <iostream>

queue<Jet> makeJets(float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, float* pz,int SIZE){
	queue<Jet> r;
	
	for (int i = 0; i < SIZE; ++i)
	{
		if (TMath::Abs(jetphi[i]-photonPhi)>7.0*TMath::Pi()/8.0)
		{
			r.push(Jet(jetpT[i],jetphi[i],jety[i],jetR[i],pz[i]));
			
		}
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

queue<Jet> getRJets(float cut,float r,float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, float* jetpz, int SIZE){
	return getRJets(cut,r,makeJets(photonPhi,jetphi,jety,jetpT,jetR,jetpz,SIZE));
}

Jet* getMaxJet(queue<Jet> jQ){
	if(jQ.empty()) return NULL;
	else{
		Jet *r = new Jet();
		while(!jQ.empty()){
			if (jQ.front()>*r)
			{
				*r=jQ.front();
			}
			jQ.pop();
		}
		return r;
	}
}

queue<XjPhi> getXjPhi(TChain* tree, float cut){
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
	queue<XjPhi> xjPhiQ;
	int isocutCount=tree->GetEntries();
	int jetCutCount=tree->GetEntries();
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		tree->GetEntry(i);
		end++; // ends are measured inclusive I want them exclusive
		jetend++;
		Photon pTemp=Photon(end,photonPosition,eT,phi,eta,direct,.3); //make a photon
		if (pTemp.getIsoEt()>0){ //isoEt cut
			isocutCount--;
			jetCutCount--;
			continue;
		}
		Jet* maxJet=NULL;
		maxJet= getMaxJet(getRJets(cut,.4,pTemp.getphi().value,jetphi,jety,jetpT,jetR,jetpz,jetend)); //make all the jets then match
		if (maxJet!=NULL) //if there is a matched jet
		{
			maxJet->setParton(Parton(id[end],phi[end],eta[end],eT[end],e[end]),Parton(id[end+1],phi[end+1],eta[end+1],eT[end+1],e[end+1]));
			xjPhiQ.push(XjPhi(pTemp,*maxJet));
			delete maxJet;
			maxJet=NULL;
		}
		else jetCutCount--;
	}
	tree->ResetBranchAddresses();
	return xjPhiQ;
}

queue<XjPhi> handleFile(string name, string extension, int filecount, float cut){
	TChain *all = new TChain("interest");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = name+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	queue<XjPhi> mainQ1 = getXjPhi(all,cut);
	delete all;
	return mainQ1;
}

inline bool inRange(float in, float low, float high){ // inclusive-exclusive
	return in>=low&&in<high;
}

void fillPlot(TH1F** hlist,queue<XjPhi> xjPhiQ, float low, float high){ //low inclusive high exclusvie
	while(!xjPhiQ.empty())
	{
		if (inRange(xjPhiQ.front().getPhoton().getpT(),low,high))
		{
			if (xjPhiQ.front().getPhoton().isDirect()) 
			{
				if (xjPhiQ.front().isQuark())
				{
					hlist[0]->Fill(xjPhiQ.front().getPhoton().getpT().value);
				}
				else hlist[1]->Fill(xjPhiQ.front().getPhoton().getpT().value);
			}
			else{
				if (xjPhiQ.front().isQuark())
				{
					hlist[2]->Fill(xjPhiQ.front().getPhoton().getpT().value);
				}
				else hlist[3]->Fill(xjPhiQ.front().getPhoton().getpT().value);
			}
		}
		xjPhiQ.pop();
	}
}

void fillPlotJet(TH1F** hlist,queue<XjPhi> xjPhiQ, float low, float high){ //low inclusive high exclusvie
	while(!xjPhiQ.empty())
	{
		if (inRange(xjPhiQ.front().getJet().getpT(),low,high))
		{
			if (xjPhiQ.front().getPhoton().isDirect()) 
			{
				if (xjPhiQ.front().isQuark())
				{
					hlist[0]->Fill(xjPhiQ.front().getJet().getpT().value);
				}
				else hlist[1]->Fill(xjPhiQ.front().getJet().getpT().value);
			}
			else{
				if (xjPhiQ.front().isQuark())
				{
					hlist[2]->Fill(xjPhiQ.front().getJet().getpT().value);
				}
				else hlist[3]->Fill(xjPhiQ.front().getJet().getpT().value);
			}
		}
		xjPhiQ.pop();
	}
}

void fillPlotJet2(TH1F** hlist,queue<XjPhi> xjPhiQ, float low, float high, float photonHigh){ //low inclusive high exclusvie
	while(!xjPhiQ.empty())
	{
		if (inRange(xjPhiQ.front().getJet().getpT(),low,high)&&inRange(xjPhiQ.front().getPhoton().getpT().value,low,photonHigh))
		{
			if (xjPhiQ.front().getPhoton().isDirect()) 
			{
				if (xjPhiQ.front().isQuark())
				{
					hlist[0]->Fill(xjPhiQ.front().getJet().getpT().value);
				}
				else hlist[1]->Fill(xjPhiQ.front().getJet().getpT().value);
			}
			else{
				if (xjPhiQ.front().isQuark())
				{
					hlist[2]->Fill(xjPhiQ.front().getJet().getpT().value);
				}
				else hlist[3]->Fill(xjPhiQ.front().getJet().getpT().value);
			}
		}
		xjPhiQ.pop();
	}
}

void filltotal(TH1F* total, queue<XjPhi> q,float low , float high){
	while(!q.empty()){
		if (inRange(q.front().getPhoton().getpT().value, low, high))
		{
			total->Fill(q.front().getPhoton().getpT().value);
		}
		q.pop();
	}
}

void filltotalJet(TH1F* total, queue<XjPhi> q, float low, float high){
	while(!q.empty()){
		if (inRange(q.front().getJet().getpT().value, low, high))
		{
			total->Fill(q.front().getJet().getpT().value);
		}
		q.pop();
	}
}

void fillXj(TH1F** hlist,queue<XjPhi> xjPhiQ){ //low inclusive high exclusvie
	while(!xjPhiQ.empty())
	{
			if (xjPhiQ.front().getPhoton().isDirect()) 
			{
				if (xjPhiQ.front().isQuark())
				{
					hlist[0]->Fill(xjPhiQ.front().getJet().getpT().value/xjPhiQ.front().getPhoton().getpT().value);
				}
				else hlist[1]->Fill(xjPhiQ.front().getJet().getpT().value/xjPhiQ.front().getPhoton().getpT().value);
			}
			else{
				if (xjPhiQ.front().isQuark())
				{
					hlist[2]->Fill(xjPhiQ.front().getJet().getpT().value/xjPhiQ.front().getPhoton().getpT().value);
				}
				else hlist[3]->Fill(xjPhiQ.front().getJet().getpT().value/xjPhiQ.front().getPhoton().getpT().value);
			}
		xjPhiQ.pop();
	}
}

void fillDeltaPhi(TH1F** hlist,queue<XjPhi> xjPhiQ){ //low inclusive high exclusvie
	while(!xjPhiQ.empty())
	{
			if (xjPhiQ.front().getPhoton().isDirect()) 
			{
				if (xjPhiQ.front().isQuark())
				{
					hlist[0]->Fill(xjPhiQ.front().getphi().value);
				}
					hlist[1]->Fill(xjPhiQ.front().getphi().value);
			}
			else{
				if (xjPhiQ.front().isQuark())
				{
					hlist[2]->Fill(xjPhiQ.front().getphi().value);
				}
					hlist[3]->Fill(xjPhiQ.front().getphi().value);
			}
		xjPhiQ.pop();
	}
}

void plotXj(queue<XjPhi> mainQ1){
	TCanvas *tc =new TCanvas();
	const int nBins = 18;
	TH1F *p_dircQ = new TH1F("plot1","",18,0,1.8); 
	TH1F *p_dircG = new TH1F("plot2","",18,0,1.8);  
	TH1F *p_fragQ = new TH1F("plot3","",18,0,1.8);  
	TH1F *p_fragG = new TH1F("plot4","",18,0,1.8);
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_fragQ,"frag Quark","p");
	tl->AddEntry(p_fragG,"frag Gluon","p");
	tl->AddEntry(p_dircQ,"direct Quark","p");
	tl->AddEntry(p_dircG,"direct Gluon","p");
	TH1F* hlist[4];
	hlist[0]=p_dircQ;
	hlist[1]=p_dircG;
	hlist[2]=p_fragQ;
	hlist[3]=p_fragG;
	makeDifferent(hlist,4);
	fillXj(hlist,mainQ1);
	normalizeTotal(hlist,4);
	gPad->SetLogy();
	cout<<"plot1 filled"<<'\n';

	/*normalizeBins(hlist,4);
	THStack *myStack = getStack(hlist,4);
	myStack->Draw();
	myStack->GetXaxis()->SetMoreLogLabels();
	axisTitles(myStack,"pT#gamma GeV","Relative Count");
	axisTitleOffset(myStack,.8);*/
	hlist[0]->Draw();
	//e1->GetXaxis()->SetMoreLogLabels();
	hlist[0]->SetTitle(";pT-Jet GeV;relative count");
	hlist[1]->Draw("same");
	hlist[2]->Draw("same");
	hlist[3]->Draw("same");
	tl->Draw();
}

void plotDeltaPhi(queue<XjPhi> mainQ1){
	TCanvas *tc =new TCanvas();
	const int nBins = 18;
	TH1F *p_dircQ = new TH1F("plot1","",18,.5,TMath::Pi()); 
	TH1F *p_dircG = new TH1F("plot2","",18,.5,TMath::Pi());  
	TH1F *p_fragQ = new TH1F("plot3","",18,.5,TMath::Pi());  
	TH1F *p_fragG = new TH1F("plot4","",18,.5,TMath::Pi());
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_fragQ,"frag Quark","p");
	tl->AddEntry(p_fragG,"frag Gluon","p");
	tl->AddEntry(p_dircQ,"direct Quark","p");
	tl->AddEntry(p_dircG,"direct Gluon","p");
	TH1F* hlist[4];
	hlist[0]=p_dircQ;
	hlist[1]=p_dircG;
	hlist[2]=p_fragQ;
	hlist[3]=p_fragG;
	makeDifferent(hlist,4);
	fillDeltaPhi(hlist,mainQ1);
	normalizeTotal(hlist,4);
	gPad->SetLogy();
	cout<<"plot1 filled"<<'\n';

	/*normalizeBins(hlist,4);
	THStack *myStack = getStack(hlist,4);
	myStack->Draw();
	myStack->GetXaxis()->SetMoreLogLabels();
	axisTitles(myStack,"pT#gamma GeV","Relative Count");
	axisTitleOffset(myStack,.8);*/
	hlist[0]->Draw();
	//e1->GetXaxis()->SetMoreLogLabels();
	hlist[0]->SetTitle(";pT-Jet GeV;relative count");
	hlist[1]->Draw("same");
	hlist[2]->Draw("same");
	hlist[3]->Draw("same");
	tl->Draw();
}

void makePlot(queue<XjPhi> mainQ1,queue<XjPhi> mainQ2,queue<XjPhi> mainQ3,queue<XjPhi> mainQ4){
	TCanvas *tc =new TCanvas();
	const int nBins = 14;
	double bins[nBins+1] = {10,11,12,13,14,16,18,20,22,26,30,33,40,47,54};
	TH1F *p_dircQ = new TH1F("plot1","",nBins,bins); 
	TH1F *p_dircG = new TH1F("plot2","",nBins,bins);  
	TH1F *p_fragQ = new TH1F("plot3","",nBins,bins);  
	TH1F *p_fragG = new TH1F("plot4","",nBins,bins);
	TH1F *totals = new TH1F("total","",nBins,bins);
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_fragQ,"Frag #gamma Quark Jet","p");
	tl->AddEntry(p_fragG,"Frag #gamma Gluon Jet","p");
	tl->AddEntry(p_dircQ,"Direct #gamma Quark Jet","p");
	tl->AddEntry(p_dircG,"Direct #gamma Gluon Jet","p");
	TH1F* hlist[4];
	hlist[0]=p_dircQ;
	hlist[1]=p_dircG;
	hlist[2]=p_fragQ;
	hlist[3]=p_fragG;
	makeDifferent(hlist,4);
	fillPlot(hlist,mainQ1,10,20);
	cout<<"plot1 filled"<<'\n';
	fillPlot(hlist,mainQ2,20,30);
	cout<<"plot2 filled"<<'\n';
	fillPlot(hlist,mainQ3,30,40);
	cout<<"plot3 filled"<<'\n';
	fillPlot(hlist,mainQ4,40,50);
	cout<<"plot4 filled"<<'\n';
	filltotal(totals,mainQ1,10,20);
	filltotal(totals,mainQ2,20,30);
	filltotal(totals,mainQ3,30,40);
	filltotal(totals,mainQ4,40,50);
	cout<<"total filled"<<'\n';
	printHist(totals);
	TEfficiency *e1 = new TEfficiency(*p_dircQ,*totals);
	TEfficiency *e2 = new TEfficiency(*p_dircG,*totals);
	TEfficiency *e3 = new TEfficiency(*p_fragQ,*totals);
	TEfficiency *e4 = new TEfficiency(*p_fragG,*totals);
	makeDifferent(e1,0);
	makeDifferent(e2,1);
	makeDifferent(e3,2);
	makeDifferent(e4,3);

	/*normalizeBins(hlist,4);
	THStack *myStack = getStack(hlist,4);
	myStack->Draw();
	myStack->GetXaxis()->SetMoreLogLabels();
	axisTitles(myStack,"pT#gamma GeV","Relative Count");
	axisTitleOffset(myStack,.8);*/
	e1->Draw();
	//e1->GetXaxis()->SetMoreLogLabels();
	e1->SetTitle(";pT#gamma GeV;relative count");
	e2->Draw("same");
	e3->Draw("same");
	e4->Draw("same");
	tl->Draw();

}

void makePlotJet(queue<XjPhi> mainQ1,queue<XjPhi> mainQ2,queue<XjPhi> mainQ3,queue<XjPhi> mainQ4){
	cout<<"Sizes 1:"<<mainQ1.size()<<" 2:"<<mainQ2.size()<<" 3:"<<mainQ3.size()<<" 4:"<<mainQ4.size()<<'\n';
	TCanvas *tc =new TCanvas();
	const int nBins = 14;
	double bins[nBins+1] = {10,11,12,13,14,16,18,20,22,26,30,33,40,47,54};
	TH1F *p_dircQ = new TH1F("plot1","",nBins,bins); 
	TH1F *p_dircG = new TH1F("plot2","",nBins,bins);  
	TH1F *p_fragQ = new TH1F("plot3","",nBins,bins);  
	TH1F *p_fragG = new TH1F("plot4","",nBins,bins);
	TH1F *totals = new TH1F("total","",nBins,bins);
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_fragQ,"frag Quark","p");
	tl->AddEntry(p_fragG,"frag Gluon","p");
	tl->AddEntry(p_dircQ,"direct Quark","p");
	tl->AddEntry(p_dircG,"direct Gluon","p");
	TH1F* hlist[4];
	hlist[0]=p_dircQ;
	hlist[1]=p_dircG;
	hlist[2]=p_fragQ;
	hlist[3]=p_fragG;
	makeDifferent(hlist,4);
	fillPlotJet(hlist,mainQ1,25,35);
	cout<<"plot1 filled"<<'\n';
	fillPlotJet(hlist,mainQ2,35,45);
	cout<<"plot2 filled"<<'\n';
	fillPlotJet(hlist,mainQ3,45,55);
	cout<<"plot3 filled"<<'\n';
	fillPlotJet(hlist,mainQ4,55,900);
	cout<<"plot4 filled"<<'\n';
	filltotalJet(totals,mainQ1,25,35);
	filltotalJet(totals,mainQ2,35,55);
	filltotalJet(totals,mainQ3,55,65);
	filltotalJet(totals,mainQ4,45,55);
	cout<<"total filled"<<'\n';
	//printHist(totals);
	TEfficiency *e1 = new TEfficiency(*p_dircQ,*totals);
	TEfficiency *e2 = new TEfficiency(*p_dircG,*totals);
	TEfficiency *e3 = new TEfficiency(*p_fragQ,*totals);
	TEfficiency *e4 = new TEfficiency(*p_fragG,*totals);
	makeDifferent(e1,0);
	makeDifferent(e2,1);
	makeDifferent(e3,2);
	makeDifferent(e4,3);

	/*normalizeBins(hlist,4);
	THStack *myStack = getStack(hlist,4);
	myStack->Draw();
	myStack->GetXaxis()->SetMoreLogLabels();
	axisTitles(myStack,"pT#gamma GeV","Relative Count");
	axisTitleOffset(myStack,.8);*/
	e1->Draw();
	//e1->GetXaxis()->SetMoreLogLabels();
	e1->SetTitle(";pT-Jet GeV;relative count");
	e2->Draw("same");
	e3->Draw("same");
	e4->Draw("same");
	tl->Draw();

}
void makeSketch(queue<XjPhi> mainQ1){
	TCanvas *tc =new TCanvas();
	const int nBins = 14;
	double bins[nBins+1] = {10,11,12,13,14,16,18,20,22,26,30,33,40,47,54};
	TH1F *p_dircQ = new TH1F("plot1","",nBins,bins); 
	TH1F *p_dircG = new TH1F("plot2","",nBins,bins);  
	TH1F *p_fragQ = new TH1F("plot3","",nBins,bins);  
	TH1F *p_fragG = new TH1F("plot4","",nBins,bins);
	TH1F *totals = new TH1F("total","",nBins,bins);
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_fragQ,"frag Quark","p");
	tl->AddEntry(p_fragG,"frag Gluon","p");
	tl->AddEntry(p_dircQ,"direct Quark","p");
	tl->AddEntry(p_dircG,"direct Gluon","p");
	TH1F* hlist[4];
	hlist[0]=p_dircQ;
	hlist[1]=p_dircG;
	hlist[2]=p_fragQ;
	hlist[3]=p_fragG;
	makeDifferent(hlist,4);
	fillPlotJet(hlist,mainQ1,10,70);
	cout<<"plot1 filled"<<'\n';
	filltotalJet(totals,mainQ1,10,70);
	cout<<"total filled"<<'\n';
	//printHist(totals);
	TEfficiency *e1 = new TEfficiency(*p_dircQ,*totals);
	TEfficiency *e2 = new TEfficiency(*p_dircG,*totals);
	TEfficiency *e3 = new TEfficiency(*p_fragQ,*totals);
	TEfficiency *e4 = new TEfficiency(*p_fragG,*totals);
	makeDifferent(e1,0);
	makeDifferent(e2,1);
	makeDifferent(e3,2);
	makeDifferent(e4,3);

	/*normalizeBins(hlist,4);
	THStack *myStack = getStack(hlist,4);
	myStack->Draw();
	myStack->GetXaxis()->SetMoreLogLabels();
	axisTitles(myStack,"pT#gamma GeV","Relative Count");
	axisTitleOffset(myStack,.8);*/
	e1->Draw();
	//e1->GetXaxis()->SetMoreLogLabels();
	e1->SetTitle(";pT-Jet GeV;relative count");
	e2->Draw("same");
	e3->Draw("same");
	e4->Draw("same");
	tl->Draw();
}

void jetpTSpectra(queue<XjPhi> mainQ1){
	TCanvas *tc =new TCanvas();
	const int nBins = 14;
	double bins[nBins+1] = {10,11,12,13,14,16,18,20,22,26,30,33,40,47,54};
	TH1F *p_dircQ = new TH1F("plot1","",18,10,50); 
	TH1F *p_dircG = new TH1F("plot2","",18,10,50);  
	TH1F *p_fragQ = new TH1F("plot3","",18,10,50);  
	TH1F *p_fragG = new TH1F("plot4","",18,10,50);
	TH1F *totals = new TH1F("total","",18,10,50);
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_fragQ,"frag Quark","p");
	tl->AddEntry(p_fragG,"frag Gluon","p");
	tl->AddEntry(p_dircQ,"direct Quark","p");
	tl->AddEntry(p_dircG,"direct Gluon","p");
	gPad->SetLogy();
	TH1F* hlist[4];
	hlist[0]=p_dircQ;
	hlist[1]=p_dircG;
	hlist[2]=p_fragQ;
	hlist[3]=p_fragG;
	makeDifferent(hlist,4);
	fillPlotJet2(hlist,mainQ1,10,70,25);
	cout<<"plot1 filled"<<'\n';

	/*normalizeBins(hlist,4);
	THStack *myStack = getStack(hlist,4);
	myStack->Draw();
	myStack->GetXaxis()->SetMoreLogLabels();
	axisTitles(myStack,"pT#gamma GeV","Relative Count");
	axisTitleOffset(myStack,.8);*/
	hlist[0]->Draw();
	//e1->GetXaxis()->SetMoreLogLabels();
	hlist[0]->SetTitle(";pT-Jet GeV;relative count");
	hlist[1]->Draw("same");
	hlist[2]->Draw("same");
	hlist[3]->Draw("same");
	tl->Draw();
}

/*class Fctor  //use Fctor to pass arguments to a thread 
{
public:
	void operator()(string name, string extension, int jobs){
		handleFile(name,extension,jobs);
	}
};*/

void FlavorTagger(){
	string fileLocation = "";
	string filename = "XjPhi3_pT5_";
	string extension = ".root";
	string temp = fileLocation+filename;
	queue<XjPhi> mainQ1 = handleFile(temp,extension,500,15);

	filename = "XjPhi3_pT15_";
	temp = fileLocation+filename;
	//queue<XjPhi> mainQ2 = handleFile(temp,extension,100,25);

	filename = "XjPhi3_pT25_";
	temp = fileLocation+filename;
	//queue<XjPhi> mainQ3 = handleFile(temp,extension,50,35);

	filename = "XjPhi3_pT35_";
	temp = fileLocation+filename;
	//queue<XjPhi> mainQ4 = handleFile(temp,extension,50,45);
	cout<<"Files are loaded"<<'\n';
	//makePlotJet(mainQ1,mainQ2,mainQ3,mainQ4);
	//makeSketch(mainQ2);
	//jetpTSpectra(mainQ2);
	//plotXj(mainQ1);
	plotDeltaPhi(mainQ1);
}