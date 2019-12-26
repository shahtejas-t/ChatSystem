//Headers
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/msg.h>
 
#define MAX_STRING 512
 
struct Msg
{
        long int msg_type;
        char sender_name[MAX_STRING];
        char receiver_name[MAX_STRING];
        char text[MAX_STRING];
};

long hash(char string[MAX_STRING]);

int main()
{
        char sender[MAX_STRING],receiver[MAX_STRING];
        char buffer[MAX_STRING];
        long sender_hash,receiver_hash;
        int sender_msgid,receiver_msgid;
        int option;
        FILE *sender_fptr, *receiver_fptr;
        
		struct Msg msg;
		
        printf("\nEnter Your username(alias) : ");
        scanf("%s",sender);
		sender_hash = hash(sender);
		
        while(1)
        {
                printf("\n\n1. Send message(Press q! to quit send message) \n2. View New message(s) \n3. View all history \n4. Exit \nYour option? : ");
                scanf("%d",&option);
                switch(option)
                {
                case 1: printf("Enter receiver's username : ");
                        scanf("%s",receiver); 
						receiver_hash = hash(receiver);
                        
                        receiver_msgid = msgget((key_t)receiver_hash, 0666 | IPC_CREAT);
                        if(receiver_msgid == -1)
                        {
                                printf("Could not create message queue for msgid : %s",receiver);
                                exit(EXIT_FAILURE);
                        }
                        
                        receiver_fptr = fopen(receiver,"a");
                        if(receiver_fptr == NULL)
                        {
                        	printf("Error while opening/creating history file for receiver");
                        }
                        sender_fptr = fopen(sender,"a");
                        if(sender_fptr == NULL)
                        {
                        	printf("Error while opening/creating history file for sender");
                        }
                        if(sender_fptr != NULL && receiver_fptr != NULL)
                        {
           				    fprintf(sender_fptr,"Message to : %s\n",receiver);
           				    fprintf(receiver_fptr,"Message from : %s\n",sender);
                        }
                        printf("Type your text :\n");
                        
                        while(1)
                        {
                                printf("%s : ",sender);
                                scanf("%s",buffer); 
                                if(strncmp(buffer,"q!",2) == 0)
                                {
                                        if(sender_fptr != NULL && receiver_fptr != NULL)
                                        {
                                            fprintf(receiver_fptr,"************************************************************************************\n");
                                            fprintf(sender_fptr,"************************************************************************************\n");
                                        }
                                        printf("closing send message..\n");
                                        break;
                                }
                                msg.msg_type = 1;
                                strcpy(msg.sender_name,sender);
                                strcpy(msg.receiver_name,receiver);
                                strcpy(msg.text,buffer);
                                if(msgsnd(receiver_msgid,&msg,sizeof(msg),0) == -1)
                                {
                                        printf("Send Message Failed\n");
                                        exit(EXIT_FAILURE);
                                }
                                if(sender_fptr != NULL && receiver_fptr != NULL)
                                {
                                    fprintf(sender_fptr,"\t%s\n",buffer);
                                    fprintf(receiver_fptr,"\t%s\n",buffer);
                                }
                        }
                        if(sender_fptr != NULL && receiver_fptr != NULL)
                        {
                            fclose(sender_fptr);
                            fclose(receiver_fptr);
                        }
                        break;
                        
                case 2: printf("\nNew Message(s) :  \n");
                        sender_msgid = msgget((key_t)sender_hash, 0666 | IPC_CREAT);
                        if(sender_msgid == -1)
                        {
                                printf("Could not create message queue for msgid : %s",sender);
                                break;
                        }
                        while(1)
                        {
                                if(msgrcv(sender_msgid,&msg,sizeof(msg),1,IPC_NOWAIT) == -1)
                                {
                                	break;
                                }
                                printf("\nSender : %s\t Message : %s",msg.sender_name,msg.text);
                        }
                        break;
                        
                case 3: printf("\nHistory of your messages :  \n");
                        sender_fptr = fopen(sender,"r");
                        if(sender_fptr == NULL)
                        {
                        	printf("\nError while searching history file for username : %s\n",sender);
                        	break;
                        }
                        while(1)
                        {
                        	char c = fgetc(sender_fptr);
                        	if(feof(sender_fptr))				
                        		break;
                        	printf("%c",c);
                        }
                        fclose(sender_fptr);
                        break;
                        
                case 4: printf("closing..\n");
                        // to destroy the message queue 
                        //msgctl(sender_msgid, IPC_RMID, NULL); 
                        exit(EXIT_SUCCESS);
                        
                default: printf("Please enter valid option number.\n");
                         break;
                }
        }
        return(0);
}

long hash(char string[MAX_STRING])
{
    int hash = 0;
    for(int i = 0 ; string[i] != '\0' ; i++)
    {
        hash = 131 * hash + string[i];
    }
    return hash;
}
