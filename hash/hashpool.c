/** HASH 桶实现 **/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

struct Pair
{
	char *key;
	char *value;
};
struct Bucket
{
	unsigned 	int count;
	Pair	*pairs;
};
struct StrMap
{
	unsigned	int count;
	Bucket	*buckets;
};

int  init_hashpool(int cnt,b_cnt)
int strmap_insert(char *key,char *value)
{
}
int main(int argc,char *argv[])
{
}
