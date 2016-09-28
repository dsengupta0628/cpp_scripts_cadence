/*coeffs for N=40, 0.10-0.2 passband

*/

/*coeffs for N=80, 0.10-0.2 passband
*/

/*coeffs for N=30, 0.10-0.25 passband
*/

/*coeffs for N=60, 0.15-0.25 passband
*/

/*coeffs for N=60, 0.20-0.25 passband
 */

/*coeffs for N=40, 0.20-0.25 passband
 */
 
/*coeffs for N=25, 0.25-0.30 passband
 */
 

#include "../include/define.h"
#include "../include/headers.h"
#include "../include/read_ckt.h"

using namespace std;

extern ifstream sig_inputfile, Napx_inputfile, coeff_inputfile;
extern ofstream bugfile;
double **Napx; double **Sig; double *Coef; double *Tvals;
int FILT_ORDER, SIG_LEN, NUM_SIG_ALTS, NUM_MONTE_APX,COUNT_ADD;

double round(double number)
{
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}

bool fileExists(string filename)
{	ifstream f (filename.c_str());
	 if (!(f.is_open()))
	{
	  return false;
	} 
    return true;
}

void read_sig_coeff()
{
string lines;
int i,j,k;


/////////////////////////////READING SIGNAL VALUES//////////////////////////////////////////////////////////////////
getline (sig_inputfile,lines);i=0;
do
{
i=i+1;	
getline (sig_inputfile,lines);
}
while ( sig_inputfile.good() );
NUM_SIG_ALTS=i;
Sig = new double *[NUM_SIG_ALTS]; //different songs in a particular genre 
//provided as the signal input file, usually max is 100, created from matlab

sig_inputfile.clear(); //now reading all lines one by one
sig_inputfile.seekg(0, ios::beg);	//by seeking to beginning of line
getline (sig_inputfile,lines);i=0;
do
{
	vector<double> rec;
	istringstream lin;
	lin.clear(); lin.str(lines);
    while(lin) {
		string data;
		if (!getline( lin, data, '\t' )) break;
		double a=atof(data.c_str());
	    rec.push_back(a);
	}
    SIG_LEN=rec.size();
    Sig[i] = new double[SIG_LEN];
    for(j=0;j<SIG_LEN; j++){
		Sig[i][j]=rec[j];	
	}
	i++;    
   getline (sig_inputfile,lines);
}
while ( sig_inputfile.good() );
//only for debugging//////////////////////////
/*	for (i = 0; i < NUM_SIG_ALTS; ++i) {	//
		for(j=0;j<SIG_LEN;j++){				//
			bugfile<<Sig[i][j]<<"\t";		//
		}									//
		bugfile<<endl;						//
	}*/										//
//////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////READING COEFFICIENTS////////////////////////////////////////////////
vector<double> vec_coef; 
getline (coeff_inputfile,lines);
do
{   
istringstream lin;
lin.clear();lin.str(lines);
double a;
lin>>a;
vec_coef.push_back(a); 
getline (coeff_inputfile,lines);
}
while ( coeff_inputfile.good() );
FILT_ORDER=vec_coef.size();
Coef = new double[FILT_ORDER];
for (i = 0; i < FILT_ORDER; ++i) {
      Coef[i] = vec_coef[i];
      //bugfile<<Coef[i]<<endl;
}
////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////READING Napx VALUES//////////////////////////////////////////////////////////////////
COUNT_ADD=FILT_ORDER-1;
getline (Napx_inputfile,lines);i=0;
do
{
i=i+1;	
getline (Napx_inputfile,lines);
}
while ( Napx_inputfile.good() );
NUM_MONTE_APX=i;
Napx = new double *[NUM_MONTE_APX];

Napx_inputfile.clear(); //now reading all lines one by one
Napx_inputfile.seekg(0, ios::beg);	//by seeking to beginning of line
getline (Napx_inputfile,lines);i=0;
do
{
   	vector<double> rec;
	istringstream lin;
	lin.clear(); lin.str(lines);
    while(lin) {
		string data;
		if (!getline( lin, data, '\t' )) break;
		double a=atof(data.c_str());
	    rec.push_back(a);
	}
    //COUNT_ADD=rec.size(); //should be equal to FILT_ORDER-1
    Napx[i] = new double[COUNT_ADD];
    for(j=0;j<COUNT_ADD; j++){
		Napx[i][j]=rec[j];	
	}
	i++;    
   getline (Napx_inputfile,lines);
}
while (Napx_inputfile.good() );
//only for debugging//////////////////////////
	/*for (i = 0; i < NUM_MONTE_APX; ++i) {	//
		for(j=0;j<COUNT_ADD;j++){			//
			bugfile<<Napx[i][j]<<"\t";		//
		}									//
		bugfile<<endl;						//
	}*/										//
//////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////Assiging Tvals///////////////////////////////////
//In MATLAB: tvals=linspace(0, length(sig)-1, length(sig))/Fs;
Tvals = new double[SIG_LEN];
double step= 1/Fs;
Tvals[0]=0;
for (i = 1; i < SIG_LEN; ++i) {
      Tvals[i] = Tvals[i-1] + step;
      //bugfile<<Coef[i]<<endl;
}
////////////////////////////////////////////////////////////////////////





}

