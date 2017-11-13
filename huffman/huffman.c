#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "heap.h"

typedef struct StructTree *Tree;

struct  StructTree {
    Tree left, right;
    int ch;
    int frequency;
};

typedef struct {
    int byte;
    int bit;
    FILE *file;
} Cache;

void deletTree(Tree tree) {
    if (tree->left != NULL) {
        deletTree(tree->left);
        deletTree(tree->right);
    }
    free(tree);
}

inline Tree newTree(int ch, int frequency) {
    Tree p = (Tree)malloc(sizeof(*Tree));
    p->left = p->right = NULL;
    p->ch = ch;
    p->frequency = frequency;
    return p;
}

inline Tree joinTree(Tree t1, Tree t2) {
    Tree p = (Tree)malloc(sizeof(*Tree));
    p->left = t1;
    p->right = t2;
    p->frequency = t1->frequency + t2->frequency;
    return p;
}

int cmpTree(const void *t1, const void *t2) {
    return (*(const Tree*)t1)->frequency - (*(const Tree*)t2)->frequency;
}

Tree buildTree(FILE *src) {
    int frequency[UCHAR_MAX + 1] = { 0 };
    int ch;
    while ((ch = fgetc(src)) != EOF) {
        frequency[ch] += 1;
    }
    heap_t h;
    heap_init(h, 0, sizeof(Tree), cmpTree);
    for (ch = 0; ch <= UCHAR_MAX; ch++) {
        if (frequency[ch]) {
            heap_push_v(h, newTree(ch, frequency[ch]), Tree);
        }
    }
    while (heap_len(h) > 1) {
        Tree t1 = heap_pop_v(h, Tree);
        Tree t2 = heap_pop_v(h, Tree);
        heap_push_v(h, joinTree(t1, t2), Tree);
    }
    Tree tree = heap_pop_v(h, Tree);
    heap_free(h);
    rewind(src);
    return tree;
}

void putBit(int v, Cache *cache) {
    ++cache->bit;
    if (v == 1) {
        cache->byte |= 1 << (CHAR_BIT - cache->bit);
    }
    else {
        cache->byte &= ~(1 << (CHAR_BIT - cache->bit));
    }
    if (cache->bit == CHAR_BIT) {
        fputc(cache->byte, cache->file);
        cache->byte = cache->bit = 0;
    }
}

void putByte(int v, Cache *cache) {
    int bit = cache->bit;
    fputc(cache->byte | (v >> bit), cache->file);
    cache->byte = (v << (CHAR_BIT - bit)) & UCHAR_MAX;
}

void encodeTree(Tree tree, Cache *cache) {
    if (tree->left == NULL) {
        putBit(1, cache);
        putByte(tree->ch, cache);
    }
    else {
        putBit(0, cache);
        encodeTree(tree->left, cache);
        encodeTree(tree->right, cache);
    }
}

void encodeHeader(Tree tree, Cache *cache) {
    fwrite(&tree->frequency, sizeof(int), 1, cache->file);
    encodeTree(tree, cache);
}

void getCodecs(Tree tree, char *codecs[], char tmp[], int depth) {
    if (tree->left == NULL) {
        tmp[depth] = '\0';
        int size = sizeof(char)*(depth + 1);
        codecs[tree->ch] = (char*)malloc(size);
        memcpy(codecs[tree->ch], tmp, size);
    }
    else {
        tmp[depth] = '0';
        getCodecs(tree->left, codecs, tmp, depth + 1);
        tmp[depth] = '1';
        getCodecs(tree->right, codecs, tmp, depth + 1);
    }
}

void encodeBody(Tree tree, FILE *src, Cache *cache) {
    char *codecs[UCHAR_MAX + 1] = { 0 };
    char tmp[UCHAR_MAX + 1] = { 0 };
    int ch;
    getCodecs(tree, codecs, tmp, 0);
    while ((ch = fgetc(src)) != EOF) {
        char *s = codecs[ch];
        while (*s != '\0') {
            putBit(*s - '0', cache);
            s++;
        }
    }
    for (ch = 0; ch <= UCHAR_MAX; ch++) {
        if (codecs[ch]) {
            free(codecs[ch]);
        }
    }
}

int getBit(Cache *cache) {
    if (cache->bit == CHAR_BIT) {
        cache->byte = fgetc(cache->file);
        cache->bit = 0;
    }
    ++cache->bit;
    return (cache->byte >> (CHAR_BIT - cache->bit)) & 1;
}

int getByte(Cache *cache) {
   int bit = cache->bit;
   int next_byte = fgetc(cache->file);
   int v = ((cache->byte << bit) & UCHAR_MAX) | (next_byte >> (CHAR_BIT - bit));
   cache->byte = next_byte & (UCHAR_MAX >> bit);
   return v;
}

Tree decodeTree(Cache *cache) {
    Tree tree = (Tree)malloc(sizeof(*Tree));
    if (getBit(cache) == 1) {
        tree->left = tree->right = NULL;
        tree->ch = getByte(cache);
    }
    else {
        tree->left = decodeTree(cache);
        tree->right = decodeTree(cache);
    }
    return tree;
}

Tree decodeHeader(Cache *cache, int *charCounts) {
    fread(charCounts, sizeof(int), 1, cache->file);
    return decodeTree(cache);
}

void decodeBody(Tree root, int charCounts, Cache *cache, FILE *out) {
    int count = 0;
    while (count < charCounts) {
        Tree tree = root;
        while (tree->left != NULL) {
            if (getBit(cache) == 1) {
                tree = tree->right;
            }
            else {
                tree = tree->left;
            }
        }
        fputc(tree->ch, out);
        count += 1;
    }
}

#define writeCached(cache) if ((cache).bit) fputc((cache).byte, (cache).file)
void encode(FILE *src, FILE *out) {
    Tree tree = buildTree(src);
    Cache cache = { 0, 0, out };
    encodeHeader(tree, &cache);
    encodeBody(tree, src, &cache);
    writeCached(cache);
    deletTree(tree);
}

void decode(FILE *src, FILE *out) {
    int charCounts;
    Cache cache = { 0, CHAR_BIT, src };
    Tree tree = decodeHeader(&cache, &charCounts);
    decodeBody(tree, charCounts, &cache, out);
    deletTree(tree);
}
