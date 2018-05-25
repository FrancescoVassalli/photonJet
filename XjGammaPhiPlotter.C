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

void plotFile(TChain *gamma_tree){
	TCanvas *tc = new TCanvas();
	//double bins[] = {.32,.36,.39,.45,.5,.56,.63,.7,.79,.88,1,1.13,1.5,2};
	TH2F *plot = new TH2F("plot1","",20,7*TMath::Pi()/8,TMath::Pi(),16,0,2.6); //can make mondular hist names 
	gStyle->SetOptStat(1);
	/*make the plot nice*/
	tc->SetRightMargin(.15);
	//plot->Scale(1,"width");
	gamma_tree->Draw("xjf:phi>>plot1");
	axisTitles(plot,"#Delta#phi","Xj");
	axisTitleSize(plot,.07);
	axisTitleOffset(plot,1);
	plot->Draw("colz");
}

void plot1D(TChain* gamma_tree){
	TCanvas *tc = new TCanvas();
	float bins[] = {.32,.36,.39,.45,.5,.56,.63,.7,.79,.88,1,1.13,1.5};
	TH1F *plot = new TH1F("plota","",12,bins); //can make mondular hist names 
	//get the normalization right
	//plot
	/*make the plot nice*/
	gamma_tree->Draw("xjd>>plota");
	plot->Scale(1/plot->Integral(),"width");
	axisTitles(plot,"Xj","");
	axisTitleSize(plot,.07);
	axisTitleOffset(plot,1);
	plot->Draw("p");
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


void XjGammaPhiPlotter(){
	string filename = "XjPhi";
	string extension = ".root";
	int filecount=70;
	TChain *data = new TChain("tree100");
	string temp=filename+extension;
	data->Add(temp.c_str());
	for (int i = 1; i < filecount; ++i)
	{
		temp = filename+to_string(i)+extension;
		data->Add(temp.c_str());
	}
	plotFile(data);
	//plot1D(data);
	//plotMonoJets(data);
}	