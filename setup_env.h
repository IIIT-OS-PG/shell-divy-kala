#ifndef SETUP_ENV_H_INCLUDED
#define SETUP_ENV_H_INCLUDED

extern string cwd, ps1, user_name, home, host_name;
extern string path; //NOTE THAT PATH IS EXPECTED TO END WITH /
extern map<string,pair<string,bool>> vars;
extern map<string,string> filemapping;
void import_env_var();
void export_var (char*, char* );


void setup_environ ()
{


    struct passwd * thispwd = getpwuid(getuid());
    string  ttt (thispwd->pw_dir);
    home = ttt;
    char * envhome = new char [home.size()+5];
    strcpy(envhome,("HOME="+home).c_str());
    putenv(envhome);


    char * logn = thispwd->pw_name;
    if(logn != NULL)
    {
        string tmp(logn);
        user_name = tmp;

        char * envname = new char[user_name.size()+5];
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
    char * envhost = new char [host_name.size()+5];
    strcpy(envhost,("HOST="+host_name).c_str());
    putenv(envhost);


    int user = getuid();
    ps1 = "#";
    if(user)
        ps1 = "$";


    string env_path = "shell_environ";
    ifstream shellrc (env_path);
    string line;
    if(shellrc.is_open())
    {
        while(getline(shellrc, line))
        {
            if(line == "\n" || line == "")
                continue;

            stringstream ss (line);
            string tok;
            getline (ss,tok, '=');
            string key = tok;
            if(tok[0] == '.')
            {


                getline(ss,tok);
                string value = tok;
                filemapping[key] = value;

            }
            else
            {

                getline(ss,tok);

                string val = tok;
                char* envpath = new char [line.size()];
                strcpy(envpath,(line).c_str());
                putenv(envpath);
                vars[key] = make_pair(val,true);

            }



            /*    else if (line == "[mapping]" ) {

                    while( getline(shellrc, line) && line != "[/mapping]") {
                        getline (ss,tok, '=');
                        string key = tok;
                        getline(shellrc,tok);
                        string value = tok;
                        filemapping[key] = value;
                        cout << key << " " << value << endl;

                    }


                }*/
        }
    }
    else
    {
        cout << endl << "could not open shell_environ, is it in the current directory?";
    }

    import_env_var();

    return;
}


void import_env_var()
{

    vars["HOME"] = make_pair(home,true);
    vars["USER"] = make_pair(user_name,true);
//   vars["PATH"] = make_pair(path,true);
    vars["HOST"] = make_pair(host_name,true);
    vars["PS1"] = make_pair(ps1,true);


    char * cvars = getenv("VARS");

    if(cvars == NULL)
    {
        return;
    }
    else
    {

        string svars (cvars);
        stringstream vstream (svars);
        string token;

        while( getline(vstream, token, ';') )
        {

            stringstream tstream (token);
            string key,val;
            //  string val;
            getline(tstream, key, '=');
            getline(tstream,val);
            vars[key] = make_pair(val,true);

        }

    }


}


void export_var (char* rarg0, char* rarg2)
{
    char * cvars = getenv("VARS");
    string  k(rarg0), v(rarg2);
    if(cvars == NULL)
    {

        char * pass = new char [ ("VARS="+k+"="+v+";").size() ];
        strcpy ( pass, ("VARS="+k+"="+v+";").c_str());
        putenv(pass);
        char * tmp =   getenv("VARS");
        //   cout << endl << "reading from new environ " <<tmp << endl;


    }
    else
    {
        // cout << "inside else cvars " << cvars << endl;
        string svars (cvars);
        stringstream vstream (svars);
        string token;
        string newvars = "VARS=";
        bool replaced = false;
        while( getline(vstream, token, ';') )
        {

            stringstream tstream (token);
            string key;
            //  string val;
            getline(tstream, key, '=');

            //    getline(tstream,val);
            if(strcmp(key.c_str(),rarg0) == 0)
            {
                string t (rarg2);
                string temp = key+"="+t+";";
                newvars = newvars + temp;
                replaced = true;
            }
            else
            {
                newvars = newvars + token +";";
            }
        }

        if(!replaced)
        {
            string temp = k+"="+v+";";
            newvars = newvars + temp;
        }
        char * cnewvars = new char [newvars.size()];
        strcpy(cnewvars, newvars.c_str());

        putenv(cnewvars);
        //  cout << endl << "modified vars : "<< cnewvars << endl;

    }

}




#endif // SETUP_ENV_H_INCLUDED
