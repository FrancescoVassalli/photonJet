

void G4Jet1(){
	string fileLocation = "/home/user/Droptemp/G4OutInitial/";
	string filename = "XjPhi1_pT5_output_";
	string extension = ".root";
	string temp = fileLocation+filename;
	queue<TH1*> reco=handleG4File(temp,extension,1000);
	
}