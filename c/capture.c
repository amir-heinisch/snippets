/* Pcap usage example. */

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/ethernet.h>

void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
   struct ether_header *eth_header; 

   eth_header = (struct ether_header *) packet;
   if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
        printf("IP\n");
   }
}

/* Use libpcap to run a live capture. */
void capture_libpcap() {
    pcap_t *handle;
    char *device = "eno1";
    int snapshot_len = 1028;
    int promi = 0;
    int timeout = 1000;
    char error_buffer[PCAP_ERRBUF_SIZE];

    handle = pcap_open_live(device, snapshot_len, promi, timeout, error_buffer);
    pcap_loop(handle, 1, packet_handler, NULL);
    pcap_close(handle);

    return;
}

int main(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "-p")) != -1) {
        switch (opt) {
            case 'p':
                capture_libpcap();
                break;
            default:
                fprintf(stderr, "Usage: %s [-p/-r] [iface]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
 
    return EXIT_SUCCESS;
}
