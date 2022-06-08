#include <stdio.h>
#include <Python.h>

#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

PyObject* getList(PyObject *dlist,float *p, int size)
{
    for(int i = 0 ; i < size; i++){
        PyList_Append(dlist, PyFloat_FromDouble(*p));
        p += sizeof(float);
    }
    return dlist;
}

int main()
{
    int shmid1;
    int shmid2;

    int im_w;
    int im_h;
    int NoP = 0;

    int *cal_num1;
    float *cal_num2;

    int *New_flag;
    int *CoP_flag;
    int *SC_flag;
    int *R_flag;

    void *shared_memory1 = (void *)0;
    void *shared_memory2 = (void *)0;

    int size_of_img;

    PyObject *myFunction, *myModule, *myModuleString, *myResult, *args;

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

    // 공유메모리 공간을 만든다.
    shmid2 = shmget((key_t)5678, sizeof(float)*30000*8*100, 0666|IPC_CREAT);

    if (shmid2 == -1)
    {
        perror("shmget failed : ");
        exit(0);
    }
    // 공유메모리를 사용하기 위해 프로세스메모리에 붙인다.
    shared_memory2 = shmat(shmid2, (void *)0, 0);
    if (shared_memory2 == (void *)-1)
    {
        perror("shmat failed : ");
        exit(0);
    }
    printf("공유메모리 설정 완료\n");
    Py_Initialize();

    myModuleString = PyUnicode_FromString((char*)"cap_edit");
    myModule = PyImport_Import(myModuleString);
    if (myModule == NULL)PyErr_Print();

    myFunction = PyObject_GetAttrString(myModule,(char*)"face_recognition");
    printf("python module load complete!\n");
    while(1)
    {
        cal_num1 = (int *)shared_memory1;

        New_flag = (int *)shared_memory1;
        New_flag += sizeof(int)*2;
        CoP_flag = (int *)shared_memory1;
        CoP_flag += sizeof(int)*3;
        SC_flag = (int *)shared_memory1;
        SC_flag += sizeof(int)*4;
        R_flag = (int *)shared_memory1;
        R_flag += sizeof(int)*5;

        while (*New_flag != 1 && *New_flag != 9) sleep(0.00005);
        if(*New_flag == 9) break;

        im_w = *cal_num1;
        cal_num1 += sizeof(int);
        im_h = *cal_num1;
        cal_num1 += sizeof(int);

        size_of_img = im_w*im_h*3;

        cal_num2 = (float *)shared_memory2;

        PyObject *dlist = PyList_New(0);

        while(1)
        {
            while(*New_flag != 1 && *SC_flag !=1) sleep(0.00005);
            if(*SC_flag == 1) break;
            dlist = getList(dlist ,cal_num2, size_of_img);
            NoP++;
            *New_flag = 0;
            *CoP_flag = 1;
        }

        args = dlist;
        args = Py_BuildValue("(O)", args);

        args = PyTuple_Pack(4, args, PyFloat_FromDouble((double)im_w), PyFloat_FromDouble((double)im_h), PyFloat_FromDouble((double)NoP));
        PyErr_Print();

        myResult = PyObject_CallObject(myFunction, args);
        PyErr_Print();
        double result = PyFloat_AsDouble(myResult);
        printf("신경망의 답은 = %lf\n",result);
        *R_flag = result;
        *SC_flag = 0;
        NoP = 0;
    }

    if ( -1 == shmctl( shmid1, IPC_RMID, 0))
    {
        printf( "공유 메모리1 제거 실패\n");
        return -1;
    }
    else
    {
        printf( "공유 메모리1 제거 성공\n");
    }

    if ( -1 == shmctl( shmid2, IPC_RMID, 0))
    {
        printf( "공유 메모리2 제거 실패\n");
        return -1;
    }
    else
    {
        printf( "공유 메모리2 제거 성공\n");
    }

    Py_DECREF(myModule);
    Py_DECREF(myModuleString);

    Py_Finalize();

	return 0;
}
