 class myEvent
 {
 public:
 	myEvent(){}
 	~myEvent(){}
 	myEvent(int particle,bool direct, int status, int id, float pt, float eta, float phi, float mother1, float mother2,float daughter1, float daughter2){
		this->particle=particle;
 		this->direct=direct;
 		this->status=status;
 		this->id=id;
 		this->pt=pt;
 		this->eta=eta;
 		this->phi=phi;
 		this->mother1=mother1;
 		this->mother2=mother2;	 
 		this->daughter1=daughter1;
 		this->daughter2=daughter2;
 	}
 private:
 	int particle;
 	bool direct;
 	int status;
 	int id;
 	float pt;
 	float eta;
 	float phi;
 	float mother1;
 	float mother2; 	 
 	float daughter1; 
 	float daughter2;
 };