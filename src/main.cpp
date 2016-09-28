#include "../include/define.h"
#include "../include/headers.h"
#include "../include/read_ckt.h"
#include "../include/myFilter.h"

using namespace std;
//valgrind --leak-check=yes ./do_filter signal_blues.txt   coeffs_fir5.txt   Napx_vals.txt 1


extern double **Napx; extern double **Sig; extern double *Coef; extern double *Tvals;
extern int FILT_ORDER, SIG_LEN, NUM_SIG_ALTS, NUM_MONTE_APX, COUNT_ADD;
int NUM_SIG_ALTS_INP, SIG_LEN_now;
int i,j,k;

ifstream sig_inputfile, Napx_inputfile, coeff_inputfile;
ofstream bugfile,resfile;

string CktPath=DATAPATH;
string AuxPath=AUXFILES;

int main(int argc, char* argv[])
{

//using namespace boost::numeric::ublas;

if(argc<6)
{cout<<"Usage:"<<endl<<"<executable> <signal_file> <coefficients_file> <Apx_file> <howmany_alts_of_signal> <signal_length_max>"<<endl;
	cout<<"NOTE: For now, consider one type of filter for all inputs..."<<endl;
exit(0);
}

string signame,coeffname, apxname, bugname, Filtname;
signame=CktPath+argv[1];
coeffname=CktPath+argv[2];
apxname=CktPath+argv[3];
NUM_SIG_ALTS_INP=(int)atof(argv[4]);
SIG_LEN_now=(int)atof(argv[5]);

bugname=AuxPath+"bugfile";
Filtname=AuxPath+"filt_signal.txt";

cout<<"signal: "<<signame<<"; coef of filter: "<<coeffname<<"; apxbits in: "<<apxname<<endl;
if(!fileExists(signame))
{
	cout<<" Signal file missing! Exiting...."<<endl;
	exit(0);
}
if(!fileExists(coeffname))
{
	cout<<" Coefficients file missing! Exiting...."<<endl;
	exit(0);
}
if(!fileExists(apxname))
{
	cout<<" Aprox LSB file missing! Exiting...."<<endl;
	exit(0);
}

sig_inputfile.open(signame.c_str(),ios::in);
coeff_inputfile.open(coeffname.c_str(),ios::in);
Napx_inputfile.open(apxname.c_str(),ios::in);

bugfile.open(bugname.c_str(),ios::out);
resfile.open(Filtname.c_str(),ios::out);

clock_t t1A,t2A;
t1A=clock();
//to assign the arrays: x for signal, Apx for the number of approx bits in each node, and b for filter coeffs
read_sig_coeff();
//FILT_ORDER, SIG_LEN, NUM_SIG_ALTS, NUM_MONTE_APX;
//Napx[NUM_MONTE_APX][COUNT_ADD],Sig[NUM_SIG_ALTS][SIG_LEN],Coef[FILT_ORDER];

if(SIG_LEN_now<FILT_ORDER){
	cout<<"signal length should be atleast filter order. exiting..."<<endl;
	exit(0);
}

t2A=clock();
float diff_A ((float)t2A-(float)t1A);
float seconds_A = diff_A / CLOCKS_PER_SEC;
cout<<"ELAPSED TIME IN POPULATING ARRAYS="<<seconds_A<<endl;	

double MULT=1024.0; //multiplying factor 
int M=32; //size of each adder
int type=5; //type of approx adder

cout<<"Num Coefficients="<<FILT_ORDER<<", signal alts="<<NUM_SIG_ALTS_INP<<", sig length="<<SIG_LEN_now<<", no. of adders="<<COUNT_ADD<<", no. of Apx sets="<<NUM_MONTE_APX<<endl;

vector<double> x_filt_exact;
clock_t t1,t2;
t1=clock();
//NUM_SIG_ALTS_INP
for (i=0; i<NUM_SIG_ALTS_INP;i++)
{//x=Sig[i][0:SIG_LEN-1] is the original signal, t=Tvals is the time instant 
    double *x = Sig[i]; 
    vector<double> x_scaled; 
    vector<double> Coef_scaled; 
    
    for(k=0;k<SIG_LEN_now;k++){
		x_scaled.push_back(round(x[k]*MULT));
		resfile<<x[k]<<"\t";} resfile<<endl; //1st line of resfile is original

	for(k=0;k<FILT_ORDER;k++){Coef_scaled.push_back(round(Coef[k]*MULT));}
     
    for (j=0; j< NUM_MONTE_APX; j++){
    //Apx(j,:)=Napx[j][0:COUNT_ADD-1] is the Apx bit count in each approx adder
			 vector<double> x_filt; //vector<double> Apx_vec; 
			 double *Apx = Napx[j]; 
//			for(k=0;k<COUNT_ADD;k++){Apx_vec.push_back(Apx[k]);}

			 x_filt = fir_filtering(Coef_scaled,x_scaled, M, Apx, type);
			 //cout<<"size of returned="<<x_filt.size()<<endl; 
			 for(k=0;k<SIG_LEN_now;k++){
				 x_filt[k] = x_filt[k]/(MULT*MULT); 
				 resfile<<x_filt[k]<<"\t";
				 }
				 resfile<<endl;

			if(j==0){
				for(k=0;k<SIG_LEN_now;k++){x_filt_exact.push_back(x_filt[k]);} 
				 }
			
			x_filt.clear(); //delete(x_filt.begin(), x_filt.end()); //if(x_filt) {delete x_filt; x_filt=NULL; }
		}
        x_filt_exact.clear();
    
}

t2=clock();
float diff ((float)t2-(float)t1);
float seconds = diff / CLOCKS_PER_SEC;
cout<<"ELAPSED TIME IN FIR="<<seconds<<endl;	

resfile.close();
bugfile.close();
sig_inputfile.close();
coeff_inputfile.close();
Napx_inputfile.close();


if(Napx){
	for (i=0; i<NUM_MONTE_APX; i++){if(Napx[i]){delete[] Napx[i]; Napx[i]=NULL;}}
	//if(Napx){delete Napx; Napx=NULL;}
	}
if(Sig){
	for (i=0; i<NUM_SIG_ALTS; i++){if(Sig[i]){delete[] Sig[i]; Sig[i]=NULL;}}
	//if(Sig){delete Sig; Sig=NULL;}
	}
if(Tvals){delete[] Tvals; Tvals=NULL;}
if(Coef){delete[] Coef; Coef=NULL;}

return 0;
}



































//was present earlier
/*
//get the mu and sigma LUTs from mult<type_add>_x<num_appx>
//pass the LUTs to the function

std::ostringstream oss,oss2;
oss <<"./data/MULT"<<type_add<<"_X"<<Napxbit;
oss2 <<"./data/ADD"<<type_add<<"_X"<<Napxbit;

std::string LUTmult_name = oss.str(); std::string LUTadd_name = oss2.str();

cout<<LUTmult_name<<", "<<LUTadd_name<<endl;
//read each line of this file; skip 1st line, in 2nd line, skip till 1st space, rest are the input sigmas
//next line onwards, 1st char is the input mean, 2nd space onwards are the mean values
ifstream ipf1,ipf2;
ipf1.open(LUTmult_name.c_str(), ios::in);
ipf2.open(LUTadd_name.c_str(), ios::in);

if(!ipf1.is_open() || !ipf2.is_open() )  {
       cout<<"multiplier and adder not characterized for adder type "<<type_add<<" and approx LSBs="<<Napxbit<<endl;
       exit(0);
    }
    
int mn=-1,sg=-1,fnd,lno=0;
vector<double> allmean,allsigma,allmean2,allsigma2,Out_mu_rowmajor,Out_sigma_rowmajor,Out_mu_rowmajor2,Out_sigma_rowmajor2;
string lines;
do
{
   getline (ipf1,lines);
   lno=lno+1;
   fnd=lines.find("op means");
   if(fnd>=0){mn=mn+1;}
   fnd=lines.find("op sigmas");
   if(fnd>=0){sg=sg+1;mn=mn-1; lno=1;}
   
   if(mn==0 && sg==-1){//cout<<"populate mean LUT"<<endl;
	   int g1,g2;
	   if(lno==2){ //2nd line, skip 1st character
			g1=lines.find(" ",0);
			   //cout<<lines<<" and g1="<<g1<<" ,line length="<<lines.length()<<endl;
			while(g1<lines.length()-1){
				g2=lines.find(" ",g1+1);
				string v=lines.substr(g1+1,g2-g1-1); //cout<<v<<"*"<<endl;
				allsigma.push_back(atof(v.c_str()));
				g1=lines.find(" ",g2);   
			}
		   }
	  if(lno>2){ //3rd line onwards actual data
			g1=lines.find(" ",0);
			string v2=lines.substr(0,g1); 
			if(v2.length()>1){//cout<<v2<<"--> "<<mn<<","<<sg<<":";
			allmean.push_back(atof(v2.c_str()));}
			while(g1<lines.length()-1){
				g2=lines.find(" ",g1+1);
				string v=lines.substr(g1+1,g2-g1-1); //cout<<v<<" ";
				Out_mu_rowmajor.push_back(atof(v.c_str()));
				g1=lines.find(" ",g2);   
			}
			//cout<<endl;    
		   }	   
	   }
   if(mn==-1 && sg==0){//cout<<"populate sigma LUT"<<endl;
      int g1,g2;
	  if(lno>2){ 
			g1=lines.find(" ",0);
			string v2=lines.substr(0,g1); 
			//if(v2.length()>1){cout<<v2<<"--> ";}
			while(g1<lines.length()-1){
				g2=lines.find(" ",g1+1);
				string v=lines.substr(g1+1,g2-g1-1); //cout<<v<<" ";
				Out_sigma_rowmajor.push_back(atof(v.c_str()));
				g1=lines.find(" ",g2);   
			}
//			cout<<endl;    
		   }	   
	   }
}
while( ipf1.good() );
ipf1.close();

mn=-1,sg=-1,fnd,lno=0;
do
{
   getline (ipf2,lines);
   lno=lno+1;
   fnd=lines.find("op means");
   if(fnd>=0){mn=mn+1;}
   fnd=lines.find("op sigmas");
   if(fnd>=0){sg=sg+1;mn=mn-1; lno=1;}
   
   if(mn==0 && sg==-1){//cout<<"populate mean LUT"<<endl;
	   int g1,g2;
	   if(lno==2){ //2nd line, skip 1st character
			g1=lines.find(" ",0);
			   //cout<<lines<<" and g1="<<g1<<" ,line length="<<lines.length()<<endl;
			while(g1<lines.length()-1){
				g2=lines.find(" ",g1+1);
				string v=lines.substr(g1+1,g2-g1-1); //cout<<v<<"*"<<endl;
				allsigma2.push_back(atof(v.c_str()));
				g1=lines.find(" ",g2);   
			}
		   }
	  if(lno>2){ //3rd line onwards actual data
			g1=lines.find(" ",0);
			string v2=lines.substr(0,g1); 
			if(v2.length()>1){//cout<<v2<<"--> "<<mn<<","<<sg<<":";
				allmean2.push_back(atof(v2.c_str()));}
			while(g1<lines.length()-1){
				g2=lines.find(" ",g1+1);
				string v=lines.substr(g1+1,g2-g1-1); //cout<<v<<" ";
				Out_mu_rowmajor2.push_back(atof(v.c_str()));
				g1=lines.find(" ",g2);   
			}
			//cout<<endl;    
		   }	   
	   }
   if(mn==-1 && sg==0){//cout<<"populate sigma LUT"<<endl;
      int g1,g2;
	  if(lno>2){ 
			g1=lines.find(" ",0);
			string v2=lines.substr(0,g1); 
			//if(v2.length()>1){cout<<v2<<"--> ";}
			while(g1<lines.length()-1){
				g2=lines.find(" ",g1+1);
				string v=lines.substr(g1+1,g2-g1-1); //cout<<v<<" ";
				Out_sigma_rowmajor2.push_back(atof(v.c_str()));
				g1=lines.find(" ",g2);   
			}
//			cout<<endl;    
		   }	   
	   }
}
while( ipf2.good() );
ipf2.close();
*/
