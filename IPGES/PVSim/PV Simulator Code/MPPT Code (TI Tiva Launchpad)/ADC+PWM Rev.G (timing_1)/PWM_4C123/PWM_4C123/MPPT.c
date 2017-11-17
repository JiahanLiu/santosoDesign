float Vinit, Dmax, Dmin, deltaV, Vold, Pold, V, I, P, dV, dP, D, Dold, deltaD, Vnew;
int main (void){
	P = V*I;
	dV = V - Vold;
	dP = P - Pold;
	
	if (dP!=0){
	  if(dP < 0){
		  if(dV < 0)
				{D = Dold + deltaD;}
			else
				{D = Dold - deltaD;}
			}	
	  else {
			if(dV < 0)
				{D = Dold - deltaD;}
			else
				{D = Dold + deltaD;}
			}
		 }
	else
		{D = Dold;}
	
  if (D>=Dmax || D<=Dmin){
	  D=Dold;
	}
  
	Dold = D;
	Vold = V;
	Pold = P;
}
