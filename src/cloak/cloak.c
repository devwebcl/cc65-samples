/*
 *	ScottFree Revision 1.14b
 *
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 *
 *	You must have an ANSI C compiler to build this program.
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
//#include <dos.h>
//#include <stdarg.h>
#include <conio.h>
#include <time.h>

#include "Scott.h"

#define TRUE 1
#define FALSE 0

Header GameHeader;
Tail GameTail;
Item *Items;
Room *Rooms;
char **Verbs;
char **Nouns;
char **Messages;
Action *Actions;
int LightRefill;
char NounText[16];
int Counters[16];	/* Range unknown */
int CurrentCounter;
int SavedRoom;
int RoomSaved[16];	/* Range unknown */
int DisplayUp;		/* Curses up */
void *Top,*Bottom;
int Redraw;		/* Update item window */
int Options;		/* Option flags set */
int Width;		/* Terminal width */
int TopHeight;		/* Height of top window */
int BottomHeight;	/* Height of bottom window */

#define TRS80_LINE	"\n<------------------------------------------------------------>\n"

#define TOPCOL	BROWN
#define BOTCOL	BLUE

#define MyLoc	(GameHeader.PlayerRoom)

long BitFlags=0;	/* Might be >32 flags - I haven't seen >32 yet */

void Fatal(char *x)
{
	//window(1,1,Width,TopHeight+BottomHeight);
	//textbackground(BLUE);
	//gotoxy(1,TopHeight+BottomHeight);
	//cprintf("%s.\n\r",x);

	//printf("fatal: %s.\n\r",x);
	printf("fatal: %s.\n",x);

	//textbackground(BLACK);
         for(;;){
               //printf("press any key\n");
               //(void) cgetc();
        }

		exit(1);
	
	
}

void Aborted()
{
	Fatal("User exit");
}

void ClearScreen(void)
{
	//window(1,TopHeight+1,Width,TopHeight+BottomHeight);
	//clrscr();
}

void *MemAlloc(int size)
{
	void *t=(void *)malloc(size);
	if(t==NULL)
		Fatal("Out of memory");
	return(t);
}

int RandomPercent(int n)
{
	unsigned int rv=rand()<<6;
	rv%=100;
	if(rv<n)
		return(1);
	return(0);
}

int CountCarried()
{
	int ct=0;
	int n=0;
	while(ct<=GameHeader.NumItems)
	{
		if(Items[ct].Location==CARRIED)
			n++;
		ct++;
	}
	return(n);
}

char *MapSynonym(char *word)
{
	int n=1;
	char *tp;
	static char lastword[16];	/* Last non synonym */
	while(n<=GameHeader.NumWords)
	{
		tp=Nouns[n];
		if(*tp=='*')
			tp++;
		else
			strcpy(lastword,tp);
		//if(strnicmp(word,tp,GameHeader.WordLength)==0)
		if(strncmp(word,tp,GameHeader.WordLength)==0)
			return(lastword);
		n++;
	}
	return(NULL);
}

int MatchUpItem(char *text, int loc)
{
	char *word=MapSynonym(text);
	int ct=0;
	
	if(word==NULL)
		word=text;
	
	while(ct<=GameHeader.NumItems)
	{
		if(Items[ct].AutoGet && Items[ct].Location==loc &&
            //strnicmp(Items[ct].AutoGet,word,GameHeader.WordLength)==0)
			strncmp(Items[ct].AutoGet,word,GameHeader.WordLength)==0)
			return(ct);
		ct++;
	}
	return(-1);
}

char *ReadString(FILE *f)
{
	char tmp[1024];
	char *t;
	int c,nc;
	int ct=0;
oops:	do
	{
		c=fgetc(f);
	}
	while(c!=EOF && isspace(c));
	if(c!='"')
	{
		Fatal("Initial quote expected");
	}
	do
	{
		c=fgetc(f);
		if(c==EOF)
			Fatal("EOF in string");
		if(c=='"')
		{
			nc=fgetc(f);
			if(nc!='"')
			{
				ungetc(nc,f);
				break;
			}
		}
		if(c==0x60) 
			c='"'; /* pdd */
		if(c=='\n')
		{
			tmp[ct++]=c;
			//c='\r';	/* 1.12a PC - pdd */
		}
		tmp[ct++]=c;
	}
	while(1);
	tmp[ct]=0;
	t=MemAlloc(ct+1);
	memcpy(t,tmp,ct+1);
	return(t);
}

//void LoadDatabase(FILE *f, int loud)
void LoadDatabase(int loud)
{
	int ni,na,nw,nr,mc,pr,tr,wl,lt,mn,trm;
	int ct;
	short lo;
	Action *ap;
	Room *rp;
	Item *ip;

	//mios para generar codigo:
	//int idx=0;
	//int los[1000]; //no se cuanto sera el maximo, 
	//char **Itemes;
	
/* Load the header */

	/*if(fscanf(f,"%*d %d %d %d %d %d %d %d %d %d %d %d",
		&ni,&na,&nw,&nr,&mc,&pr,&tr,&wl,&lt,&mn,&trm,&ct)<10)
		Fatal("Invalid database(bad header)");
			*/
			
			
	//TODO: hay bug leyendo el header, quedan todo corrido uno.

	ni=9; na=14; nw=20; nr=6; mc=9; pr=1; tr=0; wl=3; lt=-1; mn=9; trm=0; ct=2707;	
	
	//printf("11\n");
		
	GameHeader.NumItems=ni;
	Items=(Item *)MemAlloc(sizeof(Item)*(ni+1));
	GameHeader.NumActions=na;
	Actions=(Action *)MemAlloc(sizeof(Action)*(na+1));
	GameHeader.NumWords=nw;
	GameHeader.WordLength=wl;
	Verbs=(char **)MemAlloc(sizeof(char *)*(nw+1));
	Nouns=(char **)MemAlloc(sizeof(char *)*(nw+1));
	GameHeader.NumRooms=nr;
	Rooms=(Room *)MemAlloc(sizeof(Room)*(nr+1));
	GameHeader.MaxCarry=mc;
	GameHeader.PlayerRoom=pr;
	GameHeader.Treasures=tr;
	GameHeader.LightTime=lt;
	LightRefill=lt;
	GameHeader.NumMessages=mn;
	Messages=(char **)MemAlloc(sizeof(char *)*(mn+1));
	GameHeader.TreasureRoom=trm;

/* Load the actions */

	//printf("22\n");

	ct=0;
	ap=Actions;
	if(loud)
		printf("Reading %d actions.\n",na);

//		while(ct<na+1)
	//{
		/*
		if(fscanf(f,"%hd %hd %hd %hd %hd %hd %hd %hd",
			&ap->Vocab,
			&ap->Condition[0],
			&ap->Condition[1],
			&ap->Condition[2],
			&ap->Condition[3],
			&ap->Condition[4],
			&ap->Action[0],
			&ap->Action[1])!=8)
		{
			printf("Bad action line (%d)\n",ct);
			exit(1);
		}

		ap++;
		ct++;
		*/
		


/** actions **/		

ap->Vocab=2707; 
ap->Condition[0]=140; 
ap->Condition[1]=180; 
ap->Condition[2]=100; 
ap->Condition[3]=141; 
ap->Condition[4]=44; 
ap->Action[0]=8012; 
ap->Action[1]=150;
 ap++; ct++;
ap->Vocab=2707; 
ap->Condition[0]=141; 
ap->Condition[1]=47; 
ap->Action[0]=300; 
ap++; ct++;
ap->Vocab=1507; 
ap->Condition[0]=140; 
ap->Condition[1]=180; 
ap->Condition[2]=142; 
ap->Action[0]=7855; 
ap++; ct++;
ap->Vocab=450; 
ap->Action[0]=9900; 
ap++; ct++;
ap->Vocab=100; 
ap->Condition[0]=20; 
ap->Condition[1]=124; 
ap->Action[0]=8103; 
ap++; ct++;
ap->Vocab=100; 
ap->Condition[0]=84; 
ap->Condition[1]=194; 
ap->Action[0]=600; 
ap++; ct++;
ap->Vocab=100; 
ap->Condition[0]=100; 
ap->Condition[1]=193; 
ap->Condition[2]=64; 
ap->Action[0]=8100; 
ap++; ct++;
ap->Vocab=908; 
ap->Condition[0]=182; 
ap->Condition[1]=29; 
ap->Action[0]=813; 
ap++; ct++;
ap->Vocab=908; 
ap->Condition[0]=182; 
ap->Condition[1]=28; 
ap->Action[0]=963; 
ap++; ct++;
ap->Vocab=100; 
ap->Condition[0]=64; 
ap->Action[0]=8464; 
ap++; ct++;
ap->Vocab=100; 
ap->Condition[0]=24; 
ap->Action[0]=8614; 
ap++; ct++;
ap->Vocab=50; 
ap->Condition[0]=20; 
ap->Condition[1]=84; 
ap->Condition[2]=194; 
ap->Action[0]=8700; 
ap++; ct++;
ap->Vocab=907; 
ap->Condition[0]=143; 
ap->Action[0]=1050; 
ap++; ct++;
ap->Vocab=100; 
ap->Condition[0]=49; 
ap->Action[0]=1350; 
ap++; ct++;
ap->Vocab=100; 
ap->Condition[0]=40; 
ap->Condition[1]=49; 
ap->Action[0]=1258; 
ap++; ct++;
		
	// }  //fin while
	

	//printf("33\n");
	
	
	ct=0;
	if(loud)
		printf("Reading %d word pairs.\n",nw);

	/*
	while(ct<nw+1)
	{
		Verbs[ct]=ReadString(f);
		Nouns[ct]=ReadString(f);
		ct++;
	}
	*/
	
Verbs[0]="AUT";
Nouns[0]="ANY";
Verbs[1]="GO";
Nouns[1]="NOR";
Verbs[2]="*WAL";
Nouns[2]="SOU";
Verbs[3]="I";
Nouns[3]="EAS";
Verbs[4]="*INV";
Nouns[4]="WES";
Verbs[5]="XXZ";
Nouns[5]="UP";
Verbs[6]="LOO";
Nouns[6]="DOW";
Verbs[7]="*REA";
Nouns[7]="CLO";
Verbs[8]="*EXA";
Nouns[8]="MES";
Verbs[9]="XXA";
Nouns[9]="*FLO";
Verbs[10]="GET";
Nouns[10]="";
Verbs[11]="*CAR";
Nouns[11]="";
Verbs[12]="*WEA";
Nouns[12]="";
Verbs[13]="*DON";
Nouns[13]="";
Verbs[14]="XXB";
Nouns[14]="";
Verbs[15]="XXC";
Nouns[15]="";
Verbs[16]="XXD";
Nouns[16]="";
Verbs[17]="XXE";
Nouns[17]="";
Verbs[18]="DRO";
Nouns[18]="";
Verbs[19]="*PUT";
Nouns[19]="";
Verbs[20]="*HAN";
Nouns[20]="";

	
	ct=0;
	rp=Rooms;
	if(loud)
		printf("Reading %d rooms.\n",nr);
		
		
	//while(ct<nr+1)
	//{
	
		/*
		fscanf(f,"%hd %hd %hd %hd %hd %hd",
			&rp->Exits[0],&rp->Exits[1],&rp->Exits[2],
			&rp->Exits[3],&rp->Exits[4],&rp->Exits[5]);
		rp->Text=ReadString(f);
		ct++;
		rp++;
		*/
		
	//printf("44\n");

		
rp->Text="*Not in game";
ct++; rp++;
rp->Exits[0]=6;
 rp->Exits[1]=3;
 rp->Exits[3]=2;
 rp->Text="*You are standing in a spacious hall, splendidly decorated in red and gold, with glittering chandeliers overhead.  The entrance from the street is to the north, and there are doorways south and west.";
ct++; rp++;
rp->Exits[2]=1;
 rp->Text="*You are in a cloak room. The walls of this small room were clearly once lined with hooks, though now only one remains. The exit is a door to the east."; 
 ct++; rp++;
rp->Exits[0]=1;
 rp->Exits[1]=4;
 rp->Exits[2]=4;
 rp->Exits[3]=4;
 rp->Exits[4]=4;
 rp->Exits[5]=4;
 rp->Text="*It is too dark to see";
ct++; rp++;
rp->Exits[0]=1;
 rp->Exits[1]=4;
 rp->Exits[2]=4;
 rp->Exits[3]=4;
 rp->Exits[4]=4;
 rp->Exits[5]=4;
 rp->Text="*You really shouldn't be messing around in the dark";
ct++; rp++;
rp->Exits[0]=1;
 rp->Text="*You are in a bar. The bar, much rougher than you expected after the opulence of the northern foyer, is completely empty. There seems to be some sort of message scrawled in the sawdust on the floor.";
ct++; rp++;
rp->Exits[1]=1;
 rp->Text="*You're in the rain on a street";
ct++; rp++;
		
		
	// }  //fin while
	
	
		//printf("55\n");

		
	ct=0;
	if(loud)
		printf("Reading %d messages.\n",mn);
	
	/*while(ct<mn+1)
	{
		Messages[ct]=ReadString(f);
		ct++;
	}
	*/
	
Messages[0]="";
Messages[1]="You delicately hang the inky cloak on the small brass hook.  You feel... lighter, somehow.";
Messages[2]="This isn't the best place to leave a smart cloak like that lying around!";
Messages[3]="You've only just arrived, and besides, the weather outside seems to be getting worse.";
Messages[4]="You could make quite a mess this way!";
Messages[5]="The message, neatly marked in the sawdust, reads: YOU HAVE WON.";
Messages[6]="The message has been carelessly trampled, making it difficult to read. You can just distinguish the words: YOU HAVE LOST.";
Messages[7]="A handsome cloak, of velvet trimmed with satin, and slightly spattered with raindrops. Its blackness is so deep that it almost seems to suck light from the room.";
Messages[8]="Hurrying through the rainswept November night, you're glad to see the bright lights of the Opera House. It's surprising that there aren't more people about but, hey, what do you expect in a cheap demo game...?";
Messages[9]="Cloak of Darkness adopted for Scott Adams interpreter by Sam Trenholme September 2-3, 2007; tested with ScottFree and scott2zip, cc65 a8";
	
	
	
		//printf("66\n");
	
	
	ct=0;
	if(loud)
		printf("Reading %d items.\n",ni);
	ip=Items;
	

	//Itemes=(char **)MemAlloc(sizeof(char *)*(ni+1));

	
	while(ct<ni+1)
	{
		//ip->Text=ReadString(f);
		//here:
		
if (ct==0) {
   ip->Text="X";
   lo=0;
 }
if (ct==1) {
   ip->Text="N";
   lo=0;
 }
if (ct==2) {
   ip->Text="S";
   lo=0;
 }
if (ct==3) {
   ip->Text="E";
   lo=0;
 }
if (ct==4) {
   ip->Text="W";
   lo=0;
 }
if (ct==5) {
   ip->Text="U";
   lo=0;
 }
if (ct==6) {
   ip->Text="D";
   lo=0;
 }
if (ct==7) {
   ip->Text="Dark cloak";
   lo=-1;
 }
if (ct==8) {
   ip->Text="Hook";
   lo=2;
 }
if (ct==9) {
   ip->Text="Message on the floor";
   lo=0;
 }		
		
		
		ip->AutoGet=strchr(ip->Text,'/');
		/* Some games use // to mean no auto get/drop word! */
		if(ip->AutoGet && strcmp(ip->AutoGet,"//") && strcmp(ip->AutoGet,"/*"))
		{
			char *t;
			*ip->AutoGet++=0;
			t=strchr(ip->AutoGet,'/');
			if(t!=NULL)
				*t=0;
		}
		//fscanf(f,"%hd",&lo);




		
		ip->Location=(unsigned char)lo;
		ip->InitialLoc=ip->Location;
		ip++;
		ct++;
	}


		//printf("77\n");


	ct=0;
	/* Discard Comment Strings */
	while(ct<na+1)
	{
		//free(ReadString(f));
		ct++;
	}
	
	//fscanf(f,"%d",&ct);
	ct=1;

	if(loud)
		printf("Version %d.%02d of Adventure ", ct/100,ct%100);
		
	//fscanf(f,"%d",&ct);
	ct=69;
	
	if(loud)
		printf("%d.\nLoad Complete.\n\n",ct);
		
    //printf("fin.");

/*	
         for(;;){
               //printf("press any key\n");
               //(void) cgetc();
        }
	*/
	
	//exit(0);
	
}

int OutputPos=0;

void OutReset()
{
	OutputPos=0;
	//window(1,TopHeight+1,Width,TopHeight+BottomHeight);
	//gotoxy(1,BottomHeight);
	//clreol();
}

void OutBuf(char *buffer)
{
	char word[80];
	int wp;

	//window(1,TopHeight+1,Width,TopHeight+BottomHeight);
	//textbackground(BOTCOL);

	while(*buffer)
	{
		if(OutputPos==0)
		{
			while(*buffer && isspace(*buffer))
			{
				if(*buffer=='\n')
				{
					//gotoxy(1,BottomHeight);
					//cprintf("\n\r");
					
					//printf("\n\r");
					printf("\n");
					
					//clreol();
					OutputPos=0;
				}
				buffer++;
			}
		}
		if(*buffer==0)
			return;
		wp=0;
		while(*buffer && !isspace(*buffer))
		{
			word[wp++]=*buffer++;
		}
		word[wp]=0;
/*		fprintf(stderr,"Word '%s' at %d\n",word,OutputPos);*/
		if(OutputPos+strlen(word)>(Width-2))
		{
			//gotoxy(1,BottomHeight);
			//cprintf("\r\n");

			//printf("\r\n");
			printf("\n");

			//clreol();
			OutputPos=0;
		}
		//else {
			//gotoxy(OutputPos+1,BottomHeight);
		//}
		
		//cprintf("%s",word);
		printf("%s",word);
		
		OutputPos+=strlen(word);

		if(*buffer==0)
			return;

		if(*buffer=='\n')
		{
			//gotoxy(1,BottomHeight);
			//printf("\n\r");
			printf("\n");
			//clreol();
			OutputPos=0;
		}
		else
		{
			OutputPos++;
			if(OutputPos<(Width-1)){
				//cprintf(" ");
				printf(" ");
			}
		}
		buffer++;
	}
}



void Output(char *a)
{
	char block[512];
	strcpy(block,a);
	OutBuf(block);
}

void OutputNumber(int a)
{
	char buf[16];
	sprintf(buf,"%d",a);
	OutBuf(buf);
}
		
void Look()
{
	static char *ExitNames[6]=
	{
		"North","South","East","West","Up","Down"
	};
	Room *r;
	int ct,f;
	int pos;

	//window(1,1,Width,TopHeight);
	//textbackground(TOPCOL);
	//clrscr();

	if((BitFlags&(1L<<DARKBIT)) && Items[LIGHT_SOURCE].Location!= CARRIED
		    && Items[LIGHT_SOURCE].Location!= MyLoc)
	{
		if(Options&YOUARE){
			//cprintf("You can't see. It is too dark!\n\r");
			//printf("You can't see. It is too dark!\n\r");
			printf("You can't see. It is too dark!\n");
		}
		else{
			//cprintf("I can't see. It is too dark!\n\r");
			//printf("I can't see. It is too dark!\n\r");
			printf("I can't see. It is too dark!\n");
		}
		if (Options & TRS80_STYLE){
			//cprintf(TRS80_LINE);
			printf(TRS80_LINE);
		}
		return;
	}
	r=&Rooms[MyLoc];
	if(*r->Text=='*'){
		//cprintf("%s\n\r",r->Text+1);
		//printf("%s\n\r",r->Text+1);
		printf("%s\n",r->Text+1);
	}
	else
	{
		if(Options&YOUARE){
			//cprintf("You are %s\n\r",r->Text);
			//printf("You are %s\n\r",r->Text);
			printf("You are %s\n",r->Text);
		}
		else{
			//cprintf("I'm in a %s\n",r->Text);
			printf("I'm in a %s\n",r->Text);
		}
	}
	ct=0;
	f=0;
	//cprintf("\n\rObvious exits: ");
	//printf("\n\rObvious exits: ");
	printf("\nObvious exits: ");
	while(ct<6)
	{
		if(r->Exits[ct]!=0)
		{
			if(f==0)
				f=1;
			else{
				//cprintf(", ");
				printf(", ");
			}
			//cprintf("%s",ExitNames[ct]);
			printf("%s",ExitNames[ct]);
		}
		ct++;
	}
	if(f==0){
		//cprintf("none");
		printf("none");
	}
	//cprintf(".\n\r");
	//printf(".\n\r");
	printf(".\n");
	ct=0;
	f=0;
	pos=0;
	while(ct<=GameHeader.NumItems)
	{
		if(Items[ct].Location==MyLoc)
		{
			if(f==0)
			{
				if(Options&YOUARE){
					//cprintf("\r\nYou can also see: ");
					//printf("\r\nYou can also see: ");
					printf("\nYou can also see: ");
				}
				else{
					//cprintf("\r\nI can also see: ");
					//printf("\r\nI can also see: ");
					printf("\nI can also see: ");
				}
				pos=16;
				f++;
			}
			else if (!(Options & TRS80_STYLE))
			{
				//cprintf(" - ");
				printf(" - ");
				pos+=3;
			}
			if(pos+strlen(Items[ct].Text)>(Width-10))
			{
				pos=0;
				//cprintf("\n\r");
				//printf("\n\r");
				printf("\n");
			}
			//cprintf("%s",Items[ct].Text);
			printf("%s",Items[ct].Text);
			pos += strlen(Items[ct].Text);
			if (Options & TRS80_STYLE)
			{
				//cprintf(". ");
				printf(". ");
				pos+=2;
			}
		}
		ct++;
	}
	//cprintf("\n\r");
	//printf("\n\r");
	printf("\n");
	if (Options & TRS80_STYLE){
		//cprintf(TRS80_LINE);
		printf(TRS80_LINE);
	}
}

int WhichWord(char *word, char **list)
{
	int n=1;
	int ne=1;
	char *tp;
	while(ne<=GameHeader.NumWords)
	{
		tp=list[ne];
		if(*tp=='*')
			tp++;
		else
			n=ne;
		//if(strnicmp(word,tp,GameHeader.WordLength)==0)
		if(strncmp(word,tp,GameHeader.WordLength)==0)
			return(n);
		ne++;
	}
	return(-1);
}

void LineInput(char *buf)
{
	int pos=0;
	int ch;

	//window(1,TopHeight+1,Width,TopHeight+BottomHeight);
	//gotoxy(1+OutputPos,BottomHeight);

	while(1)
	{
		//ch=getch();
		ch=cgetc();
		
		//para ver el ascii de input.
		//printf("i1 %d\n", ch);
		
		switch(ch)
		{
			//case 10:;
			//case 13:;
			case 155:;
				buf[pos]=0;
				//cprintf("\r\n");
				//printf("\r\n");
				printf("\n");
				return;
			case 8:;
			case 126:;
				if(pos>0)
				{
					//cprintf("\010");
					printf("\176");
					pos--;
				}
				break;
			default:
				if(ch>=' '&&ch<=125)
				{
					buf[pos++]=ch;
					//cprintf("%c",(char)ch);
					printf("%c",(char)ch);
				}
				break;
		}
	}
}

void GetInput(vb,no)
int *vb,*no;
{
	char buf[256];
	char verb[10],noun[10];
	int vc,nc;
	int num;
	do
	{
		do
		{
			printf("free mem=%d \n\n", _heapmemavail() );

			Output("\nTell me what to do ? ");
			LineInput(buf);
			OutReset();
			num=sscanf(buf,"%9s %9s",verb,noun);
		}
		while(num==0||*buf=='\n');
		if(num==1)
			*noun=0;
		if(*noun==0 && strlen(verb)==1)
		{
			switch(isupper(*verb)?tolower(*verb):*verb)
			{
				case 'n':strcpy(verb,"NORTH");break;
				case 'e':strcpy(verb,"EAST");break;
				case 's':strcpy(verb,"SOUTH");break;
				case 'w':strcpy(verb,"WEST");break;
				case 'u':strcpy(verb,"UP");break;
				case 'd':strcpy(verb,"DOWN");break;
				/* Brian Howarth interpreter also supports this */
				case 'i':strcpy(verb,"INVENTORY");break;
			}
		}
		nc=WhichWord(verb,Nouns);
		/* The Scott Adams system has a hack to avoid typing 'go' */
		if(nc>=1 && nc <=6)
		{
			vc=1;
		}
		else
		{
			vc=WhichWord(verb,Verbs);
			nc=WhichWord(noun,Nouns);
		}
		*vb=vc;
		*no=nc;
		if(vc==-1)
		{
			Output("\"");
			Output(verb);
			Output("\" is a word I don't know...sorry!\n");
		}
	}
	while(vc==-1);
	strcpy(NounText,noun);	/* Needed by GET/DROP hack */
}

/*
void SaveGame()
{
	char buf[256];
	int ct;
	FILE *f;
	Output("Filename: ");
	LineInput(buf);
	Output("\n");
	f=fopen(buf,"w");
	if(f==NULL)
	{
		Output("Unable to create save file.\n");
		return;
	}
	for(ct=0;ct<16;ct++)
	{
		fprintf(f,"%d %d\n",Counters[ct],RoomSaved[ct]);
	}
	fprintf(f,"%ld %d %hd %d %d %hd\n",BitFlags, (BitFlags&(1L<<DARKBIT))?1:0,
		MyLoc,CurrentCounter,SavedRoom,GameHeader.LightTime);
	for(ct=0;ct<=GameHeader.NumItems;ct++)
		fprintf(f,"%hd\n",(short)Items[ct].Location);
	fclose(f);
	Output("Saved.\n");
}

void LoadGame(char *name)
{
	FILE *f=fopen(name,"r");
	int ct=0;
	short lo;
	short DarkFlag;
	if(f==NULL)
	{
		Output("Unable to restore game.");
		return;
	}
	for(ct=0;ct<16;ct++)
	{
		fscanf(f,"%d %d\n",&Counters[ct],&RoomSaved[ct]);
	}
	fscanf(f,"%ld %d %hd %d %d %hd\n",
		&BitFlags,&DarkFlag,&MyLoc,&CurrentCounter,&SavedRoom,
		&GameHeader.LightTime);
	// Backward compatibility 
	if(DarkFlag)
		BitFlags|=(1L<<15);
	for(ct=0;ct<=GameHeader.NumItems;ct++)
	{
		fscanf(f,"%hd\n",&lo);
		Items[ct].Location=(unsigned char)lo;
	}
	fclose(f);
}
*/

int PerformLine(int ct)
{
	int continuation=0;
	int param[5],pptr=0;
	int act[4];
	int cc=0;
	while(cc<5)
	{
		int cv,dv;
		cv=Actions[ct].Condition[cc];
		dv=cv/20;
		cv%=20;
		switch(cv)
		{
			case 0:
				param[pptr++]=dv;
				break;
			case 1:
				if(Items[dv].Location!=CARRIED)
					return(0);
				break;
			case 2:
				if(Items[dv].Location!=MyLoc)
					return(0);
				break;
			case 3:
				if(Items[dv].Location!=CARRIED&&
					Items[dv].Location!=MyLoc)
					return(0);
				break;
			case 4:
				if(MyLoc!=dv)
					return(0);
				break;
			case 5:
				if(Items[dv].Location==MyLoc)
					return(0);
				break;
			case 6:
				if(Items[dv].Location==CARRIED)
					return(0);
				break;
			case 7:
				if(MyLoc==dv)
					return(0);
				break;
			case 8:
				if((BitFlags&(1L<<dv))==0)
					return(0);
				break;
			case 9:
				if(BitFlags&(1L<<dv))
					return(0);
				break;
			case 10:
				if(CountCarried()==0)
					return(0);
				break;
			case 11:
				if(CountCarried())
					return(0);
				break;
			case 12:
				if(Items[dv].Location==CARRIED||Items[dv].Location==MyLoc)
					return(0);
				break;
			case 13:
				if(Items[dv].Location==0)
					return(0);
				break;
			case 14:
				if(Items[dv].Location)
					return(0);
				break;
			case 15:
				if(CurrentCounter>dv)
					return(0);
				break;
			case 16:
				if(CurrentCounter<=dv)
					return(0);
				break;
			case 17:
				if(Items[dv].Location!=Items[dv].InitialLoc)
					return(0);
				break;
			case 18:
				if(Items[dv].Location==Items[dv].InitialLoc)
					return(0);
				break;
			case 19:/* Only seen in Brian Howarth games so far */
				if(CurrentCounter!=dv)
					return(0);
				break;
		}
		cc++;
	}
	/* Actions */
	act[0]=Actions[ct].Action[0];
	act[2]=Actions[ct].Action[1];
	act[1]=act[0]%150;
	act[3]=act[2]%150;
	act[0]/=150;
	act[2]/=150;
	cc=0;
	pptr=0;
	while(cc<4)
	{
		if(act[cc]>=1 && act[cc]<52)
		{
			Output(Messages[act[cc]]);
			Output("\n");
		}
		else if(act[cc]>101)
		{
			Output(Messages[act[cc]-50]);
			Output("\n");
		}
		else switch(act[cc])
		{
			case 0:/* NOP */
				break;
			case 52:
				if(CountCarried()==GameHeader.MaxCarry)
				{
					if(Options&YOUARE)
						Output("You are carrying too much.\n");
					else
						Output("I've too much to carry!\n");
					break;
				}
				if(Items[param[pptr]].Location==MyLoc)
					Redraw=1;
				Items[param[pptr++]].Location= CARRIED;
				break;
			case 53:
				Redraw=1;
				Items[param[pptr++]].Location=MyLoc;
				break;
			case 54:
				Redraw=1;
				MyLoc=param[pptr++];
				break;
			case 55:
				if(Items[param[pptr]].Location==MyLoc)
					Redraw=1;
				Items[param[pptr++]].Location=0;
				break;
			case 56:
				BitFlags|=1L<<DARKBIT;
				break;
			case 57:
				BitFlags&=~(1L<<DARKBIT);
				break;
			case 58:
				BitFlags|=(1L<<param[pptr++]);
				break;
			case 59:
				if(Items[param[pptr]].Location==MyLoc)
					Redraw=1;
				Items[param[pptr++]].Location=0;
				break;
			case 60:
				BitFlags&=~(1L<<param[pptr++]);
				break;
			case 61:
				if(Options&YOUARE)
					Output("You are dead.\n");
				else
					Output("I am dead.\n");
				BitFlags&=~(1L<<DARKBIT);
				MyLoc=GameHeader.NumRooms;/* It seems to be what the code says! */
				Look();
				break;
			case 62:
			{
				/* Bug fix for some systems - before it could get parameters wrong */
				int i=param[pptr++];
				Items[i].Location=param[pptr++];
				Redraw=1;
				break;
			}
			case 63:
doneit:				Fatal("The game is now over");
				sleep(5);
				
         for(;;){
               //printf("press any key\n");
               //(void) cgetc();
        }
				
				exit(0);
			case 64:
				Look();
				break;
			case 65:
			{
				int ct=0;
				int n=0;
				while(ct<=GameHeader.NumItems)
				{
					if(Items[ct].Location==GameHeader.TreasureRoom &&
					  *Items[ct].Text=='*')
						n++;
					ct++;
				}
				if(Options&YOUARE)
					Output("You have stored ");
				else
					Output("I've stored ");
				OutputNumber(n);
				Output(" treasures.  On a scale of 0 to 100, that rates ");
				OutputNumber((n*100)/GameHeader.Treasures);
				Output(".\n");
				if(n==GameHeader.Treasures)
				{
					Output("Well done.\n");
					goto doneit;
				}
				break;
			}
			case 66:
			{
				int ct=0;
				int f=0;
				if(Options&YOUARE)
					Output("You are carrying:\n");
				else
					Output("I'm carrying:\n");
				while(ct<=GameHeader.NumItems)
				{
					if(Items[ct].Location==CARRIED)
					{
						if(f==1)
						{
							if (Options & TRS80_STYLE)
								Output(". ");
							else
								Output(" - ");
						}
						f=1;
						Output(Items[ct].Text);
					}
					ct++;
				}
				if(f==0)
					Output("Nothing");
				Output(".\n");
				break;
			}
			case 67:
				BitFlags|=(1L<<0);
				break;
			case 68:
				BitFlags&=~(1L<<0);
				break;
			case 69:
				GameHeader.LightTime=LightRefill;
				if(Items[LIGHT_SOURCE].Location==MyLoc)
					Redraw=1;
				Items[LIGHT_SOURCE].Location=CARRIED;
				BitFlags&=~(1L<<LIGHTOUTBIT);
				break;
			case 70:
				ClearScreen(); /* pdd. */
				OutReset();
				break;
			case 71:
				//SaveGame();
				break;
			case 72:
			{
				int i1=param[pptr++];
				int i2=param[pptr++];
				int t=Items[i1].Location;
				if(t==MyLoc || Items[i2].Location==MyLoc)
					Redraw=1;
				Items[i1].Location=Items[i2].Location;
				Items[i2].Location=t;
				break;
			}
			case 73:
				continuation=1;
				break;
			case 74:
				if(Items[param[pptr]].Location==MyLoc)
					Redraw=1;
				Items[param[pptr++]].Location= CARRIED;
				break;
			case 75:
			{
				int i1,i2;
				i1=param[pptr++];
				i2=param[pptr++];
				if(Items[i1].Location==MyLoc)
					Redraw=1;
				Items[i1].Location=Items[i2].Location;
				if(Items[i2].Location==MyLoc)
					Redraw=1;
				break;
			}
			case 76:	/* Looking at adventure .. */
				Look();
				break;
			case 77:
				if(CurrentCounter>=0)
					CurrentCounter--;
				break;
			case 78:
				OutputNumber(CurrentCounter);
				break;
			case 79:
				CurrentCounter=param[pptr++];
				break;
			case 80:
			{
				int t=MyLoc;
				MyLoc=SavedRoom;
				SavedRoom=t;
				Redraw=1;
				break;
			}
			case 81:
			{
				/* This is somewhat guessed. Claymorgue always
				   seems to do select counter n, thing, select counter n,
				   but uses one value that always seems to exist. Trying
				   a few options I found this gave sane results on ageing */
				int t=param[pptr++];
				int c1=CurrentCounter;
				CurrentCounter=Counters[t];
				Counters[t]=c1;
				break;
			}
			case 82:
				CurrentCounter+=param[pptr++];
				break;
			case 83:
				CurrentCounter-=param[pptr++];
				if(CurrentCounter< -1)
					CurrentCounter= -1;
				/* Note: This seems to be needed. I don't yet
				   know if there is a maximum value to limit too */
				break;
			case 84:
				Output(NounText);
				break;
			case 85:
				Output(NounText);
				Output("\n");
				break;
			case 86:
				Output("\n");
				break;
			case 87:
			{
				/* Changed this to swap location<->roomflag[x]
				   not roomflag 0 and x */
				int p=param[pptr++];
				int sr=MyLoc;
				MyLoc=RoomSaved[p];
				RoomSaved[p]=sr;
				Redraw=1;
				break;
			}
			case 88:
				sleep(2);	/* DOC's say 2 seconds. Spectrum times at 1.5 */
				break;
			case 89:
				pptr++;
				/* SAGA draw picture n */
				/* Spectrum Seas of Blood - start combat ? */
				/* Poking this into older spectrum games causes a crash */
				break;
			default:
				fprintf(stderr,"Unknown action %d [Param begins %d %d]\n",
					act[cc],param[pptr],param[pptr+1]);
				break;
		}
		cc++;
	}
	return(1+continuation);
}


int PerformActions(int vb,int no)
{
	static int disable_sysfunc=0;	/* Recursion lock */
	int d=BitFlags&(1L<<DARKBIT);
	
	int ct=0;
	int fl;
	int doagain=0;
	if(vb==1 && no == -1 )
	{
		Output("Give me a direction too.\n");
		return(0);
	}
	if(vb==1 && no>=1 && no<=6)
	{
		int nl;
		if(Items[LIGHT_SOURCE].Location==MyLoc ||
		   Items[LIGHT_SOURCE].Location==CARRIED)
		   	d=0;
		if(d)
			Output("Dangerous to move in the dark!\n");
		nl=Rooms[MyLoc].Exits[no-1];
		if(nl!=0)
		{
			MyLoc=nl;
			Output("O.K.\n");
			Look();
			return(0);
		}
		if(d)
		{
			if(Options&YOUARE)
				Output("You fell down and broke your neck.\n");
			else
				Output("I fell down and broke my neck.\n");
			sleep(5);
			
         for(;;){
               //printf("press any key\n");
               //(void) cgetc();
        }
			
			exit(0);
		}
		if(Options&YOUARE)
			Output("You can't go in that direction.\n");
		else
			Output("I can't go in that direction.\n");
		return(0);
	}
	fl= -1;
	while(ct<=GameHeader.NumActions)
	{
		int vv,nv;
		vv=Actions[ct].Vocab;
		/* Think this is now right. If a line we run has an action73
		   run all following lines with vocab of 0,0 */
		if(vb!=0 && (doagain&&vv!=0))
			break;
		/* Oops.. added this minor cockup fix 1.11 */
		if(vb!=0 && !doagain && fl== 0)
			break;
		nv=vv%150;
		vv/=150;
		if((vv==vb)||(doagain&&Actions[ct].Vocab==0))
		{
			if((vv==0 && RandomPercent(nv))||doagain||
				(vv!=0 && (nv==no||nv==0)))
			{
				int f2;
				if(fl== -1)
					fl= -2;
				if((f2=PerformLine(ct))>0)
				{
					/* ahah finally figured it out ! */
					fl=0;
					if(f2==2)
						doagain=1;
					if(vb!=0 && doagain==0)
						return(0);
				}
			}
		}
		ct++;
		if(Actions[ct].Vocab!=0)
			doagain=0;
	}
	if(fl!=0 && disable_sysfunc==0)
	{
		int i;
		if(Items[LIGHT_SOURCE].Location==MyLoc ||
		   Items[LIGHT_SOURCE].Location==CARRIED)
		   	d=0;
		if(vb==10 || vb==18)
		{
			/* Yes they really _are_ hardcoded values */
			if(vb==10)
			{
				//if(stricmp(NounText,"ALL")==0)
				if(strcmp(NounText,"ALL")==0)
				{
					int ct=0;
					int f=0;
					
					if(d)
					{
						Output("It is dark.\n");
						return 0;
					}
					while(ct<=GameHeader.NumItems)
					{
						if(Items[ct].Location==MyLoc && Items[ct].AutoGet!=NULL && Items[ct].AutoGet[0]!='*')
						{
							no=WhichWord(Items[ct].AutoGet,Nouns);
							disable_sysfunc=1;	/* Don't recurse into auto get ! */
							PerformActions(vb,no);	/* Recursively check each items table code */
							disable_sysfunc=0;
							if(CountCarried()==GameHeader.MaxCarry)
							{
								if(Options&YOUARE)
									Output("You are carrying too much.\n");
								else
									Output("I've too much to carry.\n");
								return(0);
							}
							Items[ct].Location= CARRIED;
							Redraw=1;
							OutBuf(Items[ct].Text);
							Output(": O.K.\n");
							f=1;
						}
						ct++;
					}
					if(f==0)
						Output("Nothing taken.\n");
					return(0);
				}
				if(no==-1)
				{
					Output("What ?\n");
					return(0);
				}
				if(CountCarried()==GameHeader.MaxCarry)
				{
					if(Options&YOUARE)
						Output("You are carrying too much.\n");
					else
						Output("I've too much to carry.\n");
					return(0);
				}
				i=MatchUpItem(NounText,MyLoc);
				if(i==-1)
				{
					if(Options&YOUARE)
						Output("It is beyond your power to do that.\n");
					else
						Output("It's beyond my power to do that.\n");
					return(0);
				}
				Items[i].Location= CARRIED;
				Output("O.K.\n");
				Redraw=1;
				return(0);
			}
			if(vb==18)
			{
				//if(stricmp(NounText,"ALL")==0)
				if(strcmp(NounText,"ALL")==0)
				{
					int ct=0;
					int f=0;
					while(ct<=GameHeader.NumItems)
					{
						if(Items[ct].Location==CARRIED && Items[ct].AutoGet && Items[ct].AutoGet[0]!='*')
						{
							no=WhichWord(Items[ct].AutoGet,Nouns);
							disable_sysfunc=1;
							PerformActions(vb,no);
							disable_sysfunc=0;
							Items[ct].Location=MyLoc;
							OutBuf(Items[ct].Text);
							Output(": O.K.\n");
							Redraw=1;
							f=1;
						}
						ct++;
					}
					if(f==0)
						Output("Nothing dropped.\n");
					return(0);
				}
				if(no==-1)
				{
					Output("What ?\n");
					return(0);
				}
				i=MatchUpItem(NounText,CARRIED);
				if(i==-1)
				{
					if(Options&YOUARE)
						Output("It's beyond your power to do that.\n");
					else
						Output("It's beyond my power to do that.\n");
					return(0);
				}
				Items[i].Location=MyLoc;
				Output("O.K.\n");
				Redraw=1;
				return(0);
			}
		}
	}
	return(fl);
}
	
//void main(int argc, char *argv[])
void main()
{
	//FILE *f;
	int vb,no;
	
	/*
	while(argv[1])
	{
		if(*argv[1]!='-')
			break;
		switch(argv[1][1])
		{
			case 'y':
				Options|=YOUARE;
				break;
			case 'i':
				Options&=~YOUARE;
				break;
			case 'd':
				Options|=DEBUGGING;
				break;
			case 's':
				Options|=SCOTTLIGHT;
				break;
			case 't':
				Options|=TRS80_STYLE;
				break;
			case 'p':
				Options|=PREHISTORIC_LAMP;
				break;
			case 'h':
			default:
				fprintf(stderr,"%s: [-h] [-y] [-s] [-i] [-t] [-d] [-p] <gamename> [savedgame].\n",
						argv[0]);
				exit(1);
		}
		if(argv[1][2]!=0)
		{
			fprintf(stderr,"%s: option -%c does not take a parameter.\n",
				argv[0],argv[1][1]);
			exit(1);
		}
		argv++;
		argc--;
	}

	if(argc!=2 && argc!=3)
	{
		fprintf(stderr,"%s <database> <savefile>.\n",argv[0]);
		exit(1);
	}
	f=fopen(argv[1],"r");
	if(f==NULL)
	{
		perror(argv[1]);
		exit(1);
	}

	if (Options & TRS80_STYLE)
	{
		Width = 64;
		TopHeight = 11;
		BottomHeight = 13;
	}
	else
	{
		Width = 80;
		TopHeight = 10;
		BottomHeight = 15;
	}
	*/
	
	//f=fopen("S","r");

	Width = 40;
	TopHeight = 11;
	BottomHeight = 13;
	
	DisplayUp=1;
	OutReset();
	//textbackground(BOTCOL);
	//clrscr();
	OutBuf("\
Scott Free, A Scott Adams game driver in C.\n\
Release 1.14b (PC), (c) 1993,1994,1995 Swansea University Computer Society.\n\
Distributed under the GNU software license\n\n");

	//LoadDatabase(f,(Options&DEBUGGING)?1:0);
	LoadDatabase((Options&DEBUGGING)?1:0);
	
	//fclose(f);
	//if(argc==3) 	LoadGame(argv[2]);
	
	srand(time(NULL));
	Look();
	while(1)
	{
		if(Redraw!=0)
		{
			Look();
			Redraw=0;
		}
		PerformActions(0,0);
		if(Redraw!=0)
		{
			Look();
			Redraw=0;
		}
		GetInput(&vb,&no);
		switch(PerformActions(vb,no))
		{
			case -1:Output("I don't understand your command.\n");
				break;
			case -2:Output("I can't do that yet.\n");
				break;
		}
		/* Brian Howarth games seem to use -1 for forever */
		if(Items[LIGHT_SOURCE].Location/*==-1*/!=DESTROYED && GameHeader.LightTime!= -1)
		{
			GameHeader.LightTime--;
			if(GameHeader.LightTime<1)
			{
				BitFlags|=(1L<<LIGHTOUTBIT);
				if(Items[LIGHT_SOURCE].Location==CARRIED ||
					Items[LIGHT_SOURCE].Location==MyLoc)
				{
					if(Options&SCOTTLIGHT)
						Output("Light has run out! ");
					else
						Output("Your light has run out. ");
				}
				if(Options&PREHISTORIC_LAMP)
					Items[LIGHT_SOURCE].Location=DESTROYED;
			}
			else if(GameHeader.LightTime<25)
			{
				if(Items[LIGHT_SOURCE].Location==CARRIED ||
					Items[LIGHT_SOURCE].Location==MyLoc)
				{
			
					if(Options&SCOTTLIGHT)
					{
						Output("Light runs out in ");
						OutputNumber(GameHeader.LightTime);
						Output(" turns. ");
					}
					else
					{
						if(GameHeader.LightTime%5==0)
							Output("Your light is growing dim. ");
					}
				}
			}
		}
	}
}
