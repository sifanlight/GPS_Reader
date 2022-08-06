#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/socket.h>

#include "datatypes.h"
#include "encoder.h"
#include "log.h"
#include "socketHandler.h"



#define MAX_LENGTH 450

int main()
{
    int socket_desc = initSocket();
    if (socket_desc < 0) {
        log_error("Error: Could not create socket");
        return 1;
    }

    if (connectToServer(socket_desc) < 0) {

        log_error("Error: Could not connect to server");
        return 1;
    }

    packet_t *data;
    packet_t *old_data;
    FILE *fp;
    char comm[210] = "(timeout 2 grep --max-count=1 -F 'GPGGA' /dev/ttyUSB1 && timeout 2 grep --max-count=1 -F 'GPRMC' /dev/ttyUSB1 && timeout 2 grep --max-count=1 -F 'GPVTG' /dev/ttyUSB1)|tr '\r\n' ' '";
    // char comm[190] = "date";
    //char comm[210] = "echo '$'GPGGA,111822.00,3544.222130,N,05125.828605,E,1,04,1.4,1325.4,M,-14.0,M,,*79 '$'GPRMC,111823.00,A,3544.222122,N,05125.828620,E,0.0,106.3,020822,3.7,E,A,V*4D '$'GPVTG,106.3,T,102.6,M,0.0,N,0.0,K,A*22";
    char *command = comm ;
    char path[MAX_LENGTH];

    data = (packet_t *)malloc(sizeof(packet_t) + MAX_LENGTH);
    old_data = (packet_t *)malloc(sizeof(packet_t)+ MAX_LENGTH);
    memset(data->data, 0, MAX_LENGTH);
    memset(old_data->data, 0, MAX_LENGTH);
    char key[5] = "abcd";
    bool firstTime = true;
    // std::copy(&key[0], &key[5], &old_data->data[0]);

    while(1)  {
        // if (!firstTime){
        //     memset(old_data->data, 0, 210);
        //     std::copy(&data->data[0], &data->data[data->len], &old_data->data[0]);
        //     memset(data->data, 0, 210);
        //     old_data->len = data->len;
        // }

        fp = popen(command, "r");
        
        fgets(path, sizeof(path), fp);
        // printf("%s \n", path);
        pclose(fp);
        std::cout<<"path = "<<path <<"\n";
        int counter = 0;
        for (int i = 0; i <sizeof(path)+4; i++){
            if (path[i] == '\0'){
                // printf(" %d \n ", i);
                counter = i;
                break;
            }
        }
        counter += 1;
        // for (int i =0; i < counter; i++)
            // printf("%c", path[i]);

        printf("\n");

        if (firstTime){
            old_data->len = counter - 1;
            firstTime = false;
        }


        // Creating packet_t with data we got
        data->len = counter - 1;
        std::copy(&path[0], &path[counter-1], &data->data[0]);
        std::cout<< "Data before encode: " << data->data<<"   data len: "<< data->len << "\n";
        std::cout<< "Old data: " <<old_data->data<<"   data len: "<< data->len << "\n";
        encodeData(data, old_data);
        std::cout<< "Data after encode: " << data->data<<"   data len: "<< data->len << "\n";
        std::cout<< "Old data after encode: " <<old_data->data<<"   data len: "<< data->len << "\n";

        int err = sendPacket(socket_desc, data);
        log_info("err is %d", err);
        if (err < 0) {
            log_error("Could not send the Packet");
            return 1;
        }
        std::cout<<"----------------------------\n";
        sleep(2);
        memcpy(old_data, data, sizeof(packet_t)+data->len);
        std::cout<< "Old data after encode: " <<old_data->data<<"   data len: "<< data->len << "\n";
        // printf("%.*s", 31, old_data->data);
        std::cout<<"----------------------------\n";
        // memset(old_data->data, 0, 210);
        // std::copy(&data->data[0], &data->data[data->len], &old_data->data[0]);
        // memset(data->data, 0, 210);
        // old_data->len = data->len;
    }
    return 0;
}
