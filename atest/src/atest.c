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
#define BUFFSZ 84
#define BLOCKSZ 16
#define INFOSZ 3
#define MAXMSGSZATEND 12
typedef enum {
	FREE =0,
	PROCESSING =1,
	USED =2,
	LOCKED =3,
	NOAREA =4
} Status_ts;
typedef struct {
	int16_t uhOffset;
	int16_t shSize;
} Data_ts;
typedef struct {
	Data_ts  stData_a[INFOSZ];
} Data_a_ts;
Data_a_ts stData;
typedef struct {
	int16_t uhOffset;
	int16_t shSize;
	uint8_t next;
	uint8_t prev;
	Status_ts status;
} List_ts;
List_ts list[BLOCKSZ];
int16_t ltop;
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
uint8_t  inmsg;
uint8_t  over;
uint8_t  current;
int32_t  mcnt;
int32_t  overcnt;
int32_t  msgsz;



// send pointer
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
void senda2(uint32_t offset, uint32_t size) {
	if (buff_full) {
		return; /* busy */
	}
	if (ftop <= ctop) {
		if (ctop + size > endp) {
			if (ftop >= size) {
				retp = ctop;
				ctop = size;
				if (ctop == ftop) {
					ftop = 0;
					ctop = retp;
				}
			} else {
				buff_full = 1;
			}
		} else {
			ctop = ctop + size;
		}
	} else if (ftop > ctop) {
		if (ctop + size <= ftop) {
			ctop = ctop + size;
			if (ctop == ftop) {
				ftop = 0;
				ctop = retp;
			}
		} else {
			buff_full = 1;
		}
	}
}
void getinf(Data_a_ts * ptr) {
	stData.stData_a[1].uhOffset=0;
	stData.stData_a[1].shSize=0;
	stData.stData_a[0].uhOffset=0;
	stData.stData_a[0].shSize = 0;

	if (ftop<ctop){
		if (ftop!=0) {
			stData.stData_a[1].uhOffset=ctop;
			stData.stData_a[1].shSize=endp - ctop + 1;
			stData.stData_a[0].uhOffset=0;
			stData.stData_a[0].shSize = ftop-1;
		} else {
			stData.stData_a[0].uhOffset=ctop;
			stData.stData_a[0].shSize=endp - ctop + 1;
		}
		//retp=ctop;
		//ctop = 0;
		//stData.stData_a[0].uhOffset=0;
		//if (ftop!=0) {
		//	stData.stData_a[0].shSize = ftop-1;
		//} else {
		//	stData.stData_a[0].shSize = 0;
		//}
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
			retp=endp;
		}
	}
	buff_full=0;
}
void sendtest() {
	Data_a_ts m;
	//Data_ts * m;
	while (1) {
	buff[0]='1';
	senda2(0,18);
	getinf((Data_a_ts *)&m);
	buff[gcp]='2';
	senda2(gcp,18);
	getinf((Data_a_ts *)&m);
	buff[gcp]='3';
	senda2(gcp,18);
	rbuff(0,18);
	rbuff(18,18);
	getinf((Data_a_ts *)&m);
	buff[gcp]='4';
	senda2(gcp,18);
	rbuff(36,18);
	getinf((Data_a_ts *)&m);
	}
}
// rcv pointer
void rcv(uint32_t sz){
    if (ftop>ctop && (ctop + sz > ftop)) {
    	buff_full = 1;
    	return; /* over flow */
    }
    if (ctop + sz > endp) {
    	buff_full = 1;
    	return; /* over flow */
    }
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
	} else if (ftop == 0 && offset==0){
		ftop = size;
	} else if (ftop==offset) {
		ftop = offset + size;
	} else if (offset > ftop && offset < rtop) { /* remember irregular case */
		ftop1 = offset;
		fsz1 = size;
		return;
	} else {
		return; /* param error */
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
	buff_full = 0;
}
// rcv list
//
uint8_t regmsg(){
	uint32_t i;
	uint8_t cl;
	current=0;
	if (over==1) { /* over flow */
		return 0;
	}
	mcnt++;
	/* get free */
	for (i=1;i<BLOCKSZ;i++){
		if(list[i].status==FREE){
			break;
		}
	}
	if (i>=BLOCKSZ) {
		return 0; /* no entry */
	}
	current=i;
	list[i].status=PROCESSING;
	list[i].shSize=0;
	list[i].uhOffset=ctop-1;
	if (ltop==0){
		list[i].prev=0;
		list[i].next=0;
		ltop = i;
	} else {
		cl=ltop;
		while(cl!=0) {
			if (list[cl].next==0) {
				if(list[cl].uhOffset>list[i].uhOffset) { /* front */
					list[i].next=cl;
					list[i].prev=list[cl].prev;
					list[cl].prev=i;
					if(cl==ltop){
						ltop=i;
					}
				} else { /* tail */
					list[i].next=list[cl].next;
					list[i].prev=cl;
					list[cl].next=i;
				}
				break;
			}
			if (list[list[cl].next].uhOffset>list[i].uhOffset) {
				if(cl==ltop){
					//if(list[cl].next!=0) {
						//list[list[cl].next].prev=i;
					//}

					//list[i].next=list[cl].next;
					//list[i].prev=cl;
					//list[cl].next=i;
					if(list[cl].uhOffset>list[i].uhOffset) { /* front */
						list[cl].prev=i;
						list[i].next=cl;
						list[i].prev=0;
						ltop=i;
					} else { /* after */
						list[list[cl].next].prev=i; /* list[cl].next!=0 here */
						list[i].next=list[cl].next;
						list[i].prev=cl;
						list[cl].next=i;
					}
				}else{
					//if(list[cl].next!=0) {
						//list[list[cl].next].prev=i;
					//}
					//list[i].next=list[cl].next;
					//list[i].prev=cl;
					//list[cl].next=i;
					//list[last].next=i;
					if(list[cl].uhOffset>list[i].uhOffset) { /* front */
						list[cl].prev=i;
						list[i].next=cl;
						list[i].prev=0;
						ltop=i;
					} else { /* after */
						list[list[cl].next].prev=i; /* list[cl].next!=0 here */
						list[i].next=list[cl].next;
						list[i].prev=cl;
						list[cl].next=i;
					}
				}
				break;
			}
			cl=list[cl].next;
		}
	}
	return 1;
}
uint8_t chkbuff(uint32_t cctop){
	uint8_t cl;
	cl=ltop;
	while (cl!=0) {
		if (cctop>=list[cl].uhOffset && cctop<(list[cl].uhOffset+list[cl].shSize)) {
			return 0;
		}
		cl=list[cl].next;
	}
	return 1;
}
// release list, rcv and send
uint8_t rlist(uint16_t offset, int16_t size){
	uint8_t cl;
	uint8_t last;
	cl=ltop;
	while (cl!=0) {
		if (offset>=list[cl].uhOffset && offset<(list[cl].uhOffset+list[cl].shSize)) {
			/* remove entry */
			if(cl==ltop){
				if (list[cl].next!=0) {
					list[list[cl].next].prev = 0;
				}
				ltop = list[cl].next;
			} else {
				list[last].next = list[cl].next;
				if (list[cl].next!=0) {
					list[list[cl].next].prev = last;
				}
			}
			list[cl].status=FREE;
			if (list[cl].next!=0 && list[cl].shSize<size && (list[cl].uhOffset
					+ list[cl].shSize)==list[list[cl].next].uhOffset) { /* more entry */
				size = size - list[cl].shSize;
				offset = list[list[cl].next].uhOffset;
			} else {
				break;
			}
		}
		last = cl;
		cl=list[cl].next;
	}
	return 1;
}
void rcvirq(uint8_t ch)
{
	uint8_t ret;
	/* read reg */
	if(over==0 && chkbuff(ctop)==1) {
		buff[ctop]=ch;
		ctop++;
		if (inmsg==1) {
			msgsz++;
		}
		if (ctop>=endp) {
			//if (chkbuff(startp)==1) { /* round ok ? */
				ctop = startp;
			//}
			if (inmsg==1) {
				over=1;
				overcnt=overcnt + msgsz;
			}
		}
    } else {
    	if (over==0) {
    		ctop++;
    	}
    	over=1;
		overcnt++;
	}
	if (ch==0x7e) {
		if (inmsg) { /* msg end */
			if (over==1) { /*　over　flow */
				/* remove entry */
				if (current != 0) {
					list[current].status=FREE;
					list[list[current].next].prev = list[current].prev;
					if (current==ltop) {
						ltop=list[current].next;
					} else {
						list[list[current].prev].next = list[current].next;
					}
					mcnt--;
				}
			} else {
				list[current].shSize=msgsz;
				list[current].status=USED;
				/* if near end, round ctop */
				if (ctop > endp - MAXMSGSZATEND) {
					if (chkbuff(startp)==1) { /* round ok ? */
						ctop = startp;
					}
				}
			}
			inmsg=0;
			over=0;
			msgsz=0;
		} else {/* new msg */
			ret=regmsg();
			if (ret==0) { /* no entry */
				ctop--; /* adjust */
				over=1;
			}
			inmsg=1;
			msgsz=1;
		}
	}
}
// rcv list test
void rcvtest() {
	uint8_t sss[] = "~0abcdefghijk~";
	uint32_t ss,uu;

	for(ss=0;ss<9;ss++){
		 uu=0;
		 if(ss==3) {
			 rlist(0,42);
		 }
		 if (ss==6) {
			 ss=6;
		 }
		 sss[uu+1]=sss[uu+1]+1;
		 while(sss[uu]!='\0') {
			rcvirq(sss[uu]);
			uu++;
		 }
    }
}
// send list
uint8_t slist(uint16_t offset, int16_t size){
	uint32_t i;
	uint8_t cl;
	/* get free */
	for (i=1;i<BLOCKSZ;i++){
		if(list[i].status==FREE){
			break;
		}
	}
	if (i>=BLOCKSZ) {
		return 0; /* no entry */
	}
	current=i;
	list[i].status=USED;
	list[i].shSize=size;
	list[i].uhOffset=offset;
	if (ltop==0){
		list[i].prev=0;
		list[i].next=0;
		ltop = i;
	} else {
		cl=ltop;
		while(cl!=0) {
			if (list[cl].next==0) {
				if(list[cl].uhOffset>list[i].uhOffset) { /* front */
					list[i].next=cl;
					list[i].prev=list[cl].prev;
					list[cl].prev=i;
					if(cl==ltop){
						ltop=i;
					}
				} else { /* tail */
					list[i].next=list[cl].next;
					list[i].prev=cl;
					list[cl].next=i;
				}
				break;
			}
			if (list[list[cl].next].uhOffset>list[i].uhOffset) {
				if(cl==ltop){
					if(list[cl].uhOffset>list[i].uhOffset) { /* front */
						list[cl].prev=i;
						list[i].next=cl;
						list[i].prev=0;
						ltop=i;
					} else { /* after */
						list[list[cl].next].prev=i; /* list[cl].next!=0 here */
						list[i].next=list[cl].next;
						list[i].prev=cl;
						list[cl].next=i;
					}
				}else{
					if(list[cl].uhOffset>list[i].uhOffset) { /* front */
						list[cl].prev=i;
						list[i].next=cl;
						list[i].prev=0;
						ltop=i;
					} else { /* after */
						list[list[cl].next].prev=i; /* list[cl].next!=0 here */
						list[i].next=list[cl].next;
						list[i].prev=cl;
						list[cl].next=i;
					}
				}
				break;
			}
			cl=list[cl].next;
		}
	}
	return 1;
}
void sinfo(Data_a_ts * ptr){
	uint8_t cl,last,idx;
	for (idx=0;idx<INFOSZ;idx++) {
		stData.stData_a[idx].uhOffset = 0;
		stData.stData_a[idx].shSize = 0;
	}
	idx=0;
	cl=ltop;
	last = 0;
	while (cl!=0) {
		if (cl==ltop && list[cl].uhOffset != 0) { /* free area */
			stData.stData_a[idx].uhOffset = 0;
			stData.stData_a[idx].shSize = list[cl].uhOffset;
			idx++;
		}
		if (list[cl].next != 0 && list[list[cl].next].uhOffset != (list[cl].uhOffset + list[cl].shSize)) { /* free area */
			stData.stData_a[idx].uhOffset = list[cl].uhOffset + list[cl].shSize;
			stData.stData_a[idx].shSize = list[list[cl].next].uhOffset - list[cl].uhOffset - list[cl].shSize;
			idx++;
		}
		last = cl;
		cl=list[cl].next;
	}
	if (last != 0 && endp != ( list[last].uhOffset + list[last].shSize - 1)) {
		stData.stData_a[idx].uhOffset = list[last].uhOffset + list[last].shSize;
		stData.stData_a[idx].shSize = endp - list[last].uhOffset - list[last].shSize + 1;
	} else if (ltop==0) {
		stData.stData_a[0].uhOffset = 0;
		stData.stData_a[0].shSize = endp + 1;
	}
	*ptr = stData;
}

int main(void) {

    // TODO: insert code here
	Data_a_ts m;
	int16_t uu;
	uint32_t ss,i,j;
	uint8_t sss[] = "~0abcdefghijk~";
	startp = 0;
	endp = BUFFSZ -1;
	retp=BUFFSZ -1;
	ctop = 0;
	ftop = 0;
	rtop = 0; /* rcv read */
	buff_full = 0;
	inmsg = 0;
	uu=0;
	mcnt=0;
	ltop = 0;
	over=0;
	overcnt=0;
	msgsz=0;
	for (i=0;i<BLOCKSZ;i++){
		list[i].status=FREE;
	}
	// send list test
	sinfo(&m);
	slist(0,14);
	slist(14,14);
	slist(28,14);
	sinfo(&m);
	slist(42,14);
	slist(56,14);
	slist(70,14);
	sinfo(&m);
	rlist(0,14);
	rlist(14,14);
	rlist(28,14);
	sinfo(&m);
	rlist(42,14);
	rlist(56,14);
	sinfo(&m);
	slist(0,14);
	slist(14,14);
	//slist(28,14);
	sinfo(&m);
	rlist(0,14);
	sinfo(&m);
	while(1);
	// rcv list test
	rcvtest();
	while(1);
	// send pointer test
	sendtest();
	ctop = 0;
	ftop = 0;
	// rcv pointer test
	rcv(16);
	rcv(18);
	rcv(12);
	read((Data_a_ts *)&m);
	release(0,16);
	//release(0,46);
	rcv(12);
	read((Data_a_ts *)&m);
	//release(0,12);
	//release(34,12);
	rcv(14);
	//release(46,12);
	read((Data_a_ts *)&m);
	release(16,18);
	release(34,12);
	release(46,12);
	uu=m.stData_a[0].shSize;
	uu=m.stData_a[1].shSize;
	while(1);
    return 0 ;
}
