#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
        if(!fork())
                while(1)
                {
                        printf("CHILD");
                }
        else
        {
                while(1)
                {
                        printf("\t\tPARENT!");
                }
        }
        return 0;
}
