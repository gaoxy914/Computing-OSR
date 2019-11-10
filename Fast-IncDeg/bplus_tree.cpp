#include "bplus_tree.h"

namespace bpt {
	/* custom compare operator for STL algorithms */
	OPERATOR_KEYCMP(index_t)
	OPERATOR_KEYCMP(record_t)

	/* helper iterating function */
	template<class T>
	inline typename T::child_t begin(T& node) {
		return node.children;
	}
	template<class T>
	inline typename T::child_t end(T& node) {
		return node.children + node.n;
	}

	/* helper searching function */
	inline index_t* find(internal_node_t& node, const key_t& key) {
		return upper_bound(begin(node), end(node) - 1, key);
	}
	inline record_t* find(leaf_node_t& node, const key_t& key) {
		return lower_bound(begin(node), end(node), key);
	}

	void bplus_tree::init_from_empty() {
		// init default meta
		bzero(&meta, sizeof(meta_t));
		meta.order = BP_ORDER;
		meta.value_size = sizeof(value_t);
		meta.key_size = sizeof(key_t);
		meta.height = 1;
		meta.slot = OFFSET_BLOCK;

		// init root node
		internal_node_t root;
		root.next = root.prev = root.parent = 0;
		meta.root_offset = alloc(&root);

		// init empty leaf
		leaf_node_t leaf;
		leaf.next = leaf.prev = 0;
		leaf.parent = meta.root_offset;
		meta.leaf_offset = root.children[0].child = alloc(&leaf);

		// save
		unmap(&meta, OFFSET_META);
		unmap(&root, meta.root_offset);
		unmap(&leaf, root.children[0].child);
	}

	off_t bplus_tree::search_index(const key_t& key) const {
		off_t org = meta.root_offset;
		int height = meta.height;
		while (height > 1) {
			internal_node_t node;
			map(&node, org);
			index_t* i = upper_bound(begin(node), end(node) - 1, key);
			org = i->child;
			--height;
		}
		return org;
	}

	off_t bplus_tree::search_leaf(off_t index, const key_t& key) const {
		internal_node_t node;
		map(&node, index);

		index_t* i = upper_bound(begin(node), end(node) - 1, key);
		return i->child;
	}

	off_t bplus_tree::search_leaf(const key_t& key) const {
		return  search_leaf(search_index(key), key);
	}

	void bplus_tree::insert_record_no_split(leaf_node_t* leaf, const key_t& key, const data_t& data) {
		record_t* where = upper_bound(begin(*leaf), end(*leaf), key);
		std::copy_backward(where, end(*leaf), end(*leaf) + 1);
		value_t value, next_value, prev_value;
		value.data = data;
		off_t offset = alloc(&value);
		where->key = key;
		where->data = offset;
		where->n = 1;
		where->n_le_key = 0;
		leaf->n++;
		/* first key to insert */
		if (leaf->n == 1) {
			unmap(&value, offset);
			return;
		}

		if (where < end(*leaf) - 2) {
			record_t* next = where + 1;
			map(&next_value, next->data);
			value.next = next->data;
			value.prev = next_value.prev;
			if (next_value.prev != 0) {
				map(&prev_value, next_value.prev);
				prev_value.next = offset;
				unmap(&prev_value, next_value.prev);
			}
			next_value.prev = offset;
			unmap(&next_value, next->data);
			unmap(&value, offset);
		}
		else {
			record_t* prev = where - 1;
			map(&prev_value, prev->data);
			value.prev = prev->data;
			value.next = prev_value.next;
			if (prev_value.next != 0) {
				map(&next_value, prev_value.next);
				next_value.prev = offset;
				unmap(&next_value, prev_value.next);
			}
			prev_value.next = offset;
			unmap(&prev_value, prev->data);
			unmap(&value, offset);
		}
	}

	void bplus_tree::insert_key_to_index(off_t offset, const key_t& key, off_t old, off_t after) {
		if (offset == 0) {
			// create new root node
			internal_node_t root;
			root.next = root.prev = root.parent = 0;
			meta.root_offset = alloc(&root);
			meta.height++;

			// insert `old` and `after`
			root.n = 2;
			root.children[0].key = key;
			root.children[0].child = old;
			root.children[1].child = after;

			unmap(&meta, OFFSET_META);
			unmap(&root, meta.root_offset);

			// update children's parent
			reset_index_children_parent(begin(root), end(root),
				meta.root_offset);
			return;
		}

		internal_node_t node;
		map(&node, offset);
		assert(node.n <= meta.order);

		if (node.n == meta.order) {
			// split when full

			internal_node_t new_node;
			node_create(offset, &node, &new_node);

			// find even split point
			size_t point = (node.n - 1) / 2;
			bool place_right = keycmp(key, node.children[point].key) > 0;
			if (place_right)
				++point;

			// prevent the `key` being the right `middle_key`
			// example: insert 48 into |42|45| 6|  |
			if (place_right && keycmp(key, node.children[point].key) < 0)
				point--;

			key_t middle_key = node.children[point].key;

			// split
			std::copy(begin(node) + point + 1, end(node), begin(new_node));
			new_node.n = node.n - point - 1;
			node.n = point + 1;

			// put the new key
			if (place_right)
				insert_key_to_index_no_split(new_node, key, after);
			else
				insert_key_to_index_no_split(node, key, after);

			unmap(&node, offset);
			unmap(&new_node, node.next);

			// update children's parent
			reset_index_children_parent(begin(new_node), end(new_node), node.next);

			// give the middle key to the parent
			// note: middle key's child is reserved
			insert_key_to_index(node.parent, middle_key, offset, node.next);
		}
		else {
			insert_key_to_index_no_split(node, key, after);
			unmap(&node, offset);
		}
	}

	void bplus_tree::insert_key_to_index_no_split(internal_node_t& node, const key_t& key, off_t value) {
		index_t* where = upper_bound(begin(node), end(node) - 1, key);

		// move later index forward
		std::copy_backward(where, end(node), end(node) + 1);

		// insert this key
		where->key = key;
		where->child = (where + 1)->child;
		(where + 1)->child = value;

		node.n++;
	}

	void bplus_tree::reset_index_children_parent(index_t* begin, index_t* end, off_t parent) {
		// this function can change both internal_node_t and leaf_node_t's parent
		// field, but we should ensure that:
		// 1. sizeof(internal_node_t) <= sizeof(leaf_node_t)
		// 2. parent field is placed in the beginning and have same size
		internal_node_t node;
		while (begin != end) {
			map(&node, begin->child);
			node.parent = parent;
			unmap(&node, begin->child, SIZE_NO_CHILDREN);
			++begin;
		}
	}

	template<class T>
	inline void bplus_tree::node_create(off_t offset, T* node, T* next) {
		// new sibling node
		next->parent = node->parent;
		next->next = node->next;
		next->prev = offset;
		node->next = alloc(next);
		// update next node's prev
		if (next->next != 0) {
			T old_next;
			map(&old_next, next->next, SIZE_NO_CHILDREN);
			old_next.prev = node->next;
			unmap(&old_next, next->next, SIZE_NO_CHILDREN);
		}
		unmap(&meta, OFFSET_META);
	}

	void bplus_tree::arrange_leaf_nodes()
	{
		leaf_node_t leaf;
		off_t offset = meta.leaf_offset;
		size_t n = 0;
		while (offset != 0) {
			map(&leaf, offset);
			for (int i = 0; i < leaf.n; i++) {
				n += leaf.children[i].n;
				leaf.children[i].n_le_key += n;
				// cout << leaf.children[i].key.k << " " << leaf.children[i].n_le_key << endl;
			}
			unmap(&leaf, offset);
			offset = leaf.next;
		}
	}

	void bplus_tree::arrange_internal_nodes(size_t height, off_t offset)
	{
		if (height > 1) {
			--height;
			internal_node_t node, child;
			map(&node, offset);
			for (int i = 0; i < node.n; i++) {
				arrange_internal_nodes(height, node.children[i].child);
				map(&child, node.children[i].child);
				node.children[i].n_l_key = (end(child) - 1)->n_l_key;
			}
			unmap(&node, offset);
		}
		else {
			internal_node_t node;
			map(&node, offset);
			leaf_node_t leaf;
			for (int i = 0; i < node.n; i++) {
				map(&leaf, node.children[i].child);
				node.children[i].n_l_key = (end(leaf) - 1)->n_le_key;
			}
			unmap(&node, offset);
		}
	}



	bplus_tree::bplus_tree(const char* p, bool force_empty)
		: fp(NULL), fp_level(0) {
		bzero(path, sizeof(path));
		strcpy(path, p);

		if (!force_empty)
			// read tree from file
			if (map(&meta, OFFSET_META) != 0)
				force_empty = true;

		if (force_empty) {
			open_file("wb+"); // truncate file

			// create empty tree if file doesn't exist
			init_from_empty();
			close_file();
		}
	}


	int bplus_tree::search(const key_t& key, data_t* data) const {
		leaf_node_t leaf;
		map(&leaf, search_leaf(key));

		// finding the record
		record_t* record = find(leaf, key);
		if (record != leaf.children + leaf.n) {
			/*
			off_t offset = record->data;
			size_t i = 0;
			value_t value;
			while (true) {
				map(&value, offset);
				if (!keycmp(value.data.price, key)) {
					cout << value.data.id << " " << value.data.price << endl;
					offset = value.next;
				}
				else {
					break;
				}
			}
			*/
			value_t value;
			map(&value, record->data);
			*data = value.data;

			return keycmp(record->key, key) == 0 ? 1 : -1;
		}
		else {
			return -1;
		}
	}

	int bplus_tree::search_range(key_t* left, const key_t& right, data_t* datas, size_t max, bool* next) const {
		if (left == NULL || keycmp(*left, right) > 0)
			return -1;

		off_t off_left = search_leaf(*left);
		off_t off_right = search_leaf(right);
		off_t off = off_left;
		size_t i = 0;
		record_t* b = NULL;
		record_t* e = NULL;

		leaf_node_t leaf;
		while (off != off_right && off != 0 && i < max) {
			map(&leaf, off);

			// start point
			if (off_left == off)
				b = find(leaf, *left);
			else
				b = begin(leaf);

			// copy
			e = leaf.children + leaf.n;
			for (; b != e && i < max; ++b) {
				value_t value;
				off_t offset = b->data;
				while (true) {
					map(&value, offset);
					if (!keycmp(value.data.key, b->key)) {
						datas[i++] = value.data;
						offset = value.next;
					}
					else {
						break;
					}
				}
			}

			off = leaf.next;
		}

		// the last leaf
		if (i < max) {
			map(&leaf, off_right);

			b = find(leaf, *left);
			e = upper_bound(begin(leaf), end(leaf), right);
			for (; b != e && i < max; ++b) {
				value_t value;
				off_t offset = b->data;
				while (true) {
					map(&value, offset);
					if (!keycmp(value.data.key, b->key)) {
						datas[i++] = value.data;
						offset = value.next;
					}
					else {
						break;
					}
				}
			}
		}

		// mark for next iteration
		if (next != NULL) {
			if (i == max && b != e) {
				*next = true;
				*left = b->key;
			}
			else {
				*next = false;
			}
		}

		return i;
	}

	int bplus_tree::insert(const key_t& key, data_t data) {
		off_t parent = search_index(key);
		off_t offset = search_leaf(parent, key);
		leaf_node_t leaf;
		map(&leaf, offset);

		// check if we have the same key
		if (binary_search(begin(leaf), end(leaf), key)) {
			record_t* record = find(leaf, key);
			value_t new_value, prev_value, next_value;
			off_t new_value_offset = alloc(&new_value);

			map(&next_value, record->data);

			new_value.data = data;
			new_value.next = record->data;
			new_value.prev = next_value.prev;

			if (next_value.prev != 0) {
				map(&prev_value, next_value.prev);
				prev_value.next = new_value_offset;
				unmap(&prev_value, next_value.prev);
			}

			next_value.prev = new_value_offset;

			record->n++;
			record->data = new_value_offset;

			unmap(&next_value, new_value.next);
			unmap(&new_value, next_value.prev);
			unmap(&leaf, offset);

			return record->n;
		}

		if (leaf.n == meta.order) {
			// split when full

			// new sibling leaf
			leaf_node_t new_leaf;
			node_create(offset, &leaf, &new_leaf);

			// find even split point
			size_t point = leaf.n / 2;
			bool place_right = keycmp(key, leaf.children[point].key) > 0;
			if (place_right)
				++point;

			// split
			std::copy(leaf.children + point, leaf.children + leaf.n,
				new_leaf.children);
			new_leaf.n = leaf.n - point;
			leaf.n = point;

			// which part do we put the key
			if (place_right)
				insert_record_no_split(&new_leaf, key, data);
			else
				insert_record_no_split(&leaf, key, data);

			// save leafs
			unmap(&leaf, offset);
			unmap(&new_leaf, leaf.next);

			// insert new index key
			insert_key_to_index(parent, new_leaf.children[0].key,
				offset, leaf.next);
		}
		else {
			insert_record_no_split(&leaf, key, data);
			unmap(&leaf, offset);
		}

		return 0;
	}

	size_t bplus_tree::result_size(const key_t& left, const key_t& right, size_t& start, size_t& end_p, size_t& left_node_id, size_t& right_node_id) const
	{
		off_t off_left = search_leaf(left);
		off_t off_right = search_leaf(right);

		leaf_node_t leaf_left, leaf_right;
		map(&leaf_left, off_left);
		map(&leaf_right, off_right);

		record_t* record_left, * record_right;
		record_left = find(leaf_left, left);
		if (record_left == end(leaf_left)) {
			off_left = leaf_left.next;
			map(&leaf_left, off_left);
			record_left = begin(leaf_left);
		}
		record_right = upper_bound(begin(leaf_right), end(leaf_right), right);
		if (record_right == end(leaf_right)) {
			if (leaf_right.next == 0)
				record_right = end(leaf_right) - 1;
			else {
				off_right = leaf_right.next;
				// cout << off_right << endl;
				map(&leaf_right, off_right);
				record_right = begin(leaf_right);
			}
		}

		// cout << record_left->key.k << " " << record_right->key.k << endl;
		// cout << record_left->n_le_key << " " << record_right->n_le_key << endl;
		start = record_left->n_le_key - record_left->n + 1;
		end_p = record_right->n_le_key - record_right->n;

		value_t value;
		map(&value, record_left->data);
		left_node_id = value.data.id;
		map(&value, record_right->data);
		right_node_id = value.data.id;
		if (!keycmp(right, record_right->key))
			end_p = record_right->n_le_key;
		return end_p - start + 1;
	}

	int n_cmp_ind(index_t ind_a, index_t ind_b) {
		return ind_a.n_l_key < ind_b.n_l_key;
	}

	int n_cmp_rec(record_t rec_a, record_t rec_b) {
		return rec_a.n_le_key < rec_b.n_le_key;
	}

	void bplus_tree::sample(const size_t& number, size_t& sample) const
	{
		off_t offset = meta.root_offset;
		int height = meta.height;
		internal_node_t node;
		index_t index;
		index.n_l_key = number;
		while (height > 0) {
			map(&node, offset);
			// for (int i = 0; i < node.n; i++) {
			//	cout << node.children[i].key.k << ", " << node.children[i].n_l_key << endl;
			//	cout << endl;
			// }
			/*if (number > (end(node) - 2)->n_l_key) {
				// cout << (end(node) - 2)->key.k << ", " << (end(node) - 2)->n_l_key << endl;
				offset = (end(node) - 1)->child;
			}
			else {
				for (int i = 0; i < node.n - 1; i++) {
					// cout << node.children[i].key.k << ", " << node.children[i].n_l_key << endl;
					if (number <= node.children[i].n_l_key) {
						offset = node.children[i].child;
						break;
					}
				}
			}*/

			index_t* i = lower_bound(begin(node), end(node) - 1, index, n_cmp_ind);
			offset = i->child;

			--height;
		}
		leaf_node_t leaf;
		value_t value;
		map(&leaf, offset);
		// cout << leaf.n << endl;

		record_t record;
		record.n_le_key = number;
		record_t* r = lower_bound(begin(leaf), end(leaf) - 1, record, n_cmp_rec);
		size_t n = number - r->n_le_key + r->n;
		offset = r->data;
		while (n > 0) {
			map(&value, offset);
			offset = value.next;
			--n;
		}
		sample = value.data.id;
		/*for (int i = 0; i < leaf.n; i++) {
			if (number <= leaf.children[i].n_le_key) {
				// cout << "number" << number << endl;
				size_t n = number - leaf.children[i].n_le_key + leaf.children[i].n;
				// cout << "leaf_n " << leaf.children[i].n << endl;
				// cout << "leaf_n_le " << leaf.children[i].n_le_key << endl;
				// cout << "n " << n << endl;
				offset = leaf.children[i].data;
				while (n > 0) {
					map(&value, offset);
					offset = value.next;
					--n;
				}
				sample = value.data.id;
				break;
			}
		}*/
	}

	void bplus_tree::build_q(const char* path, const size_t& number) {
		
		data_t datas[DATA_BATCH];
		off_t offset = OFFSET_DATA;
		size_t round = ceil((double)number / DATA_BATCH);
		// cout << round << endl;
		for (size_t i = 0; i < round; i++) {
			int size = read_data(path, offset, DATA_BATCH, datas);
			// cout << size << endl;
			for (int j = 0; j < size; j++) {
				// datas[j].print();
				insert(datas[j].key, datas[j]);
			}
			offset += sizeof(data_t) * DATA_BATCH;
		}

		/*FILE* fp = fopen(path, "rb");
		data_t data;
		for(size_t i = 0; i < number; i++) {
			fread(&data, sizeof(data_t), 1, fp);
			// data.print();
			insert(data.key, data);
		}
		fclose(fp);*/
		arrange_leaf_nodes();
		arrange_internal_nodes(meta.height, meta.root_offset);
	}

	void bplus_tree::build_id(const char* path, const size_t& number) {
		data_t datas[DATA_BATCH];
		off_t offset = OFFSET_DATA;
		size_t round = ceil((double)number / DATA_BATCH);
		for (size_t i = 0; i < round; i++) {
			int size = read_data(path, offset, DATA_BATCH, datas);
			for (int j = 0; j < size; j++) {
				insert(datas[j].id, datas[j]);
			}
			offset += sizeof(data_t) * DATA_BATCH;
		}

		/*FILE* fp = fopen(path, "rb");
		data_t data;
		for (size_t i = 0; i < number; i++) {
			fread(&data, sizeof(data_t), 1, fp);
			insert(data.id, data);
		}
		fclose(fp);*/
	}
}

