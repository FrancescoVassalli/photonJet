using namespace std;

//#include "Utils.C"
#include "CollisionClasses.C"
//#include "XjPhi.C"
#include <sstream>
#include "Utils.C"

const double PI = TMath::Pi();
bool namein(string test, std::vector<string> v);

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

/*void plotXjPhi(TChain *dirc,TChain *frag){
	vector<string> interestBranches={"xj","direct","isoEt","photonPosition",};
	TObjArray* branches = (TObjArray*)(tree->GetListOfBranches());
	for (TObject* loopBranch : *branches)
	{
		if (!namein(string(loopBranch->GetName()),interestBranches))
		{
			tree->SetBranchStatus(loopBranch->GetName(),0); //disable other branches 
		}
	}
	float xj,isoET;
	bool direct;
	tree->SetBranchAddress("xj",&xj);
	tree->SetBranchAddress("direct",&direct);
	tree->SetBranchAddress("isoEt",&isoET);
	TCanvas *tc = new TCanvas();
	//double bins[] = {.32,.36,.39,.45,.5,.56,.63,.7,.79,.88,1,1.13,1.5,2};
	TH2F *p_dirc = new TH2F("plot1","",20,7*TMath::Pi()/8,TMath::Pi(),16,0,2.6); //can make mondular hist names 
	TH2F *p_frag = new TH2F("plot2","",20,7*TMath::Pi()/8,TMath::Pi(),16,0,2.6); //can make mondular hist names 
	//gStyle->SetOptStat(1);
	/*make the plot nice*/
	/*tc->SetRightMargin(.15);
	//plot->Scale(1,"width");
	axisTitles(p_dirc,"#Delta#phi","Xj");
	axisTitleSize(p_dirc,.07);
	axisTitleOffset(p_dirc,1);
	axisTitles(p_frag,"#Delta#phi","Xj");
	axisTitleSize(p_frag,.07);
	axisTitleOffset(p_frag,1);
	p_dirc->Scale(1/p_dirc->Integral());
	p_frag->Scale(1/p_frag->Integral());
	cout<<dirc->GetEntries()+frag->GetEntries()<<endl;
	//gPad->SetLogz(); //this looks bad so getting more stats
	tc->cd(1);
	p_frag->Draw("colz");
	//tc2->cd(1);
	//p_dirc->Draw("lego2");
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

/*void plot1D(TChain *tree){
	//could also turn them all off and turn the ones I want on
	vector<string> interestBranches={"xj","direct","isoEt"};
	TObjArray* branches = (TObjArray*)(tree->GetListOfBranches());
	for (TObject* loopBranch : *branches)
	{
		if (!namein(string(loopBranch->GetName()),interestBranches))
		{
			tree->SetBranchStatus(loopBranch->GetName(),0); //disable other branches 
		}
	}
	float xj,isoET;
	bool direct;
	tree->SetBranchAddress("xj",&xj);
	tree->SetBranchAddress("direct",&direct);
	tree->SetBranchAddress("isoEt",&isoET);
	TCanvas *tc = new TCanvas();
	float bins[] = {.32,.36,.39,.45,.5,.56,.63,.7,.79,.88,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9};
	const int binL=19;
	TH1F *plot = new TH1F("plota","",binL,bins); //can make mondular hist names 
	TH1F *other = new TH1F("plotb","",binL,bins);
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		tree->GetEntry(i);
		if (isoET<3) //ET cut
		{
			if (direct) //direct vs frag
			{
				plot->Fill(xj);
			}
			else{
				other->Fill(xj);
			}
		}
	}
	cout<<other->Integral()<<endl;
	cout<<plot->Integral()<<endl;
	plot->Scale(1/plot->Integral(),"width");
	other->Scale(1/other->Integral(),"width");
	TLegend *tl =new TLegend(.1,.6,.4,.9);
	tl->AddEntry(plot,"direct","p");
	tl->AddEntry(other,"frag","p");
	axisTitles(plot,"Xj","");
	axisTitleSize(plot,.06);
	axisTitleOffset(plot,.7);
	smallBorders();
	makeDifferent(other,1);
	doubleZero(plot,1.6,1.9);
	plot->Draw("p");
	other->Draw("same p");
	tl->Draw();
}*/

Jet matchJet(Photon p, queue<Jet> jQ){

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
	queue<Jet> r;
	int count=0;
	while(!q.empty()){
		if (q.front().getr()==r)
		{
			r.push(q.front());
			count++;
		}
	}
	cout<<count<<'\n';
	return r;
}

queue<Jet> getRJets(float r,float photonPhi,float* jetphi,float* jety, float* jetpT, float* jetR, int SIZE){
	return getRJets(r,makeJets(photonPhi,jetphi,jety,jetpT,jetR,mult));
}

void plot1D(TChain *tree,queue<Photon> photonQ){
	//could also turn them all off and turn the ones I want on
	vector<string> interestBranches={"jetphi","jetpT","jety","jetR"};
	TObjArray* branches = (TObjArray*)(tree->GetListOfBranches());
	for (TObject* loopBranch : *branches)
	{
		if (!namein(string(loopBranch->GetName()),interestBranches))
		{
			tree->SetBranchStatus(loopBranch->GetName(),0); //disable other branches 
		}
	}
	float jetphi[200];
	float jetpT[200];
	float jety[200];
	float jetR[200];
	std::vector<int> *mult=new vector<int>();
	tree->SetBranchAddress("jetphi",&jetphi);
	tree->SetBranchAddress("jetpT",&jetpT);
	tree->SetBranchAddress("jety",&jety);
	tree->SetBranchAddress("jetR",&jetR);
	tree->SetBranchAddress("mult",&mult);
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		tree->GetEntry(i);
		getRJets(.4,photonQ.front().getphi().value,jetphi,jety,jetpT,jetR,mult->size());
		photonQ.pop();
	}
	/*TCanvas *tc = new TCanvas();
	float bins[] = {.32,.36,.39,.45,.5,.56,.63,.7,.79,.88,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9};
	const int binL=19;
	TH1F *plot = new TH1F("plota","",binL,bins); //can make mondular hist names 
	TH1F *other = new TH1F("plotb","",binL,bins);
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		tree->GetEntry(i);
		if (isoET<3) //ET cut
		{
			if (direct) //direct vs frag
			{
				plot->Fill(xj);
			}
			else{
				other->Fill(xj);
			}
		}
	}
	cout<<other->Integral()<<endl;
	cout<<plot->Integral()<<endl;
	plot->Scale(1/plot->Integral(),"width");
	other->Scale(1/other->Integral(),"width");
	TLegend *tl =new TLegend(.1,.6,.4,.9);
	tl->AddEntry(plot,"direct","p");
	tl->AddEntry(other,"frag","p");
	axisTitles(plot,"Xj","");
	axisTitleSize(plot,.06);
	axisTitleOffset(plot,.7);
	smallBorders();
	makeDifferent(other,1);
	doubleZero(plot,1.6,1.9);
	plot->Draw("p");
	other->Draw("same p");
	tl->Draw();*/
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

void xjgpT(TChain* dirc, TChain *frag){
	TCanvas *tc = new TCanvas();
	//tc->Divide(2,1);
	//double bins[] = {.32,.36,.39,.45,.5,.56,.63,.7,.79,.88,1,1.13,1.5,2};
	TH2F *p_dirc = new TH2F("plot1","",20,20,26,16,0,2.6); //can make mondular hist names 
	TH2F *p_frag = new TH2F("plot2","",20,20,26,16,0,2.6); //can make mondular hist names 
	dirc->Draw("xj:gpT>>plot1");
	frag->Draw("xj:gpT>>plot2");
	axisTitles(p_dirc,"#gammapT","Xj");
	axisTitleSize(p_dirc,.07);
	axisTitleOffset(p_dirc,.8);
	axisTitles(p_frag,"#gammapT","Xj");
	axisTitleSize(p_frag,.07);
	axisTitleOffset(p_frag,1);
	p_dirc->Scale(1/p_dirc->Integral());
	p_frag->Scale(1/p_frag->Integral());

	//gPad->SetLogz(); //this looks bad so getting more stats
	tc->cd(1);
	p_frag->Draw("colz");
	//tc->cd(2);
	//p_dirc->Draw("lego2");
}

queue<Photon> makePhotons(TChain *chain){
	vector<string> interestBranches={"photonPosition","eT","phi","eta","Status","direct"};
	TObjArray* branches = (TObjArray*)(tree->GetListOfBranches());
	for (TObject* loopBranch : *branches)
	{
		if (!namein(string(loopBranch->GetName()),interestBranches))
		{
			tree->SetBranchStatus(loopBranch->GetName(),0); //disable other branches 
		}
	}
	float eT[300];
	float phi[300];
	float eta[300];
	int photonPosition;
	bool direct;
	std::vector<int> *status=new vector<int>();
	chain->SetBranchAddress("eT",&eT);
	chain->SetBranchAddress("phi",&phi);
	chain->SetBranchAddress("eta",&eta);
	chain->SetBranchAddress("photonPosition",&photonPosition);
	chain->SetBranchAddress("Status",&status);
	chain->SetBranchAddress("direct",&direct);
	queue<Photon> r;
	for (int i = 0; i < chain->GetEntries(); ++i)
	{
		chain->GetEntry(i);
		r.push(Photon(status->size(),photonPosition,eT,phi,eta,direct));
	}
	return r;
}

void XjGammaPhiPlotter(){
	string filename = "XjPhi_pT15_";
	string extension = ".root";
	int filecount=50;
	TChain *all = new TChain("interest");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = filename+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	//plotXjPhi(dirc,frag);
	queue<Photon> photonQ = makePhotons(all);
	plot1D(all,photonQ);
	//xjgpT(dirc,frag);
	//plot4Bars(dirc,frag);
	//plotFlavpT(dirc,frag);
	//plotMonoJets(data);
}	