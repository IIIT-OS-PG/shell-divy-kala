#ifndef SETUP_ENV_H_INCLUDED
#define SETUP_ENV_H_INCLUDED

extern string cwd, ps1, user_name, home, host_name;
extern string path; //NOTE THAT PATH IS EXPECTED TO END WITH /


void setup_environ ()
{

    char * logn = getlogin();
    if(logn != NULL)
    {
        string tmp(logn);
        user_name = tmp;

        char envname[user_name.size()+5];
        strcpy(envname,("NAME="+user_name).c_str());
        putenv(envname);
    }
    else
    {
        cout << "Login name could not be read, change terminal and try again";
    }


    char tmpc[100];
    gethostname(tmpc,100);
    string tmp2(tmpc);
    host_name = tmp2;
    char envhost[host_name.size()+5];
    strcpy(envhost,("HOST="+host_name).c_str());
    putenv(envhost);


    struct passwd * thispwd = getpwnam(user_name.c_str());
    string  ttt (thispwd->pw_dir);
    home = ttt;
    char envhome[home.size()+5];
    strcpy(envhome,("HOME="+home).c_str());
    putenv(envhome);


    int user = getuid();
    ps1 = "#";
    if(user)
        ps1 = "$";





    string env_path = "shell_environ";
    ifstream shellrc (env_path);
    string line;
    if(shellrc.is_open())
    {
        while(getline(shellrc, line, '='))
        {

            if(line == "PATH" )
            {

                getline(shellrc,line);
                line = line.substr(1,line.size()-2);
                path = line;
                char envpath[line.size()+5];
                strcpy(envpath,("PATH="+line).c_str());
                putenv(envpath);

            }
        }
    }



    return;
}


#endif // SETUP_ENV_H_INCLUDED
