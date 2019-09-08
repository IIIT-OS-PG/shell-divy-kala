#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#define MAXARGS 400
#define MAX_ARG_LENGTH 100

using namespace std;

string cwd, ps1, user_name, home;
string path; //NOTE THAT PATH IS EXPECTED TO END WITH /

int scriptfile;
bool record = false;


void prompt()
{

//    user_name = getenv("USER");
//    char* buff = new char[4096];
//    getcwd(buff, 4096);
//    cwd = getcwd(buff,4096);
    //cout << endl;
    cout  << ps1 << flush;
    if(record)
    {
        int buffsiz = ps1.size();
        char buff[buffsiz];
        strcpy(buff,ps1.c_str());
        write(scriptfile,buff,buffsiz);
    }

}

int tokenize_string(string str, char ** tokens)    //0th place is filled
{
    //  cout << "tokenizing " << endl;
    string cs = str;
    int tokenI = 0;
    for (int i = 0; i < str.length(); i++)
    {
        if(cs[i] == '"')
        {
            //      cout << " \" found " << endl;
            int j = 0;
            char * heapToken = new char[min((int)str.length()-i, MAX_ARG_LENGTH) ];        //TODO: remember to delete tokens
            bool quoteFound = false;
            while(cs[i]!='\0')
            {
                i++;
                heapToken[j++] = cs[i];

                if(cs[i] == '\"')
                {
                    quoteFound = true;
                    heapToken[j-1] = '\0';


                    tokens[tokenI++] = heapToken;
                    //              cout << "Inserted : " << tokens[tokenI] << endl;
                    break;
                }

            }
            if(!quoteFound)
            {
                heapToken[j] = '\0';
                tokens[tokenI++] = heapToken;
                //             cout << "Inserted : " << tokens[tokenI] << endl;
            }

        }
        else if(isspace(cs[i]))
        {
            i++;
            while(cs[i] != '\0' && isspace(cs[i]))
            {
                i++;
            }
            i--;

        }
        else
        {
            int j = 0;
            char * heapToken = new char[min((int)str.length()-i, MAX_ARG_LENGTH) ];
            bool broken = false;
            while(cs[i] != '\0' && !isspace(cs[i]))
            {
                if(cs[i] == '>' && cs[i+1] == '>')           //TODO check if i+1 overflows
                {
                    if(j!=0)
                    {
                        heapToken[j] = '\0';
                        tokens[tokenI++] = heapToken;
                    }
                    char * redir = new char[3];

                    redir[0] = '>';
                    redir[1] = '>';
                    redir [2] = '\0';
                    tokens[tokenI++] = redir;
                    i+=1;
                    broken = true;
                    break;

                }
                else if (cs[i] == '>' )
                {
                    if(j!=0)
                    {
                        heapToken[j] = '\0';
                        tokens[tokenI++] = heapToken;
                    }
                    char * redir = new char[2];
                    redir[0] = '>';
                    redir[1] = '\0';

                    tokens[tokenI++] = redir;

                    broken = true;
                    break;

                }
                else
                {
                    broken = false;
                    heapToken[j++]= cs[i];
                    i++;
                }
            }
            if(!broken)
            {

                heapToken[j] = '\0';
                tokens[tokenI++] = heapToken;
                i--;
            }
            //         cout << "Inserted : " << tokens[tokenI] << endl;



        }
    }




//    cout << "ret";
    return tokenI;

}

//
//int tokenize_string(string str, char ** tokens)    //0th place is filled
//{
//
//    char  charstring[str.length()];
//
//    strcpy(charstring, str.c_str());
//
//    char * token =  strtok(charstring," ");
//
//
//
//    int i;
//    for( i = 0;  token != NULL; i++)
//    {
//        char * heapToken;
//
//
//
//
//        heapToken = new char[strlen(token)];        //TODO: remember to delete tokens
//        for(int i = 0; i < strlen(token); i++)
//        {
//            heapToken[i] = token[i];
//        }
//        tokens[i] = heapToken;
//           cout << tokens[i] << " Inserted. " << endl;
//        if(token[0] =='\"'  )
//        {
//
//
//            token = strtok(NULL, "\"");
//            token++;
//
//
//        }
//        else
//            token = strtok(NULL, " ");
//
//    }
//    tokens[i] = NULL;
//
//    cout << "ret";
//    return --i;
//
//}

void run_redirect ( char ** rargs, int tokcount)
{
    if(tokcount == 0)
        return;

    char * edargs[tokcount-1];
    int i,j;
    for( i = 0, j = 0; i < tokcount-2; i++,j++ )
    {

        if(strcmp(rargs[j], ">") == 0)
        {

            break;
        }
        edargs[i] = rargs[j];
    }
    edargs[i] = 0;
//    int lastEdargI = i-1;

    int ret = fork();
    if(ret == -1)
    {
        cout << "Process fork failed." << endl;


    }
    else if ( ret == 0 )
    {


        int fd = open(rargs[tokcount-1], O_WRONLY | O_TRUNC);
        dup2(fd,1);
        close(fd);
        string fullcom = path + rargs[0];
        execv( fullcom.c_str(), edargs);


    }
    else
    {
        wait(NULL);


    }

}

void run_double_redirect ( char ** rargs, int tokcount)
{
    if(tokcount == 0)
        return;

    char * edargs[tokcount-1];
    int i,j;
    for( i = 0, j = 0; i < tokcount-2; i++,j++ )
    {

        if(strcmp(rargs[j], ">>") == 0)
        {

            break;
        }
        edargs[i] = rargs[j];
    }
    edargs[i] = 0;
//    int lastEdargI = i-1;

    int ret = fork();
    if(ret == -1)
    {
        cout << "Process fork failed." << endl;

    }
    else if ( ret == 0 )
    {


        int fd = open(rargs[tokcount-1], O_APPEND | O_WRONLY);
        dup2(fd,1);
        close(fd);
        string fullcom = path + rargs[0];
        execv( fullcom.c_str(), edargs);


    }
    else
    {
        wait(NULL);


    }

}


void run_command(char ** trueargs, int tokcount)
{
    if(tokcount == 0)
        return;
    int pip[2];
    pipe(pip);
    //fcntl(pip[0],F_SETFL,O_NONBLOCK);
    int ret = fork();

    if (ret == -1)
    {

        cout << "Command failed to execute, the process could not be started\n";
    }


    else if(ret == 0)
    {

        if(record)
        {

            string fullcom = path + trueargs[0];
            dup2(pip[1],1);
            close(pip[1]);
            close(pip[0]);

            execv(fullcom.c_str(), trueargs);

        }
        else
        {
            string fullcom = path + trueargs[0];
            close(pip[1]);
            close(pip[0]);
            execv(fullcom.c_str(), trueargs);
        }


    }



    if(record)
    {
        close(pip[1]);
        int k = fork();
        if(k==0)
        {

            //  signal(SIGPIPE, SIG_IGN);
            char c[100];
            int count;

//        FILE * infile = fdopen(pip[0],"r");
//        ifstream ifsfile;
//        ifsfile.open
            int con;

            //   while (ioctl(pip[0], FIONREAD, &con) == 0 && con > 0)
            while ( (count = read(pip[0], c,100) ) > 0 )
            {
                //count = read(pip[0], c,1000) ;
                write(scriptfile, c, count);
                //   scriptfile.write(buff.c_str(), buff.size());
                write(1,c,count);//    cout.write(buff.c_str(), buff.size());
                //     cout << "count is " << count << endl;
                //  count = write(scriptfile,buff.c_str(),count);

            }
            close(pip[0]);
            _exit(0);

        }
        close(pip[0]);
         waitpid(ret,NULL,0);
        waitpid(k,NULL,0);
        //    cout << "k term";

    }
    else
    {
            close(pip[1]);
    close(pip[0]);
        waitpid(ret,NULL,0);
        //  cout << "ret term";
    }

    close(pip[1]);
    close(pip[0]);
//   waitpid(ret,NULL,0);

}

void run_pipes (char ** rargs, int tokcount, int pipes)
{

    char * cmds[pipes+1][MAXARGS];

    int cmdi = 0;
    int j = 0;
    for(int i =0 ; i < tokcount; i++)
    {
        if(strcmp(rargs[i], "|") == 0)
        {
            cmds[cmdi][j] = NULL;
            j=0;
            cmdi++;


            continue;
        }

        cmds[cmdi][j++] = rargs[i];


    }
    cmds[cmdi][j] = NULL;

    int cmdnum = cmdi+1;

    int mypipe[pipes][2];
    for(int i = 0; i < pipes; i++ )
    {
        pipe(mypipe[i]);
    }
    vector<int> child_pids;

    for(int i = 0; i < cmdnum; i++ )
    {
        int f = fork();
        if(f==-1)
        {
            cout << "couldn't fork";

        }
        if ( f != 0)
        {
            child_pids.push_back(f);
            //  cout << "process pushed : " << f << endl;
        }
        if(f==0)
        {
            if(i==0)
            {
                dup2(mypipe[0][1], 1);
//                close (mypipe[0][1]);
//                close (mypipe[0][1]);

            }
            else if (i > 0 && i < cmdnum-1)
            {
                dup2(mypipe[i-1][0], 0);
//                close(mypipe[i-1][0]);
                dup2(mypipe[i][1],1);
//                close(mypipe[i][1]);
//                close(mypipe[i][0]);

            }

            else
            {

                dup2(mypipe[i-1][0],0);
//                close(mypipe[i-1][0]);
//                close(mypipe[i-1][1]);

            }
            for(int i = 0; i < pipes; i++ )
            {
                close(mypipe[i][0]);
                close(mypipe[i][1]);
            }

            string fullcom = path + cmds[i][0];

            if( execv(fullcom.c_str(), cmds[i]) == -1)
            {
                cout << "\ncommand could not be executed\n";
                exit(-1);
            }
        }

    }
    for(int i = 0; i < pipes; i++ )
    {
        close(mypipe[i][0]);
        close(mypipe[i][1]);
    }


    for(auto i = child_pids.begin(); i != child_pids.end(); i++)
    {
        //  cout << "waitig for " << *i <<endl;
//        if(i==child_pids.back) {
//            kill(*i, 0);
//        }
        waitpid(*i,NULL,0);
    }



    // while(wait(NULL) != -1) ;
//    wait(NULL);


//TODO delete pipes



}


int main()
{
    path = "/bin/";

    int user = getuid();
    ps1 = "#";
    if(user)
        ps1 = "$";








    home = getenv("HOME");




    string input = "";

    map<string,string> alias;




    while(input != "exit")
    {




        prompt();

        getline(cin, input);


        if(strcmp(input.c_str(), "exit")==0)
            return 0;



        char * tokens[MAXARGS];
        int tokcount = tokenize_string(input, tokens);
        if(tokcount > 0)
        {
            char * rargs[tokcount+1];

            for(int i = 0; i < tokcount; i++)
            {
                rargs[i] = tokens[i];

                // cout << trueargs[i] << endl;
            }
            //TODO DELETE ALL after num_tokens FROM ARGS AFTER COPY
            rargs[tokcount] = NULL;



            int rediri = 0;
            int pipes = 0;
            int dredir =0;
            for(int i = 0; i < tokcount; i++)
            {
                if(strcmp(rargs[i], ">") == 0)
                {
                    rediri++;

                }
                if(strcmp(rargs[i], ">>") == 0)
                {
                    dredir++;

                }
                if(strcmp(rargs[i], "|") == 0)
                {
                    pipes++;

                }
            }








            int sh;
            if(record)
            {

                char inp[input.size()+1];
                strcpy(inp,input.c_str());
                inp[input.size()] = '\n';
                write(scriptfile,inp,input.size()+1);
            }

            if( strcmp(rargs[0], "record") == 0)
            {


                if ( tokcount == 2 &&  strcmp(rargs[1], "start") == 0 )
                {



                    cout << "you are under surveillance" << endl;



                    scriptfile = open("scriptfile.txt", O_CREAT | O_WRONLY | O_TRUNC,S_IRWXU);
                    if(scriptfile == -1)
                        cout << "unable to open scriptfile, make sure you have appropriate permissions" << endl;
                    record = true;
                    continue;



                }
                else if (  tokcount == 2 && strcmp(rargs[1], "stop") == 0)
                {
                    if(record)
                    {


                        close(scriptfile);
                        record = false;
                        cout << "recording stopped" << endl;
                        continue;


                    }
                    else
                    {
                        cout << "not recording" << endl;

                    }

                }



            }
            /*
                                setenv("record", "true", 1);

                                int mypip[2], pipe3[2];
                                pipe(mypip);
                                pipe(pipe3);

                                sh = fork();
                                if(sh == 0)
                                {

                                    dup2(mypip[1], 1);
                                    dup2(pipe3[1],3);
                                    close(mypip[0]);
                                    close(mypip[1]);
                                    close(pipe3[0]);
                                    close(pipe3[1]);

                                    string shellpath = "/home/divy/Documents/OS/1/Shell/bin/Debug/Shell";     //TODO MAY NEED TO RUN PWD TO GET THIS PATH just when prog runs

                                    char * args[2] {"Shell",NULL};
                                    execv(shellpath.c_str(),args);
                                    cout << "couldn't run shell";


                                }
                                */
//                    int out = fork();
//                    if(out == 0) {
//                        dup2(mypip[0], 0);
//
//
//                        close(mypip[0]);
//                        close(mypip[1]);
//
//
//                    }

//was attempting to read pipe file
//                    int scriptfile = open("/home/divy/Documents/OS/1/Shell/bin/Debug/scriptfile", O_CREAT | O_WRONLY | O_TRUNC,777); //TODO
//                    int status;
//
//                    int count;
//                    while(status = waitpid(sh,NULL,WNOHANG ) == 0)
//                    {
//
//                        int total = 0;
//                        char * buff;
//
//                        while( count = read(mypip[0], buff,  4096) != -1 )
//                        {
//                            cout << "read from buffer" << endl;
//                            total+=count;
//
//                            cout << buff;
//
//                            write(scriptfile,buff,count);
//                        }
//
//
//                    }
// reading pipe through stdin
            //  int savedin = dup(0);
            // dup2(mypip[0],0);
            //TODO
//
//                    int status,cnt ;
//                    char c[5];
//                    //  FILE * cmdfile = fdopen(0,"r");
//                    while(status = waitpid(sh,NULL,WNOHANG ) == 0)
//                    {
//
//                        //     string buff;
//                        //   getline(cin, buff);         //only total 44 read
//                        //       while (( c = getc(cmdfile))!= EOF)
//                        while ( read(mypip[0], &c,1) !=0 )
//                        {
//                            //buff = buff + "\n";
//                            //   scriptfile << c;
//                            // putc(c,scriptfile);
//                            //       putc(c, script);
//                            read(mypip[0], &c, 1);
//                            write(scriptfile, &c, 1);
//
//                            //   cout.put(c);
//                            putchar(c[0]);
//                            //  scriptfile.write(buff.c_str(), buff.size());
//                            //    cout << c;
//                            //  cout.write(buff.c_str(), buff.size());
//
//                            //            count = write(scriptfile,buff.c_str(),count);
//                        }
//
//


//                        scriptfile << buff;
//                        cout << buff;
//                        char cmd [1000];
//                        read(pipe3[0],cmd, 1000);
//                        //  scriptfile << cmd;
//                        cout << endl << cmd << " written to file";
//                        write(scriptfile, cmd, 1000);
//                        //       (stdin,buff);
            //  scanf("%[^\n]s ",&buff);

//                        while ((c = getc(cmdfile))!= EOF)
//                        {
//                            //buff = buff + "\n";
//                            // scriptfile << c;
//                            //  scriptfile.write(buff.c_str(), buff.size());
//                            //cout << c;
//                            //  cout.write(buff.c_str(), buff.size());
//
//                            //            count = write(scriptfile,buff.c_str(),count);
//                            //          scriptfile.put(c);
//                            write(scriptfile, &c, 1);
//
//                            //   cout.put(c);
//                            putchar(c);
//                        }
//





            //    scriptfile << flush;

//                    close(mypip[0]);
//                    close(mypip[1]);
//                    close(pipe3[0]);
//                    close(pipe3[1]);
//
//                    //     dup2(savedin, 0);
//                    unsetenv("record");
//                    record = false;
//                    continue;

            //     }






            if(strcmp(rargs[0], "alias") ==0)
            {
                string tmp(rargs[1]);
                string tmp2 (rargs[3] );
                alias[tmp] = tmp2;
            }


            string firsttoken(rargs[0]);
            auto k = alias.find(firsttoken);
            if( k != alias.end() )
            {
                string aliasval = k->second;
                char * aliastok [MAXARGS];
                int aliascount = tokenize_string(aliasval, aliastok);
                int alargcount = tokcount + aliascount;
                char * alargs[alargcount];
                int i;
                for( i = 0; i < aliascount; i++)
                {
                    alargs[i] = aliastok[i];

                    // cout << trueargs[i] << endl;
                }

                for(int j = 1; j < tokcount - 1; j++)
                {
                    alargs[i++] = rargs[j];

                    // cout << trueargs[i] << endl;
                }
                //TODO DELETE ALL after num_tokens FROM ARGS AFTER COPY
                alargs[alargcount-1] = NULL;
                run_command(alargs,alargcount);

            }




            else if (strcmp(rargs[0],"cd") == 0)
            {
                if(tokcount == 1 || strcmp("~", rargs[1]) == 0)
                {
                    //no second argument
                    chdir( home.c_str());
                }
                else
                {
                    chdir(  rargs[1]);

                }

            }
            else if (dredir == 1 )
            {
                run_double_redirect(rargs,tokcount);
            }
            else if (rediri==1)
            {

                run_redirect(rargs, tokcount);
            }
            else if (pipes > 0)
            {
                run_pipes(rargs,tokcount,pipes);

            }
            else if (rediri >1)
            {
                cout << endl << "Syntax error" << endl;
            }


            else
            {

                run_command(rargs, tokcount);
            }











            for(int i = tokcount ; i >= 0; i-- )
            {
                delete[] rargs[i];
            }
            //no need to delete trueargs or args as they are not allocated using new

        }
    }

    return 0;
}
