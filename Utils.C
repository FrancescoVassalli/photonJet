template<class T>
	T quadrature(T d1, T d2){
		return TMath::Sqrt((double)d1*d1+d2*d2);
	}
	template<class T>
	T quadrature(T* a, int SIZE){
		T* b = clone(a);
		arrayMultiply(b,b,SIZE);
		T r = sum(b,SIZE);
		return TMath::Sqrt(r);

	}
class Scalar
{
public:
	Scalar(){
		value=uncertainty=0;
	}
	Scalar(float value){
		this->value=value;
		uncertainty=0;
	}
	Scalar(float value, float uncertainty){
		this->value=value;
		this->uncertainty=uncertainty;
	}
	Scalar(const Scalar &s){
		this->value=s.value;
		uncertainty=s.uncertainty;
	}
	~Scalar(){}; 
	//operators might need to return class types but I'm not sure
	Scalar operator+(const Scalar &s){
		//cout<<"Addition "<<value<<" + "<<s.value<<"\n";;
		Scalar next = Scalar();
		next.value = this->value + s.value;
		next.uncertainty = quadrature(this->uncertainty, s.uncertainty);
		return next;
	}
	Scalar operator/(const Scalar &s){
		Scalar next = Scalar();
		next.uncertainty = (value/s.value)*quadrature(this->uncertainty/value, s.uncertainty/s.value);
		next.value = value/s.value;
		if (value==0||s.value==0)
		{
			next.uncertainty=0;
		}
		return next;
	}
	void operator+=(Scalar s){
		value+=s.value;
		uncertainty = quadrature(this->uncertainty, s.uncertainty);
	}
	void operator*=(Scalar s){
		*this = *this * s;
	}
	void operator/=(Scalar s){
		*this = *this / s;
	}
	void operator-=(Scalar s){
		*this = *this - s;
	}
	Scalar operator+(float s){
		Scalar next;
		next.value = this->value + s;
		return next;
	}
	Scalar operator-(Scalar s){
		Scalar next;
		next.value = this->value - s.value;
		next.uncertainty = quadrature(this->uncertainty, s.uncertainty);
		return next;
	}
	Scalar operator-(float s){
		Scalar next;
		next.value = this->value - s;
		return next;
	}
	void operator=(Scalar s){
		value=s.value;
		uncertainty=s.uncertainty;
	}
	bool operator==(Scalar s){
		return value==s.value;
	}
	bool operator>(Scalar s){
		return value>s.value;
	}
	bool operator<(Scalar s){
		return value<s.value;
	}
	bool operator>=(Scalar s){
		return value>=s.value;
	}
	bool operator<=(Scalar s){
		return value<=s.value;
	}
	bool operator!=(Scalar s){
		return value!=s.value;
	}
	Scalar operator*(Scalar s){
		Scalar next;
		next.uncertainty = (value*s.value)*quadrature(this->uncertainty/value, s.uncertainty/s.value);
		if (value==0||s.value==0)
		{
			next.uncertainty=0;
		}
		next.value = value*s.value;
		return next;
	}
	Scalar operator*(float s){
		Scalar next;
		next.value = value*s;
		next.uncertainty = (value*s)*uncertainty/value;
		if (value==0||s==0)
		{
			next.uncertainty=0;
		}
		return next;
	}
	Scalar operator/(float s){ //progate uncertainty
		Scalar next;
		next.uncertainty=(value/s)*uncertainty/value;
		if (value==0)
		{
			next.uncertainty=0;
		}
		next.value = value/s;
		return next;
	}
	Scalar pow(double n){
		Scalar r;
		if (n==0)
		{
			r.value=1;
			r.uncertainty=0;
			return r;
		}
		r.value = TMath::Power((double)value,n);
		r.uncertainty=(r.value*n*uncertainty/value);
		return r;
	}
	Scalar log(float base){
		Scalar r;
		r.value = TMath::Log(value)/TMath::Log(base);
		r.uncertainty = uncertainty/(value*TMath::Log(base));
		return r;
	}

	friend ostream& operator<<(ostream& os, Scalar const & tc) {
        return os <<"Scalar: " << tc.value <<char(241)<<tc.uncertainty<<'\n';
    }

float value;
float uncertainty;
	
};

struct Point
{
	Scalar x;
	Scalar y;
};

template<class T>
T bigger(T x, T y){
	if (x>y)
	{
		return x;
	}
	else{
		return y;
	}
}
template<class T>
T smaller(T x, T y){
	if (x<y)
	{
		return x;
	}
	else{
		return y;
	}
}

class Photon
{
public:
	Photon(){}
	Photon(float _pT){
		this->pT = Scalar(_pT);
	}
	Photon(float _pT,float _phi){
		this->pT = Scalar(_pT);
		this->phi= Scalar(_phi);
	}
	Photon(double _pT,double _phi){
		this->pT = Scalar((float)_pT);
		this->phi= Scalar((float)_phi);
	}
	Photon(float _pT,float _phi, float _eta){
		this->pT = Scalar(_pT);
		this->phi= Scalar(_phi);
		this->eta= Scalar(_eta);
	}
	Photon(double _pT,double _phi, double _eta){
		this->pT = Scalar((float)_pT);
		this->phi= Scalar((float)_phi);
		this->eta= Scalar((float)_eta);
	}
	~Photon(){}
	Scalar getpT(){
		return pT;
	}
	void setpT(float _pT){
		this->pT = Scalar(_pT);
	}
	Scalar getphi(){
		return phi;
	}
	void setphi(float _phi){
		this->phi = Scalar(_phi);
	}
	Scalar geteta(){
		return eta;
	}
	void seteta(float _eta){
		this->eta = Scalar(_eta);
	}
	Point getAngle(){
		Point r;
		r.x=phi;
		r.y=eta;
		return r;
	}

private:
	Scalar pT;
	Scalar phi;
	Scalar eta;

};

class Jet
{
public:
	Jet(){}
	Jet(float _pT, float _phi, float _y, float _r){
		this->pT =Scalar(_pT);
		this->phi = Scalar(_phi);
		this->y = Scalar(_y);
		this->r = Scalar(_r);
	}
	Jet(double _pT, double _phi){
		pT=Scalar(_pT);
		phi=Scalar(_phi);
		y=Scalar(0);
		r=0;
	}
	~Jet(){
		if (next!=NULL)
		{
			delete next;
			next =NULL;
		}
		if (pair!=NULL)
		{
			delete pair;
			pair=NULL;
		}
	}
	void setMult(int m){
		mult=m;
	}
	int getmult(){
		return mult;
	}
	Scalar getpT(){
		return pT;
	}
	Scalar getphi(){
		return phi;
	}
	Scalar gety(){
		return y;
	}
	Scalar getr(){
		return r;
	}
	Scalar operator/(float s){ 
		return pT/s;
	}
	Scalar operator/(Jet s){ 
		return pT/s.getpT();
	}
	bool operator==(Jet s){
		return pT==s.pT&&phi==s.getphi();
	}
	bool operator>(Jet s){
		return pT>s.getpT();
	}
	bool operator<(Jet s){
		return pT<s.getpT();
	}
	bool operator>=(Jet s){
		return pT>=s.getpT();
	}
	bool operator<=(Jet s){
		return pT<=s.getpT();
	}
	bool operator!=(Jet s){
		return pT!=s.getpT();
	}
private:
	Scalar pT=-1;
	Scalar phi =7;
	Scalar y=0;
	Scalar r=-1;
	int mult=0;
	Jet* next=NULL;
	Jet* pair=NULL;
	
};

class XjPhi
{
public:
	XjPhi(float pt1, float phi1, float pt2, float phi2){
		//calculate scalar Xjs from parameters 
		xj=Scalar(smaller(pt1,pt2)/bigger(pt1,pt2));
		phi=Scalar(TMath::Abs(Scalar(phi2-phi1).value));
	}
	XjPhi(Jet leading, Jet subleading){
		xj=Scalar(subleading/leading);
		phi = Scalar(TMath::Abs(Scalar(subleading.getphi()-leading.getphi()).value));
		if (phi>TMath::Pi())
		{
			phi-=TMath::Pi();
		}
	}
	XjPhi(Photon p, Jet j){
		xj=(smaller(p.getpT(),j.getpT())/bigger(p.getpT(),j.getpT()));
		phi = TMath::Abs(Scalar(p.getphi()-j.getphi()).value);
		if (phi>TMath::Pi())
		{
			phi-=TMath::Pi();
		}
	}
	XjPhi(Scalar xj, Scalar phi){
		this->xj=xj;
		this->phi=phi;
	}
	XjPhi(){}
	~XjPhi(){}
	Scalar getXj(){
		return xj;
	}
	Scalar getphi(){
		return phi;
	}
	friend ostream& operator<<(ostream& os, XjPhi const & tc) {
        return os << tc.xj.value <<','<<tc.phi.value<<'\n';
    }
	
private:
	Scalar xj;
	Scalar phi;
};

#include <queue>

float** qXjPhiTo2DArray(queue<XjPhi> in){
	float** out = new float*[2];
	float *o1 = new float[in.size()];
	float *o2 = new float[in.size()];
	int i=0;
	while(!in.empty()){
		o1[i]=in.front().getXj().value;
		o2[i]=in.front().getphi().value;
		i++;
		in.pop();
	}
	out[0]=o1;
	out[1]=o2;
	return out;
}