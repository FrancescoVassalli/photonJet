using namespace std;

//#include "Utils.C"
#include "CollisionClasses.C"
#include "XjPhi.C"
#include <sstream>

const double PI = TMath::Pi();

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

}

void plotFile(string filename){
	TCanvas *tc = new TCanvas();
	TChain *gamma_tree = new TChain("tree100");
	gamma_tree->Add(filename.c_str());
	TH2F *plot = new TH2F("plot1","",20,0,TMath::Pi(),20,0,1); //can make mondular hist names 
	gStyle->SetOptStat(1);
	/*make the plot nice*/
	gamma_tree->Draw("xj:phi>>plot1");
	plot->Draw("colz");
}

void plot1D(string filename){
	TCanvas *tc = new TCanvas();
	TChain *gamma_tree = new TChain("tree100");
	gamma_tree->Add(filename.c_str());
	float bins[] = {.32,.36,.39,.45,.5,.56,.63,.7,.79,.88,1};
	TH1F *plot = new TH1F("plota","",10,bins); //can make mondular hist names 
	//get the normalization right
	plot->Scale(1/plot->Integral(),"width");
	gStyle->SetOptStat(1);
	/*make the plot nice*/
	gamma_tree->Draw("xj>>plota");
	plot->Draw("p");
}


void XjGammaPhiPlotter(){
	string filename = "XjgP1.root";
	plotFile(filename);
	plot1D(filename);
}