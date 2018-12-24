/*
===============================================================================
 Name        : atest.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

//#include <cr_section_macros.h>
#include <stdint.h>
#include <string.h>
// TODO: insert other include files here

// TODO: insert other definitions and declarations here
#define BUFFSZ 64
typedef struct {
	int16_t uhOffset;
	int16_t shSize;
} Data_ts;
typedef struct {
	Data_ts  stData_a[3];
} Data_a_ts;
Data_a_ts stData;
//Data_a_ts stData_p_a = {&stData_a[0],&stData_a[1],&stData_a[3]};

uint8_t buff[BUFFSZ];
int32_t  ctop;
int32_t  startp;
int32_t  endp;
int32_t  retp;
int32_t  rtop;
int32_t  gcp; /* get info */
int32_t  gsz;
int32_t  gcp2;
int32_t  gsz2;
int32_t  ftop;
int32_t  ftop1;
int32_t  fsz1;
int32_t  grp; /* read offset */
int32_t  grsz;/* read size */
int32_t  grp2; /* read offset */
int32_t  grsz2;/* read size */
uint8_t  buff_full;



void senda(uint32_t offset, uint32_t size) {
	if (buff_full) {
		return; /* busy */
	}
	if (ftop>endp-ctop+1) {
		if (ftop > size) {
			retp=ctop;
			ctop=size;
		}
		else {
			buff_full = 1;
		}
	} else if (ctop + size > endp) {
		buff_full = 1;
	}
	else {
		ctop = ctop + size;
	}
	/* to do: ctop over */
}
void getinf(Data_a_ts * ptr) {
	if (ftop>endp-ctop+1){
		stData.stData_a[1].uhOffset=ctop;
		stData.stData_a[1].shSize=endp - ctop + 1;
		//retp=ctop;
		//ctop = 0;
		stData.stData_a[0].uhOffset=ctop;
		if (ftop!=0) {
			stData.stData_a[0].shSize = ftop-1;
		} else {
			stData.stData_a[0].shSize = ftop-1;
		}
		stData.stData_a[0].shSize = ftop-1;
	} else {
		stData.stData_a[0].uhOffset = ctop;
		stData.stData_a[0].shSize = endp - ctop + 1;
	}
	*ptr = stData;
}
void rbuff(uint32_t offset, uint32_t size) {
	if (ctop==(offset+size)){
		ctop = offset;
	} else if (ftop == 0){
		ftop = size;
	} else if (ftop==offset) {
		ftop = offset + size;
	} else {
		ftop1 = offset;
		fsz1 = size;
	}
	if (ctop >= ftop) {
		if (ctop==0 || ctop==ftop){
			ftop=0;
			ctop=0;
		}
	} else {
		if (retp==ftop){
			ftop=0;
		}
	}
	buff_full=0;
}
void sendtest() {
	Data_a_ts m;
	//Data_ts * m;
	buff[0]='1';
	senda(0,18);
	getinf((Data_a_ts *)&m);
	buff[gcp]='2';
	senda(gcp,18);
	getinf((Data_a_ts *)&m);
	buff[gcp]='3';
	senda(gcp,18);
	rbuff(0,18);
	rbuff(18,18);
	getinf((Data_a_ts *)&m);
	buff[gcp]='4';
	senda(gcp,18);
	rbuff(36,18);
	getinf((Data_a_ts *)&m);
}
//
void rcv(uint32_t sz){
	ctop = ctop + sz;
	if (ftop>endp-ctop+1){
		retp=ctop;
		ctop = 0;
	}
}
void read(Data_a_ts * ptr){
	uint32_t uwIndex;
	if (ctop > rtop) {
		stData.stData_a[0].uhOffset = rtop;
		stData.stData_a[0].shSize = ctop - rtop;
		rtop = ctop;
	} else if(ctop < rtop) {
		stData.stData_a[0].uhOffset = 0;
		stData.stData_a[0].shSize = ctop;
		/* if ctop==0 grp2 -> grp */
		if (ctop == 0) {
			uwIndex = 0;
		} else {
			uwIndex = 1;
		}
		stData.stData_a[uwIndex].uhOffset = rtop;
		stData.stData_a[uwIndex].shSize = retp - rtop;
		rtop = ctop;
	} else { /* ctop == rtop */
		stData.stData_a[0].uhOffset = 0;
		stData.stData_a[0].shSize = 0;
	}
	*ptr = stData;
}
void release(uint32_t offset, uint32_t size){
	if (ctop==(offset+size)){
		ftop = ctop;
	} else if (ftop == 0){
		ftop = size;
	} else if (ftop==offset) {
		ftop = offset + size;
	} else {
		ftop1 = offset;
		fsz1 = size;
	}
	if (ctop >= ftop) {
		if (ctop==0 || ctop==ftop){
			ftop=0;
			ctop=0;
			rtop=0; /* temp */
		}
	} else {
		if (retp==ftop){
			ftop=0;
			retp=BUFFSZ -1;
		}
	}
}
int main(void) {

    // TODO: insert code here
	Data_a_ts m;
	int16_t uu;
	uint32_t ss;
	startp = 0;
	endp = BUFFSZ -1;
	retp=BUFFSZ -1;
	ctop = 0;
	ftop = 0;
	rtop = 0; /* rcv read */
	buff_full = 0;
	//
	sendtest();
	ctop = 0;
	ftop = 0;
	//
	rcv(16);
	rcv(18);
	rcv(12);
	read((Data_a_ts *)&m);
	//release(0,34);
	release(0,16);
	rcv(12);
	read((Data_a_ts *)&m);
	release(16,18);
	release(34,12);
	rcv(14);
	release(46,12);
	read((Data_a_ts *)&m);
	uu=m.stData_a[0].shSize;
	uu=m.stData_a[1].shSize;
	while(1);
    return 0 ;
}
