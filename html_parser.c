#include<stdio.h>
#include<stdlib.h>

#include<string.h>

int scripstyle(FILE* ptr){
char c[10],ch;
int i=0;
while(1)
{ 
 ch=fgetc(ptr);
if(ch=='>')
{       c[i]='\0';
        if(strcmp("script",c)==0||strcmp("style",c)==0)
		return 1;
	else 
		return 0;
}
 c[i++]=ch;
}
return 0;
}

void findword(FILE* ptr,int k){
 int i=0;
 char c[100],ch;
 int afl=0;
 c[i++]=k;
  while(1){
		  if((ch=fgetc(ptr))=='>'){
		     c[i]='\0';
		     int flag=0; 	
                     if(strcmp("script",c)==0||strcmp("style",c)==0){
			     while(1){
				  ch=fgetc(ptr);
				 if(ch=='<'&&(ch=fgetc(ptr))=='/')
		                if(scripstyle(ptr))
				       return ;	
			     }
		     }
		  return;		  }
        if(ch==' '||ch=='\n') afl=1;
	if(!afl)
	  c[i++]=ch;
  }
}
void fn1(){
FILE* ptr=fopen("received_file.txt","r");
FILE* fp=fopen("Words.txt","w");
int ch;
int flag=0;
int linenum=1;
 do{
	 ch=fgetc(ptr);
	 if(ch=='<'){
            ch=fgetc(ptr);
	    if(ch=='!')
	    {
		    while(1){
                       if((ch=fgetc(ptr))=='-'||ch=='D')
			       if((ch=fgetc(ptr))=='-'||ch=='O')
				       if((ch=fgetc(ptr))=='>'||ch=='C')
					       break;
		    }
			if(ch=='>')
			continue;
			else{
				while(ch!='>') ch=fgetc(ptr);
			}
	    }
	    else
		 findword(ptr,ch);
	 }
	 else{
		        if(ch=='\n'&&!flag)
		 	 fputc(ch,fp);
			  else if(ch!='\n'){
			  fputc(ch,fp);
			  flag=0;
	                    }
	                 else{
	                  flag=1;
	                  continue;
	                   }
	 }
 }while(ch!=EOF);
fclose(fp);
fclose(ptr);
FILE *fpt=fopen("Words.txt","r");
FILE *ptrr=fopen("Revised_word.txt","w");
int chr;

do 
{
chr=fgetc(fpt);
if(chr>=32&&chr<=126){
if(chr=='.'||chr==','||chr=='('||chr=='['||chr=='{'||chr==')'||chr==']'||chr=='}'||chr==':'||chr==';')
chr='\n';
fputc(chr,ptrr);
}
if(chr=='\n')
fputc(' ',ptrr);
if(chr==EOF)
fputc(chr,ptrr);
}while((chr!=EOF));

fclose(fpt);
fclose(ptrr);
}


