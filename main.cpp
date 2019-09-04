#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#define MAXARGS 400
#define MAX_ARG_LENGTH 100

using namespace std;

string cwd, ps1, user_name, home;
string path; //NOTE THAT PATH IS EXPECTED TO END WITH /

void prompt()
{
    path = "/bin/";
    user_name = getenv("USER");
    char* buff = new char[4096];
    getcwd(buff, 4096);
    cwd = getcwd(buff,4096);
    //cout << endl;
    cout  << user_name << ":" << cwd <<":" << ps1;

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
//        //   cout << tokens[i] << " Inserted. " << endl;
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
////    cout << "ret";
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


        int fd = open(rargs[tokcount-1], O_WRONLY);
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

    int ret = fork();

    if (ret == -1)
    {

        cout << "Command failed to execute, the process could not be started\n";
    }
    else if(ret == 0)
    {




//        int num_tokens = tokenize_string(com, args);


        // cout << "command "  << fullcom <<  endl << "0 arg "<< args[0] << endl << "1 arg "  << args[1]  << endl;
        //    if( args[2] == NULL) cout << "NULL" ;
        //    cout << "args1 read" << endl;
//        char * trueargs[num_tokens+1]; // TRUE ARGS MUST BE NULL TERMINATED ARRAY
//        for(int i = 0; i < num_tokens; i++) {
//            trueargs[i] = args[i];
//           // cout << trueargs[i] << endl;
//        }
        //TODO DELETE ALL after num_tokens FROM ARGS AFTER COPY
//        trueargs[num_tokens] = NULL;
//


        string fullcom = path + trueargs[0];

        execv(fullcom.c_str(), trueargs);
        //      cout << "error" << errno << endl << fullcom << endl;

        //    cout << "execed";




    }
    else
    {

        wait(NULL);
    }

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
    ps1 = "$";
    home = getenv("HOME");
    string input = "";

    map<string,string> alias;

    while(input != "exit")
    {




        prompt();

        getline(cin, input);
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






            if(input == "exit")
                return 0;


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
