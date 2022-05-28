#include "capture.h"
#include <stdlib.h>


void print_ip(uint8_t ip[4]){

    for (uint8_t i = 0; i < 4; i++){
        if (i == 3){
            printf("%u", ip[i]);
            continue;
        }
        printf("%u.", ip[i]);
    }
}

// --- Filter helpers
int protocols_equal(struct packet_t packet, int8_t protocol){
    return packet.ip_header -> protocol == protocol;
}

int size_larger(struct packet_t packet, uint32_t size){
    return packet.packet_header -> orig_len >= size;
}

int ip_match(const uint8_t a[4], const uint8_t b[4]){

    for (uint8_t i = 0; i < 4; i++){
        if (a[i] != b[i]){
            return 0;
        }
    }
    return 1;
}

int source_dest_match(struct packet_t packet, uint8_t source[4], uint8_t dest[4]){
    return ip_match(packet.ip_header -> src_addr, source) &&
           ip_match(packet.ip_header -> dst_addr, dest);
}

uint8_t get_nth_bit(uint32_t num, uint8_t n){
    return (num >> n) & 1;
}

int match_mask(uint8_t const network_prefix[4],
               uint8_t mask_length,
               uint8_t const ip[4]){

    uint32_t mask_val = 0;
    uint32_t ip_val = 0;

    for (uint8_t i = 0; i < 4; i++){

        mask_val <<= 8;
        mask_val += network_prefix[i];

        ip_val <<= 8;
        ip_val += ip[i];

    }

    uint8_t mask_bit, ip_bit;

    for (uint8_t i = 0; i < mask_length; i++){

        mask_bit = get_nth_bit(mask_val, 31 - i);
        ip_bit = get_nth_bit(ip_val, 31 - i);

        if ((mask_bit & ip_bit) != ip_bit){
            return 0;
        }
    }

    return 1;

}

// ---

int load_capture(struct capture_t *capture, const char *filename){

    capture -> header = NULL;
    capture -> first_packet = NULL;

    struct pcap_context context[1];
    if (init_context(context, filename) != PCAP_SUCCESS) {
        destroy_capture(capture);
        return -1;
    }

    struct pcap_header_t *pcap_header = malloc(sizeof(struct pcap_header_t));
    if (load_header(context, pcap_header) != PCAP_SUCCESS) {
        free(pcap_header);
        destroy_context(context);
        destroy_capture(capture);
        return -1;
    }

    capture -> header = pcap_header;

    struct packet_t *packet = malloc(sizeof(struct packet_t));

    if (packet == NULL){
        free(pcap_header);
        destroy_context(context);
        destroy_capture(capture);
        return -1;
    }

    int status;
    int first = 1;
    struct linked_node_t *previous = NULL;

    while ((status = load_packet(context, packet)) != PCAP_FILE_END){

        if (status != PCAP_SUCCESS) {
            free(pcap_header);
            destroy_capture(capture);
            destroy_context(context);
            return -1;
        }

        struct linked_node_t *node = malloc(sizeof(struct linked_node_t));

        node -> packet = *packet;

        if (previous != NULL){
            previous -> next = node;
        }

        // Init list
        if (first == 1){
            capture -> first_packet = node;
            first = 0;
        }

        previous = node;
    }

    if (previous != NULL){
        previous -> next = NULL;
    }

    destroy_context(context);
    free(packet);
    return 0;
}

void destroy_capture(struct capture_t *capture){

    if (capture == NULL){
        return;
    }

    if (capture -> header != NULL){
        free(capture -> header);
        capture -> header = NULL;
    }

    if (capture -> first_packet == NULL){
        return;
    }

    struct linked_node_t *packet_node = capture -> first_packet;
    struct linked_node_t *nxt;

    while (packet_node != NULL){

        destroy_packet(&packet_node -> packet);
        nxt = packet_node -> next;

        free(packet_node);
        packet_node = nxt;

    }

    capture -> first_packet = NULL;

}


const struct pcap_header_t *get_header(const struct capture_t *const capture){
    if (capture == NULL){
        return NULL;
    }

    return capture -> header;
}


struct packet_t *get_packet(
        const struct capture_t *const capture,
        size_t index){
    size_t current = 0;

    struct linked_node_t *packet_node = capture -> first_packet;

    // traverse linked-list
    while (current != index){

        if (packet_node == NULL){
            return NULL;
        }

        packet_node = packet_node -> next;
        current++;
    }

    return &packet_node -> packet;
}

size_t packet_count(const struct capture_t *const capture){

    if (capture == NULL){
        return 0;
    }

    size_t count = 0;
    struct linked_node_t *packet_node = capture -> first_packet;

    while (packet_node != NULL){

        count++;
        packet_node = packet_node -> next;
    }

    return count;

}

size_t data_transfered(const struct capture_t *const capture){

    if (capture == NULL){
        return 0;
    }

    size_t count = 0;
    struct linked_node_t *packet_node = capture -> first_packet;

    while (packet_node != NULL){

        count += (packet_node -> packet).packet_header -> orig_len;
        packet_node = packet_node -> next;
    }

    return count;

}


// --- 2nd part of hw

int filter_general(
        const struct capture_t *const original,
        struct capture_t *filtered,
        int filter_type,
        struct filter_arg_t filter_args)
{

    /*
     filter_type:
     - 1 = filter_protocol
     - 2 = filter_larger_than
     - 3 = filter_from_to
     - 4 = filter_from_mask
     - 5 = filter_to_mask
     */
    struct linked_node_t *packet_node = original -> first_packet;

    // --- Init of filtered
    filtered -> header = NULL;
    filtered -> first_packet = NULL;
    // ---

    if (original -> header == NULL){
        return -1;
    }

    struct pcap_header_t *new_header = malloc(sizeof(struct pcap_header_t));

    if (new_header == NULL){
        destroy_capture(filtered);
        return -1;
    }

    *new_header = *original -> header;
    filtered -> header = new_header;

    struct packet_t *packet = malloc(sizeof(struct packet_t));

    if (packet == NULL){
        destroy_capture(filtered);
        return -1;
    }

    int first = 1;
    int status, condition;
    struct linked_node_t *prev = NULL;

    while (packet_node != NULL) {

        if (filter_type == 1){
            condition = protocols_equal(packet_node -> packet, filter_args.protocol);
        }
        else if (filter_type == 2){
            condition = size_larger(packet_node -> packet, filter_args.size);
        }
        else if (filter_type == 3){
            condition = source_dest_match(packet_node -> packet, filter_args.source, filter_args.dest);
        }
        else if (filter_type == 4){
            uint8_t *ip = (packet_node -> packet).ip_header -> src_addr;
            condition = match_mask(filter_args.mask_ip, filter_args.mask, ip);
        }
        else if (filter_type == 5){
            uint8_t *ip = (packet_node -> packet).ip_header -> dst_addr;
            condition = match_mask(filter_args.mask_ip, filter_args.mask, ip);
        }

        if (condition) {

            status = copy_packet(&packet_node->packet, packet);

            if (status != PCAP_SUCCESS) {
                free(packet);
                destroy_capture(filtered);
                return -1;
            }

            struct linked_node_t *node = malloc(sizeof(struct linked_node_t));

            if (node == NULL) {
                free(packet);
                destroy_capture(filtered);
                return -1;
            }

            node -> next = NULL;
            node->packet = *packet;

            if (prev != NULL){
                prev -> next = node;
            }

            prev = node;

            if (first == 1) {
                first = 0;
                filtered->first_packet = node;
            }
        }

        packet_node = packet_node->next;
    }

    if (prev != NULL){
        prev -> next = NULL;
    }

    free(packet);
    return 0;
}

int filter_protocol(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t protocol)
{

    struct filter_arg_t args;
    args.protocol = protocol;

    return filter_general(original, filtered, 1, args);

}


int filter_larger_than(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint32_t size)
{

    struct filter_arg_t args;
    args.size = size;

    return filter_general(original, filtered, 2, args);
}

int filter_from_to(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t source_ip[4],
        uint8_t destination_ip[4])
{

    struct filter_arg_t args;
    args.source = source_ip;
    args.dest = destination_ip;

    return filter_general(original, filtered, 3, args);
}

int filter_from_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{

    struct filter_arg_t args;
    args.mask_ip = network_prefix;
    args.mask = mask_length;

    return filter_general(original, filtered, 4, args);

}

int filter_to_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{
    struct filter_arg_t args;
    args.mask_ip = network_prefix;
    args.mask = mask_length;

    return filter_general(original, filtered, 4, args);
}

int pair_in_array(size_t length, struct pair_t *addresses, struct pair_t pair){


    for (size_t i = 0; i < length; i++){

        struct pair_t present_pair = addresses[i];

        if (ip_match(present_pair.src, pair.src) && ip_match(present_pair.dst, pair.dst)){
            return i;
        }
    }

    return -1;

}

void fill_zeros(size_t length, size_t from, int counts[length]){

    for (size_t i = from; i < length; i++){
        counts[i] = 0;
    }
}


int print_flow_stats(const struct capture_t *const capture)
{

    // --- Gather information
    struct linked_node_t *packet_node = capture -> first_packet;

    uint8_t *src;
    uint8_t *dst;

    size_t length = 30;
    size_t current_length = 0;

    struct pair_t *addresses = malloc(length * sizeof(struct pair_t));

    if (addresses == NULL){
        fprintf(stderr, "Mem-alloc failed!\n");
        return -1;
    }

    int *counts = calloc(length, sizeof(int));

    if (counts == NULL){
        free(addresses);
        return -1;
    }

    while (packet_node != NULL){

        struct packet_t packet = packet_node -> packet;
        src = packet.ip_header -> src_addr;
        dst = packet.ip_header -> dst_addr;

        struct pair_t new;
        new.src = src;
        new.dst = dst;

        int pos = pair_in_array(current_length, addresses, new);

        if (pos == -1){

            if (current_length == length){

                struct pair_t *new_addr = realloc(addresses, sizeof(struct pair_t) * length * 2);

                if (new_addr == NULL){
                    fprintf(stderr, "Mem-re-alloc failed!\n");
                    free(addresses);
                    free(counts);
                    return -1;
                }

                int *new_counts = realloc(counts, sizeof(int) * length * 2);

                if (new_counts == NULL){
                    fprintf(stderr, "Mem-re-alloc failed!\n");
                    free(addresses);
                    free(counts);
                    free(new_addr);
                    return -1;
                }
                counts = new_counts;

                // Zero on new positions
                fill_zeros(length, current_length, counts);

                addresses = new_addr;
                length *= 2;
            }

            current_length++;
            addresses[current_length - 1] = new;
            counts[current_length - 1] = 1;

        }

        else{
            counts[pos] += 1;
        }

        packet_node = packet_node -> next;
    }

    // --- Actual printing
    for (size_t i = 0; i < current_length; i++){

        struct pair_t pair = addresses[i];
        print_ip(pair.src);
        printf(" -> ");
        print_ip(pair.dst);
        printf(" : %d", counts[i]);
        putchar('\n');

    }
    // ---

    free(addresses);
    free(counts);

    return 0;
}

void calculate_flow_length(struct packet_t first,
                          struct packet_t last,
                          struct time_stamp_t *output){

    output -> sec = last.packet_header->ts_sec - first.packet_header->ts_sec;
    output -> s_sec = last.packet_header-> ts_usec - first.packet_header-> ts_usec;
}

int is_longer(struct time_stamp_t a, struct  time_stamp_t b){

    if (a.sec > b.sec){
        return 1;
    }

    if (a.sec == b.sec){
        if (a.s_sec > b.s_sec){
            return 1;
        }
    }

    return 0;

}

int print_longest_flow(const struct capture_t *const capture)
{

    // --- Gather information
    struct linked_node_t *packet_node = capture -> first_packet;

    if (packet_count(capture) == 0){
        fprintf(stderr, "Empty capture provided!\n");
        return -1;
    }

    uint8_t *src;
    uint8_t *dst;

    // Array of pointers to tuples
    size_t length = 30;
    size_t current_length = 0;

    struct pair_t *addresses = malloc(length * sizeof(struct pair_t));

    if (addresses == NULL){
        fprintf(stderr, "Mem-alloc failed!\n");
        return -1;
    }

    struct packet_t *f_packets = malloc(length * sizeof(struct packet_t));
    if (f_packets == NULL){
        fprintf(stderr, "Mem-alloc failed!\n");
        free(addresses);
        return -1;
    }

    struct packet_t *l_packets = malloc(length * sizeof(struct packet_t));
    if (l_packets == NULL){
        fprintf(stderr, "Mem-alloc failed!\n");
        free(addresses);
        free(f_packets);
        return -1;
    }

    while (packet_node != NULL){

        struct packet_t packet = packet_node -> packet;
        src = packet.ip_header -> src_addr;
        dst = packet.ip_header -> dst_addr;

        struct pair_t new;
        new.src = src;
        new.dst = dst;

        int pos = pair_in_array(current_length, addresses, new);

        if (pos == -1){

            if (current_length == length){

                struct pair_t *new_addr = realloc(addresses, sizeof(struct pair_t) * length * 2);
                if (new_addr == NULL){
                    fprintf(stderr, "Mem-re-alloc failed!\n");
                    free(addresses);
                    free(l_packets);
                    free(f_packets);
                    return -1;
                }

                struct packet_t *new_l = realloc(l_packets, sizeof(struct packet_t) * length * 2);
                if (new_l == NULL){
                    fprintf(stderr, "Mem-re-alloc failed!\n");
                    free(addresses);
                    free(l_packets);
                    free(f_packets);
                    free(new_addr);
                    return -1;
                }

                struct packet_t *new_f = realloc(f_packets, sizeof(struct packet_t) * length * 2);
                if (new_f == NULL){
                    fprintf(stderr, "Mem-re-alloc failed!\n");
                    free(addresses);
                    free(l_packets);
                    free(f_packets);
                    free(new_l);
                    free(new_addr);
                    return -1;
                }

                f_packets = new_f;
                l_packets = new_l;
                addresses = new_addr;
                length *= 2;

            }

            current_length++;

            f_packets[current_length - 1] = packet;
            l_packets[current_length - 1] = packet;
            addresses[current_length - 1] = new;
        }

        else{
            l_packets[pos] = packet;
        }

        packet_node = packet_node -> next;
    }


    // --- Calculations of longest flow

    size_t max_pos = 0;
    struct time_stamp_t longest_flow;
    struct time_stamp_t current_flow;

    int first = 1;

    for (size_t i = 0; i < current_length; i++){

        if (first == 1){
            first = 0;
            max_pos = 0;
            calculate_flow_length(f_packets[i], l_packets[i], &longest_flow);
            continue;
        }

        calculate_flow_length(f_packets[i], l_packets[i], &current_flow);


        if (is_longer(current_flow, longest_flow)){

            max_pos = i;
            longest_flow = current_flow;

        }

    }

    // --- printing
    print_ip(f_packets[max_pos].ip_header->src_addr);
    printf(" -> ");
    print_ip(f_packets[max_pos].ip_header->dst_addr);
    printf(" : ");
    printf("%u:%u", f_packets[max_pos].packet_header->ts_sec, f_packets[max_pos].packet_header->ts_usec);
    printf(" - ");
    printf("%u:%u", l_packets[max_pos].packet_header->ts_sec, l_packets[max_pos].packet_header->ts_usec);
    putchar('\n');

    free(addresses);
    free(f_packets);
    free(l_packets);

    return 0;

}
