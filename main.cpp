//
// Created by EgorRychkov on 09.01.2025.
//

#include <iostream>
#include "vector"
#include "algorithm"
#include "bitset"
#include "cstring"

typedef unsigned char uchar;
typedef unsigned int uint;

FILE *source_file, *zipped_file, *unzipped_file;


struct Forest {
    int weight = 0;
    int root = 0;

    Forest() {
        weight = 0;
        root = -1;
    }

    Forest(int _weight, int _root) {
        weight = _weight;
        root = _root;
    }
};

struct Tree {
    int left = -1, right = -1;
    int parent = -1;
    int element = 0;

    Tree() {
        left = -1;
        right = -1;
        parent = -1;
        element = 0;
    }

    Tree(int _left, int _right, int _parent) {
        left = _left;
        right = _right;
        parent = _parent;
    }
};

struct Code {
    int len = 0;
    uint code = 0;

    Code() {
        len = 0;
        code = 0;
    }
};

std::pair<int, int> min_min(std::vector<Forest> &forest, int forest_len) {
    int min_1 = 1000000000, min_2 = 1000000000;
    int min_1_index = -1, min_2_index = -1;
    for (int i = 0; i < forest_len; i++) {
        if (forest[i].weight < min_1) {
            min_2 = min_1;
            min_1 = forest[i].weight;
            min_2_index = min_1_index;
            min_1_index = i;
            continue;
        }
        if (forest[i].weight == min_1) {
            if (min_2 > forest[i].weight) {
                min_2_index = i;
                min_2 = forest[i].weight;
            }
            continue;
        }
        if (forest[i].weight > min_1) {
            if (min_2 > forest[i].weight) {
                min_2_index = i;
                min_2 = forest[i].weight;
            }
        }
    }
    return {min_1_index, min_2_index};
}

void dfs(int v, uint s, std::vector<int> &chain, std::vector<Tree> &tree, int depth, Code (&codes)[256]) {
    if (v == -1) {
        return;
    }
    if (tree[v].right == -1 and tree[v].left == -1) {
        codes[chain[v]].len = depth;
        codes[chain[v]].code = s;
        return;
    }
    dfs(tree[v].left, (s << 1), chain, tree, depth + 1, codes);
    dfs(tree[v].right, ((s << 1) | uchar(1)), chain, tree, depth + 1, codes);
}

std::vector<int> get_symbol(int v, uchar byte, int len, int go, std::vector<Tree> &tree) {
    if (tree[v].right == -1 and tree[v].left == -1) {
        return {v, 1, go};
    }
    if (len == 8) {
        return {v, 0, go};
    }
    if ((byte >> (7 - len)) & 1) {
        return get_symbol(tree[v].right, byte, len + 1, go + 1, tree);
    } else {
        return get_symbol(tree[v].left, byte, len + 1, go + 1, tree);
    }
}

void f_write_int(int number, FILE *file_to_write) {
    uchar byte = 0;
    int len_number = number;
    int len = 0;
    for (int i = 0; i < 4; i++) {
        if (len_number != 0) {
            len++;
        }
        len_number >>= 8;
    }
    fprintf(file_to_write, "%c", uchar(len));
    for (int i = 0; i < len; i++) {
        byte |= number;
        number >>= 8;
        fprintf(file_to_write, "%c", byte);
        byte = 0;
    }
}

int f_read_int(FILE *file_to_read) {
    int number = 0;
    unsigned char ch;
    int index = 0;
    int len = 0;
    fscanf(file_to_read, "%c", &ch);
    len = ch;
    for (int i = 0; i < len; i++) {
        fscanf(file_to_read, "%c", &ch);
        number |= (int(ch) << (index * 8));
        index++;
    }
    return number;
}

void f_write_cluster(FILE *file_to_write, int a, int b, int c, int d) {
    int len_a = 1, len_b = 1, len_c = 1, len_d = 1;
    int a_copy = a, b_copy = b, c_copy = c, d_copy = d;
    for (int i = 0; i < 3; i++) {
        a_copy >>= 8;
        b_copy >>= 8;
        c_copy >>= 8;
        d_copy >>= 8;
        if (a_copy != 0) {
            len_a++;
        }
        if (b_copy != 0) {
            len_b++;
        }
        if (c_copy != 0) {
            len_c++;
        }
        if (d_copy != 0) {
            len_d++;
        }
    }
    uchar byte_len = 0;
    len_a--;
    len_b--;
    len_c--;
    len_d--;
    byte_len |= (uchar(len_a) << 6);
    byte_len |= (uchar(len_b) << 4);
    byte_len |= (uchar(len_c) << 2);
    byte_len |= (uchar(len_d) << 0);
    fprintf(file_to_write, "%c", byte_len);
    uchar byte = 0;
    for (int i = 0; i <= len_a; i++) {
        byte |= a;
        a >>= 8;
        fprintf(file_to_write, "%c", byte);
        byte = 0;
    }
    for (int i = 0; i <= len_b; i++) {
        byte |= b;
        b >>= 8;
        fprintf(file_to_write, "%c", byte);
        byte = 0;
    }
    for (int i = 0; i <= len_c; i++) {
        byte |= c;
        c >>= 8;
        fprintf(file_to_write, "%c", byte);
        byte = 0;
    }
    for (int i = 0; i <= len_d; i++) {
        byte |= d;
        d >>= 8;
        fprintf(file_to_write, "%c", byte);
        byte = 0;
    }
}

std::vector<int> f_read_cluster(FILE *file_to_read) {
    unsigned char ch;
    fscanf(file_to_read, "%c", &ch);

    int len_a = 0, len_b = 0, len_c = 0, len_d = 0;
    len_a = int((ch >> 6) & uchar(3));
    len_b = int((ch >> 4) & uchar(3));
    len_c = int((ch >> 2) & uchar(3));
    len_d = int((ch >> 0) & uchar(3));
    len_a++;
    len_b++;
    len_c++;
    len_d++;
    int a = 0, b = 0, c = 0, d = 0;
    int index = 0;
    for (int i = 0; i < len_a; i++) {
        fscanf(file_to_read, "%c", &ch);
        a |= (int(ch) << (index * 8));
        index++;
    }
    index = 0;
    for (int i = 0; i < len_b; i++) {
        fscanf(file_to_read, "%c", &ch);
        b |= (int(ch) << (index * 8));
        index++;
    }
    index = 0;
    for (int i = 0; i < len_c; i++) {
        fscanf(file_to_read, "%c", &ch);
        c |= (int(ch) << (index * 8));
        index++;
    }
    index = 0;
    for (int i = 0; i < len_d; i++) {
        fscanf(file_to_read, "%c", &ch);
        d |= (int(ch) << (index * 8));
        index++;
    }
    return {a, b, c, d};
}

int main(int argc, char *argv[]) {
    std::string archive_file_name, source_file_name;
    archive_file_name = argv[2];
    source_file_name = argv[3];
    if (!strcmp("encode", argv[1])) {
        source_file = fopen(source_file_name.c_str(), "rb");
        unsigned char ch;
        int freq[256];
        std::fill(freq, freq + 256, 0);
        int elements_cnt = 0;
        while (fscanf(source_file, "%c", &ch) == 1) {
            freq[ch]++;
            if (freq[ch] == 1) {
                elements_cnt++;
            }
        }
        std::vector<Forest> forest(elements_cnt);
        std::vector<Tree> tree(elements_cnt, {-1, -1, -1});
        std::vector<int> chain(256);
        int element_index = 0;
        for (int i = 0; i < 256; i++) {
            if (freq[i] != 0) {
                chain[element_index] = i;
                forest[element_index].weight = freq[i];
                forest[element_index].root = element_index;
                element_index++;
            }
        }
        int forest_len = forest.size();
        while (forest_len > 1) {
            std::pair<int, int> min_ans = min_min(forest, forest_len);
            int min_1_index = min_ans.first;
            int min_2_index = min_ans.second;
            tree.push_back(Tree(forest[min_1_index].root, forest[min_2_index].root, -1));
            elements_cnt++;
            tree[forest[min_1_index].root].parent = elements_cnt - 1;
            tree[forest[min_2_index].root].parent = elements_cnt - 1;
            if (min_1_index > min_2_index) {
                std::swap(min_2_index, min_1_index);
            }
            forest[min_1_index] = Forest(forest[min_1_index].weight + forest[min_2_index].weight, elements_cnt - 1);
            forest[min_2_index] = forest[forest_len - 1];
            forest_len--;
        }
        for (int i = 0; i < tree.size(); i++) {
            tree[i].element = chain[i];
        }
        Code codes[256];
        if (tree.size() == 1) {
            dfs(tree.size() - 1, uint(0), chain, tree, 1, codes);
        } else {
            dfs(tree.size() - 1, uint(0), chain, tree, 0, codes);
        }
        fclose(source_file);


        source_file = fopen(source_file_name.c_str(), "rb");
        zipped_file = fopen(archive_file_name.c_str(), "wb");
        f_write_int(int(tree.size()), zipped_file);
        for (int i = 0; i < tree.size(); i++) {
            f_write_cluster(zipped_file, tree[i].left, tree[i].right, tree[i].parent, tree[i].element);
        }
        uchar new_byte = 0;
        int len_byte = 0;
        while (fscanf(source_file, "%c", &ch) == 1) {
            uint new_code = codes[ch].code;
            int len_code = codes[ch].len;
            while (len_code > 0) {
                int len_sub_code = 8 - len_byte;
                if (len_code - len_sub_code >= 0) {
                    new_byte |= (new_code >> (len_code - len_sub_code));
                    len_byte += len_sub_code;
                } else {
                    new_byte |= (new_code << (len_sub_code - len_code));
                    len_byte += len_code;
                }
                len_code -= std::min(len_sub_code, len_code);
                if (len_byte == 8) {
                    len_byte = 0;
                    fprintf(zipped_file, "%c", new_byte);
                    new_byte = 0;
                }
            }
        }
        if (len_byte != 0) {
            fprintf(zipped_file, "%c", new_byte);
            fprintf(zipped_file, "%c", uchar(len_byte));
        } else {
            fprintf(zipped_file, "%c", uchar(8));
        }
        fclose(source_file);
        fclose(zipped_file);
    } else {
        unsigned char ch;
        int len_byte;
        zipped_file = fopen(archive_file_name.c_str(), "rb");
        unzipped_file = fopen(source_file_name.c_str(), "wb");

        int tree_len;
        tree_len = f_read_int(zipped_file);
        std::vector<Tree> unzipped_tree(tree_len);
        for (int i = 0; i < tree_len; i++) {
            std::vector<int> read = f_read_cluster(zipped_file);
            unzipped_tree[i].left = read[0];
            unzipped_tree[i].right = read[1];
            unzipped_tree[i].parent = read[2];
            unzipped_tree[i].element = read[3];
        }
        int root = unzipped_tree.size() - 1;
        int node = root;

        std::vector<uchar> input;
        while (fscanf(zipped_file, "%c", &ch) == 1) {
            input.push_back(ch);
        }
        uchar back = input.back();
        input.pop_back();
        for (int i = 0; i < input.size(); i++) {
            uchar t = input[i];
            len_byte = 0;
            while (len_byte != 8) {
                if (i == input.size() - 1 and len_byte >= back) {
                    break;
                }
                int go = 0;
                if (unzipped_tree.size() == 1) {
                    go = 1;
                }
                std::vector<int> v = get_symbol(node, t, len_byte, go, unzipped_tree);
                if (v[1] == 1) {
                    node = unzipped_tree.size() - 1;
                    fprintf(unzipped_file, "%c", unzipped_tree[v[0]].element);
                    len_byte += v[2];
                } else {
                    node = v[0];
                    break;
                }
            }
        }
        fclose(zipped_file);
        fclose(unzipped_file);
    }
    return 0;
}
