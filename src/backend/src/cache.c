#include <stdio.h>
#include <stdlib.h>
//
#include "coherence_protocol_constants.h"
#include "structs.h"
//

///Cache line methods
void construct_cache_line(Cache_line* line, int tag, bool is_valid, bool is_dirty, protocol_state state, int data)
{
    line->tag=tag;
    line->is_valid=is_valid;
    line->is_dirty=is_dirty;
    line->state = state;
    line->data = data;
}
Cache_line** generate_cache_lines()
{
    Cache_line** cache_lines = malloc(CACHE_ENTRIES*sizeof(Cache_line*));
    if(cache_lines == NULL)
    {
        fprintf(stderr, "construct_cache_line/cache_lines/Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < CACHE_ENTRIES; i++)
    {
        cache_lines[i] = malloc(sizeof(Cache_line));
        if(cache_lines[i] == NULL)
        {
            fprintf(stderr, "construct_cache_line/cache_lines[i]/Memory allocation failed\n");
            exit(1);
        }
        construct_cache_line(cache_lines[i], 0, false, false, Invalid, 0);
    }
    return cache_lines;
}
void print_cache_line(Cache_line* line)
{
    printf("Tag: %d\tValid: %d\tDirty: %d\tState: %c\tData: %d\n", line->tag, line->is_valid, line->is_dirty, line->state, line->data);
}
void print_cache_lines(Cache_line** lines)
{
    for (int i = 0; i < CACHE_ENTRIES; i++)
    {
        print_cache_line(lines[i]);
    }
}

///Cache methods
void initialize_Caches(Cache** caches)
{
    for (int i = 0; i < MAX_CORES; i++)
    {
        caches[i] = malloc(sizeof(Cache));
    }
}
void construct_Cache(Cache* $, int id)
{
    $->id = id;
    $->lines = generate_cache_lines();
}
void construct_Caches(Cache** caches)
{
    for (int i = 0; i < MAX_CORES; i++)
    {
        construct_Cache(caches[i], i);
    }
}
Cache_line* get_cache_line(Cache** caches, int cache_id, int line_addr)
{
    return caches[cache_id]->lines[line_addr];
}
void set_cache_line(Cache** caches, int cache_id, int line_addr, int tag, bool is_valid, bool is_dirty, protocol_state state, int data)
{
    construct_cache_line(caches[cache_id]->lines[line_addr], tag, is_valid, is_dirty, state, data);
}
void print_Cache(Cache* $)
{
    printf("Cache: %d\n", $->id);
    printf("Lines:\n");
    print_cache_lines($->lines);
}
void print_Caches(Cache** caches)
{
    printf_blue_on();
    for (int i = 0; i < MAX_CORES; ++i)
    {
        print_Cache(caches[i]);
        printf("\n");
    }
    printf_color_reset();
}