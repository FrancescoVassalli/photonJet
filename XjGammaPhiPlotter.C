using namespace std;

//#include "Utils.C"
#include "CollisionClasses.C"
#include "XjPhi.C"
#include <sstream>

const double PI = TMath::Pi();

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

void plotXjPhi(TChain *dirc,TChain *frag){
	TCanvas *tc = new TCanvas();
	//double bins[] = {.32,.36,.39,.45,.5,.56,.63,.7,.79,.88,1,1.13,1.5,2};
	TH2F *p_dirc = new TH2F("plot1","",20,7*TMath::Pi()/8,TMath::Pi(),16,0,2.6); //can make mondular hist names 
	TH2F *p_frag = new TH2F("plot2","",20,7*TMath::Pi()/8,TMath::Pi(),16,0,2.6); //can make mondular hist names 
	//gStyle->SetOptStat(1);
	/*make the plot nice*/
	tc->SetRightMargin(.15);
	//plot->Scale(1,"width");
	dirc->Draw("xj:phi>>plot1");
	frag->Draw("xj:phi>>plot2");
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
}

void plot1D(TChain* dirc, TChain *frag){
	TCanvas *tc = new TCanvas();
	float bins[] = {.32,.36,.39,.45,.5,.56,.63,.7,.79,.88,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9};
	const int binL=19;
	TH1F *plot = new TH1F("plota","",binL,bins); //can make mondular hist names 
	TH1F *other = new TH1F("plotb","",binL,bins);
	/*make the plot nice*/
	dirc->Draw("xj>>plota");
	frag->Draw("xj>>plotb");
	//plot->Scale(1/plot->Integral(),"width");
	//other->Scale(1/other->Integral(),"width");
	TLegend *tl =new TLegend(.1,.6,.4,.9);
	tl->AddEntry(plot,"direct","p");
	tl->AddEntry(other,"frag","p");
	axisTitles(plot,"Xj","");
	axisTitleSize(plot,.06);
	axisTitleOffset(plot,.7);
	smallBorders();
	makeDifferent(other,1);
	plot->Draw("p");
	other->Draw("same p");
	tl->Draw();
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


void XjGammaPhiPlotter(){
	string filename = "Xj2Phi";
	string extension = ".root";
	int filecount=70;
	TChain *dirc = new TChain("tree100"); //use tree200 for frag and tree100 for direct
	TChain *frag = new TChain("tree200");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = filename+to_string(i)+extension;
		dirc->Add(temp.c_str());
		frag->Add(temp.c_str());
	}
	//plotXjPhi(dirc,frag);
	plot1D(dirc,frag);
	//xjgpT(dirc,frag);
	//plot4Bars(dirc,frag);
	//plotFlavpT(dirc,frag);
	//plotMonoJets(data);
}	