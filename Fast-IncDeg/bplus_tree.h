#pragma once

#include "util.h"
#include <cassert>
#include <cstring>

/* predefined B+ info */
#define bzero(a, b) memset(a, 0, b)

namespace bpt {

	#define BP_ORDER 128

	struct key_t {
		char k[16];

		key_t(const char* str = "")
		{
			bzero(k, sizeof(k));
			strcpy(k, str);
		}

		key_t(const double& key) {
			bzero(k, sizeof(k));
			sprintf(k, "%f", key);
		}
	};

	inline int keycmp(const key_t& a, const key_t& b) {
		int x = strlen(a.k) - strlen(b.k);
		return x == 0 ? strcmp(a.k, b.k) : x;
	}

#define OPERATOR_KEYCMP(type) \
    bool operator< (const key_t &l, const type &r) {\
        return keycmp(l, r.key) < 0;\
    }\
    bool operator< (const type &l, const key_t &r) {\
        return keycmp(l.key, r) < 0;\
    }\
    bool operator== (const key_t &l, const type &r) {\
        return keycmp(l, r.key) == 0;\
    }\
    bool operator== (const type &l, const key_t &r) {\
        return keycmp(l.key, r) == 0;\
    }


	struct value_t {
		off_t prev;
		off_t next;

		data_t data;
	};

	/* offsets */
#define OFFSET_META 0
#define OFFSET_BLOCK OFFSET_META + sizeof(meta_t)
#define SIZE_NO_CHILDREN sizeof(leaf_node_t) - BP_ORDER * sizeof(record_t)

/* meta information of B+ tree */
	typedef struct {
		size_t order; /* `order` of B+ tree */
		size_t value_size; /* size of value */
		size_t key_size;   /* size of key */
		size_t internal_node_num; /* how many internal nodes */
		size_t leaf_node_num;     /* how many leafs */
		size_t height;            /* height of tree (exclude leafs) */
		off_t slot;        /* where to store new block */
		off_t root_offset; /* where is the root of internal nodes */
		off_t leaf_offset; /* where is the first leaf */
	} meta_t;

	/* internal nodes' index segment */
	struct index_t {
		key_t key;
		off_t child; /* child's offset */

		size_t n_l_key; /* how many data with k < key */
	};

	/***
	 * internal node block
	 ***/
	struct internal_node_t {
		typedef index_t* child_t;
		off_t parent; /* parent node offset */
		off_t next;
		off_t prev;
		size_t n; /* how many children */
		index_t children[BP_ORDER];
	};

	/* the final record of value */
	struct record_t {
		key_t key;
		// value_t value;

		size_t n_le_key; /* how many data with k <= key */
		size_t n; /* how many data */
		off_t data;
	};

	/* leaf node block */
	struct leaf_node_t {
		typedef record_t* child_t;
		off_t parent; /* parent node offset */
		off_t next;
		off_t prev;
		size_t n; /* total number in leaf node */
		record_t children[BP_ORDER];
	};

	class bplus_tree
	{
		char path[512];
		meta_t meta;
		/* init empty tree */
		void init_from_empty();
		/* find index */
		off_t search_index(const key_t& key) const;
		/* find leaf */
		off_t search_leaf(off_t index, const key_t& key) const;
		off_t search_leaf(const key_t& key) const;
		/* insert into leaf without split */
		void insert_record_no_split(leaf_node_t* leaf, const key_t& key, const data_t& data);
		/* add key to the internal node */
		void insert_key_to_index(off_t offset, const key_t& key, off_t value, off_t after);
		void insert_key_to_index_no_split(internal_node_t& node, const key_t& key, off_t value);
		/* change children's parent */
		void reset_index_children_parent(index_t* begin, index_t* end, off_t parent);
		template<class T>
		void node_create(off_t offset, T* node, T* next);
		/* multi-level file open/close */
		mutable FILE* fp;
		mutable int fp_level;
		void open_file(const char* mode = "rb+") const {
			// `rb+` will make sure we can write everywhere without truncating file
			if (fp_level == 0)
				fp = fopen(path, mode);
			++fp_level;
		}

		void close_file() const {
			if (fp_level == 1)
				fclose(fp);
			--fp_level;
		}

		/* alloc from disk */
		off_t alloc(size_t size) {
			off_t slot = meta.slot;
			meta.slot += size;
			return slot;
		}

		off_t alloc(leaf_node_t* leaf) {
			leaf->n = 0;
			meta.leaf_node_num++;
			return alloc(sizeof(leaf_node_t));
		}

		off_t alloc(internal_node_t* node) {
			node->n = 1;
			meta.internal_node_num++;
			return alloc(sizeof(internal_node_t));
		}

		off_t alloc(value_t* value) {
			value->prev = 0;
			value->next = 0;
			return alloc(sizeof(value_t));
		}

		void unalloc(leaf_node_t* leaf, off_t offset) {
			--meta.leaf_node_num;
		}

		void unalloc(internal_node_t* node, off_t offset) {
			--meta.internal_node_num;
		}

		/* read block from disk */
		int map(void* block, off_t offset, size_t size) const {
			open_file();
			fseek(fp, offset, SEEK_SET);
			size_t rd = fread(block, size, 1, fp);
			close_file();
			return rd - 1;
		}

		template<class T>
		int map(T* block, off_t offset) const {
			return map(block, offset, sizeof(T));
		}

		/* write block to disk */
		int unmap(void* block, off_t offset, size_t size) const {
			open_file();
			fseek(fp, offset, SEEK_SET);
			size_t wd = fwrite(block, size, 1, fp);
			close_file();
			return wd - 1;
		}

		template<class T>
		int unmap(T* block, off_t offset) const {
			return unmap(block, offset, sizeof(T));
		}

		void arrange_leaf_nodes();

		void arrange_internal_nodes(size_t height, off_t offset);

	public:
		bplus_tree() { }

		bplus_tree(const char* path, bool force_empty = false);

		int search(const key_t& key, data_t* data) const;
		int search_range(key_t* left, const key_t& right, data_t* datas, size_t max, bool* next = NULL) const;

		int insert(const key_t& key, data_t data);

		size_t result_size(const key_t& left, const key_t& right, size_t& start, size_t& end, size_t& left_node_id, size_t& right_node_id) const;

		void sample(const size_t& number, size_t& sample) const;

		void build_q(const char* path, const size_t& number);

		void build_id(const char* path, const size_t& number);
	};

}

