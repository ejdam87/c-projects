#include <stdlib.h>
#include "capture.h"
#include <string.h>


int main(int argc, char **argv)
{

    if (argc < 5){
        fprintf(stderr, "Too few arguments provided!\n");
        return 1;
    }

    char *filename = argv[1];
    struct capture_t *capture = malloc(sizeof(struct capture_t));

    if (capture == NULL){
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;

    }
    if (load_capture(capture, filename) != 0){
        free(capture);
        fprintf(stderr, "Failed to load .pcap file content!\n");
        return 1;
    }

    uint8_t src_ip[4];
    uint8_t src_mask;
    uint8_t dst_ip[4];
    uint8_t dst_mask;

    sscanf(argv[2], "%hhu.%hhu.%hhu.%hhu/%hhu", src_ip, src_ip + 1, src_ip + 2, src_ip + 3, &src_mask);
    sscanf(argv[3], "%hhu.%hhu.%hhu.%hhu/%hhu", dst_ip, dst_ip + 1, dst_ip + 2, dst_ip + 3, &dst_mask);

    if (src_mask > 32 || dst_mask > 32){
        destroy_capture(capture);
        free(capture);
        fprintf(stderr, "Invalid mask provided!\n");
        return 1;
    }

    struct capture_t *filtered = malloc(sizeof(struct capture_t));

    if (filtered == NULL){
        fprintf(stderr, "Memory allocation failed!\n");
        destroy_capture(capture);
        free(capture);
        return 1;
    }

    if (filter_from_mask(capture, filtered, src_ip, src_mask) != 0){
        fprintf(stderr, "Filtering failed!\n");
        destroy_capture(capture);
        free(capture);
        return 1;
    }

    struct capture_t *filtered2 = malloc(sizeof(struct capture_t));

    if (filtered2 == NULL){
        fprintf(stderr, "Memory allocation failed!\n");
        destroy_capture(capture);
        free(capture);
        free(filtered);
        return 1;
    }

    if (filter_to_mask(filtered, filtered2, src_ip, src_mask) != 0){
        fprintf(stderr, "Filtering failed!\n");
        destroy_capture(capture);
        free(capture);
        return 1;
    }

    int failed = 0;

    if (strcmp(argv[4], "flowstats") == 0){
        if (print_flow_stats(filtered2) != 0){
            destroy_capture(capture);
            free(capture);
            destroy_capture(filtered);
            free(filtered);
            destroy_capture(filtered2);
            free(filtered2);
            return 1;
        }
    }
    else if (strcmp(argv[4], "longestflow") == 0){
        if (print_longest_flow(filtered2) != 0){
            destroy_capture(capture);
            free(capture);
            destroy_capture(filtered);
            free(filtered);
            destroy_capture(filtered2);
            free(filtered2);
            return 1;
        }
    }
    else{
        failed = 1;
        fprintf(stderr, "Invalid 4th argument!\n");
    }

    // --- Garbage cleaning
    destroy_capture(capture);
    free(capture);
    destroy_capture(filtered);
    free(filtered);
    destroy_capture(filtered2);
    free(filtered2);

    if (failed){
        return 1;
    }

    return 0;
}
