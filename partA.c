#include <stdio.h>//for input and output(for use printf function)
#include <stdlib.h>//for use exit funtion
#include <unistd.h>//related to fork functions
#include <fcntl.h>
#include <errno.h>//related to error handlling
#include <sys/types.h>//related to fork handlling functions
#include <sys/wait.h>//related to wait funtion
#include <sys/msg.h>
#include <sys/stat.h>//related to msgflags
#include <sys/ipc.h>//inter process communication with msg queues
#include <sys/msg.h>//define many functions which are used in implementing msg queues
#include <string.h>//copy string 
//PARTA
struct Student {//Declare the 

    char index[30];
    float assign1;
    float assign2;
    float prj;
    float fin;
};
  float sum = 0;
  float avgSum = 0;
  int count = 0;


struct msg{//Declare the msg que structure and members
    long type;
    char text[100];

};

struct msg create_msg(long msgType, char* msgBody);//Create a function to initialize the structure

void main(){
    
    FILE* of;
    
    of = fopen("newStudentData.txt", "w");//open the file for write

    struct Student stud1 = { "EG/2018/1110",14,23,12.4,15 };//initialize the structure
    struct Student stud2 = { "EG/2018/1111",12,13.3,24.5,42 };
    struct Student stud3 = { "EG/2018/1112",25,2.4,3.5,12 };
    struct Student stud4 = { "EG/2018/1112",21.5,42,13,12 };
    struct Student stud5 = { "EG/2018/1113",11.4,23.2,32.4 }; 
    
    fwrite(&stud1, sizeof(struct Student),1, of);//write to the file
    fwrite(&stud2, sizeof(struct Student),1, of);
    fwrite(&stud3, sizeof(struct Student),1, of);
    fwrite(&stud4, sizeof(struct Student),1, of);
    fwrite(&stud5, sizeof(struct Student),1, of);



    if (fwrite != 0)

        printf("Contents to file written successfully !\n");

    else

        printf("Error writing file !\n");

    fclose(of);

    FILE* inf;

    struct Student stud;//create instance of student

    inf = fopen("newStudentData.txt", "r");
 

    if (inf == NULL) {

        fprintf(stderr,"\nError to open the file\n");
        exit(1);

    }
 
   
    while (fread(&stud, sizeof(struct Student),1, inf)){
     
        sum = stud.assign1 + stud.assign2 + stud.prj;
        printf("index: %s \tASSIGN1: %f\tASSIGN2:%f \tPRJ: %f \tCAsum: %.2f\tfinal:%f  \n ",stud.index, stud.assign1,stud.assign2,stud.prj,sum,stud.fin);//print all the marks
         
        avgSum+= sum;//calculate sum of marks
        if(sum < 25.00){
            count++;
        }
        
    }
   
    fclose(inf);
 
//PARTB

    key_t fileTok = ftok("myFIle", 56); //create msg queue(get unique key)

    
    int msgQID = msgget(fileTok,IPC_CREAT | 0666);//using above key create msg queue 
    if(msgQID == -1){//error handling
        perror ("msgget error");
        printf("Error No: %d\n",errno);
        exit(0);
    }
    //C1
    pid_t PID = fork();//call for child1(C1)

    if(PID == -1){//error handling
        perror ("fork error");
        printf("Error No: %d\n",errno);
        exit(0);
    }else if(PID == 0){
        usleep(5);

        struct msg rcvMsg[3];

        for(int i = 0;i<3;i++){
            int rcvmsgRet = msgrcv(msgQID,&rcvMsg[i],sizeof(rcvMsg[i].text),-400,IPC_NOWAIT);
            if(rcvmsgRet == -1){
                perror ("msgrcverror");
                printf("Error No: %d\n",errno);
                exit(0);
            }
        }
        
        printf("sum of CA marks:  %s\n",rcvMsg[0].text);//print sum of CA marks
        printf("Average of CA marks:  %s\n",rcvMsg[1].text);//print average of CA marks
        printf("No of students below 25 percent marks out of total CA marks:  %s\n",rcvMsg[2].text);//print no of students below 25 percent marks out of total CA marks 

        //C2 
        pid_t PID1 = fork();//call for child2(C2)
        if(PID1 == -1){//error handling
            perror ("fork error");
            printf("Error No: %d\n",errno);
            exit(0);
        }else if(PID1 == 0){

            printf("Child C2 recieved msgs from parent P\n");
        }
        //C3 and CC3
        pid_t PID2 = fork();//call for child3(C3)
        if(PID2 == -1){//error handling
            perror ("fork error");
            printf("Error No: %d\n",errno);
            exit(0);
        }
        //C3
        else if(PID2 == 0){
            printf("Parent C3 send msgs to Child CC3\n");
        //CC3       
        pid_t PID3 = fork();//call for child4(CC1)
       
        if(PID3 == -1){//error handling
            perror ("fork error");
            printf("Error No: %d\n",errno);
            exit(0);
        }else if(PID3 == 0){
            printf("Child CC3 recieved msgs from parent C3\n");
        }
          else{
            exit(0);
          }   
        }
        
       

    }
    //P(Parent process)
    else{
        
        struct msg sndMsg[3];

        sndMsg[0]=create_msg(100,gcvt(avgSum,6,sndMsg[0].text));
        sndMsg[1]=create_msg(200,gcvt(avgSum/5,6,sndMsg[1].text));
        sndMsg[2]=create_msg(300,gcvt(count,6,sndMsg[2].text));

        for(int i = 0; i<3;i++){
            int sentRet = msgsnd(msgQID,&sndMsg[i],sizeof(sndMsg[i].text),IPC_NOWAIT);
            if(sentRet == -1){
                perror ("msgsnd error");
                printf("Error No: %d\n",errno);
                exit(0);
            }
        }
        printf("Parent sent messages to C1\n");
        pid_t wpidRet = waitpid(PID,NULL,0);
        if(wpidRet == -1){//wait for child processces are terminated
           perror ("msgsnd error at parent");
            printf("Error No: %d\n",errno);
           exit(0); 
        }
        

        int ctrlRet =  msgctl(msgQID,IPC_RMID,0);
        if(ctrlRet == -1){
           perror ("msgsnd error");
            printf("Error No: %d\n",errno);
           exit(0); 
        }
    }
}
struct msg create_msg(long msgType, char* msgBody){

    struct msg newMsg;
    newMsg.type = msgType;
    strcpy(newMsg.text,msgBody);
    return newMsg;
}