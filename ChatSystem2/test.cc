#include<iostream>
#include<unordered_map>
#include<string>
using namespace std;

typedef struct User
{
    int id;
    string name;
}User;

int main(int argc ,char *argv[])
{
    unordered_map<int,struct User> user;
    User u={1,"lxk"};
    user.insert(1,&u);
    cout<<user.size()<<endl;
}