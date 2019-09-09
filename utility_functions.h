#ifndef UTILITY_FUNCTIONS_H_INCLUDED
#define UTILITY_FUNCTIONS_H_INCLUDED

#define MAXARGS 400
#define MAX_ARG_LENGTH 100

using namespace std;
extern string cwd, ps1, user_name, home, host_name;
extern string path;
extern int scriptfile;
extern bool record;



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


#endif // UTILITY_FUNCTIONS_H_INCLUDED
