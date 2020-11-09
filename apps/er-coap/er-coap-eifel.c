#include "er-coap-transactions.h"

void calculateRTO(coap_rtt_estimations_t *t){
	clock_time_t delta;
	clock_time_t rto;
	int deltaCoef,gainbar,gain=3;
	int counter=0;
	//Calculate delta
	if(t->srttCoef == 1){
	  if(t->rtt >= t->srtt){
	    //printf("Eifel - %d\n",counter++);
	    delta = t->rtt - t->srtt;
	    deltaCoef = 1;
	  }
	  else{
		//printf("Eifel1 - %d\n",counter++);
	    delta = t->srtt - t->rtt;
	    deltaCoef = 0;
	  }
	}
	else{
	//printf("Eifel2 - %d\n",counter++);
	  delta = t->rtt + t->srtt;
	  deltaCoef = 1;
	}

	//printf("DELTA COEF:%d\n",deltaCoef);
	//printf("DELTA:%ld\n",delta);
	//calculate inverse of gain
	if(deltaCoef == 1){
//printf("Eifel3 - %d\n",counter++);
	  if(t->rttvarCoef == 0){
	    gainbar = 3;
	  }
	  else{
	    if(delta >= t->rttvar)
	      gainbar = 3;
	    else
	      gainbar = 9;
	  }
	}
	else{
//printf("Eifel4 - %d\n",counter++);
	  if(t->rttvarCoef == 1)
	    gainbar = 9;
	  else{
	    if(t->rttvar >= delta)
	      gainbar = 3;
	    else
	      gainbar = 9;
	  }
	}

	//Calculate srtt
	if(t->srttCoef == 1){
	  if(deltaCoef == 1){
	    t->srtt = t->srtt + delta/3;
	    t->srttCoef = 1;
	  }
	  else{
	    if(t->srtt >= delta/3){
	      t->srtt = t->srtt - delta/3;
	      t->srttCoef = 1;
	    }
	    else{
	      t->srtt = delta/3 - t->srtt;
	      t->srttCoef = 0;
	    }
	  }
	}
	else{
	  if(deltaCoef == 1){
	    if(delta/3 >= t->srtt){
	      t->srtt = delta/3 - t->srtt;
	      t->srttCoef = 1;
	    }
	    else{
	      t->srtt = t->srtt - delta/3;
	      t->srttCoef = 0;
	    }
	  }
	  else{
	    t->srtt = t->srtt + delta/3;
	    t->srttCoef = 0;
	  }
	}
	//printf("SRTT COEF:%d\n",t->srttCoef );
	//printf("SRTT:%ld\n",t->srtt);
	//Calculate rttvar
	if(deltaCoef == 1){
	  if(t->rttvarCoef == 1){
	//printf("Eifel6 - %d\n",counter++);
	    if(delta >= t->rttvar){
	      //printf("case 1\n");
	      t->rttvar = t->rttvar + (delta - t->rttvar)/gainbar ;
	      t->rttvarCoef = 1;
	    }
	    else{
	      if(t->rttvar >= (t->rttvar - delta)/gainbar){
		//printf("case 2\n");
		t->rttvar = t->rttvar - (t->rttvar - delta)/gainbar ;
		t->rttvarCoef = 1;
	      }
	      else{
		//printf("case 3\n");
		t->rttvar = (t->rttvar - delta)/gainbar - t->rttvar ;
		t->rttvarCoef = 0;
	      }
	    }
	  }
	  else{
//		printf("Eifel5 - %d\n",counter++);
	    if(t->rttvar <= (delta + t->rttvar)/gainbar){
	      //printf("case 4\n");
	      t->rttvar = (delta + t->rttvar)/gainbar - t->rttvar;
	      t->rttvarCoef = 1;
	    }
	    else{
	      //printf("case 5\n");
	      t->rttvar = t->rttvar - (delta + t->rttvar)/gainbar;
	      t->rttvarCoef = 0;
	    }
	  }
	}

	//printf("RTTVAR COEF:%d\n",t->rttvarCoef );
	//printf("RTTVAR:%ld\n",t->rttvar);

	if(t->srttCoef == 1){
//printf("Eifel7 - %d\n",counter++);
	  if(t->rttvarCoef == 1){
	    if((t->rtt + 2) > t->srtt + t->rttvar*gain)
	      //rto = t->rtt + 2*CLOCK_SECOND;
		rto = t->rtt + 2;
	    else
	      rto = t->srtt + t->rttvar*gain;

	  }
	  else{
	    if(t->srtt >= t->rttvar*gain){
	      if((t->rtt + 2) > t->srtt - t->rttvar*gain)
		//rto = t->rtt + 2*CLOCK_SECOND;
		  rto = t->rtt + 2;
	      else
		rto = t->srtt - t->rttvar*gain;
	    }
	    else{
	      //rto = t->rtt + 2*CLOCK_SECOND;
		rto = t->rtt + 2;
	    }
	  }
	}
	else{
//printf("Eifel8 - %d\n",counter++);
	  if(t->rttvarCoef == 1){
	    if(t->rttvar*gain >= t->srtt){
	      if((t->rttvar*gain - t->srtt) >= t->rtt + 2)
		rto = t->rttvar*gain - t->srtt;
	      else
		//rto = t->rtt + 2*CLOCK_SECOND;
		  rto = t->rtt + 2;
	    }
	  }
	  else
	    //rto = t->rtt + 2*CLOCK_SECOND;
	    rto = t->rtt + 2;
	}
	printf("RTO : %lu,RTT: %lu\n",rto,t->rtt);
	t->rto = rto;
	//t->rto = 2*CLOCK_SECOND;
	//printf("RTO : %lu,RTT: %lu\n",t->rto,t->rtt);
}

