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
	Data_ts ar[3];
} tf_a;
Data_ts stDara_a[3];

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



void senda(uint32_t offset, uint32_t size) {
	ctop = ctop + size;
	/* to do: ctop over */
}
void getinf() {
	if (ftop>endp-ctop+1){
		stDara_a[1].uhOffset=ctop;
		stDara_a[1].shSize=endp - ctop + 1;
		retp=ctop;
		ctop = 0;
		stDara_a[0].uhOffset=ctop;
		if (ftop!=0) {
			stDara_a[0].shSize = ftop-1;
		} else {
			stDara_a[0].shSize = ftop-1;
		}
		stDara_a[0].shSize = ftop-1;
	} else {
		stDara_a[0].uhOffset = ctop;
		stDara_a[0].shSize = endp - ctop + 1;
	}
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
}
void sendtest() {
	buff[0]='1';
	senda(0,18);
	getinf();
	buff[gcp]='2';
	senda(gcp,18);
	getinf();
	buff[gcp]='3';
	senda(gcp,18);
	rbuff(0,18);
	rbuff(18,18);
	getinf();
	buff[gcp]='4';
	senda(gcp,18);
	rbuff(36,18);
	getinf();
}
//
void rcv(uint32_t sz){
	ctop = ctop + sz;
	if (ftop>endp-ctop+1){
		retp=ctop;
		ctop = 0;
	}
}
void read(Data_ts ptr[]){
	uint32_t uwIndex;
	if (ctop > rtop) {
		stDara_a[0].uhOffset = rtop;
		stDara_a[0].shSize = ctop - rtop;
		rtop = ctop;
	} else if(ctop < rtop) {
		stDara_a[0].uhOffset = 0;
		stDara_a[0].shSize = ctop;
		/* if ctop==0 grp2 -> grp */
		if (ctop == 0) {
			uwIndex = 0;
		} else {
			uwIndex = 1;
		}
		stDara_a[uwIndex].uhOffset = rtop;
		stDara_a[uwIndex].shSize = retp - rtop;
		rtop = ctop;
	} else { /* ctop == rtop */
		stDara_a[0].uhOffset = 0;
		stDara_a[0].shSize = 0;
	}
	ptr = stDara_a;
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
	Data_ts * m;
	int16_t uu;
	uint32_t ss;
	startp = 0;
	endp = BUFFSZ -1;
	retp=BUFFSZ -1;
	ctop = 0;
	ftop = 0;
	rtop = 0; /* rcv read */
	//
	rcv(16);
	rcv(18);
	rcv(12);
	read(m);
	//release(0,34);
	release(0,16);
	rcv(12);
	read(m);
	release(16,18);
	release(34,12);
	rcv(14);
	release(46,12);
	read(m);
	uu=m[0].shSize;
	uu=m[1].shSize;
	while(1);
    return 0 ;
}
