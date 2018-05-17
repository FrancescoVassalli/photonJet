using namespace std;

#include "Utils.C"
#include <sstream>

void XjGammaPhiPlotter(){
	ifstream inFile ("PbGl_dataC.txt"); //txt file containing the names of files to process
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
	

}