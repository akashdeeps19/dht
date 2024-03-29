#include <stdio.h>
    #include<iostream>
    #include <stdlib.h>
    #include <string.h>
    #include<string>
    #include<thread>
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <fstream>



    using namespace std;
    char filename[100000];
    int c;
    // structure for double linked list
    struct Node{
        int id;
        struct Node* next;
        struct Node* prev;
    };


    struct Node* head;


    // inserting into a double linked list
    void insert( int id) 
    { 
        
        if (head == NULL) 
        { 
            struct Node* new_node = new Node; 
            new_node->id = id; 
            new_node->next = new_node->prev = new_node; 
            head  = new_node; 
            return; 
        } 
        Node *last = head->prev; 
        struct Node* new_node = new Node; 
        new_node->id =id; 
        new_node->next = head; 
        head->prev = new_node; 
        new_node->prev = last; 
        last->next = new_node; 
    } 

    // prework means to make the circular double linked list with the existing nodes
    void prework()
    {
        insert(2000);
        insert(3000);
        insert(4000);
        insert(5000);
        insert(6000);
    }


    // class to define a node - each node has 2 function -> server function and client function
    class node
    {
        int PORT;

        public:
        node(int p){
            PORT = p;
        }

        int server(){
            int sockfd, ret;
            struct sockaddr_in serverAddr;

            int newSocket;
            struct sockaddr_in newAddr;

            socklen_t addr_size;

            char buffer[1024];
            pid_t childpid;

            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if(sockfd < 0){
                printf("[-]Error in connection.\n");
                exit(1);
            }
            printf("[+]Server Socket is created.\n");

            memset(&serverAddr, '\0', sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(PORT);
            serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

            ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            if(ret < 0){
                printf("[-]Error in binding.\n");
                return 0;
            }
            printf("[+]Bind to port %d\n", PORT);

            if(listen(sockfd, 10) == 0){
                printf("[+]Listening....\n");
            }else{
                printf("[-]Error in binding.\n");
            }


                newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
                if(newSocket < 0){
                    return 0 ;
                }
                printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

                if((childpid = fork()) == 0){
                    close(sockfd);

                    
                        char recievedstring[1000000];
                        char c[1];
                        char fname[100000];
                        recv(newSocket,c,1,0);
                        if(c[0]=='1')
                        { 
                           // recieving file and storing it in it's directory
                            recv(newSocket,fname,10000,0);
                            FILE *ptr=fopen(fname,"w");
                            while(recv(newSocket,recievedstring,10000,0))
                            {
                                fputs(recievedstring,ptr);break;
                            }
                            fclose(ptr);
                            cout<<"Recieved"<<endl;
                            return 0;
                            

                        }
                        else if(c[0]=='2')
                        {
                            // recieving filename and sending contents of the file
                            recv(newSocket,fname,10000,0);
                            std::ifstream ifs(fname);
                            std::string content( (std::istreambuf_iterator<char>(ifs) ),(std::istreambuf_iterator<char>()    ) );
                            char sending[100000];
                            for(int i=0;i<content.length();i++){sending[i]=content[i];}
                        
                            send(newSocket, sending, 100000, 0);
                            return 0;

                        }
                        if(strcmp(buffer, ":exit") == 0){
                        close(newSocket);
                        printf("[-]Disconnected from server.\n");
                        exit(1);
                        }

                        }

            

            close(newSocket);}
        

        void client(int port){
            int clientSocket, ret;
            struct sockaddr_in serverAddr;
            char buffer[1024];

            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if(clientSocket < 0){
                printf("[-]Error in connection.\n");
                exit(1);
            }
            printf("[+]Client Socket is created.\n");

            memset(&serverAddr, '\0', sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

            ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            if(ret < 0){
                printf("[-]Error in connection.\n");
                exit(1);
            }
            printf("[+]Connected to Server.\n");

            while(1){
                if(c==1)
                {
                    // reads contents of a file and sends contents  to the server 
                FILE *ptr=fopen(filename,"r");
                char sending[100000];
                char rand[1];
                rand[0]='1';
                send(clientSocket,rand,1,0);    
                send(clientSocket,filename,10000,0);  
                while(fgets(sending,10000,ptr))  
                {send(clientSocket, sending, 10000, 0);}
                cout<<"Sent "<<endl;break;
            }
                else if(c==2)
                {
                    // recieves contents of the file that client requested for and prints it's cotents
                    char rand[1];char recievedfile[1000000];
                    rand[0]='2';
                    send(clientSocket,rand,1,0);
                    send(clientSocket,filename,100000,0);
                    recv(clientSocket,recievedfile,1000000,0);
                    cout<<"file recieved is - "<<endl<<recievedfile;c=0;
                    FILE *ptr=fopen("filename","w+");
                    break;
                }

                if(strcmp(buffer, ":exit") == 0){
                    close(clientSocket);
                    printf("[-]Disconnected from server.\n");
                    exit(1);
                }
            }

        }
    };

    // Hash function 
    int hashing(char str[])
    {
        
        int sum=0;
        for(int i=0;str[i]!=0;i++)
        {
                sum=(sum+(i+19973)*str[i])%5;
        }
        sum=sum%5;
        sum*=1000;
        sum+=2000;
        
        return sum;
    }

    // search for the node whose id is closest to the hashvalue of the filename
    int  search(char str[])
    {
        struct Node* temp=head;
        struct Node* temp2=head;
        int diff=1000000;;struct Node* p=head;
        int hv=hashing(str);//value obtained from hash function
        while(temp->next!=temp2)
        {
            int ds=temp->id-hv;
            if(ds<0){ds*=-1;}
            if(ds<diff)
            {
                if(ds<0){ds*=-1;} 
                diff=ds;
                p=temp;
            }
            
            temp=temp->next;
        }
        
        int ds=temp->id-hv;
        if(ds<0){ds*=-1;}
        if(ds<diff)
        {
            if(ds<0){ds*=-1;}
            p=temp;
        }
        return p->id;
        
    }


    int main(int argc, char** argv){
        node* n = new node(atoi(argv[1]));
        prework();

        thread ser(&node::server,n);
        // client can enter any of these commands

        string commands[] = {"help","store","search","quit","\0"};
        string command = "";
        while(1){
            cout<<"Enter command : ";
            cin>>command;
            if(command == "store"){
                cout<<"enter file name that you want to store ";
                cin>>filename;
                int port;

                port =search(filename);

                
                c=1;
                n->client(port);
            }
            else if(command=="search"){
            cout<<"enter file name whose information you need ";
                cin>>filename;
                int port;

                port =search(filename);

                
                c=2;
                n->client(port);

            }
            else if(command == "quit" || command == "q"){
                ser.detach();
                break;
            }
            else if(command == "help"){
                for(int i = 0;commands[i] != "\0";i++)
                    cout<<commands[i]<<endl;
            }
        }
    }
