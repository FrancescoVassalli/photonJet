using namespace std;
using namespace Pythia8;
void printPythiaEvent(TChain* frag){
	bool run=true;
	float xj;
	frag->SetBranchAddress("xj",&xj);
	long myeventI;
	for (long i = 0; i < frag->GetEntries()&&run; ++i)
	{
		frag->GetEntry(i);
		if (xj>1.5&&xj<1.7)
		{
			run=false;
			myeventI=i;
		}
	}
	Event myEvent;
	frag->SetBranchAddress("FullEvent",&myEvent);
	frag->GetEntry(myEvent); //such a wrid way to program
	myEvent.list(0,1,3);
}

void PrintPythiaFromTree(){
	string filename = "XjPhi";
	string extension = ".root";
	int filecount=139;
	TChain *dirc = new TChain("tree100"); //use tree200 for frag and tree100 for direct
	TChain *frag = new TChain("tree200");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = filename+to_string(i)+extension;
		dirc->Add(temp.c_str());
		frag->Add(temp.c_str());
	}
}