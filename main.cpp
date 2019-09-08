#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include "run_commands.h"
#include "utility_functions.h"


using namespace std;

string cwd, ps1, user_name, home;
string path; //NOTE THAT PATH IS EXPECTED TO END WITH /

int scriptfile;
bool record = false;



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
            else if (rediri ==1 && pipes > 0)
            {
                redirect_from_pipe(rargs,tokcount,pipes);

            }
            else if (dredir ==1 && pipes > 0)
            {
                dredirect_from_pipe(rargs,tokcount,pipes);


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
