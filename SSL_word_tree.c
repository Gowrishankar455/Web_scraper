#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
struct tree{
	char *word;
	int count;
	struct tree* left;
	struct tree* right;
};
void kstrlwr(char ch[],int i)
{
 for(int k=0;k<i;k++)
 if(ch[k]>='A'&&ch[k]<='Z')
 ch[k]=ch[k]+32;
 
}

char *mstrdup(char *s)
{
    char *p;
    p = (char *) malloc(strlen(s) + 1);
    if(p != NULL)
        strcpy(p, s);
    return p;
}

struct tree* talloc(char* ch){
struct tree* temp=(struct tree*)malloc(sizeof(struct tree));
	  temp->word=mstrdup(ch);
	  temp->count=1;
	  temp->left=NULL;
	  temp->right=NULL;
	  return temp;
	
}
struct tree*  addword(char* ch,struct tree* root)
{
int cmp;
  if(root==NULL){
	  root= talloc(ch);
	  return root;
  }
  else if((cmp=strcmp(ch,root->word))<0)
	  root->left=addword(ch,root->left);
  else if(cmp>0)
	  root->right=addword(ch,root->right);
  else if(cmp==0){
	  root->count++;
  }
    
  return root;
}

void rank(int t,struct tree* root,FILE* fp){
if(root==NULL)
return ;
rank(t,root->left,fp);
if(t==root->count){
      fputs(root->word,fp);
	  fputc(' ',fp);
	  fprintf(fp,"%d",t);
	  fputc('\n',fp);
}
rank(t,root->right,fp);
}


void findmax(int *max,struct tree* root)
{
   if(root==NULL)
	   return;
   findmax(max,root->left);
   
   if(root->count>*max){
	   *max=root->count;
	   
	   }
    
   findmax(max,root->right);
}


void maketree(){
struct tree* root=NULL;
FILE *fp=fopen("SSL_Words_revised.txt","r");
if(fp==NULL)
printf("No file exists");
int c;
char ch[30];
int i=0;
while((c=fgetc(fp))!=EOF){

if(c!='\n'&&c!=' ')
	ch[i++]=c;
else if(c=='\n'&&i==0)
continue;
else if(c==' '&&i==0)
continue;
else{
       ch[i]='\0';
  if(strlen(ch)==1||strlen(ch)>30){
     i=0;
     continue;
  }     
  kstrlwr(ch,i);
 root=addword(ch,root);
	i=0;
}
}

fclose(fp);
int max=0;
findmax(&max,root);
fp=fopen("SSL_Result.txt","w");
for(int t=max;t>0;t--){
	rank(t,root,fp);
}
fclose(fp);
}

