//MEX code for calculating filtered output from an FIR filter using approximate adders:
#include "../include/define.h"
#include "../include/headers.h"
#include "../include/myFilter.h"

extern int FILT_ORDER, SIG_LEN, SIG_LEN_now;

vector<int> fulladd_call(int A, int B, int c_in, int type)
{
int S,Cout;
vector<int> toReturn;
//toReturn=new int[2]; 

int Cin1, Cin2, Cin3, Cin4, Cin5, Cin_ref;
        Cin1=c_in;Cin2=c_in;Cin3=c_in;Cin4=c_in; Cin5=c_in;Cin_ref=c_in;
 int i;

  if(type==1)
       {
            //approx 1
            S=((1-A) && (1-B) && Cin1) || (A && B && Cin1);
            Cout=(A && B && Cin1) || (A && B && (1-Cin1)) || (A && (1-B) && Cin1) || ((1-A) && B && Cin1) || ((1-A) && B && (1-Cin1)) ;
       }
        else if(type==2){
            //approx 2
            Cout=(A && B) || ((1-A) && B && Cin2) || (A && (1-B) && Cin2);
            S=1-Cout;
        }
       else if(type==3)
        {   //approx 3
            Cout=(A && B && Cin3) || (A && B && (1-Cin3)) || (A && (1-B) && Cin3) || ((1-A) && B && Cin3) || ((1-A) && B && (1-Cin3)) ;
            S=1-Cout; 
        }
        else if(type==4)
        {   //approx 4
            S=((1-A) && (1-B) && Cin4) || ((1-A) && B && Cin4) || (A && B && Cin4);
            Cout=A; 
        }
       else if(type==5)
       {    //approx 5
            S=B; Cout=A;
       }
        else
        {    //reference
            S= (A && B && Cin_ref) || ((1-A) && (1-B) && Cin_ref) || ((1-A) && B && (1-Cin_ref)) || (A && (1-B) && (1-Cin_ref)) ;
            Cout= (A && B) || ((1-A) && B && Cin_ref) || (A && (1-B) && Cin_ref);
        }
//cout<<"InsideFull adder: a="<<A<<", b="<<B<<", c="<<c_in<<", Sum="<<S<<",Cout="<<Cout<<endl;
toReturn.push_back(S); toReturn.push_back(Cout);

//cout<<"InsideFull adder: a="<<A<<", b="<<B<<", c="<<c_in<<", sum="<<toReturn[0]<<",carr="<<toReturn[1]<<endl; 
return toReturn; 
}

double fulladd_signedRCA(double A, double B, int N, int N_apx, int type)
{

//int *prod_bin=NULL;prod_bin=new int[N];// (mxGetPr(_prod_bin));
double prod_dec,prod_dec_true;



//actual addition begins------------------------
//MSB is the sign bit
int signA=0, signB=0, i, j,k;
double A_eff,B_eff;
if(A<0) {
    signA=1;
    A_eff=pow(2.0,N)+(A);
}
else {
    A_eff=A;
}

if(B<0) {
    signB=1; 
    B_eff=pow(2.0,N)+(B); 
}
else {
    B_eff=B;
}

int A_b[N]; int B_b[N];

for(i=0;i<N;i++){
A_b[i]=0;
B_b[i]=0;
}
long long int tempA,tempB;
tempA=(long long int)A_eff; tempB=(long long int)B_eff;

i=0;
while(tempA!=0 || tempB!=0){ //N bit representation of A_eff and B_eff
 A_b[i]=(int)((long long int)tempA % 2);
 B_b[i]=(int)((long long int)tempB % 2);
 i++;
 tempA=tempA/2.0; tempB=tempB/2.0; 
}

int Res[N];

vector<int> SC;
int c_out, a,b,c,t;

for(i=0;i<N;i++){	
	Res[i]=0; 
	}

///////////////////////////////////////////////Initialization over///////////////////////////////////////
k=0;
//Single row of adders
 int Cout;
c=0; //since the c-input is 0, so prob of 1 is 0
for (j=0;j<N_apx;j++){
 a=A_b[j]; b=B_b[j];
 SC=fulladd_call(a,b,c,type); 
 Res[k]=SC[0]; 
 k=k+1;
 c=SC[1];
}
for (j=N_apx;j<N;j++){
 a=A_b[j]; b=B_b[j]; 
 SC=fulladd_call(a,b,c,0);
 Res[k]=SC[0]; k=k+1;
 c=SC[1];
}

double sm,er;
sm=0;
for(i=0;i<N-1;i++){
    double pw = (pow(2.0,i)* Res[i]);
    sm=sm + pw;
}
sm=sm - (pow(2.0,N-1)* Res[N-1]);
double appx_prod=sm;
double exact_prod=A+B;
/*
for(j=0;j<N;j++){
  prod_bin[j]=Res[j];
 }*/
prod_dec=appx_prod;
prod_dec_true=exact_prod;
er=exact_prod-A - B;

//if(!prod_dec)free(prod_dec); if(!prod_dec_true)free(prod_dec_true); 
//if(!prod_bin)delete(prod_bin); 
if(!A_b)delete A_b; if(!B_b)delete B_b; 

return appx_prod;
}



//Usage: x_filt = fir_filtering(round(MULT*b),round(MULT*x), M, Apx(j,:), type); 
vector<double> fir_filtering(vector<double> coef, vector<double> inp, int M, double* Apx, int type)
{
 
vector<double> filt_out;

//double *SC=new double[2];
double SC;
int elem_coef,elem_inp;
int Nd,Nf;


elem_coef=FILT_ORDER; //mxGetNumberOfElements(prhs[0]);
elem_inp=SIG_LEN_now; //mxGetNumberOfElements(prhs[1]);
Nd=elem_inp;   Nf=elem_coef;
//filt_out= new double[SIG_LEN];

int i,j; 
double prev, Aval, Bval;
 
 for(i=0;i<Nf-1;i++) {
     filt_out.push_back(0);
 }
//cout<<"TILL HERE:  Input data size= "<<Nd<<", Filter size="<<Nf<<endl;
/*cout<<"inputs:";
for(i=0;i<Nd;i++) {
    cout<<inp[i]<<", ";
    //prev=round(coef[Nf-1]*inp[i]);
} cout<<endl<<"-------------------------------------------"<<endl;
cout<<"coeffs:";
for(i=0;i<Nf;i++) {
    cout<<coef[i]<<", ";
    //prev=round(coef[Nf-1]*inp[i]);
} cout<<endl<<"############################################"<<endl;
*/


for(i=Nf-1;i<Nd;i++) {
    prev=round(coef[Nf-1]*inp[i]);
    for(j=1;j<Nf; j++) {
        int apxbits=(int)Apx[j-1];     
        Aval=round(coef[Nf-j-1]*inp[i-j]); Bval=prev;
        SC=fulladd_signedRCA( Bval,Aval, M, apxbits, type);//usage of mex function
        prev=SC;
     }
    filt_out.push_back(prev);
  }
    
 return filt_out;         
}

void destroyEntry(double *entry)
{
    delete entry;
    entry=NULL;
}

