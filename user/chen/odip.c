/* omnidirectional dip estimation by PWD */

#include <rsf.h>
#include "pwd.h"

static int n1, n2, nf;
static float **u1, **u2, **u3;

void odip_init(int mf, int m1, int m2, int *rect, int niter, bool verb)
/*< initialize >*/
{
	int n, nn[4];
	nf = mf;
	n1 = m1;
	n2 = m2;

	n = n1*n2;
	nn[0] = n1;
	nn[1] = n2;

	u1 = sf_floatalloc2(n1,n2);
	u2 = sf_floatalloc2(n1,n2);
	u3 = sf_floatalloc2(n1,n2);

	pwd_init(nf, n1, n2);
	sf_divn_init (2, n, nn, rect, niter, verb);
}

void odip_close()
/*< release memory >*/
{
	free(u1[0]);
	free(u1);
	free(u2[0]);
	free(u2);
	free(u3[0]);
	free(u3);
	pwd_close();
	sf_divn_close();
}

void odip(float **in, float **dip, int nit, bool od)
/*< omnidirectional dip estimation >*/
{
	int it, i1;
	double norm, eta;

	for(i1=0; i1<n1*n2; i1++) 
		dip[0][i1] = 0.0; // M_PI_2*rand()/(RAND_MAX+1.0);

	for (it=0; it<nit; it++)
	{	
		eta = 1.0/(1.0+it*it);
		if(od)
		{
			opwd(in, u1, dip);
			opwd_der(in, u2, dip);
		}else{
			lpwd(in, u1, dip);
			lpwd_der(in, u2, dip);
		}

		norm = 0.0;
		for(i1=0; i1<n1*n2; i1++) 
			norm += u2[0][i1]*u2[0][i1];
		if(norm == 0.0) return;
		norm = sqrtf(norm/n1/n2);

		for(i1=0; i1<n1*n2; i1++) 
		{
			u1[0][i1] /= norm;
			u2[0][i1] /= norm;
		}

		sf_divn(u1[0], u2[0], u3[0]);
		for(i1=0; i1<n1*n2; i1++)
		{
			dip[0][i1] -= eta*u3[0][i1];
			while(dip[0][i1]>M_PI_2) dip[0][i1] -= M_PI_2;
			while(dip[0][i1]<-M_PI_2) dip[0][i1] += M_PI_2;
		}
	}
}



