#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define MAX 10000
#define PMAX 10000
#define LINE 143

using namespace std;

typedef struct map{
    char str[MAX];
    int id;
}Map;

char BWT[MAX];//BWT
char BWT_reverse[MAX];
char matrix[MAX][MAX];
char Rmatrix[MAX][MAX];
char str[MAX];
char str_reverse[MAX];
char pattern[PMAX];
Map maps[MAX];
char lftitle[MAX];
int *C;
int *Occ;
int lflen;
int len;
int plen;

int D[MAX];
int I[MAX];
FILE *outfile;

int generateCAndOcc(){
    int i,j;
    //int len = strlen(BWT);
    C = (int *)malloc(sizeof(int)*lflen);

    char ch;
    //ch = BWT[i];
    for(i=0;i<lflen;i++){
        *(C+i) = 0;
    }

    for(j=0;j<lflen;j++){
        ch = lftitle[j];
        for(i=0;i<len;i++){
            if(BWT[i]<ch){
                (*(C+j))++;
            }
        }
    }

    Occ = (int *)malloc(sizeof(int)*lflen*(len+1));
    int count;
    for(j=0;j<lflen;j++){
        count=0;
        ch = lftitle[j];
        for(i=0;i<=len;i++){
            *(Occ+j*(len+1)+i) = count;
            if(ch == BWT[i]){
                count++;
            }
        }
    }
    return 0;
}

int LF(int n){

    int j;
    for(j=0;j<lflen;j++){
        if(BWT[n] == lftitle[j]){
            return (*(C+j))+*(Occ+(j*(len+1)+n));
        }
    }

}

int comp(const void *a,const void *B){

    return strcmp((*(Map *)a).str,(*(Map *)B).str);
}

int cmp(const void *a,const void *B){
    return *(char *)a-*(char *)B;
}
//产生轮转矩阵
char * Transform(char str[MAX],char BWT[MAX]){

    int len;
    len = strlen(str);

    printf("目标串长度 = %d\n",len);
    strcpy(matrix[0],str);
    strrev(matrix[0]);

    int i;
    for(i=1;i<len;i++){
        matrix[i][0]=matrix[i-1][len-1];
        strncat(matrix[i],matrix[i-1],len-1);
    }
    for(i=0;i<len;i++){
        strrev(matrix[i]);
    }

    for(i=0;i<len;i++){
        strcpy(maps[i].str,matrix[i]);
        maps[i].id = i;
    }

    qsort(maps,len,sizeof(Map),comp);//对轮转矩阵进行排序

    for(i=0;i<len;i++){
        BWT[i] = maps[i].str[len-1];
    }
    BWT[i] = '\0';
    //printf("BWT = %s\n",BWT);

    return 0;
}
//根据BWT还原出原串
int Reverse(){
    char origin[MAX];

    origin[len-1] = '\0';

    int temp[127]={0};
    int pos=0,i=0;
    for(i=0;i<len;i++){
        if(temp[(int)BWT[i]]==0){
            temp[BWT[i]]++;
            lftitle[pos] = BWT[i];
            pos++;
        }
    }
    lftitle[pos]='\0';

    lflen = strlen(lftitle);

    generateCAndOcc();

    int n=0;
    int k=len-2;
    while(BWT[n]!= '$'){
        origin[k] = BWT[n];
        n = LF(n);
        k--;
    }
    //printf("用BWT还原的原串 ：%s\n",origin);
    return 0;
}

int Search(char pattern[PMAX],int type){

    int pos=0,i=0;
    int len = strlen(BWT);

    //产生lfmapping

    int *lfmapping = (int *)malloc((MAX+1)*lflen*sizeof(int));

    int j;
    for(j=0;j<lflen;j++){
        int number=0;
        for(i=0;i<len+1;i++){
            *(lfmapping+i*lflen+j)=number;
            if(BWT[i]==lftitle[j]){
                number++;
            }
        }
    }

    int plen = strlen(pattern);
    int start,end,count;
    char ch;
    ch = pattern[plen-1];
    count=0;
    for(j=0;j<len;j++){
        if(ch == maps[j].str[0] && count == 0){
                start = j;
                count++;
            }
            if((ch != maps[j].str[0] && count == 1)|| j+1 == len){
                if(j+1 == len){
                    end=len;
                }
                else {
                    end = j;
                }
                j=len;
                count++;
            }
        }

    int start2=0,end2=0;
    for(i=plen-2;i>=0;i--){
        ch = pattern[i];

        for(j=0;j<lflen;j++){
            if(ch==lftitle[j]){
                pos = j;
            }
        }
        start2 = *(lfmapping+(start)*lflen+pos);
        end2 = *(lfmapping+(end)*lflen+pos);

        count = 0;
        for(j=0;j<len;j++){
            if(ch == maps[j].str[0] && count == 0){
                start = j;
                count++;
            }
            if((ch != maps[j].str[0] && count == 1)||j+1 == len){
                if(j+1 == len){
                    end = len;
                }
                else {
                    end = j;
                }
                j=len;
                count++;
            }
        }

        start = start+start2;
        end = start+end2-start<end?start+end2-start2:end;
    }
//    if(start>=end){
//            if(type==0){
//                printf("not exist");
//            }
//        return 0;
//    }
//    else{
//        if(type==0){
//            printf("模式串的长度 = %d\n",plen);
//            printf("模式串在矩阵中的位置 ： [%d,%d)\n",start,end);
//        for(i=start;i<end;i++){
//            printf("模式串的位置：[%d,%d)\n",maps[i].id,maps[i].id+plen);
//        }
//        }
//        return 1;
//    }


	if (start >= end) {
		if (type == 0) {
			fputs("not exits\n",outfile);
			printf("not exits\n");
		}
		return 0;
	}
	else {
		if (type == 0) {
			fputs(pattern,outfile);
			char buf[100];
			sprintf(buf, "模式串的长度 = %d\n", plen);
			printf("%s",buf);
			fputs(buf,outfile);
			sprintf(buf, "模式串在矩阵中的位置 ： [%d,%d)\n", start, end);
			printf("%s",buf);
			fputs(buf, outfile);
			for (i = start; i<end; i++) {
				sprintf(buf, "模式串的位置：[%d,%d)\n", maps[i].id, maps[i].id + plen);
				printf("%s",buf);
				fputs(buf,outfile);
			}
		}
	}
    return 0;

}

int CalculateD(char pattern[PMAX]){
    int z=0;

    int j=0;
    int i=0;
    int m,pos,result;
    char temp[MAX];
    for(i=0;i<plen;i++){
        pos = 0;
        for(m=j;m<=i;m++){
            temp[pos]=pattern[m];
            pos++;
        }
        temp[pos] = '\0';
        result = Search(temp,1);
        //printf("temp = %s || result = %d\n",temp,result);
        if(result ==0){
            z++;
            j=i+1;
        }
        D[i] = z;
    }
    return 0;
}



int misSearch(char pattern[PMAX],int z){
    int i,j;
    for(i=0;i<len;i++){
        int mis=0;
        int flag=0;
        int pos=i;
        for(j=0;j<plen;j++){
            if(pattern[j]!=str[pos]){
                mis++;
                if(mis>z){
                    j=plen;
                    flag=1;
                }
            }
            pos++;
        }
        if(flag==0 && mis>0) printf("[%d,%d)\n",i,i+plen);
    }
    return 0;
}
//--------------------------------------------------------------------
int SA[MAX];
char BWTSA[MAX];
int cmpSA(int x[MAX], int i, int j, int k)
	{
		return x[i] == x[j] && x[i + k] == x[j + k];
	}
	int  swap(int *a,int* b)
{  int t;
   t=*a;
   *a=*b;
   *b=t;
}
int generateSA(){
    int m=5;
    int len = strlen(str);
    int x[MAX];
    int wst[MAX];
    int y[MAX];
    int wv[MAX];

    for(int i=0 ; i<len ; i++){
        if(str[i] == '$') x[i]=0;
        else if(str[i] == 'a') x[i]=1;
        else if(str[i] == 'c') x[i]=2;
        else if(str[i] == 'g') x[i]=3;
        else if(str[i] == 't') x[i]=4;
     }


     for(int i=0 ; i<m ;i++) wst[i]=0;
     for(int i=0 ; i<len ;i++) wst[x[i]]++;
     for (int i = 1; i < m; i++)		wst[i] += wst[i - 1];
     for(int i=len-1 ;i>=0;i-- ) SA[--wst[x[i]]] = i;
     for(int j = 1; m < len; j *= 2)
		{
			int t = 0;
			for (int i = len - j; i < len; i++)	y[t++] = i;
			for (int i = 0; i < len; i++)
			{
				if (SA[i] >= j)	y[t++] = SA[i] - j;
			}
			for (int i = 0; i <len; i++)		wv[i] = x[y[i]];
			for (int i = 0; i < m; i++)		wst[i] = 0;
			for (int i = 0; i < len; i++)		wst[wv[i]]++;
			for (int i = 1; i < m; i++)		wst[i] += wst[i - 1];
			for (int i = len - 1; i >= 0; i--)		SA[--wst[wv[i]]] = y[i];

			for(int i =0;i<len;i++){
                swap(&x[i],&y[i]);
			}
			int p = 1;
			x[SA[0]] = 0;
			for (int i = 1; i < len; i++)
				x[SA[i]] = cmpSA(y, SA[i - 1], SA[i], j) ? p - 1 : p++;
			m = p;
		}
		return 0;
}
int Get_BWT()
	{
		int len = strlen(str);
		for (int i = 0; i < len; i++)
		{
			int n = SA[i] - 1;
			if (n < 0)
				n = len + n;
			BWTSA[i] = str[n];
		}

		//printf("BWTSA%s\n",BWTSA);
		return 0;
	}



char BWT2_L[MAX][3];
char BWT2_R[MAX][3];
int Count1[5]= {0};
int Count2[24]= {0};
int C1[5];
int C2[24];
int Occount1[3]={0};
int Occount2[24]={0};
int Occ1[MAX]={0};
int Occ2[MAX]={0};

int generateBWT2LR()
{
    for(int i=0 ; i<strlen(BWT) ; i++)
    {
        strncpy(BWT2_L[i],maps[i].str,2);
        //将BWT（T）前两列打印出来
        //printf("%s\n",BWT2_L[i]);
    }

    for(int i=0 ; i<strlen(BWT) ; i++)
    {
        strncpy(BWT2_R[i],maps[i].str+strlen(BWT)-2,3);
        //将BWT（T）最后两列打印出来
        //printf("%s\n",BWT2_R[i]);
    }
}

int generateCAndOcc1(){
    char table[5] = {'$','a','c','g','t'};

    int first_match_pos = 0;
    for(int i=first_match_pos; i< strlen(BWT); i++)
    {
        for(int j=0;j<5;j++){
            //printf("%s and %s\n",BWT2_L[i],table[j]);
            if( table[j] == BWT2_L[i][0]){
                Count1[j]++;
                first_match_pos = j-1;
                break;
            }
        }
    }
    //打印数组Count1，存储每两个字符组合出现了几次
//    printf("Count1\n");
//    for(int i=0 ; i<5 ; i++){
//        printf("%d\t",Count1[i]);
//    }
    for(int i=1 ; i<5 ; i++){
        C1[i] = C1[i-1]+Count1[i-1];
    }
//打印数组C1，存储每两个字符前面有几个比自己小的字符组合
//    printf("C1\n");
//    for(int i=0 ; i<5 ; i++){
//        printf("%d\t",C1[i]);
//    }
    for(int i=0 ; i<strlen(BWT) ; i++){
        for(int j=0 ; j<5 ; j++){
            if(  table[j]==BWT2_R[i][1] ){
                Occount1[j]++;
                Occ1[i] = Occount1[j];
                break;
            }
        }
    }
// 打印数组Occount1，存储后两列每两个字符组合在前面出现几次
//    printf("Occount1\n");
//    for(int i=0 ; i<5 ; i++){
//        printf("%d\t",Occount1[i]);
//    }

// 打印数组Occ1，存储自己是相同字符中第几个出现的
//    printf("打印数组Occ1\n");
//    for(int i=0 ; i<strlen(BWT) ; i++){
//        printf("%d\t",Occ1[i]);
//    }

}

int generateCAndOcc2()
{
    char table[24][3] = {"$a","$c","$g","$t","a$","aa","ac","ag","at"
                         ,"c$","ca","cc","cg","ct","g$","ga","gc","gg","gt",
                         "t$","ta","tc","tg","tt"};
    int first_match_pos = 0;
    for(int i=first_match_pos; i< strlen(BWT); i++)
    {
        for(int j=0;j<24;j++){
            //printf("%s and %s\n",BWT2_L[i],table[j]);
            if(strcmp(table[j],BWT2_L[i]) == 0){
                Count2[j]++;
                first_match_pos = j-1;
                break;
            }
        }
    }
//打印数组Count2，存储每两个字符组合出现了几次
//    for(int i=0 ; i<24 ; i++){
//        printf("%d\t",Count2[i]);
//    }
    for(int i=1 ; i<24 ; i++){
        C2[i] = C2[i-1]+Count2[i-1];
    }
//打印数组C2，存储每两个字符前面有几个比自己小的字符组合
//    for(int i=0 ; i<24 ; i++){
//        printf("%d\t",C2[i]);
//    }
    for(int i=0 ; i<strlen(BWT) ; i++){
        for(int j=0 ; j<24 ; j++){
            if( strcmp( table[j] ,BWT2_R[i] )==0 ){
                Occount2[j]++;
                Occ2[i] = Occount2[j];
                break;
            }
        }
    }
// 打印数组Occount2，存储后两列每两个字符组合在前面出现几次
//    for(int i=0 ; i<24 ; i++){
//        printf("%d\t",Occount2[i]);
//    }

// 打印数组Occ2，存储自己是相同字符中第几个出现的
//    for(int i=0 ; i<strlen(BWT) ; i++){
//        printf("%d\t",Occ2[i]);
//    }

    return 0;
}

int Search2(char pattern[PMAX],int* sp,int* ep,int type)
{

    char table[24][3] = {"$a","$c","$g","$t","a$","aa","ac","ag","at"
                         ,"c$","ca","cc","cg","ct","g$","ga","gc","gg","gt",
                         "t$","ta","tc","tg","tt"};

    int slen;
    char temp[3]={""};
    len = strlen(pattern);
    strncpy(temp,pattern+len-2,2);

    for(int j=0 ; j<24 ; j++){

        if( strcmp( temp,table[j] )==0 ){
            (*sp) = C2[j];
            (*ep) = Count2[j]+(*sp);
            break;
        }
    }

    int sl,el,pos;
    for(int i=len-4 ; i>=0 ; i=i-2){

        char temp[3]={""};
        strncpy(temp,pattern+i,2);

        int count = 0;
        int j=0;
        sl=-1;
        el=-1;
        for(j=(*sp) ; j<(*ep) ; j++){
            //printf("%d,%s,%s,%s\n",j,BWT2_L[j],temp,BWT2_R[j]);
            if( strcmp(temp , BWT2_R[j])==0){
                if( count==0 ) sl = j;
                count++;
            }
        }
        //printf("count=%d\n",count);
        if(sl == -1 || sl == (*ep)){
           (*sp) = 1;
           (*ep) = -1;
           break;
        }
        el = sl+count;


        for(int j=0 ; j<24 ; j++){

            if( strcmp( temp,table[j] )==0 ){
                pos = j;
            }
        }

        (*sp) = Occ2[sl]+C2[pos]-1;
        (*ep) = (*sp) + el - sl;
//        printf("sp = %d,ep = %d",sp,ep);
//        printf("first%s\n",BWT2_L[sp]);

        //printf("%s\t",table[pos]);
//        printf("sl = %d , el = %d\n",sl,el);
//        printf("sp = %d , ep = %d\n",sp,ep);
    }

    if(strlen(pattern)%2==1){
        sl=0;
        el=0;
        char ch = pattern[0];
        int j=0;
        int count1=0;

        for(j=(*sp) ; j< (*ep) ; j++){

            if( BWT2_R[j][1]==ch){
                if( count1==0 ) sl = j;
                count1++;
            }

        }
        if(sl == -1 || sl == (*ep)){
           (*sp) = 1;
           (*ep) = -1;

        }else{
            el = sl+count1;

            char table2[5] = {'$','a','c','g','t'};
            for(int j=0 ; j<5 ; j++){

            if( ch == table2[j]){
                pos = j;
            }
            (*sp) = Occ1[sl]+C1[pos]-1;
            (*ep) = (*sp) + el - sl;
            }
        }


    }


    if(type == 0){
        printf("二阶BWT查找结果");
        if((*sp)>=(*ep))
    {
        printf("not exist\n");
        return 0;
    }
    else
    {
        printf("原串长度 = %d\n",strlen(BWT));
        printf("模式串的长度 = %d\n",len);

        printf("模式串在矩阵中的位置 ： [%d,%d)\n",(*sp),(*ep));
        for(int i=(*sp); i<(*ep); i++)
        {
            printf("模式串的位置：[%d,%d)\n",maps[i].id,maps[i].id+len);
        }
    }
    }

    return 1;
}


int sw_matrix2[PMAX][MAX];
char sw_path2[PMAX][MAX][4];

int SW2(char pattern[PMAX],char string[PMAX]){


    int row = strlen(pattern)+1;
    int col = strlen(string)+1;

    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            sw_path2[i][j][0]='\0';
        }
    }


    for(int i = 0 ; i<row;i++){
        sw_matrix2[i][0] = i;
    }
    for(int i = 0 ; i<col;i++){
        sw_matrix2[0][i] = i;
    }

    int a,b,c;
    for(int i=1 ; i<row ; i++){
        for(int j=1 ; j<col ; j++){
            int cost = 1;
            if(pattern[i-1] == string[j-1]){
                cost = 0;
            }
            a = sw_matrix2[i-1][j-1]+cost;
            b = sw_matrix2[i-1][j]+1;
            c = sw_matrix2[i][j-1]+1;

            if(a<=b && a<=c){
                sw_matrix2[i][j]=a;
                strcat(sw_path2[i][j],"\\");
            }
            if(b<=a && b<=c){
                sw_matrix2[i][j]=b;
                strcat(sw_path2[i][j],"|");
            }
            if(c<=a && c<=b){
                sw_matrix2[i][j]=c;
                strcat(sw_path2[i][j],"-");
            }

        }
    }

//    for(int i=0;i<row;i++){
//        for(int j=0;j<col;j++){
//            printf("%d\t",sw_matrix2[i][j]);
//        }
//        printf("\n");
//    }
//    for(int i=0;i<row;i++){
//        for(int j=0;j<col;j++){
//            printf("%4s\t",sw_path2[i][j] );
//        }
//        printf("\n");
//    }
    return sw_matrix2[row-1][col-1];
}

char sss[MAX]={'\0'};
char ttt[MAX]={'\0'};
int back_SW2(char tmp[4],int n,int m,char str[PMAX]){
    if(n==1 || m==1){
        char tmp_ch1[2];
        tmp_ch1[0]=str[m-1];
        tmp_ch1[1]='\0';
        char tmp_ch2[2];
        tmp_ch2[0]=pattern[n-1];
        tmp_ch2[1]='\0';
        strcat(sss,tmp_ch1);
        strcat(ttt,tmp_ch2);
        char buf[PMAX];

        sprintf(buf,"sss = %s\n",sss);
        fputs(buf,outfile);
        sprintf(buf,"ttt = %s\n",ttt);
        fputs(buf,outfile);
        printf("sss = %s\n",sss);
        printf("ttt = %s\n",ttt);
        int score = 0;
        for(int k = 0;k<strlen(sss);k++){
            if(sss[k]==ttt[k]) score = score+3;
            else if(sss[k]=='-'||ttt[k]=='-') score = score-2;
            else score = score-1;
        }
        printf("此匹配得分为 %d\n\n",score);
        sss[strlen(sss)-1]='\0';
        ttt[strlen(ttt)-1]='\0';
        return 1;
    }
    //printf("tmp = %s,%d,%d\n",tmp,n,m);
    int count = strlen(tmp);

    for(int i=0;i<=count;i++){
        char ch = tmp[i];

        if(ch =='|'){
            char tmp_ch[2];
            tmp_ch[0]=pattern[n-1];
            tmp_ch[1]='\0';
            strcat(sss,"-");
            strcat(ttt,tmp_ch);
            back_SW2(sw_path2[n-1][m],n-1,m,str);
            sss[strlen(sss)-1]='\0';
            ttt[strlen(ttt)-1]='\0';
        }
        if(ch == '-'){
            char tmp_ch[2];
            tmp_ch[0]=str[m-1];
            tmp_ch[1]='\0';

            strcat(sss,tmp_ch);
            strcat(ttt,"-");
            back_SW2(sw_path2[n][m-1],n,m-1,str);
            sss[strlen(sss)-1]='\0';
            ttt[strlen(ttt)-1]='\0';
        }
        if(ch=='\\'){
            char tmp_ch1[2];
            tmp_ch1[0]=str[m-1];
            tmp_ch1[1]='\0';
            char tmp_ch2[2];
            tmp_ch2[0]=pattern[n-1];
            tmp_ch2[1]='\0';
            strcat(sss,tmp_ch1);
            strcat(ttt,tmp_ch2);
            back_SW2(sw_path2[n-1][m-1],n-1,m-1,str);
            sss[strlen(sss)-1]='\0';
            ttt[strlen(ttt)-1]='\0';
        }

    }

    return 1;

}

int inexact_Search(char pattern[PMAX],float error){

    int plen  = strlen(pattern);
    int costT = error*plen;
    int k = 11;

    char string[PMAX];



    int result[10]={0};
    int count = 1;
    if(plen<=k) k = plen/2;

    for(int i=0;i<plen-k;i++){

        char p[12];
        strncpy(p,pattern+i,k);
        p[k]='\0';
        int sp=0;
        int ep=0;
        int re = Search2(p,&sp,&ep,1);

        if(i==0 && (ep-sp)!=0) result[0] = maps[sp].id;

        if(re == 1){
            for(int m = sp ; m < ep ; m++){
                int start = maps[m].id;
                int flag =1;
                for(int n=0;n<count;n++){
                    if(abs(start-result[n])<=i){
                        flag =0;
                        break;
                    }
                }
                if(flag != 0 ){
                    result[count]=start-i;
                    count++;
                }
            }
        }
    }
    for(int m=0;m<count;m++){
            //printf("种子的位置%d\t",result[m]);
            char string_on_str[PMAX];
            strncpy(string_on_str,str+result[m],plen);
            string_on_str[plen] ='\0';

            SW2(pattern,string_on_str);
            int mincost = sw_matrix2[plen][0];
            int minpos = 0;
            for(int j=0;j<strlen(string_on_str);j++){
                if(sw_matrix2[plen][j]<mincost){
                    mincost = sw_matrix2[plen][j];
                    minpos = j;
                }
            }


            if(mincost <= costT){
                char buf[100];
                sprintf(buf,"错配数目 = %d,允许最大错配数目 = %d",mincost,costT);
                fputs(buf,outfile);
                printf("错配数目 = %d,允许最大错配数目 = %d",mincost,costT);
//                int score=0;
//                for(int h=0;h<strlen(string_on_str);h++){
//                    if(string_on_str[h] == pattern[h]){
//                        score = score+3;
//                    }else if(string_on_str[h]=='-' || pattern[h]=='-'){
//                        score = score-2;
//                    }else{
//                        score = score-1;
//                    }
//
//                }
                sprintf(buf,"存在模糊匹配[%d,%d]\n",result[m],result[m]+plen);
                fputs(buf,outfile);
                printf("存在模糊匹配[%d,%d]\n",result[m],result[m]+plen);
                back_SW2(sw_path2[plen][minpos],plen,minpos,string_on_str);

            }
    }
}



int main()
{

    FILE *fp;
    char origin_filename[20],pattern_filename[20];
    printf("请输入原串所在的文件名称（xxx.txt）\n");
    scanf("%s",origin_filename);
    printf("请输入模式串所在的文件名称（xxx.txt）\n");
    scanf("%s",pattern_filename);
    fp = fopen(origin_filename,"r");
    if(fp == NULL)
    {
        printf("fail to open the file");
        return 0;
    }

    int i=0;
    char ch;
    ch= fgetc(fp);
    while(ch!=EOF){
        if(ch!='\n'){
            str[i]=ch;
            i++;
        }
        ch = fgetc(fp);
    }
    str[i]='\0';
    //printf("原串 ：%s\n",str);
    strcat(str,"$");

    FILE *outfile1;
    outfile1 = fopen("C:\\MyCode\\outfile_yuanchuan.txt","at+");
    if(outfile1==NULL){
        printf("打开写文件失败");
    }
    fputs(str,outfile1);
    fclose(outfile1);

    fclose(fp);
    len = strlen(str);

    clock_t s1,f1,s2,f2,s3,f3,s4,f4;
    s1 = clock();
    Transform(str,BWT);
    f1 = clock();
    //printf("BWT(T) :%s\n",BWT);

    outfile1 = fopen("C:\\MyCode\\outfile_bwt.txt","at+");
    if(outfile1==NULL){
        printf("打开写文件失败");
    }
    fputs(BWT,outfile1);
    fclose(outfile1);

    printf("轮转矩阵生成BWT %f\n",(double)(f1-s1));
    clock_t sa1,sa2;


    sa1 = clock();
    generateSA();
    Get_BWT();
    sa2 = clock();
    printf("后缀数组获得BWT %f\n",(double)(sa2-sa1));



    s2 = clock();
    Reverse();
    f2 = clock();



    fp = fopen(pattern_filename,"r");
    if(fp == NULL){
        printf("fail to open the file\n");
        return 0;
    }

    int line;
    printf("请输入要查找的字符串的个数 : ");
    scanf("%d",&line);

    printf("--------------------------------------\n");
    generateBWT2LR();
    generateCAndOcc1();
    generateCAndOcc2();
    outfile = fopen("C:\\MyCode\\outfile_search.txt","at+");
    if(outfile==NULL){
        printf("打开写文件失败");
    }
    for(i=0;i<line;i++){
        fscanf(fp,"%s",pattern);
        printf("模式串 = %s\n",pattern);
        plen = strlen(pattern);

        s3 = clock();
        Search(pattern,0);
        f3 = clock();

        printf("一阶查找的运行时间%f\n",(double)(f3-s3));
        printf("==============================\n");

        s4 = clock();
        int sp=0,ep=0;
        Search2(pattern,&sp,&ep,0);
        f4 = clock();
        printf("二阶查找的运行时间%f\n",(double)(f4-s4));
        printf("==============================\n");


        printf("---------------------------------------------------------\n");
        printf("请输入允许错配率\n");
        float error;
        scanf("%f",&error);
        inexact_Search(pattern,error);
    }
    fclose(fp);
    fclose(outfile);




    return 0;
}
