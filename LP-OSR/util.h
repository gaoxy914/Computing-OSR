#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstring>

#define OFFSET_DATA 0
#define DATA_BATCH 200

using namespace std;

/*
 * AC, PN -> STR, CT, ST
 * zip -> CT, ST
 * id -> name, PR
 * CT, STR -> zip
 */
struct order_t {
	size_t id; // unique_id
	double key; // b+tree index, price
	char name[512];

	char area_code[4];
	char phone_number[32]; // generate with area_code

	char street[80];
	char country[40];
	char city[40];
	char state[32];
	char zip[16];

	void print() const {
		cout << id << ","
			<< key << ","
			<< area_code << ","
			<< phone_number << ","
			<< street << ","
			<< country << ","
			<< city << ","
			<< state << ","
			<< zip << endl;
	}

	string lhs_fd1() const {
		return (string)area_code + (string)phone_number;
	}

	string rhs_fd1() const {
		return (string)street + (string)city + (string)state;
	}

	string lhs_fd2() const {
		return zip;
	}

	string rhs_fd2() const {
		return (string)city + (string)state;
	}

	string lhs_fd3() const {
		return to_string(id);
	}

	string rhs_fd3() const {
		return (string)name + to_string(key);
	}

	string lhs_fd4() const {
		return (string)city + (string)street;
	}

	string rhs_fd4() const {
		return zip;
	}
};


/*
 * title -> authors
 * ee -> title
 * year, publication, pages -> title, ee, url
 * url -> title
 */
struct dblp_t {
	size_t id;
	size_t key; // b+tree index, price

	char title[1500];
	char authors[5000];
	char year[5];
	char publication[150];
	char pages[100];
	char ee[600];
	char url[100];

	void print() const {
		cout << id << ","
			<< key << ","
			<< title << ","
			<< authors << ","
			<< year << ","
			<< publication << ","
			<< pages << ","
			<< ee << ","
			<< url << endl;
	}

	string lhs_fd1() const {
		return title;
	}

	string rhs_fd1() const {
		return authors;
	}

	string lhs_fd2() const {
		return ee;
	}

	string rhs_fd2() const {
		return title;
	}

	string lhs_fd3() const {
		return (string)year + (string)publication + (string)pages;
	}

	string rhs_fd3() const {
		return (string)title + (string)ee + (string)url;
	}

	string lhs_fd4() const {
		return url;
	}

	string rhs_fd4() const {
		return title;
	}
};

typedef dblp_t data_t;

int read_data(const char* path, const off_t& offset, const size_t& max, data_t* datas);

bool is_conflict(const order_t& order1, const order_t& order2);
bool is_conflict(const dblp_t& dblp1, const dblp_t& dblp2);

bool is_conflict_fd1(const order_t& data_l, const order_t& data_r);
bool is_conflict_fd2(const order_t& data_l, const order_t& data_r);
bool is_conflict_fd3(const order_t& data_l, const order_t& data_r);
bool is_conflict_fd4(const order_t& data_l, const order_t& data_r);

bool is_conflict_fd1(const dblp_t& dblp1, const dblp_t& dblp2);
bool is_conflict_fd2(const dblp_t& dblp1, const dblp_t& dblp2);
bool is_conflict_fd3(const dblp_t& dblp1, const dblp_t& dblp2);
bool is_conflict_fd4(const dblp_t& dblp1, const dblp_t& dblp2);


class Generator {
	vector<string> books;
	vector<string> US;

	vector<string> dblps;

	void parse(const string& content, vector<string>& tuple);

	void populate(const size_t& Id, order_t* order);
	void pollute(const size_t& Id, const size_t& clean_Id, order_t* order);
	void pollute_bd(const size_t& Id, const size_t& clean_Id, order_t* order);

	void populate(const size_t& Id, dblp_t* dblp);
	void pollute(const size_t& Id, const size_t& clean_Id, dblp_t* dblp);
	void pollute_bd(const size_t& Id, const size_t& clean_Id, dblp_t* dblp);

public:
	void load_rawdata();

	void gen_data(const char* path, const size_t& number, const double& rho);
	void gen_data_bd(const char* path, const size_t& number, const double& rho, const size_t& max_degree);
};

// void gen_rand(const char* path, const size_t& number);

class RandomSequenceOfUnique {
	unsigned int m_index;
	unsigned int m_intermediateOffset;

	static unsigned int permuteQPR(unsigned int x) {
		static const unsigned int prime = 4294967291u;
		if (x >= prime)
			return x;
		unsigned int residue = ((unsigned long long) x * x) % prime;
		return (x <= prime / 2) ? residue : prime - residue;
	}
public:
	RandomSequenceOfUnique(unsigned int seedBase, unsigned int seedOffset) {
		m_index = permuteQPR(permuteQPR(seedBase) + 0x682f0161);
		m_intermediateOffset = permuteQPR(permuteQPR(seedOffset) + 0x46790905);
	}

	unsigned int next() {
		return permuteQPR((permuteQPR(m_index++) + m_intermediateOffset) ^ 0x5bf0365);
	}
};