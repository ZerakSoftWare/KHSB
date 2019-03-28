#ifndef __MAP_H__
#define __MAP_H__

#include <map>
#include <string.h>
#include <string>

using std::map;
using namespace std; 

void insert_map_relation(char * pA, char * pB);
void free_map_relation(char * pA);
void get_map_relation(char * pA, char * pOut);

void ali_insert_map_relation(char * pA, int B);
void ali_delete_map_relation(char * pA);
void ali_get_map_relation(char * pA, int *bOut);

void _insert_map_relation(map<string, int>& s_map, char * pA, int B);
void _delete_map_relation(map<string, int>& s_map, char * pA);
void _get_map_relation(map<string, int>& s_map, char * pA, int *bOut);

#endif
