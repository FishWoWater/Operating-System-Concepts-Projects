/*************************************************************************
	> File Name: test.c
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月19日 星期日 22时16分17秒
 ************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(){
    FILE *fp = fopen("/home/dreamboy/OSProjects/project8/BACKING_STORE.bin", "rb");
    assert(fp);
    char *buffer = malloc(sizeof(char) * 10);
    fread(buffer, 10, 10, fp);
    printf("%s\n", buffer);
    fclose(fp);
    return 0;
}
