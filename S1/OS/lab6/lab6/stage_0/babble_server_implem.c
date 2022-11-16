#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "babble_server.h"
#include "babble_utils.h"
#include "babble_types.h"
#include "babble_communication.h"
#include "babble_registration.h"

time_t server_start;

/* used to create a timeline of publications*/
typedef struct timeline_item{
    publication_t *pub;   /* the publication */
    client_bundle_t *client;   /* publication author */
    struct timeline_item *next;
} timeline_item_t;


/* freeing client_bundle_t struct */
static void free_client_data(client_bundle_t *client)
{
    if(client == NULL){
        return;
    }


    /* IMPORTANT: we choose not to free client_bundle_t structures when
     * a client disconnects. The reason is that pointers to this data
     * structure are stored in several places in the code, and so,
     * freeing properly would be a complex operation */
    
    /*publication_t *item= client->pub_list, *previous;

    while(item!=NULL){
        previous=item;
        item = item->next;
        free(previous);
    }

    free(client);*/
}

/* stores an error message in the answer_set of a command */
static void generate_cmd_error(command_t *cmd, answer_t **answer)
{
    answer_t *the_answer=NULL;
    char *msg_buffer=NULL;

    /* lookup client */
    client_bundle_t *client = registration_lookup(cmd->key);
    
    if(client == NULL){
        fprintf(stderr, "Error -- no client found\n");
        return;
    }
    
    the_answer = alloc_answer(client->key);

    msg_buffer = malloc(BABBLE_BUFFER_SIZE);

    if(cmd->cid == LOGIN || cmd->cid == PUBLISH || cmd->cid == FOLLOW){
        snprintf(msg_buffer, BABBLE_BUFFER_SIZE,"%s[%ld]: ERROR -> %d { %s } \n", client->client_name, time(NULL)-server_start, cmd->cid, cmd->msg);
    }
    else{
        snprintf(msg_buffer, BABBLE_BUFFER_SIZE,"%s[%ld]: ERROR -> %d \n", client->client_name, time(NULL)-server_start, cmd->cid);

    }

    add_msg_to_answer(the_answer, BABBLE_BUFFER_SIZE, msg_buffer);
    free(msg_buffer);
    
    *answer = the_answer;
}

/* can be used to display the content of a command */
void display_command(command_t *cmd, FILE* stream)
{
    switch(cmd->cid){
    case LOGIN:
        fprintf(stream,"LOGIN: %s\n", cmd->msg);
        break;
    case PUBLISH:
        fprintf(stream,"PUBLISH: %s\n", cmd->msg);
        break;
    case FOLLOW:
        fprintf(stream,"FOLLOW: %s\n", cmd->msg);
        break;
    case TIMELINE:
        fprintf(stream,"TIMELINE\n");
        break;
    case FOLLOW_COUNT:
        fprintf(stream,"FOLLOW_COUNT\n");
        break;
    case RDV:
        fprintf(stream,"RDV\n");
        break;
    default:
        fprintf(stream,"Error -- Unknown command id\n");
        return;
    }
}

/* initialize the server */
void server_data_init(void)
{
    server_start = time(NULL);

    registration_init();
}

/* open a socket to receive client connections */
int server_connection_init(int port)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    int reuse_opt=1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        perror("ERROR opening socket");
        return -1;
    }

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse_opt, sizeof(reuse_opt)) < 0 && setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (void*)&reuse_opt, sizeof(reuse_opt)) < 0 ){
        perror("setsockopt failed\n");
        close(sockfd);
        return -1;
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("ERROR on binding");
        close(sockfd);
        return -1;
    }
    
    if(listen(sockfd, BABBLE_BACKLOG)){
        perror("ERROR on listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

/* accept connections of the server socket and return corresponding
 * new file descriptor */
int server_connection_accept(int sock)
{
    int new_sock;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    new_sock = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
    
    if (new_sock < 0){
        perror("ERROR on accept");
        close(sock);
        return -1;
    }
    
    return new_sock;
}


/* create a new command for client corresponding to key */
command_t* new_command(unsigned long key)
{
    command_t *cmd = malloc(sizeof(command_t));
    cmd->key = key;
    cmd->answer_expected=0;
    
    return cmd;
}


int run_login_command(command_t *cmd, answer_t **answer)
{
    answer_t *the_answer=NULL;
    char* msg_buffer=NULL;

    struct timespec tt;
    clock_gettime(CLOCK_REALTIME, &tt);
    
    /* compute hash of the new client id */
    cmd->key = hash(cmd->msg);
    
    client_bundle_t *client_data=malloc(sizeof(client_bundle_t));
    
    strncpy(client_data->client_name, cmd->msg, BABBLE_ID_SIZE);
    client_data->sock = cmd->sock;
    client_data->key=cmd->key;
    client_data->pub_set=publication_set_create();
    client_data->last_timeline=(uint64_t)1000000000 * tt.tv_sec + tt.tv_nsec;
    /* by default, we follow ourself*/
    client_data->followed[0]=client_data;
    client_data->nb_followed=1;
    client_data->nb_followers=1;

    if(registration_insert(client_data)){
        free(client_data->pub_set);
        free(client_data);
        generate_cmd_error(cmd, answer);
        return -1;
    }

    client_data->disconnected = 0;
    
    printf("### New client %s (key = %lu)\n", client_data->client_name, client_data->key);

    /* answer to client */
    assert(cmd->answer_expected);
    
    the_answer = alloc_answer(client_data->key);
    msg_buffer = malloc(BABBLE_BUFFER_SIZE);
        
    snprintf(msg_buffer, BABBLE_BUFFER_SIZE,"%s[%ld]: registered with key %lu\n", client_data->client_name, tt.tv_sec - server_start, client_data->key);

    add_msg_to_answer(the_answer, BABBLE_BUFFER_SIZE, msg_buffer);
    free(msg_buffer);
    
    *answer = the_answer;

    return 0;
}

int run_publish_command(command_t *cmd, answer_t **answer)
{    
    client_bundle_t *client = registration_lookup(cmd->key);

    answer_t *the_answer=NULL;
    char* msg_buffer=NULL;

    
    if(client == NULL){
        fprintf(stderr, "Error -- no client found\n");
        generate_cmd_error(cmd, answer);
        return -1;
    }
    
    publication_t *pub = publication_set_insert(client->pub_set, cmd->msg);
    
    printf("### Client %s published { %s } at date %ld\n", client->client_name, pub->msg, pub->date);

    if(cmd->answer_expected){
        the_answer = alloc_answer(client->key);
        msg_buffer = malloc(BABBLE_BUFFER_SIZE);
        
        snprintf(msg_buffer, BABBLE_BUFFER_SIZE,"%s[%ld]: { %s }\n", client->client_name, pub->date, cmd->msg);
        
        add_msg_to_answer(the_answer, BABBLE_BUFFER_SIZE, msg_buffer);
        free(msg_buffer);
    }

    *answer = the_answer;
    
    return 0;
}

int run_follow_command(command_t *cmd, answer_t **answer)
{
    answer_t *the_answer=NULL;
    char* msg_buffer=NULL;
    client_bundle_t *client = registration_lookup(cmd->key);
    
    if(client == NULL){
        fprintf(stderr, "Error -- no client found\n");
        generate_cmd_error(cmd, answer);
        return -1;
    }


    /* compute hash of the client to follow */
    unsigned long f_key = hash(cmd->msg);

    /* lookup client to follow */
    client_bundle_t *f_client = registration_lookup(f_key);
    
    if(f_client == NULL){
        generate_cmd_error(cmd, answer);
        return 0;
    }
    
    /* if client is not already followed, add it */
    int i=0;

    for(i=0; i<client->nb_followed; i++){
        if(client->followed[i]->key == f_key){
            break;
        }
    }
    
    if(i == client->nb_followed){
        client->followed[i]=f_client;
        client->nb_followed++;
        f_client->nb_followers++;
    }
    else{
        printf("Warning: %s already follows %s\n", client->client_name, f_client->client_name);
    }


    /* generate answer to client */
    if(cmd->answer_expected){

        the_answer = alloc_answer(client->key);

        msg_buffer = malloc(BABBLE_BUFFER_SIZE);
        
        snprintf(msg_buffer, BABBLE_BUFFER_SIZE,"%s[%ld]: follow %s\n", client->client_name, time(NULL)-server_start, f_client->client_name);
        
        add_msg_to_answer(the_answer, BABBLE_BUFFER_SIZE, msg_buffer);

        free(msg_buffer);
    }

    *answer = the_answer;
    
    return 0;
}


static void generate_timeline_summary(unsigned long client_key, timeline_item_t *pub_list, int timeline_size, answer_t **answer)
{
    answer_t *the_answer=NULL;

    the_answer = alloc_answer(client_key);

    /* the first msg of the answer is the number of publications since
     * the last call to timeline */    
    add_msg_to_answer(the_answer, sizeof(unsigned int), &timeline_size);

    timeline_item_t *time_iter=pub_list;
    char msg[BABBLE_BUFFER_SIZE];
    int remaining = timeline_size;
    
    while(time_iter != NULL){
        if(remaining <= BABBLE_TIMELINE_MAX){
            snprintf(msg, BABBLE_BUFFER_SIZE,"    %s[%ld]: %s\n", time_iter->client->client_name, time_iter->pub->date, time_iter->pub->msg);
            add_msg_to_answer(the_answer, BABBLE_BUFFER_SIZE, msg);
        }
        time_iter = time_iter->next;
        remaining--;
    }
    assert(remaining == 0);
    
    *answer = the_answer;
}

int run_timeline_command(command_t *cmd, answer_t **answer)
{
    int i=0;
    timeline_item_t *pub_list=NULL;
    int item_count=0;
    
    /* get current time to know up to when we publish*/
    struct timespec tt;
    clock_gettime(CLOCK_REALTIME, &tt);

    uint64_t end_time= (uint64_t)1000000000 * tt.tv_sec + tt.tv_nsec;

    /* lookup client */
    client_bundle_t *client = registration_lookup(cmd->key);

    if(client == NULL){
        fprintf(stderr, "Error -- no client found\n");
        generate_cmd_error(cmd, answer);
        return -1;
    }

    /* start from where we finished last time*/
    uint64_t start_time=client->last_timeline;

    int client_disconnected = 0;
    
    /* gather publications over all followed clients */
    for(i=0; i < client->nb_followed; i++){
        client_bundle_t *f_client=client->followed[i];
        if(!f_client->disconnected){

            publication_t *pub=NULL;

            timeline_item_t *time_iter=pub_list, *prev;
            /* add recent items to the totally ordered list */
            while((pub = publication_set_getnext(f_client->pub_set, pub, start_time)) != NULL){
                /* if publication is more recent than the timeline
                   command, we do not consider it */
                if(pub->ndate > end_time){
                    break;
                }
            
                timeline_item_t *item=malloc(sizeof(timeline_item_t));
                item->pub=pub;
                item->client= f_client;
                item->next = NULL;

                while(time_iter!=NULL && time_iter->pub->date < pub->date){
                    prev = time_iter;
                    time_iter = time_iter->next;
                }

                /* insert in first position */
                if(time_iter == pub_list){
                    pub_list = item;
                    item->next=time_iter;
                }
                else{
                    item->next=time_iter;
                    prev->next=item;
                }
                item_count++;
                time_iter = item;
            }
        }
        else{
            client_disconnected = 1;
        }
    }
    
    /* removing disconnected clients from the list of followed clients */
    if(client_disconnected){
        for(i=0; i<client->nb_followed; i++){
            if(client->followed[i]->disconnected){
                /* remove the client from the set of followers */
                printf("### Client %s removed disconnected client %s from the client it follows\n", client->client_name, client->followed[i]->client_name);
                client->followed[i] = client->followed[client->nb_followed - 1];
                client->nb_followed--;
                /* decrease the index to go through the follower we moved
                   in the array */
                i--;
            }
        }
    }

    

    /* now that we have a full timeline, we generate the messages to
     * the client */
    generate_timeline_summary(client->key, pub_list, item_count, answer);

    /* Finally we free */
    timeline_item_t *time_iter=pub_list, *previous;
    while(time_iter != NULL){
        previous = time_iter;
        time_iter = time_iter->next;
        free(previous);
    }
    

    
    client->last_timeline = end_time;
    
    return 0;
}

int run_fcount_command(command_t *cmd, answer_t **answer)
{
    answer_t *the_answer=NULL;
    char* msg_buffer=NULL;

    /* lookup client */
    client_bundle_t *client = registration_lookup(cmd->key);

    if(client == NULL){
        fprintf(stderr, "Error -- no client found\n");
        generate_cmd_error(cmd, answer);
        return -1;
    }

    /* generate answer to client */
    the_answer = alloc_answer(client->key);

    msg_buffer = malloc(BABBLE_BUFFER_SIZE);
    
    snprintf(msg_buffer, BABBLE_BUFFER_SIZE,"%s[%ld]: has %d followers\n", client->client_name, time(NULL) - server_start, client->nb_followers);
    
    add_msg_to_answer(the_answer, BABBLE_BUFFER_SIZE, msg_buffer);

    free(msg_buffer);

    *answer = the_answer;

    return 0;
}

int run_rdv_command(command_t *cmd, answer_t **answer)
{
    answer_t *the_answer=NULL;
    char* msg_buffer=NULL;

    /* lookup client */
    client_bundle_t *client = registration_lookup(cmd->key);
    
    if(client == NULL){
        fprintf(stderr, "Error -- no client found\n");
        generate_cmd_error(cmd, answer);
        return -1;
    }

    /* generate answer to client */
    the_answer = alloc_answer(client->key);

    msg_buffer = malloc(BABBLE_BUFFER_SIZE);

    snprintf(msg_buffer, BABBLE_BUFFER_SIZE,"%s[%ld]: rdv_ack\n", client->client_name, time(NULL) - server_start);
    
    add_msg_to_answer(the_answer, BABBLE_BUFFER_SIZE, msg_buffer);

    free(msg_buffer);

    *answer = the_answer;

    return 0;
}


int unregisted_client(command_t *cmd)
{
    int i=0;
    
    assert(cmd->cid == UNREGISTER);
    
    /* remove client */
    client_bundle_t *client = registration_remove(cmd->key);
    
    if(client != NULL){

        /* unfollow all followed clients */
        for(i=0; i < client->nb_followed; i++){
            client_bundle_t *f_client=client->followed[i];
            f_client->nb_followers--;
            client->followed[i]= NULL;
        }
        
        printf("### Unregister client %s (key = %lu)\n", client->client_name, client->key);
        close(client->sock);
        client->disconnected = 1;     
        
        free_client_data(client);
    }
    

    return 0;
}


/* send error msg to client in case the input msg could not be parsed */
int notify_parse_error(command_t *cmd, char *input, answer_t **answer)
{
    answer_t *the_answer=NULL;
    char *msg_buffer=NULL;

    /* lookup client */
    client_bundle_t *client = registration_lookup(cmd->key);

    if(client == NULL){
        fprintf(stderr, "Error -- no client found\n");
        return -1;
    }


    if(cmd->answer_expected){
        the_answer = alloc_answer(client->key);
        
        msg_buffer = malloc(BABBLE_BUFFER_SIZE);
        
        snprintf(msg_buffer, BABBLE_BUFFER_SIZE,"%s[%ld]: ERROR -> %s\n", client->client_name, time(NULL)-server_start, input);
        
        add_msg_to_answer(the_answer, BABBLE_BUFFER_SIZE, msg_buffer);
        
        free(msg_buffer);
    }

    *answer = the_answer;
    
    return 0;
}


/* send buf to client identified by key */
int write_to_client(unsigned long key, int size, void* buf)
{
    client_bundle_t *client = registration_lookup(key);

    if(client == NULL){
        fprintf(stderr, "Error -- writing to non existing client %lu\n", key);
        return -1;
    }
    
    int write_size = network_send(client->sock, size, buf);
            
    if (write_size < 0){
        perror("writing to socket");
        return -1;
    }

    return 0;
}


char* get_name_from_key(unsigned long key)
{
    char *name = (char*) malloc(BABBLE_ID_SIZE);
    memset(name, 0, BABBLE_ID_SIZE);
    
    client_bundle_t *client = registration_lookup(key);

    if(client == NULL){
        strcpy(name, "???");
    }
    else{
        strcpy(name, client->client_name);
    }
    
    return name;
}
