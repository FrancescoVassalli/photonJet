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

queue<Jet> makeJets(float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, int SIZE){
	queue<Jet> r;
	
	for (int i = 0; i < SIZE; ++i)
	{
		if (TMath::Abs(jetphi[i]-photonPhi)>7.0*TMath::Pi()/8.0)
		{
			r.push(Jet(jetpT[i],jetphi[i],jety[i],jetR[i]));
			
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

queue<Jet> getRJets(float r,float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, int SIZE){
	return getRJets(r,makeJets(photonPhi,jetphi,jety,jetpT,jetR,SIZE));
}

/*Jet selectJet(queue<Jet> q){

}*/

void plot1D(queue<XjPhi> xjPhiQ){
	
	TCanvas *tc = new TCanvas();
	TH1F *plot = new TH1F("plota","",30,0,1.8); //can make mondular hist names 
	TH1F *other = new TH1F("plotb","",30,0,1.8);
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
	plot->Scale(1/plot->Integral(),"width");
	other->Scale(1/other->Integral(),"width");
	TLegend *tl =new TLegend(.25,.7,.4,.85);
	tl->AddEntry(plot,"frag","p");
	tl->AddEntry(other,"direct","p");
	axisTitles(plot,"Xj","");
	axisTitleSize(plot,.06);
	axisTitleOffset(plot,.7);
	smallBorders();
	makeDifferent(other,1);
	plot->GetYaxis()->SetRangeUser(0,1.8);
	plot->Draw("p");
	other->Draw("same p");
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

queue<XjPhi> getXjPhi(TChain *tree){
	queue<Photon> photonQ=makePhotons(tree);
	float jetphi[200];
	float jetpT[200];
	float jety[200];
	float jetR[200];
	int jetend=0;
	tree->SetBranchAddress("jetphi",&jetphi);
	tree->SetBranchAddress("jetpT",&jetpT);
	tree->SetBranchAddress("jety",&jety);
	tree->SetBranchAddress("jetR",&jetR);
	tree->SetBranchAddress("jetend",&jetend);
	queue<float> photonpTQ;
	queue<XjPhi> xjPhiQ;
	analysisStream<<"Total:"<<tree->GetEntries()<<'\n';
	int isocutCount=tree->GetEntries();
	int jetCutCount=tree->GetEntries();
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		if (photonQ.front().getIsoEt()>3){
			photonQ.pop();
			isocutCount--;
			jetCutCount--;
			continue;
		}
		tree->GetEntry(i);
		queue<Jet> jTempQ;
		jTempQ= getRJets(.4,photonQ.front().getphi().value,jetphi,jety,jetpT,jetR,jetend);
		if (!jTempQ.empty())
		{
			xjPhiQ.push(XjPhi(photonQ.front(),jTempQ.front()));
		}
		else jetCutCount--;
		photonQ.pop();
	}
	analysisStream<<"IsoCut:"<<isocutCount<<'\n';
	analysisStream<<"JetCut:"<<jetCutCount<<'\n';
	tree->ResetBranchAddresses();
	return xjPhiQ;
}

void xjgpT(queue<XjPhi> xjPhiQ){
	
	TCanvas *tc = new TCanvas();
	TH2F *p_dirc = new TH2F("plot1","",20,10,20,16,0,2.6); //can make mondular hist names 
	TH2F *p_frag = new TH2F("plot2","",20,10,20,16,0,2.6); //can make mondular hist names 
	
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
	p_frag->Draw("lego2");
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
		Photon pTemp =Photon(end,photonPosition,eT,phi,eta,direct,.3);\
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

void XjGammaPhiPlotter(){
	string fileLocation = "/home/user/Droptemp/XjPhiOverFlow/";
	string filename = "XjPhi_pT35_";
	string extension = ".root";
	int filecount=50;
	TChain *all = new TChain("interest");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = fileLocation+filename+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	//plotXjPhi(dirc,frag);
	queue<XjPhi> mainQ = getXjPhi(all);
	plot1D(mainQ);
	//xjgpT(mainQ);
	analysisStream<<"end"<<endl;
	cout<<analysisStream.str();
}	