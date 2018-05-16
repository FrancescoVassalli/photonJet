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