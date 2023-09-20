#include <dirent.h>     
#include <stdio.h>      
#include <stdlib.h>
#include <unistd.h>    
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#define MAX 4096
#define SIZE 512

struct node{
    struct node *next;
    struct node *parent;
    struct node *children[SIZE];
    char data[SIZE];
    int pid, ppid;
};

struct node * find_parent(struct node *, int);
void print_tree(struct node *, int);


int main(int argc, char *argv[]){

    DIR *dir;              
    struct dirent *pid;  
    char p;    
    int fd, child, space;            
    ssize_t rn;            
    char path[MAX];        
    char list[MAX];
    char pid_output[SIZE];  
    int all_process[MAX];    
    static char delim[] = " \t\n():"; 
    char *token, *p_cmd, *p_pid, *p_parentid;
    struct node *new_node, *head, *prev, *parent; 

    //find all pid  //////////////////////////////////////////////////
    if((dir = opendir("/proc")) == NULL){
        perror("opendir fail");
        exit(EXIT_FAILURE);
    }
    else{
        while ((pid = readdir(dir)) != NULL){
            
            p = *(pid->d_name);
            if (isdigit(p)){

                snprintf(path, 4096, "/proc/%s/status", pid->d_name);
                fd = open(path, O_RDONLY);
                if (fd == -1){
                    perror("open pid status fail");
                    exit(EXIT_FAILURE); 
                }

                rn = read(fd, list, MAX);
                if (rn == -1){
                    perror("read problem");
                    exit(EXIT_FAILURE);
                }

                token = strtok(list, delim);
                if (token == NULL){
		            printf("NULL");
	            }

                while (token != NULL){
                    if (strcmp(token, "Name") == 0){
                        token = strtok(NULL, delim);
                        p_cmd = token;
                    }
                    else if (strcmp(token, "Pid") == 0){
                        token = strtok(NULL, delim);
                        p_pid = token;
                    }
                    else if (strcmp(token, "PPid") == 0){
                        token = strtok(NULL, delim);
                        p_parentid = token;
                        token = NULL;
                    }
                    else{
                        token = strtok(NULL, delim);
                    }
                }
                // printf("%s(%s)\n", p_cmd, p_pid);

                //create node  //////////////////////////////////////////////////
                new_node = (struct node *)malloc(sizeof(struct node));
                if (new_node == NULL){
                    perror("add node fail");
                    exit(EXIT_FAILURE);
                }

                new_node->next = NULL;
                new_node->pid = atoi(p_pid);
                new_node->ppid = atoi(p_parentid);
                strcpy(new_node->data, p_cmd);
                new_node->children[0] = NULL;
                new_node->parent = NULL;

                if (head == NULL){
                    head = new_node;
                    prev = new_node;
                }
                else{
                    prev->next = new_node;
                    prev = new_node; 
                }

                if (close(fd) == -1){
		            perror("Closing file error");
		            exit(EXIT_FAILURE);
	            }
        	} 
        }
    }

    //link parent and children  //////////////////////////////////////////////////
    for (new_node = head; new_node != NULL; new_node = new_node->next){
        child = 0;
        parent = find_parent(head, new_node->ppid);    //return parent address(pointer)
        if (parent != NULL){
            new_node->parent = parent;
            while (parent->children[child++] != NULL);
            parent->children[child-1] = new_node;
            parent->children[child] = NULL;
        }
    }

    //draw  //////////////////////////////////////////////////
    for (new_node = head; new_node != NULL; new_node = new_node->next){

        if (new_node->ppid == 0){    //is root
            print_tree(new_node, 0);
        }
    }
    
    exit(EXIT_SUCCESS);

}

struct node * find_parent(struct node *head, int pid){
    struct node *node;
    for (node = head; node != NULL; node = node->next){
        if(node->pid == pid){
            return node;
        }
    }
    return NULL;
}

void print_tree(struct node *root, int tree_level){

    struct node *child_node;
    for (int i = 0; i < tree_level; i++){
        printf("%*s", 3, " ");
    }
    if (tree_level > 0){
        printf("|-");
    }

    // for check
    // printf("%s(pid:%d, ppid:%d)\n", root->data, root->pid, root->ppid);

    printf("%s(%d)\n", root->data, root->pid);

    int j = 0;
    tree_level++;
    while (root->children[j++] != NULL){
        child_node = root->children[j-1];
        print_tree(child_node, tree_level);
    }
    
}
