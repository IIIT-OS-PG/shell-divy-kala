#ifndef ALARM_H_INCLUDED
#define ALARM_H_INCLUDED

void set_alarm ( unsigned int secs)
{
    unsigned int uptime;
    uptime = time(NULL) + secs;
    int writer = fork();
    if (writer == 0)
    {

        string alfile = ".alarm";
        ofstream alstream (alfile, ios_base::app);
        string line;
        if(alstream.is_open())
        {
            alstream << uptime << endl;

        }
        _exit(0);




    }

    int pid = fork();

    if(pid == 0)
    {

        if (sleep( secs-1 ) == 0);
        while( time(NULL) != uptime) ;
        time_t t = (time_t) time(NULL) - secs;
        cout << endl <<"ALARM ALARM ALARM" << endl << "This alarm was set at: " << ctime(&t) << endl;
        _exit(0);


    }


}

#endif // ALARM_H_INCLUDED
