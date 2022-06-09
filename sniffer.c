# include <stdio.h>
# include <netinet/ip_icmp.h>
# include <netinet/ip.h>
# include <string.h>
# include <stdlib.h>
# include <netinet/if_ether.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
# include <arpa/inet.h>
#include <unistd.h>

struct sockaddr_in server;
struct sockaddr_in client;

int main() {
    int PACKET_LEN = 512;
    char buffer[PACKET_LEN];
    struct sockaddr saddr;
    struct packet_mreq mr;

    // Create the raw socket
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    // Turn on the promiscuous mode.
    mr.mr_type = PACKET_MR_PROMISC;
    setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr,
               sizeof(mr));

    // Getting captured packets
    while (1) {
        int data_size=recvfrom(sock, buffer, PACKET_LEN, 0,
                               &saddr, (socklen_t*)sizeof(saddr));
//        if(data_size) printf("Got one packet\n");

        struct iphdr * iph = (struct iphdr *) (buffer + sizeof(struct ethhdr));
        // ICMP transport
        if(iph->protocol == 1){
            // get struct of ICMP
            struct icmphdr * icmph = (struct icmphdr *) ((char *) iph + (4 * iph->ihl));

            // make copy
            memset(&server, 0, sizeof(server));
            server.sin_addr.s_addr = iph->saddr;
            memset(&client, 0, sizeof(client));
            client.sin_addr.s_addr =iph->daddr;

            printf("CODE: %d\n", icmph->code);
            printf("TYPE: %d\n", icmph->type);
            printf("IP_DST: %s\n", inet_ntoa(client.sin_addr));
            printf("IP_SRC: %s\n", inet_ntoa(server.sin_addr));
        }
    }

    close(sock);
    return 0;
}



