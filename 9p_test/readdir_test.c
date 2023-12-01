#include <stdio.h> 
#include <dirent.h> 
#include <string.h>
  
int main(int argc, char *argv[]) 
{ 
    struct dirent *de;  // Pointer for directory entry 
    char *dirName = ".";

    if (argc > 1) {
      dirName = argv[1];
    } 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir(dirName); 
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory"); 
        return 0; 
    } 
  
    long offset = 0;
    int count = 0;
    const int RESTART_INDEX=33;
    char expectedNextName[256];
    printf("\n ====== First iteration ========\n");
    // Stop readdir before it comes to the end
    while ((de = readdir(dr)) != NULL && ++count < 600) {
        if(count == RESTART_INDEX) {
           offset = telldir(dr);
           printf("Saved offset=0x%lx(%ld)\n", offset, offset);
        }
        if (count == RESTART_INDEX + 1) {
           strncpy(expectedNextName, de->d_name, sizeof(expectedNextName));
        }

        printf("%s, offset=0x%lx(%ld)\n", de->d_name, telldir(dr), telldir(dr));
    }

    printf("\n ====== Second iteration: Start from 0x%lx(%ld).========\n", offset, offset);

    seekdir(dr, offset);
    printf("New offset: 0x%lx(%ld)\n", telldir(dr), telldir(dr));

    count = 0;
    while ((de = readdir(dr)) != NULL) {
        if (++count == 1) {
           if (strncmp(de->d_name, expectedNextName, sizeof(expectedNextName)) == 0) {
              fprintf(stderr, "Test passed: expectedNextName:%s, actual next name:%s\n", expectedNextName, de->d_name);
              break;
           } else {
              fprintf(stderr, "Test failed: expectedNextName:%s, actual next name:%s\n", expectedNextName, de->d_name);
           }

        }
        printf("%s, offset=0x%lx(%ld)\n", de->d_name, telldir(dr), telldir(dr));
    }
  
    closedir(dr);
    return 0; 
}
