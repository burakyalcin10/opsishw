/* RPC Interface Definition for Task Processing System */

struct task {
    int id;
    int type;                /* 1=Reverse String, 2=Sum Integers, 3=Fibonacci */
    string payload<256>;
};

struct result {
    int id;
    string output<256>;
};

program TASKPROG {
    version TASKVERS {
        int SUBMIT_TASK(task) = 1;
        result GET_RESULT(int) = 2;
    } = 1;
} = 0x23451111;
