/*
Задание - написать программу, объединяющую два введенных по запросу файла в один. Переслать содержимое полученного файла через межпроцессный канал в параллельный процесс, где получить информацию о количестве строк, слов и символов этого файла. Предусмотреть возможность прерывания от клавиатуры. При поступлении первого прерывания вывести информацию о количестве строк, слов и символов первого файла, при поступлении второго прерывания вывести те же самые атрибуты второго файла
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

int input_error_handler(char *fileName) {
    if (access(fileName, F_OK)) { // check exsisting file (unistd.h)
        fprintf(stderr, "File %s not exist!\nTry again:\n", fileName); // stdio.h
        return 1;
    }
    if (access(fileName, R_OK)) { // check readeble file unistd.h)
        fprintf(stderr, "Not permission to read the file!\nTry again - "); //stdio.h
        return 1;
    }
    return 0;
}

int COUNT = 0; // counter prerivaniy

sigjmp_buf oblBuf; // setjmp.h

char fileName1[256], fileName2[256];

int flg1_input = 0; // state of input first file
int flg2_input = 0; // state of input second file
int flg1_hand = 0; // if was handler before flg1_input = 1
int flg2_hand = 0; // if was handler before flg2_input = 1
int flg1_end = 1;
int flg2_end = 1;

void wc_print();
void hand_check();

void pHandler() {
	++COUNT;
	if (!flg1_input && COUNT == 1) {
		flg1_hand = 1;
	} else {
		if (!flg2_input) {
			flg2_hand = 1;
		}
		else hand_check();
	}
	siglongjmp(oblBuf,1); // go back to last setjmp setjmp.h
}


void wc_print(char *fileName) {
	int s;
	int p[2];
	pipe(p); // create pipe
	if(fork() == 0) {
		close(p[0]);
		dup2(p[1],1);
		execl("/bin/cat", "cat", fileName, NULL); // send to pipe text
		exit(0);
	} else {
		if (fork() == 0) {
			dup2(p[0],0); // input from pipe text
			close(p[0]);
			close(p[1]);
			execl("/usr/bin/wc", "wc", NULL);
			exit(0);
		}
	}
	wait(&s);
}


void hand_check() {
	if (COUNT > 0 && flg1_input && flg1_end) {
		flg1_end = 0;
		wc_print(fileName1);
	}
	if (COUNT > 1 && flg2_input && flg2_end) {
		flg2_end = 0;
		wc_print(fileName2);
	}
}

int main()
{
    // cat file1 file2 > file3
    int s;
    char buf[80];
    int p[2];
    struct sigaction prer; //sys/wait.h
    prer.sa_handler = pHandler;
    sigprocmask(0, 0, &prer.sa_mask); //sys/wait.h
    prer.sa_flags = 0;
    sigaction(2, &prer, 0); //SIGINT sys/wait.h

    memset(fileName1, 0, 256); //string.h
    memset(fileName2, 0, 256);
    memset(buf, 0, 336);
	
    printf("Hello! This programm concatinate two files,\nplease input file's names:\n");
    
    sigsetjmp(oblBuf, 1); // setjmp.h
	printf("First file:\n");

	scanf("%s", fileName1);
	if (input_error_handler(fileName1)) siglongjmp(oblBuf, 1); // setjmp.h
	flg1_input = 1;
	if (flg1_hand) hand_check();

	sigsetjmp(oblBuf, 1);
	printf("\nSecond File:\n");
	scanf("%s", fileName2);
	if (input_error_handler(fileName2)) siglongjmp(oblBuf, 1);
	flg2_input = 1;
	if (flg2_hand) hand_check();
	
	
	sigsetjmp(oblBuf, 1);
	
	pipe(p); // create pipe
	if(fork() == 0) {
		close(p[0]);
		dup2(p[1],1);
		execl("/bin/cat", "cat", fileName1, fileName2, NULL); // send to pipe text
		exit(0);
	} else {
		if (fork() == 0) {
			dup2(p[0],0); // input from pipe text
			close(p[0]);
			close(p[1]);
			execl("/usr/bin/wc", "wc", NULL);
			exit(0);
		}
	}
    wait(&s);
    exit(0);
}
