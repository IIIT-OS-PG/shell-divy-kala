#ifndef RUN_COMMANDS_H_INCLUDED
#define RUN_COMMANDS_H_INCLUDED

#define MAXARGS 400
#define MAX_ARG_LENGTH 100


using namespace std;
extern string cwd, ps1, user_name, home, host_name;
extern string path;

extern int scriptfile;
extern bool record;
extern int dolques;

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


        int fd = open(rargs[tokcount-1], O_WRONLY | O_TRUNC, S_IRWXU);
        dup2(fd,1);
        close(fd);
        string fullcom = path + rargs[0];
        //     execv( fullcom.c_str(), edargs);
        execvpe(rargs[0],edargs,environ);



    }
    else
    {
        int * status;
        waitpid(ret, status, 0);
         if(*status != 0) {

            dolques = WEXITSTATUS(*status);
        }
        else {
            dolques = 0;
        }




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


        int fd = open(rargs[tokcount-1], O_APPEND | O_WRONLY | O_CREAT, S_IRWXU);
        dup2(fd,1);
        close(fd);
        string fullcom = path + rargs[0];
        //    execv( fullcom.c_str(), edargs);
        execvpe(rargs[0],edargs,environ);


    }
    else
    {
        int * status;
        waitpid(ret, status, 0);
         if(*status != 0) {

            dolques = WEXITSTATUS(*status);
        }
        else {
            dolques = 0;
        }


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
          //  execvpe(trueargs[0],trueargs,environ);
            //   execv(fullcom.c_str(), trueargs);

        }
        else
        {
            string fullcom = path + trueargs[0];
            close(pip[1]);
            close(pip[0]);
            //  execv(fullcom.c_str(), trueargs);
           // execvpe(trueargs[0],trueargs,environ);
        }

         execvpe(trueargs[0],trueargs,environ);
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
        int * status;
        waitpid(ret,status,0);

        if(*status != 0) {

            dolques = WEXITSTATUS(*status);
        }
        else {
            dolques = 0;
        }

        //  cout << "ret term";
    }

    close(pip[1]);
    close(pip[0]);
//   waitpid(ret,NULL,0);

}

void run_pipes (char ** rargs, int tokcount, int pipes)
{

    int pip[2];
    pipe(pip);


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

                if(record)
                {
                    dup2(pip[1],1);

                }

                dup2(mypipe[i-1][0],0);
//                close(mypipe[i-1][0]);
//                close(mypipe[i-1][1]);

            }
            close(pip[1]);
            close(pip[0]);
            for(int i = 0; i < pipes; i++ )
            {

                close(mypipe[i][0]);
                close(mypipe[i][1]);
            }

            string fullcom = path + cmds[i][0];

            //if( execv(fullcom.c_str(), cmds[i]) == -1)
            if(execvpe(cmds[i][0],cmds[i],environ) == -1)
            {
                cout << "\ncommand could not be executed\n";
                exit(-1);
            }
        }

    }
    close(pip[1]);
    for(int i = 0; i < pipes; i++ )
    {
        close(mypipe[i][0]);
        close(mypipe[i][1]);
    }



    if(record)
    {

        int k = fork();
        if(k==0)
        {

            char c[100];
            int count;

            int con;

            //   while (ioctl(pip[0], FIONREAD, &con) == 0 && con > 0)
            while ( (count = read(pip[0], c,100) ) > 0 )
            {
                write(scriptfile, c, count);
                write(1,c,count);
            }

            close(pip[0]);
            _exit(0);

        }


        //    cout << "k term";

    }
    close(pip[0]);


    for(auto i = child_pids.begin(); i != child_pids.end(); i++)
    {
        //  cout << "waitig for " << *i <<endl;
//        if(i==child_pids.back) {
//            kill(*i, 0);
//        }
        int * status;

        waitpid(*i,status,0);
           if(*status != 0) {

            dolques = WEXITSTATUS(*status);
        }
        else {
            dolques = 0;
        }

    }



    // while(wait(NULL) != -1) ;
//    wait(NULL);


//TODO delete pipes



}

void redirect_from_pipe (char ** rargs, int tokcount, int pipes)
{


    char * cmds[pipes+1][MAXARGS];
    string filepath = rargs[tokcount-1];
    tokcount = tokcount -2;

    int  writefile = open(filepath.c_str(), O_WRONLY | O_CREAT| O_TRUNC, S_IRWXU);
    int cmdi = 0;
    int j = 0;
    for(int i =0 ; i < tokcount; i++)
    {
        if(strcmp(rargs[i], "|" ) == 0 || strcmp(rargs[i], ">" ) == 0)
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
                dup2(writefile, 1);

//                close(mypipe[i-1][0]);
//                close(mypipe[i-1][1]);

            }
            close(writefile);
            for(int i = 0; i < pipes; i++ )
            {
                close(mypipe[i][0]);
                close(mypipe[i][1]);

            }

            string fullcom = path + cmds[i][0];

//            if( execv(fullcom.c_str(), cmds[i]) == -1)
            if(execvpe(cmds[i][0], cmds[i],environ) == -1)
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

    close(writefile);
    for(auto i = child_pids.begin(); i != child_pids.end(); i++)
    {
        //  cout << "waitig for " << *i <<endl;
//        if(i==child_pids.back) {
//            kill(*i, 0);
//        }
        int * status;
        waitpid(*i,status,0);
           if(*status != 0) {

            dolques = WEXITSTATUS(*status);
        }
        else {
            dolques = 0;
        }

    }



    // while(wait(NULL) != -1) ;
//    wait(NULL);


//TODO delete pipes



}

void dredirect_from_pipe (char ** rargs, int tokcount, int pipes)
{


    char * cmds[pipes+1][MAXARGS];
    string filepath = rargs[tokcount-1];
    tokcount = tokcount -2;

    int  writefile = open(filepath.c_str(), O_APPEND | O_CREAT | O_WRONLY, S_IRWXU);
    int cmdi = 0;
    int j = 0;
    for(int i =0 ; i < tokcount; i++)
    {
        if(strcmp(rargs[i], "|" ) == 0 || strcmp(rargs[i], ">>" ) == 0)
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
                dup2(writefile, 1);

//                close(mypipe[i-1][0]);
//                close(mypipe[i-1][1]);

            }
            for(int i = 0; i < pipes; i++ )
            {
                close(mypipe[i][0]);
                close(mypipe[i][1]);

            }
            close(writefile);
            string fullcom = path + cmds[i][0];

            //     if( execv(fullcom.c_str(), cmds[i]) == -1)
            if(execvpe(cmds[i][0],cmds[i],environ) == -1)
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

    close(writefile);
    for(auto i = child_pids.begin(); i != child_pids.end(); i++)
    {
        //  cout << "waitig for " << *i <<endl;
//        if(i==child_pids.back) {
//            kill(*i, 0);
//        }
        int * status;
        waitpid(*i,status,0);
           if(*status != 0) {

            dolques = WEXITSTATUS(*status);
        }
        else {
            dolques = 0;
        }

    }



    // while(wait(NULL) != -1) ;
//    wait(NULL);


//TODO delete pipes



}


#endif // RUN_COMMANDS_H_INCLUDED
