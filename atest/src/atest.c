/*
===============================================================================
 Name        : atest.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include <cr_section_macros.h>
#include <stdint.h>
// TODO: insert other include files here

// TODO: insert other definitions and declarations here
#define BUFFSZ 64
typedef struct {
	uint8_t bb;
	uint32_t btr;
} tf;
typedef struct {
	tf ar[5];
} tf_a;
tf_a st;
void aa(tf_a * ptr) {
	st.ar[0].bb=5;
	st.ar[0].btr=23;
	ptr = &st;
}
uint8_t buff[BUFFSZ];
uint32_t  ctop;
uint32_t  startp;
uint32_t  endp;
uint32_t  retp;
uint32_t  rtop;
uint32_t  gcp; /* get info */
uint32_t  gsz;
uint32_t  gcp2;
uint32_t  gsz2;
uint32_t  ftop;
uint32_t  ftop1;
uint32_t  fsz1;
uint32_t  grp; /* read offset */
uint32_t  grsz;/* read size */
uint32_t  grp2; /* read offset */
uint32_t  grsz2;/* read size */



void senda(uint32_t offset, uint32_t size) {
	ctop = ctop + size;
	/* to do: ctop over */
}
void getinf() {
	if (ftop>endp-ctop+1){
		gcp2=ctop;
		gsz2=endp - ctop + 1;
		retp=ctop;
		ctop = 0;
		gcp=ctop;
		if (ftop!=0) {
			gsz = ftop-1;
		} else {
			gsz = ftop-1;
		}
		gsz = ftop-1;
	} else {
		gcp = ctop;
		gsz = endp - ctop + 1;
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
void read(){
	if (ctop > rtop) {
		grp = rtop;
		grsz = ctop - rtop;
		rtop = ctop;
	} else if(ctop < rtop) {
		grp = 0;
		grsz = ctop;
		/* if ctop==0 grp2 -> grp */
		grp2 = rtop;
		grsz2 = retp - rtop;
		rtop = ctop;
	} else { /* ctop == rtop */
		grp = 0;
		grsz = 0;
	}
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
	tf_a * m;
	uint8_t uu;
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
	read();
	release(0,34);
	rcv(12);
	read();
	release(34,12);
	release(46,12);
	while(1);
    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
    	aa(m);
    	uu=m->ar[0].bb;
		i++;
		while (1);
    }
    return 0 ;
}
