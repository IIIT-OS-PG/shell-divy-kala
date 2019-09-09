#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <stdlib.h>
#include <time.h>
#include "utility_functions.h"
#include "run_commands.h"
#include "setup_env.h"
#include "alarm.h"
#define MAXARGS 400
#define MAX_ARG_LENGTH 100


using namespace std;

string cwd, ps1, user_name, home, host_name;
string path; //NOTE THAT PATH IS EXPECTED TO END WITH /
map<string,pair<string,bool>> vars;
int scriptfile;
int dolques;
bool record = false;
map<string,string> filemapping;


int main(int argc, char *argv[], char *envp[])
{

    setup_environ();
    string input = "";
    map<string,string> fm = filemapping;
    map<string,string> alias;


    while(input != "exit")
    {

        prompt();

        getline(cin, input);

        char * tokens[MAXARGS];
        int tokcount = tokenize_string(input, tokens);

        if(strcmp(input.c_str(), "exit")==0 && tokcount == 1)
            return 0;


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
            int equals = 0;
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
                else if(strcmp(rargs[i], "|") == 0)
                {
                    pipes++;

                }
                else if(strcmp(rargs[i], "=") == 0)
                {
                    equals++;

                }
                else if (strcmp(rargs[i], "~") == 0)
                {
                    char * temp = new char [home.size()];
                    strcpy(temp, home.c_str());
                    delete rargs[i];
                    rargs[i] = temp;
                }
                else if (strcmp(rargs[i], "$$") == 0)
                {

                    int pid = getpid();
                    string x = to_string(pid);
                    char * temp = new char [x.size()];
                    strcpy(temp, x.c_str());
                    delete rargs[i];
                    rargs[i] = temp;

                }
                else if (strcmp(rargs[i], "$?") == 0)
                {

                    int pid = dolques;
                    string x = to_string(pid);
                    char * temp = new char [x.size()];
                    strcpy(temp, x.c_str());
                    delete rargs[i];
                    rargs[i] = temp;

                }

                else if( rargs[i][0] == '$' )
                {
                    int len = strlen(rargs[i]) ;
                    char temp[len];
                    strcpy(temp, rargs[i]+1);
                    string cmd(temp);
                    auto j =  vars.find(cmd) ;
                    if( j != vars.end())
                    {
                        char * to = new char [ j->second.first.size() ];
                        const char * val = j->second.first.c_str();
                        strcpy(to, val  );
                        delete rargs[i];
                        rargs[i] = to;
                    }


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
            else if (strcmp(rargs[0],"open") == 0)
            {
                string name (rargs[1]);
                string revname = name;
                reverse(revname.begin(), revname.end());

                stringstream ss (revname);
                string r_ext;

                getline (ss, r_ext, '.');
                reverse(r_ext.begin(), r_ext.end());
                string ext = "." + r_ext;

                string sprog = filemapping[ext];
                char * prog  = new char [sprog.size()];
                strcpy (prog, sprog.c_str());
                delete rargs[0];
                rargs[0] = prog;
                run_command_bg(rargs, tokcount);


            }
            else if (strcmp(rargs[0],"remindme") == 0)
            {
                unsigned int secs = atoi(rargs[1]);
                set_alarm( secs);
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
            else if (rediri >1 || dredir > 1)
            {
                cout << endl << "Syntax error multiple redirection not allowed" << endl;
            }
            else if (strcmp(rargs[0],"export") == 0)
            {
                vars[rargs[1]].second = true;
                char tmp [ vars[rargs[1]].first.size() ];
                strcpy( tmp, vars[rargs[1]].first.c_str());
                export_var(rargs[1], tmp );
            }


            else if (equals ==1 && strcmp(rargs[1], "=") == 0)
            {

                bool exists = vars.find(rargs[0]) != vars.end();
                if(!exists)
                {
                    vars[rargs[0]] = make_pair(rargs[2], false);
                }
                else
                {
                    bool exported = vars[rargs[0]].second;

                    if(exported)
                    {
                        export_var(rargs[0], rargs[2]);
                        vars[rargs[0]] = make_pair(rargs[2], true);


                    }
                    else
                    {
                        vars[rargs[0]] = make_pair(rargs[2], false);
                    }

                }
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
