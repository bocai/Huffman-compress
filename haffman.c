/*
 * =====================================================================================
 *
 *       Filename:  haffman.c
 *
 *    Description:  huffman coder decoder
 *
 *        Version:  1.0
 *        Created:  
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HuangBocai, bocaihuang@gmail.com
 *        Company:  XIYOU
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define  N 256
#define Maxsize 80
#define  SOME 1
#define Empty 0
#define FULL -1

typedef unsigned long int WeightType;
typedef unsigned char MyType;
typedef struct			//哈夫曼树
{
	MyType ch;		//存字符
	WeightType weight;	/* 用来存放各个结点的权值 */
	int parent, LChild, RChild;	/*指向双亲、孩子结点的指针 */
} HTNode;

typedef struct			//队列
{
	int tag;
	int front;
	int rear;
	MyType length;
	char elem[Maxsize];
} SeqQueue;

void writeFile();
void printHFM(HTNode * ht, int n);
void code(char **hc, int n, unsigned char *ch);

int InitQueue(SeqQueue * Q)
{
	if (!Q)
		return 1;
	Q->tag = Empty;
	Q->front = Q->rear = 0;
	Q->length = 0;

	return 0;
}

int In_seqQueue(SeqQueue * Q, char x)
{
	if (Q->front == Q->rear && Q->tag == SOME)
		return FULL;	//full

	Q->elem[Q->rear] = x;	// printf("in = %c",x);
	Q->rear = (Q->rear + 1) % Maxsize;
	Q->length++;
	Q->tag = SOME;
	return SOME;
}

int Out_Queue(SeqQueue * Q, char *x)
{
	if (Q->tag == Empty)
		return Empty;

	*x = Q->elem[Q->front];
	Q->length--;
	Q->front = (Q->front + 1) % Maxsize;

	if (Q->front == Q->rear)
		Q->tag = Empty;

	return SOME;
}

/* ------------------以上是队列的操作-------------------------  */

void SelectMinTree(HTNode * ht, int n, int *k)
{
	int i, temp;
	WeightType min;

	//      printf(" Selecting……n= %d",n);
	for (i = 0; i <= n; i++) {
		if (0 == ht[i].parent) {
			min = ht[i].weight;	//init min
			temp = i;
			break;
		}
	}
	for (i++; i <= n; i++) {
		if (0 == ht[i].parent && ht[i].weight < min) {
			min = ht[i].weight;
			temp = i;
		}
	}
	*k = temp;
}

// 对哈夫曼树排序，并统计叶子数量
int SortTree(HTNode * ht)
{
	short i, j;
	HTNode tmp;

	for (i = N - 1; i >= 0; i--) {
		for (j = 0; j < i; j++)
			if (ht[j].weight < ht[j + 1].weight) {
				tmp = ht[j];
				ht[j] = ht[j + 1];
				ht[j + 1] = tmp;
			}
	}
	for (i = 0; i < N; i++)
		if (0 == ht[i].weight)
			return i;
	return i;		//返回叶子个数
}

//求哈夫曼0-1字符编码表
char **CrtHuffmanCode(HTNode * ht, short LeafNum)
/*从叶子结点到根，逆向求每个叶子结点对应的哈夫曼编码*/
{
	char *cd, **hc;		//容器
	int i, start, p, last;

	hc = (char **)malloc((LeafNum) * sizeof(char *));	/*分配n个编码的头指针 */

	if (1 == LeafNum)	//只有一个叶子节点时 
	{
		hc[0] = (char *)malloc((LeafNum + 1) * sizeof(char));
		strcpy(hc[0], "0");
		return hc;
	}

	cd = (char *)malloc((LeafNum + 1) * sizeof(char));	/*分配求当前编码的工作空间 */
	cd[LeafNum] = '\0';	/*从右向左逐位存放编码，首先存放编码结束符 */
	for (i = 0; i < LeafNum; i++) {	/*求n个叶子结点对应的哈夫曼编码 */
		start = LeafNum;	/*初始化编码起始指针 */
		last = i;
		for (p = ht[i].parent; p != 0; p = ht[p].parent) {	/*从叶子到根结点求编码 */
			if (ht[p].LChild == last)
				cd[--start] = '0';	/*左分支标0 */
			else
				cd[--start] = '1';	/*右分支标1 */
			last = p;
		}
		hc[i] = (char *)malloc((LeafNum - start) * sizeof(char));	/*为第i个编码分配空间 */
		strcpy(hc[i], &cd[start]);
		//
		printf("%3d号 %3c 码长:%2d;编码:%s\n", ht[i].ch, ht[i].ch,
		       LeafNum - start, &cd[start]);
	}			//getchar();
	free(cd);		//     Printcode(hc,n);
	return hc;
}

HTNode *CreatHFM(FILE * fp, short *n, WeightType * FileLength)
{
	HTNode *ht = NULL;
	int i, m, s1, s2;
	MyType ch;

	ht = (HTNode *) malloc(2 * N * sizeof(HTNode));
	if (!ht)
		exit(1);

	for (i = 0; i < N; i++) {
		ht[i].weight = 0;
		ht[i].ch = (MyType) i;	/*1-n号ch 为字符，初始化 */
	}

	for (*FileLength = 0; !feof(fp); ++(*FileLength)) {
		ch = fgetc(fp);	//fread(&ch,1,1,fp);     
		ht[ch].weight++;	//printf("ht[%c].weight= %ld\n",0,ht[0].weight);
	}
	--(*FileLength);	//去掉文件结束后的长度
	*n = SortTree(ht);
	m = *n * 2 - 1;		//free(&ht[m+1]);
	//printf("叶子个数= %d\n",*n);//getchar();
	if (1 == *n) {
		ht[0].parent = 1;
		return ht;
	} else if (0 > *n)
		return NULL;

	for (i = m - 1; i >= 0; i--) {
		ht[i].LChild = 0;
		ht[i].parent = 0;
		ht[i].RChild = 0;
	}
	/*      ------------初始化完毕！对应算法步骤1--------- */
	for (i = *n; i < m; i++)	//创建非叶子结点,建哈夫曼树
	{			//在ht[0]~ht[i-1]的范围内选择两个parent为0且weight最小的结点，其序号分别赋值给s1、s2返回
		SelectMinTree(ht, i - 1, &s1);
		ht[s1].parent = i;
		ht[i].LChild = s1;

		SelectMinTree(ht, i - 1, &s2);
		ht[s2].parent = i;
		ht[i].RChild = s2;

		ht[i].weight = ht[s1].weight + ht[s2].weight;
	}			/*哈夫曼树建立完毕 *///        puts(" over^_^");

	return ht;
}

//从队列里取8个字符（0、1），转换成一个字节
MyType GetBits(SeqQueue * Q)
{
	MyType i, bits = 0;
	char x;

	for (i = 0; i < 8; i++) {
		if (Out_Queue(Q, &x) != Empty) {	//printf("%c",x);
			if ('0' == x)
				bits = bits << 1;
			else
				bits = (bits << 1) | 1;
		} else
			break;
	}			//printf("   bits=%d\n",bits);puts("");

	return bits;
}

//求最长（最短）编码长度
void
MaxMinLength(FILE * File, HTNode * ht, char **hc, short NLeaf, MyType * Max,
	     MyType * Min)
{
	int i;
	MyType length;

	*Max = *Min = strlen(hc[0]);
	for (i = 0; i < NLeaf; i++) {
		length = strlen(hc[i]);
		fwrite(&ht[i].ch, sizeof(MyType), 1, File);	//字符和对应的
		fwrite(&length, sizeof(MyType), 1, File);	//编码长度写进文件
		if (length > *Max)
			*Max = length;
		if (length < *Min)
			*Min = length;
	}
}

//把出现过的字符编码表经过压缩写进文件
short CodeToFile(FILE * fp, char **hc, short n, SeqQueue * Q, MyType * length)
{
	int i;
	char *p;
	MyType j, bits;
	short count = 0;

	for (i = 0; i < n; i++)	// 将n个叶子压缩并写入文件
	{
		for (p = hc[i]; '\0' != *p; p++)
			In_seqQueue(Q, *p);

		while (Q->length > 8) {	//  puts("出队");
			bits = GetBits(Q);	//出队8个元素
			fputc(bits, fp);	//fwrite(&bits,sizeof(char),1,fp);
			//printf("压字符 %c\n",bits);
			count++;
		}
	}			//     printf("码字最后剩: %d\n",Q->length);

	*length = Q->length;
	i = 8 - *length;
	bits = GetBits(Q);	//取8个如果队不空
	for (j = 0; j < i; j++)
		bits = bits << 1;	//printf("压 字符 %c\n",bits);
	fputc(bits, fp);	//fwrite(&bits,sizeof(char),1,fp);
	count++;		//printf(" 指 针 在%d \n",ftell(fp));

	InitQueue(Q);
	return count;
}

//压缩
void Compress()
{
	char desFile[80], rename[80];
	MyType maxLen, minLen, ch, bits, n, finalLength;
	int i;
	short LeafNum, codeNum;
	WeightType count = 0, Length = 0, FileLength;
	FILE *fp, *compressFile;
	SeqQueue *Q;
	HTNode *ht = NULL;
	char **hc = NULL, **Map = NULL, *p;

	printf("filename to be compressed:");
	scanf("%s", desFile);
	//getchar();//fflush(stdin);//清空输入缓冲区域

	printf("filename after compressed:");
	scanf("%s", rename);
	if ('\0' == rename[0]) {
		printf("输入不能为空，请重新输入:");
		scanf("%s", rename);
	}

	compressFile = fopen(rename, "wb");
	fp = fopen(desFile, "rb");	//原文件

	if (!fp || !compressFile) {
		puts("Cannot open file.");
		return;
	}

	ht = CreatHFM(fp, &LeafNum, &FileLength);	//创建哈夫曼树,统计叶子个数和原文件长度
	if (!FileLength) {	//      printf("文件为空，无须压缩...");
		fclose(fp);
		fclose(compressFile);
		free(ht);
		return;
	}
	Q = (SeqQueue *) malloc(sizeof(SeqQueue));
	InitQueue(Q);		//SEEK_SET:文件开头 SEEK_CUR:当前位置 SEEK_END:文件结尾
	hc = CrtHuffmanCode(ht, LeafNum);	//取得哈夫曼0、1编码,hc的长度为LeafNum
	//Map为了取编码好定位，再建立全部(256个)//
	Map = (char **)malloc(N * sizeof(char *));	//字符编码表
	if (!Map) {
		puts("申请空间失败");
		return;
	}

	for (i = 0; i < N; i++)	//初始化
		Map[i] = NULL;

	for (i = 0; i < LeafNum; i++)	// 定位，编码指针数组Map[256]
		Map[(int)(ht[i].ch)] = hc[i];

	fseek(compressFile, sizeof(WeightType) + sizeof(short) + 6 * sizeof(MyType), SEEK_SET);	//先占个位置 
	//先占个位置,等下填压缩叶子编码剩几个和最长编码长//getchar();

	MaxMinLength(compressFile, ht, hc, LeafNum, &maxLen, &minLen);	//获得最长码串长度,顺便填写字符对应编码长
	//      for(i = 0;i < LeafNum;i++)
	//              if(ht[i].ch == (MyType)1)
	//                      printf("char %c %s\n",ht[i].ch,hc[i]);

	free(ht);		//getchar();printf("最长码串长度: %d\n",MaxCode);printf("ftell = %d\n",ftell(compressFile));
	codeNum = CodeToFile(compressFile, hc, LeafNum, Q, &finalLength);	//把字符转成其二进制编码写入文件,返回压成多少个

	rewind(compressFile);	//使文件指针移到开始printf("ftelll = %d\n",ftell(compressFile));
	fseek(compressFile, sizeof(WeightType) + sizeof(MyType), SEEK_SET);
	fwrite(&LeafNum, sizeof(short), 1, compressFile);	//写入叶子个数
	fwrite(&maxLen, sizeof(MyType), 1, compressFile);	//最长码串长度
	fwrite(&minLen, sizeof(MyType), 1, compressFile);	//最短码串长度
	//printf("MaxLen %d Min %d ftell = %d\n",maxLen,minLen,ftell(compressFile));getchar();
	fwrite(&codeNum, sizeof(short), 1, compressFile);	//填写叶子编码压多少个
	fwrite(&finalLength, sizeof(MyType), 1, compressFile);	//最后剩
	//printf("叶子共压：%d个,最后剩%d个\n\n",codeNum,finalLength);

	fseek(compressFile, 2 * LeafNum * sizeof(MyType) + codeNum, SEEK_CUR);
	//printf("开始正文ftell= %d 原文长%d\n",ftell(compressFile),FileLength);getchar();
	fseek(fp, 0, SEEK_SET);
	printf("Please wait a minute,compressing...");

	while (count < FileLength) {
		ch = fgetc(fp);	//fread(&ch,sizeof(MyType),1,fp);        
		++count;	//     printf("Read读 : %c 值 %d %s\n",ch,ch,Map[ch]);
		for (p = Map[ch]; *p != '\0'; p++)
			In_seqQueue(Q, *p);
		//printf("\nQlength: %d\n",Q->length);

		while (Q->length > 8)	//  printf("OutQueue: ");
		{
			bits = GetBits(Q);	//出队8个元素,合成一个字节
			fputc(bits, compressFile);	//fwrite(&bits,1,1,compressFile);      
			Length++;	//     printf("压：%c\n",bits);
		}
	}			//end of while
	//最后一个bits ;
	finalLength = Q->length;	//printf("最后剩Qlength:%d\n",Q->length);
	n = 8 - finalLength;
	bits = GetBits(Q);
	//printf("最后Qlength:%d\n",Q->length);
	for (i = 0; i < n; i++)
		bits = bits << 1;	//以‘0’补
	//fputc(bits,compressFile);
	fwrite(&bits, sizeof(MyType), 1, compressFile);	//printf("\nfinal压：%c\n",bits);
	Length++;

	rewind(compressFile);	//原文件长fwrite(&FileLength,sizeof(WeightType),1,compressFile);
	fwrite(&Length, sizeof(WeightType), 1, compressFile);	//压缩后的长
	fwrite(&finalLength, sizeof(char), 1, compressFile);	//最后的串长

	Length = Length + 12 + codeNum;	//printf("原文件长 %ld;压缩后 %ld\n",FileLength,Length);

	if (Length >= FileLength)
		puts("\nCompression rate: 0.0%");
	else
		printf("\nCompression rate: %.2lf%c\n",
		       (double)((FileLength -
				 Length) / (double)FileLength) * 100.0, '%');

	fclose(fp);
	fclose(compressFile);
	free(Q);
	free(hc);
	free(Map);
}

//把读出的字符，转换成8个0、1字符串并入队
void ToQueue(SeqQueue * Q, MyType ch)
{
	int i;
	MyType temp;

	for (i = 0; i < 8; i++) {
		temp = ch << i;
		temp = temp >> 7;
		if (1 == temp)
			In_seqQueue(Q, '1');	//printf("1");
		else
			In_seqQueue(Q, '0');	//printf("0");
	}			//puts("");
}

//解压缩
void UnCompress()
{
	MyType *str, MaxLength, MinLength, ch, *num, finalLength = 0, final = 0;
	FILE *cf, *uf;
	short NLeaf, Ncom;
	char CName[N], UName[N], **hc, *p, x, *buf;
	SeqQueue *Q = NULL;
	int i, j;
	WeightType srcLen = 0, thisFile = 0;

	fflush(stdin);		//清空输入缓冲区域
	printf("filename to be uncompressed:");
	scanf("%s", CName);
	if (CName[0] == '\0') {
		printf("try again:");
		scanf("%s", CName);
		fflush(stdin);
	}
	fflush(stdin);		//清空输入缓冲区域
	printf("filename after uncompressed:");
	scanf("%s", UName);
	if (UName[0] == '\0') {
		printf("try again:");
		scanf("%s", UName);
	}

	cf = fopen(CName, "rb");
	uf = fopen(UName, "wb");
	if (!cf || !uf) {
		puts("Cannot open files.");
		return;
	}

	fread(&srcLen, sizeof(WeightType), 1, cf);	//printf("压后文件长：%ld\n",srcLen);
	fread(&finalLength, 1, 1, cf);	//   printf("原文件压剩:%d个\n",finalLength);
	fread(&NLeaf, sizeof(short), 1, cf);	//printf("叶子:%d个\n",NLeaf);
	fread(&MaxLength, sizeof(MyType), 1, cf);
	fread(&MinLength, sizeof(MyType), 1, cf);
//printf("MaxLength = %d;minLen= %d,Leaf= %d\n",MaxLength,MinLength,NLeaf);getchar();
	Q = (SeqQueue *) malloc(sizeof(SeqQueue));

	buf = (char *)malloc((2 + MaxLength * sizeof(char)));
	str = (MyType *) malloc(NLeaf * sizeof(MyType));
	num = (MyType *) malloc(NLeaf * sizeof(MyType));
	//压缩叶子数量x和最后剩长
	if (!Q || !str || !num || !buf) {
		puts("Memery error.");
		exit(1);
	}
	InitQueue(Q);		//初始化

	fread(&Ncom, sizeof(short), 1, cf);
	fread(&final, sizeof(MyType), 1, cf);

	for (i = 0; i < NLeaf; i++)	//读取叶子及其码长
	{
		fread(&str[i], sizeof(MyType), 1, cf);
		fread(&num[i], sizeof(MyType), 1, cf);
		//printf("chars= %c;num= %d\n",Chars[i],CHlength[i]);
	}
	//printf("read ftell= %d\n",ftell(cf));//getchar();
	//      printf("\n叶子码字压缩后%d个,最后剩%d个\n",Ncom,final);

	hc = (char **)malloc((NLeaf) * sizeof(char *));	//hc为编码表的指针数组
	if (!hc)
		exit(1);

	--Ncom;			//printf("开始取叶子编码ftell = %d Ncom=%d\n",ftell(cf),Ncom);
	for (j = i = 0; i < Ncom; i++) {
		ch = fgetc(cf);	//fread(&ch,1,1,cf);
		ToQueue(Q, ch);	//printf("Q->Len= %d\n",Q->length);
		while (Q->length > MaxLength) {
			hc[j] = p = (char *)malloc(1 + num[j]);
			for (ch = 0; ch < num[j]; ch++) {
				Out_Queue(Q, &x);
				*p++ = x;
			}
			*p = '\0';	//printf("%c编 码%s\n",str[j],hc[j]);
			j++;
		}
	}
	ch = fgetc(cf);		//fread(&ch,1,1,cf);//最后一个
	ToQueue(Q, ch);
	final = 8 - final;
	while (Q->length > final) {
		p = hc[j] = (char *)malloc(1 + num[j]);
		for (ch = 0; ch < num[j]; ch++) {
			Out_Queue(Q, &x);
			*p++ = x;
		}
		*p = '\0';	//printf("%c编码%s\n",str[j],hc[j]);getchar();
		j++;
	}

	InitQueue(Q);
	--srcLen;
	--MinLength;
	//printf("开始解压正文%d ,压后长= %d\n",ftell(cf),srcLen);getchar();
	printf("Please wait a minute,uncompressing...");
	while (thisFile < srcLen) {
		//      printf("QLength= %d\n",Q->length);
		ch = fgetc(cf);	//fread(&ch,sizeof(MyType),1,cf);
		ToQueue(Q, ch);
		thisFile++;	//   printf("char:%c",ch);
		//完了后队列长于码串的最大值       

		while (Q->length > MaxLength) {
			for (i = 0; i < MinLength; i++) {
				Out_Queue(Q, &x);
				buf[i] = x;
			}
			for (; i < MaxLength; i++) {
				Out_Queue(Q, &x);
				buf[i] = x;
				buf[i + 1] = '\0';
				for (j = 0; j < NLeaf; j++) {
					if (i + 1 == num[j]
					    && 0 == strcmp(hc[j], buf)) {
						ch = str[j];	//printf("出队 %s\n",buf);
						fputc(ch, uf);	//fwrite(&ch,1,1,uf);////printf("解   出：%c\n",ch);
						break;
					}
				}
				if (j < NLeaf)
					break;
			}
		}		//while MaxLength
	}

	ch = fgetc(cf);		//fread(&ch,1,1,cf);//读取最后一个压缩字getchar();
	ToQueue(Q, ch);

	finalLength = 8 - finalLength;
	while (Q->length > finalLength) {
		for (i = 0; i < MinLength; i++) {
			Out_Queue(Q, &x);
			buf[i] = x;
		}
		for (; i < MaxLength; i++) {
			Out_Queue(Q, &x);
			buf[i] = x;
			buf[i + 1] = '\0';
			for (j = 0; j < NLeaf; j++) {
				if (i + 1 == num[j] && 0 == strcmp(hc[j], buf)) {
					ch = str[j];	//printf("出 %s\n",buf);
					fputc(ch, uf);	//fwrite(&ch,1,1,uf);//printf("解   出：%c\n",ch);
					break;
				}
			}
			if (j < NLeaf)
				break;
		}
	}
	//printf("Q->length= %d\n",Q->length);printf("ftell= %d\n",ftell(uf));
	free(Q);
	free(str);
	free(num);
	free(hc);
	fclose(uf);
	fclose(cf);
}

int main(int argc, char *argv[])
{
	char choice, blank[] = "              ";
	//      writeFile();
	system("color 8a");
	while (1) {
		system("clear");	//清屏
		puts("          * * * *Welcome use huffman encoder\\decoder* * *");
		puts("          **********************^_^***********************");
		puts("          *                                              *");
		printf("          * %s 1 ]. Compress   %s*\n", blank, blank);
		puts("          *                                              *");
		printf("          * %s 2 ]. Uncompress%s *\n", blank, blank);
		puts("          *                                              *");
		printf("          * %s 3 ]. Exit  ^_^ %s *\n", blank, blank);
		puts("          *                                              *");
		puts("          ************************************************");
		printf
		    ("          (Apply to text file) Copyright 2011 By Bocai\n");
		printf("          Choose (1 to 3):");
		choice = getchar();

		puts("");
		getchar();
		fflush(stdin);	//清空输入缓冲区域，否则键入的回车符将作为程序结尾处的scanf输入，此函数在stdio.h中
		switch (choice) {
		case '1':
			Compress();
			printf("Press Enter to continue...");
			getchar();
			break;
		case '2':
			UnCompress();
			printf("\nPress Enter to continue...");
			getchar();
			break;
		case '3':
			return 0;
			break;
		}
	}

	return 0;
}

/*----------------------------END----------------------------------------*/
