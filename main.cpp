#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <ctime>
#include <limits>
#include <vector>
#include <random>
#include <iomanip>
#include <libpq-fe.h>
struct user
{
    int id;
    std::string password,username;
    public:
    bool checkpass(std::string& password){
        bool alpha=false, number=false,special=false,upper=false,lower=false,space=false;
        if (password.length()>=8)
        {
         for(char a : password){
            unsigned char c = static_cast<unsigned char>(a);
             if(std::isalpha(c))   alpha = true ;
             if(std::isdigit(c) )  number = true ; 
            if(std::isupper(c) )  upper = true ;
            if(std::islower(c) )  lower = true ;
            if(!std::isalnum(c)  && !std::isspace(c) )  special = true ;
            if(std::isspace(c) )  space = true ;
         }
         if (alpha && number && special && upper && lower && !space)
         {
            return true;
         }else{
            if (!alpha)
            {
                std::cout << "Please enter at least two letters (1 upper case and 1 lower case)\n";
                return false;
            }else if(!number){
                std::cout << "PLease enter at lease one number\n";
                return false;
            }else if (!special)
            {
                std::cout << "PLease enter at least one special character(@#$., etc..)\n";
                return false;
            }else if (!upper)
            {
                std::cout << "PLease enter at least one upper case letter\n";
                return false;
            }else if (!lower)
            {
               std::cout << "PLease enter at least one lower case letter\n";
               return false;
            }else if (space)
            
                std::cout << "Please remove spaces\n";
                return false;
            }
         }
         else
         std::cout << "Your password is too short make sure it's 8 characters long\n";
         return false;
        }
    };


void today(int& day,int& month, int& year){
    time_t now = time(nullptr);
    struct tm* local = localtime(&now);
    day = local->tm_mday;
    month = local->tm_mon;
    year = local->tm_year+1900;
}
void connection(PGconn* conn){
    if (PQstatus(conn) == CONNECTION_OK)
    {
        std::cout << "Connection is successful\n";
    }else{
        std::cout << "Connection error: " << PQerrorMessage(conn) <<'\n';
    }
    
}
template <typename T>
void valid(T& a){
while (true)
{
    if (std::cin >> a)
    {
        if (std::cin.peek() == '\n' && a >= 0)
        {
           break;
        }
    }
    std::cout << "Invalid input please try again\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits <std::streamsize>::max(),'\n');
}
}
void state(std::vector <std::string>& statement,int& cal,int& sum,int& goal){
    time_t now = time(nullptr);
    std::string timing = std::string(ctime(&now));
    std::string calo = std::to_string(static_cast<double>(cal)/100),summa=std::to_string(goal-static_cast<double>(sum)/100);
    std::string str =  timing.substr(0,timing.length()-1) + ": " + calo.substr(0,calo.length()-4) + " cal     |   " + summa.substr(0,summa.length()-4) + " left to reach " + std::to_string(goal) +" cal"; 
statement.emplace_back(str);
}
void writeuser(PGconn* conn,user& users){
std::string id = std::to_string(users.id).c_str();
const char* param[3];
param[0] = id.c_str();
param[1] = users.username.c_str();
param[2] = users.password.c_str();
PGresult* res = PQexecParams(
    conn,
    "Insert into app_user "
    "(user_id, username, password) "
    "values ($1,$2,$3) ;",
    3,
    NULL,
    param,
    NULL,
    NULL,
    0
);
if (PQresultStatus(res) != PGRES_COMMAND_OK)
{
    std::cout << "Something went wrong error: " << PQerrorMessage(conn);
}

PQclear(res);
}
void updateuser(PGconn* conn,user& users){
    std::string id = std::to_string(users.id);
    const char* param[3];
    param[0] = id.c_str();
    param[1] = users.username.c_str();
    param[2] = users.password.c_str();
    PGresult* res = PQexecParams(
        conn,
        "update app_user "
        "set username = $2 password = $3 "
        "where user_id = $1 ;",
        3,
        NULL,
        param,
        NULL,
        NULL,
        0
    );
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cout << "Something went wrong error: " << PQerrorMessage(conn);
    }
    
    PQclear(res);
}
bool readuser(PGconn* conn, std::vector<user>& users){
    user use;
    PGresult* res = PQexec(
        conn,
        "select user_id, username, password"
        " from app_user "
    );
    int rows = PQntuples(res);
    if(rows > 0){
    for (size_t i = 0; i < rows; i++)
    {
        use.id = std::stoi(PQgetvalue(res,i,0));
        use.username = PQgetvalue(res,i,1);
        use.password = PQgetvalue(res,i,2);
        users.emplace_back(use);
    }
    return true;
    }
    PQclear(res);
    return false;
}
void deleteuser(PGconn* conn,user& users){
std::string id = std::to_string(users.id);
const char* param[1];
param[0] = id.c_str();
PGresult* res = PQexecParams(
    conn,
    "delete from app_user "
    "where user_id = $1; ",
    1,
    NULL,
    param,
    NULL,
    NULL,
    0
);
PQclear(res);
}
void write(std::vector <std::string>& statement,PGconn* conn,user& users){
    std::string id = std::to_string(users.id);
    const char* paramvalue[2];
    paramvalue[0] = statement[statement.size()-1].c_str();
    paramvalue[1] = id.c_str();
    PGresult* res = PQexecParams(
        conn,
        "Insert into state "
        "(para, user_id) "
        "values ($1, $2)",
        2,
        NULL,
        paramvalue,
        NULL,
        NULL,
        0
    );
    PQclear(res);
}
void readstatement(std::vector<std::string>& state,PGconn* conn,user& users){
    std::string id = std::to_string(users.id);
    const char* param[1];
    param[0] = id.c_str();
    PGresult* res = PQexecParams(
        conn,
        "select para "
        "from state "
        "where user_id = $1",
        1,
        NULL,
        param,
        NULL,
        NULL,
        0
    );
    int rows = PQntuples(res);
    std::string str;
if (rows >= 1)
    for (size_t i = 0; i < rows; i++)
    {
        str = PQgetvalue(res,i,0);
        state.emplace_back(str);
    }
    PQclear(res);
}
void deletestatement(PGconn* conn,user& users){
    std::string id = std::to_string(users.id);
    const char* param[1];
    param[0] = id.c_str();
    PGresult* res = PQexecParams(
        conn,
        "delete from state"
        " where user_id = $1",
        1,
        NULL,
        param,
        NULL,
        NULL,
        0
    );
    PQclear(res);
}
void writelatest(int& day,int& sum,int& goal,int& month, int& year,PGconn* conn,user& users){
    const char* paramvalues[6];
std::string id = std::to_string(users.id);
std::string g = std::to_string(goal);
std::string s = std::to_string(sum);
std::string d = std::to_string(day);
std::string m = std::to_string(month);
std::string y = std::to_string(year);

paramvalues[0] = g.c_str();
paramvalues[1] = s.c_str();
paramvalues[2] = d.c_str();
paramvalues[3] = m.c_str();
paramvalues[4] = y.c_str();
paramvalues[5] = id.c_str();
    PGresult* res = PQexecParams(
        conn,
        "insert into latest "
        "(goal, sum, day, month, year, user_id)"
        "values ($1,$2,$3,$4,$5,$6) ",
        6,
        NULL,
        paramvalues,
        NULL,
        NULL,
        0
    );
    PQclear(res);
}
bool readlatest(int& day,int& sum,int& goal,int& month, int& year,PGconn* conn, user& users){
std::string id = std::to_string(users.id);
const char* param[1];
param[0] = id.c_str();
PGresult* res = PQexecParams(
conn,
"select goal, sum, day, month, year "
"from latest "
"where user_id = $1 ;",
1,
NULL,
param,
NULL,
NULL,
0
);
int rows = PQntuples(res);
if (rows >= 1)
{
goal = std::stoi(PQgetvalue(res,0,0));
sum = std::stoi(PQgetvalue(res,0,1));
day = std::stoi(PQgetvalue(res,0,2));
month = std::stoi(PQgetvalue(res,0,3));
year = std::stoi(PQgetvalue(res,0,4));
    return true;
}
PQclear(res);
return false;
}
void updatelatest(int& day,int& sum,int& goal,int& month, int& year,PGconn* conn,user& users){
    const char* paramvalues[6];
std::string id = std::to_string(users.id);
std::string g = std::to_string(goal);
std::string s = std::to_string(sum);
std::string d = std::to_string(day);
std::string m = std::to_string(month);
std::string y = std::to_string(year);

paramvalues[0] = g.c_str();
paramvalues[1] = s.c_str();
paramvalues[2] = d.c_str();
paramvalues[3] = m.c_str();
paramvalues[4] = y.c_str();
paramvalues[5] = id.c_str();
    PGresult* res = PQexecParams(
        conn,
        "update latest "
        "set goal = $1 , sum = $2, day = $3, month= $4, year = $5"
        " where user_id = $6",
        6,
        NULL,
        paramvalues,
        NULL,
        NULL,
        0
    );
    PQclear(res);
}
bool finduser(std::vector<user>& v_users, user& users){
    for (size_t i = 0; i < v_users.size(); i++)
    {
        if (v_users[i].username == users.username)
        {
            if (v_users[i].password == users.password)
            {
                users.id = v_users[i].id;
                return true;
            }
    }
    }
    std::cout << "Incorrect username or password please try again!\n";
    return false;
    
}
bool findusername(std::vector<user>& v_users, user& users){
    for (size_t i = 0; i < v_users.size(); i++)
    {
        if (v_users[i].username == users.username)
        {
            std::cout << "Username already exist try another one\n";
            return true;
        }
    }return false;
}
void userid(std::vector <user>& v_users,user& users){
for (size_t i = 0; i < v_users.size(); i++)
{
    users.id = v_users[i].id + 1;
}

}
int main(){
    std::cout << std::fixed << std::setprecision(2);
    std::vector <std::string> statement;
    std::vector <user> v_users;
    user users;
    double calories;
    int sum=0,cal,goal=0,day,mon,year,day1,mon1,year1,option=0,option2=0,index;
    char cond;
    bool login =false;
    PGconn* conn = PQconnectdb(
        "host=aws-1-ap-southeast-1.pooler.supabase.com dbname=postgres port=6543 user=postgres.sxnjyosvswjpscqbokdl password=pass"
    );
    connection(conn);
    while (!login && option2 != 7)
    {
    if (!readuser(conn,v_users))
    {
        std::cout << "Welcome first user ever!\n Before you start please create an account!\n";
        std::cout << "Enter the username you'd like: ";
        std::cin >> users.username;
        do{
            std::cout << "Enter your password: ";
        std::cin >> users.password;
        }while (!users.checkpass(users.password));
        users.id = 1;
        writeuser(conn,users);
        login =true;
    }else{
        while (option !=3 && !login)
        {
        std::cout << "Would you like to:\n1- Login\n2- Create an account\n3- Exit\n";
        std::cin >> option;
        switch (option)
        {
        case 1:
            std::cout << "Enter your username: ";
            std::cin >> users.username;
            std::cout << "Enter your password: ";
            std::cin >> users.password;
            if(finduser(v_users,users)){
                login =true;
            }
            break;
        case 2:
        {
            std::string pass;
        std::cout << "Enter your username: ";
        std::cin >> users.username;
        do{
            std::cout << "Enter your password: ";
            std::cin >> users.password;
        }while(!users.checkpass(users.password));
        std::cout << "Re-enter your password: ";
        std::cin >> pass;
        if (pass == users.password)
        {
            userid(v_users,users);
            std::cout << "Your account has been succesfully created!\n";
            writeuser(conn,users);
            login=true;
        }else
        std::cout << "Incorrect password please re-create your account\n";
        break;
    }
        default:
            break;
        }
    }
}
    if(login){
    std::cout <<"Welcome to the calorie app\n";
        if(!readlatest(day,sum,goal,mon,year,conn,users)){
        std::cout << "Please put your daily goal: ";
        valid(goal);
        today(day1,mon1,year1);
        writelatest(day1,sum,goal,mon1,year1,conn,users);
        }else{
        readstatement(statement,conn,users);
        readlatest(day,sum,goal,mon,year,conn,users);
        }
        today(day1,mon1,year1);
if (day == day1 && mon == mon1 && year == year1)
{
}else{
    sum =0;
    updatelatest(day1,sum,goal,mon1,year1,conn,users);
}
while (option != 7 && login)
{
    if(sum/100 < goal)
    std::cout << "You have ate " << static_cast<double> (sum)/100 << " cal / " << goal << " cal, you have to eat " << goal- static_cast<double>(sum)/100 << " cal\n";
    if (sum/100 >= goal)
    {
    std::cout << "You have ate " << static_cast<double> (sum)/100 << " cal / " << goal << " cal, you have reached/surpassed your goal\n";
    }
    std::cout << "Would you like to:\n1- Add the calories\n2- Remove calories\n3- Modify your goal\n4- View your last intake\n5- Delete your list of intakes\n6- Edit your account\n7- Exit\n";
    valid(option);
    switch (option)
    {
    case 1:
        std::cout << "How much calories did you consumed: ";
        valid(calories);
        cal = std::round(calories*100);
        sum+=cal;
        state(statement,cal,sum,goal);
        write(statement,conn,users);
        updatelatest(day1,sum,goal,mon1,year1,conn,users);
        std::cout <<"Your calories has been added\n";
        break;
    case 2:
    std::cout << "How much calories do u want to remove: ";
        valid(calories);
        cal = std::round(calories*100);
        sum-=cal;
        state(statement,cal,sum,goal);
        write(statement,conn,users);
        updatelatest(day1,sum,goal,mon1,year1,conn,users);
        std::cout <<"Your calories has been removed\n";
    break;
    case 3:
    std::cout << "What is your new goal: ";
    valid(goal);
    updatelatest(day1,sum,goal,mon1,year1,conn,users);
    std::cout <<"Your goal have been changed \n";
    break;
    case 4:
    for (size_t i = 0; i < statement.size(); i++)
    {
        std::cout << i+1 << ") " <<statement[i] <<"\n";
        std::cout << "* * * * * * * * * * * * * * *\n";
    }
    break;
    case 5:
    std::cout << "Are you sure you want to delete the list? y/n: ";
    std::cin>> cond;
    std::cin.ignore(std::numeric_limits <std::streamsize>::max(),'\n');
    if (tolower(cond) == 'y')
    {
        statement.clear();
        deletestatement(conn,users);
        std::cout << "Your last intake list has been deleted\n";
    }else
    std::cout << "Exiting the page...\n";
    break;
    case 6:
    std::cout << "Would you like to: \n1- Edit your username\n2- Edit your password\n3- Delete your account\n4- Logout\n";
    valid(option2);
    switch (option2)
    {
    case 1:
       do{ std::cout << "Enter your new username: ";
        std::cin >> users.username;
        } while(finduser(v_users,users));
        updateuser(conn,users);
        break;
    case 2:
    do
    {
        std::cout << "Enter your new password: ";
        std::cin >> users.password;
    } while (users.checkpass(users.password));
    updateuser(conn,users);
    break;
    case 3:
    std::cout << "Are you sure you want to delete your account? y/n";
    std::cin >> cond;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    if (towlower(cond) == 'y' )
    {
        deleteuser(conn,users);
        statement.clear();
        v_users.clear();
        std::cout << "Your account has been deleted successfully exiting...\n";
        login=false;
        break;
    }
    case 4:
    std::cout << "Are you sure you want to logout? y/n: ";
    std::cin >> cond;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    if (tolower(cond) == 'y')
    {
        statement.clear();
        v_users.clear();
        login =false;
    }else
    std::cout << "Returning...\n";
    break;
    default:
        break;
    }
    break;
    case 7:
    std::cout << "Exiting...";
    PQfinish(conn);
    break;
    default:
        std::cout << "Invalid input try again\n";
        break;
    }
}
}
}
}
