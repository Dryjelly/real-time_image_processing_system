#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

int main()
{
    // gcc -I/usr/include/python2.7 test.c -lpython2.7
    int shmid1;
    int shmid2;

    int im_w;
    int im_h;

    int *cal_num1;
    float *cal_num2;
    void *shared_memory1 = (void *)0;
    void *shared_memory2 = (void *)0;

    int size_of_img;
    printf("시작\n");
    // 공유메모리 공간을 만든다.
    shmid1 = shmget((key_t)1234, sizeof(int)*6, 0666|IPC_CREAT);

    if (shmid1 == -1)
    {
        perror("shmget failed : ");
        exit(0);
    }

    // 공유메모리를 사용하기 위해 프로세스메모리에 붙인다.
    shared_memory1 = shmat(shmid1, (void *)0, 0);
    if (shared_memory1 == (void *)-1)
    {
        perror("shmat failed : ");
        exit(0);
    }

    cal_num1 = (int *)shared_memory1;

    im_w = *cal_num1;
    cal_num1 += sizeof(int);
    im_h = *cal_num1;
    cal_num1 += sizeof(int);

    if ( -1 == shmctl( shmid1, IPC_RMID, 0))
    {
        printf( "공유 메모리1 제거 실패\n");
        return -1;
    }
    else
    {
        printf( "공유 메모리1 제거 성공\n");
    }

    size_of_img = im_w*im_h*3;
    printf("%d,%d,%d\n",im_w,im_h,size_of_img);
    // 공유메모리 공간을 만든다.
    shmid2 = shmget((key_t)5678, sizeof(float)*30000*8*100, 0666|IPC_CREAT);

    if (shmid2 == -1)
    {
        perror("shmget failed : ");
        exit(0);
    }
    printf("시작\n");
    // 공유메모리를 사용하기 위해 프로세스메모리에 붙인다.
    shared_memory2 = shmat(shmid2, (void *)0, 0);
    if (shared_memory2 == (void *)-1)
    {
        perror("shmat failed : ");
        exit(0);
    }
    cal_num2 = (float *)shared_memory2;



    if ( -1 == shmctl( shmid2, IPC_RMID, 0))
    {
        printf( "공유 메모리2 제거 실패\n");
        return -1;
    }
    else
    {
        printf( "공유 메모리2 제거 성공\n");
    }
    return 0;


	return 0;
}
