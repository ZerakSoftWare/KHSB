
#include "map.h"

using std::map;
using namespace std; 


map<string, string> stdmap;

void insert_map_relation(char * pA, char * pB)
{
    stdmap.insert(make_pair(pA, pB));
    
    return;
}

void free_map_relation(char * pA)
{
    stdmap.erase(pA);
    
    return ;
}

void get_map_relation(char * pA, char * pOut)
{   
    map<string, string>::iterator key = stdmap.find(pA);  
    if( key != stdmap.end() )  
    {  
        string tmpString = key->second;
        strcpy((char *)pOut, (const char *)tmpString.c_str());
    }
    
    return ;
}

map<string, int> alimap;

void ali_insert_map_relation(char * pA, int B)
{
    alimap.insert(make_pair(pA, B));
    
    return;
}

void ali_delete_map_relation(char * pA)
{
    alimap.erase(pA);
    
    return ;
}

void ali_get_map_relation(char * pA, int *bOut)
{   
    map<string, int>::iterator key = alimap.find(pA);
    if( key != alimap.end() )  
    {  
        int tmpInt = key->second;
        *bOut = tmpInt;
    }
    
    return ;
}


void _insert_map_relation(map<string, int>& s_map, char * pA, int B)
{
    s_map.insert(make_pair(pA, B));
    
    return;
}

void _delete_map_relation(map<string, int>& s_map, char * pA)
{
    s_map.erase(pA);
    
    return ;
}

void _get_map_relation(map<string, int>& s_map, char * pA, int *bOut)
{   
    map<string, int>::iterator key = s_map.find(pA);  
    if( key != s_map.end() )  
    {  
        int tmpInt = key->second;
        *bOut = tmpInt;
    }
    
    return ;
}


