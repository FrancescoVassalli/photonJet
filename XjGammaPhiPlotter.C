using namespace std;

#include "Utils.C"
//#include "CollisionClasses.C"
//#include "XjPhi.C"
#include <sstream>
#include <iostream>

#ifdef __MAKECINT__
#pragma link C++ class vector<int>+;
#endif

const double PI = TMath::Pi();
bool namein(string test, std::vector<string> v);
queue<Photon> makePhotons(TChain *chain);
stringstream analysisStream;
/* //for when you dont use TTrees which you always should 
void plotText(string filename){
	ifstream inFile (filename.c_str()); //txt file containing the names of files to process
	queue<string> files;
	string intemp;
	while(getline(inFile,intemp)){
		files.push(intemp);
	}
	inFile.close();
	queue<XjPhi> xjp;
	Scalar temp;
	string str;
	stringstream ss;
	while(!files.empty()){
		ss<<files.front();
		getline(ss,str,',');
		temp = Scalar(stof(str));
		getline(ss,str);
		xjp.push(XjPhi(temp,Scalar(stof(str))));
		ss.clear();
		files.pop();
	}
	//use the XjPhi queue to make a plot 
	float** doubleArray;
	doubleArray=qXjPhiTo2DArray(xjp);
	TCanvas *tc = new TCanvas();


	TH2F *plot = new TH2F("plot","",20,-3.14,TMath::Pi(),20,0,1);

}*/

bool namein(string test, std::vector<string> v){
	bool in =false;
	for (std::vector<string>::iterator i = v.begin(); i != v.end(); ++i)
	{
		in= (*i==test);
		if (in)
		{
			return in;
		}
	}
	return in;
}

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

queue<Jet> getRJets(float r,queue<Jet> q){
	queue<Jet> rQ;
	while(!q.empty()){
		if (q.front().getr()==r)
		{
			rQ.push(q.front());
		}
		q.pop();
	}	
	return rQ;
}

queue<Jet> getRJets(float r,float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, float* jetpz, int SIZE){
	return getRJets(r,makeJets(photonPhi,jetphi,jety,jetpT,jetR,jetpz,SIZE));
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

/*Jet selectJet(queue<Jet> q){

}*/

void plot1D(queue<XjPhi> xjPhiQ){
	
	TCanvas *tc = new TCanvas();
	TH1F *plot = new TH1F("plota","",20,0,2); //can make mondular hist names 
	TH1F *other = new TH1F("plotb","",20,0,2);
	TH2F *p_dirc = new TH2F("plot1","",20,7*TMath::Pi()/8,TMath::Pi(),16,0,2.6); //can make mondular hist names 
	TH2F *p_frag = new TH2F("plot2","",20,7*TMath::Pi()/8,TMath::Pi(),16,0,2.6); //can make mondular hist names 
	
	while(!xjPhiQ.empty())
	{
		if (xjPhiQ.front().getPhoton().isDirect()) //direct vs frag
		{
			other->Fill(xjPhiQ.front().getXj().value);
			p_dirc->Fill(xjPhiQ.front().getphi().value,xjPhiQ.front().getXj().value);
		}
		else{
			plot->Fill(xjPhiQ.front().getXj().value);
			p_frag->Fill(xjPhiQ.front().getphi().value,xjPhiQ.front().getXj().value);
		}
		xjPhiQ.pop();
	}
	//cout<<other->Integral()<<endl;
	analysisStream<<"Frag:"<<plot->Integral()<<'\n';
	plot->Scale(1/plot->Integral());
	other->Scale(1/other->Integral());
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(plot,"frag","p");
	tl->AddEntry(other,"direct","p");
	axisTitles(plot,"Xj","");
	axisTitleSize(plot,.06);
	axisTitleOffset(plot,.7);
	smallBorders();
	makeDifferent(other,1);
	
	other->Draw("p");
	plot->Draw("same p");
	
	tl->Draw();
	tc->SetRightMargin(.15);
	//plot->Scale(1,"width");
	axisTitles(p_dirc,"#Delta#phi","Xj");
	axisTitleSize(p_dirc,.07);
	axisTitleOffset(p_dirc,1);
	axisTitles(p_frag,"#Delta#phi","Xj");
	axisTitleSize(p_frag,.07);
	axisTitleOffset(p_frag,.6);
	p_dirc->Scale(1/p_dirc->Integral());
	p_frag->Scale(1/p_frag->Integral());
	//p_frag->Draw("colz");
}

void plotMonoJets(TChain* gamma_tree){
	TCanvas *tc = new TCanvas();
	TH1F* plot =new TH1F("mono","",20,0,2*TMath::Pi());
	gamma_tree->Draw("monPhi>>mono");
	plot->Draw("p");
	cout<<"Monojets:"<<plot->Integral()<<'\n';
	cout<<"Total:"<<gamma_tree->GetEntries()<<'\n';
	cout<<"Ratio:"<<plot->Integral()/gamma_tree->GetEntries()<<'\n';
}

queue<XjPhi> getXjPhi(TChain* tree){
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
	analysisStream<<"Total:"<<tree->GetEntries()<<'\n';
	int isocutCount=tree->GetEntries();
	int jetCutCount=tree->GetEntries();
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		tree->GetEntry(i);
		end++; // ends are measured inclusive I want them exclusive
		jetend++;
		Photon pTemp=Photon(end,photonPosition,eT,phi,eta,direct,.3); //make a photon
		if (pTemp.getIsoEt()>3){ //isoEt cut
			isocutCount--;
			jetCutCount--;
			continue;
		}
		Jet* maxJet=NULL;
		maxJet= getMaxJet(getRJets(.4,pTemp.getphi().value,jetphi,jety,jetpT,jetR,jetpz,jetend)); //make all the jets then match
		if (maxJet!=NULL) //if there is a matched jet
		{
			maxJet->setParton(Parton(id[end],phi[end],eta[end],eT[end],e[end]),Parton(id[end+1],phi[end+1],eta[end+1],eT[end+1],e[end+1]));
			xjPhiQ.push(XjPhi(pTemp,*maxJet));
			delete maxJet;
			maxJet=NULL;
		}
		else jetCutCount--;
	}
	analysisStream<<"IsoCut:"<<isocutCount<<'\n';
	analysisStream<<"JetCut:"<<jetCutCount<<'\n';
	tree->ResetBranchAddresses();
	return xjPhiQ;
}

void xjgpT(queue<XjPhi> xjPhiQ){
	
	TCanvas *tc = new TCanvas();
	TH2F *p_dirc = new TH2F("plot1","",20,40,50,16,0,2.6); //can make mondular hist names 
	TH2F *p_frag = new TH2F("plot2","",20,40,50,16,0,2.6); //can make mondular hist names 
	
	while(!xjPhiQ.empty())
	{
		if (xjPhiQ.front().getPhoton().isDirect()) 
		{
			p_dirc->Fill(xjPhiQ.front().getPhoton().getpT().value,xjPhiQ.front().getXj().value);
		}
		else{
			p_frag->Fill(xjPhiQ.front().getPhoton().getpT().value,xjPhiQ.front().getXj().value);
		}
		xjPhiQ.pop();
	}
	tc->SetRightMargin(.15);
	//plot->Scale(1,"width");
	axisTitles(p_dirc,"pT#gamma","Xj");
	axisTitleSize(p_dirc,.07);
	axisTitleOffset(p_dirc,1);
	axisTitles(p_frag,"pT#gamma","Xj");
	axisTitleSize(p_frag,.07);
	axisTitleOffset(p_frag,1);
	p_dirc->Scale(1/p_dirc->Integral());
	p_frag->Scale(1/p_frag->Integral());
	p_dirc->Draw("lego2");
}

void plotpTMatched(queue<XjPhi> xjPhiQ){
	TCanvas *tc = new TCanvas();
	TH1F *p_dirc = new TH1F("plot1","",20,10,20); //can make mondular hist names 
	TH1F *p_frag = new TH1F("plot2","",20,10,20); //can make mondular hist names 
	
	while(!xjPhiQ.empty())
	{
		if (xjPhiQ.front().getPhoton().isDirect()) 
		{
			p_dirc->Fill(xjPhiQ.front().getPhoton().getpT().value);
		}
		else{
			p_frag->Fill(xjPhiQ.front().getPhoton().getpT().value);
		}
		xjPhiQ.pop();
	}
	tc->SetRightMargin(.15);
	gPad->SetLogy();
	axisTitles(p_dirc,"pT#gamma","");
	axisTitleSize(p_dirc,.07);
	axisTitleOffset(p_dirc,1);
	axisTitles(p_frag,"pT#gamma","");
	axisTitleSize(p_frag,.07);
	axisTitleOffset(p_frag,1);
	smallBorders();
	makeDifferent(p_dirc,1);
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_frag,"frag","p");
	tl->AddEntry(p_dirc,"direct","p");
	p_dirc->Scale(1/p_dirc->Integral(),"width");
	p_frag->Scale(1/p_frag->Integral(),"width");
	p_frag->Draw("p");
	p_dirc->Draw("p same");
	tl->Draw();
}

queue<Photon> makePhotons(TChain *chain){
	float eT[300];
	float phi[300];
	float eta[300];
	int id[300];
	int photonPosition;
	bool direct;
	int end;
	chain->SetBranchAddress("eT",&eT);
	chain->SetBranchAddress("phi",&phi);
	chain->SetBranchAddress("eta",&eta);
	chain->SetBranchAddress("photonPosition",&photonPosition);
	chain->SetBranchAddress("end",&end);
	chain->SetBranchAddress("direct",&direct);
	chain->SetBranchAddress("ID",&id);
	queue<Photon> r;
	for (int i = 0; i < chain->GetEntries(); ++i)
	{
		chain->GetEntry(i);
		Photon pTemp =Photon(end,photonPosition,eT,phi,eta,direct,.3);
		//cout<<pTemp.getPosition()<<": d:"<<pTemp.isDirect()<<'\n';
		r.push(pTemp);
	}
	chain->ResetBranchAddresses();
	return r;
}

queue<Photon> testPhotonQ(queue<Photon> pQ){
	queue<Photon> r = pQ;
	int i=0;
	while(!pQ.empty()){
		if (pQ.front().getpT()<10)
		{
			cout<<"Bad Event:"<<i<<'\n';
		}
		pQ.pop();
		i++;
	}
	return r;
}

void classifyTypeAndFlavorpTg(queue<XjPhi> xjPhiQ){
	TCanvas *tc = new TCanvas();
	TH1F *p_dircQ = new TH1F("plot1","",20,29,38); //can make mondular hist names 
	TH1F *p_dircG = new TH1F("plot2","",20,29,38); //can make mondular hist names 
	TH1F *p_fragQ = new TH1F("plot3","",20,29,38); //can make mondular hist names 
	TH1F *p_fragG = new TH1F("plot4","",20,29,38); //can make mondular hist names 
	//p_dircG->Sumw2();
	//p_dircQ->Sumw2();
	//p_fragQ->Sumw2();
	//p_fragG->Sumw2();
	int totalSize= xjPhiQ.size();
	while(!xjPhiQ.empty())
	{
		if (xjPhiQ.front().getPhoton().isDirect()) 
		{
			if (xjPhiQ.front().isQuark())
			{
				p_dircQ->Fill(xjPhiQ.front().getPhoton().getpT().value);
			}
			else p_dircG->Fill(xjPhiQ.front().getPhoton().getpT().value);
		}
		else{
			if (xjPhiQ.front().isQuark())
			{
				p_fragQ->Fill(xjPhiQ.front().getPhoton().getpT().value);
			}
			else p_fragG->Fill(xjPhiQ.front().getPhoton().getpT().value);
		}
		xjPhiQ.pop();
	}
	tc->SetRightMargin(.15);
	//gPad->SetLogy();
	gStyle->SetErrorX(0);
	axisTitles(p_dircQ,"pT#gamma","");
	axisTitleSize(p_dircQ,.06);
	axisTitleOffset(p_dircQ,.8);
	axisTitles(p_fragQ,"pT#gamma","");
	axisTitleSize(p_fragQ,.07);
	axisTitleOffset(p_fragQ,1);
	smallBorders();
	queue<TH1F*> diff;
	diff.push(p_dircQ);
	diff.push(p_dircG);
	diff.push(p_fragG);
	makeDifferent(diff);
	//gPad->SetLogy();
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_fragQ,"frag Quark","p");
	tl->AddEntry(p_dircQ,"direct Quark","p");
	tl->AddEntry(p_dircG,"direct Gluon","p");
	tl->AddEntry(p_fragG,"frag Gluon","p");
	TH1F* hlist[4];
	hlist[0]= p_dircQ;
	hlist[1]=p_dircG;
	hlist[2]=p_fragQ;
	hlist[3]=p_fragG;
	normalizeBins(hlist,4);
	THStack *myStack = getStack(hlist,4,"hist","pT#gamma","");
	//myStack->GetXaxis()->SetTitle("pT#gamma");
	myStack->Draw();
	axisTitles(myStack,"pT#gamma GeV","Relative Count");
	axisTitleOffset(myStack,.8);
	tl->Draw();
}


void classifyTypeAndFlavorpTJet(queue<XjPhi> xjPhiQ){
	TCanvas *tc = new TCanvas();
	TH1F *p_dircQ = new TH1F("plot1","",20,9,22); //can make mondular hist names 
	TH1F *p_dircG = new TH1F("plot2","",20,9,22); //can make mondular hist names 
	TH1F *p_fragQ = new TH1F("plot3","",20,9,22); //can make mondular hist names 
	TH1F *p_fragG = new TH1F("plot4","",20,9,22); //can make mondular hist names 
	//p_dircG->Sumw2();
	//p_dircQ->Sumw2();
	//p_fragQ->Sumw2();
	//p_fragG->Sumw2();
	int totalSize= xjPhiQ.size();
	while(!xjPhiQ.empty())
	{
		if (xjPhiQ.front().getPhoton().isDirect()) 
		{
			if (xjPhiQ.front().isQuark())
			{
				p_dircQ->Fill(xjPhiQ.front().getJet().getpT().value);
			}
			else p_dircG->Fill(xjPhiQ.front().getJet().getpT().value);
		}
		else{
			if (xjPhiQ.front().isQuark())
			{
				p_fragQ->Fill(xjPhiQ.front().getJet().getpT().value);
			}
			else p_fragG->Fill(xjPhiQ.front().getJet().getpT().value);
		}
		xjPhiQ.pop();
	}
	tc->SetRightMargin(.15);
	//gPad->SetLogy();
	gStyle->SetErrorX(0);
	axisTitles(p_dircQ,"pT-Jet GeV","");
	axisTitleSize(p_dircQ,.06);
	axisTitleOffset(p_dircQ,.8);
	axisTitles(p_fragQ,"pT-Jet GeV","");
	axisTitleSize(p_fragQ,.07);
	axisTitleOffset(p_fragQ,1);
	smallBorders();
	queue<TH1F*> diff;
	diff.push(p_dircQ);
	diff.push(p_dircG);
	diff.push(p_fragG);
	makeDifferent(diff);
	//gPad->SetLogy();
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(p_fragQ,"frag Quark","p");
	tl->AddEntry(p_dircQ,"direct Quark","p");
	tl->AddEntry(p_dircG,"direct Gluon","p");
	tl->AddEntry(p_fragG,"frag Gluon","p");
	TH1F* hlist[4];
	hlist[0]= p_dircQ;
	hlist[1]=p_dircG;
	hlist[2]=p_fragQ;
	hlist[3]=p_fragG;
	normalizeBins(hlist,4);
	THStack *myStack = getStack(hlist,4,"hist","pT#gamma","");
	//myStack->GetXaxis()->SetTitle("pT#gamma");
	myStack->Draw();
	axisTitles(myStack,"pT-Jet GeV","Relative Count");
	axisTitleOffset(myStack,.8);
	tl->Draw();
}

void XjGammaPhiPlotter(){
	string fileLocation = "";
	string filename = "XjPhi3_pT25_";
	string extension = ".root";
	int filecount=50;
	TChain *all = new TChain("interest");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = fileLocation+filename+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	//TChain* all=new TChain("interest");
	//all->Add("Xj315.root");
	//plotXjPhi(dirc,frag);
	queue<XjPhi> mainQ = getXjPhi(all);
	//plot1D(mainQ);s
	classifyTypeAndFlavorpTJet(mainQ);
	//plotpTMatched(mainQ);
	//xjgpT(mainQ);
	analysisStream<<"end"<<endl;
	cout<<analysisStream.str();
}	