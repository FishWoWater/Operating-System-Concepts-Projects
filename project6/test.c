/*************************************************************************
	> File Name: test.c
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月18日 星期六 21时38分03秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>

int main(){
    int *arr = malloc(sizeof(int) * 4);
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    printf("%d", *(arr+2));
    return 0;
}
