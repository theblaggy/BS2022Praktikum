#include <stdbool.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>


typedef struct knoten_ {
    char key[1024];
    char value[1024];
}knoten;


//Array in dem alle Keys gespeichert werden
knoten keyArray[100];
int arraySize = 100;

//initalisiert alle Array-Felder mit null-bytes
void initializeKeyArray(){
    for(int i=0; i<arraySize;i++){
        for(int j=0;j<1024;j++) {
            keyArray[i].key[j] = '\0';
            keyArray[i].value[j] = '\0';
        }
    }
}


//return -1 if key not existent, saves key in res if existent
int get(char* key, char* res){

    for(int i=0; i<arraySize;i++) {

        if (strcmp(keyArray[i].key, key) == 0) {
            for (int k = 0; keyArray[i].value[k] != '\0'; k++) {
                res[k] = keyArray[i].value[k];
                res[k+1] = '\0';
            }
            return 1;
        }
    }
    return -1;
}

//return -1 if array full
int put(char* key, char* value){
    char  res[1024];
    if(get(key,res)==-1){ //falls Key noch nicht vorhanden, an leere Stelle einfügen
        for(int i=0; i<arraySize;i++){
            if(keyArray[i].key[0]=='\0'){
                strcpy(keyArray[i].key,key);
                strcpy(keyArray[i].value,value);
                return 1;
            }
        }
        return -1;
    }
    else{ //falls Key vorhanden, den Value ersetzen und den alten Value zurückgeben
        for(int i=0; i<arraySize;i++){
            if(strcmp(keyArray[i].key,key)==0){

                char oldValue[1024];
                int j=0;
                while(keyArray[i].value[j]!='\0'){
                    oldValue[j]=keyArray[i].value[j];
                    j++;
                }
                oldValue[j]='\0';
                j=0;

                strcpy(keyArray[i].value,value);
                while(oldValue[j]!='\0'){
                    value[j]=oldValue[j];
                    j++;
                }
                value[j]='\0';
                return 1;
            }
        }
    }


}

//return -1 if key not existant, else deletes key
int del(char* key){
    char res[1024];
    if(get(key,res)==-1){
        return -1;
    }
    else{
        for(int i=0; i<arraySize;i++) {
            if (strcmp(keyArray[i].key, key) == 0) {
                for (int j = 0; keyArray[i].key[j]!= '\0'; j++) {
                    keyArray[i].key[j] = '\0';
                }
                return 1;
            }
        }
    }
    return -1;
}
