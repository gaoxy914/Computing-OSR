#include "util.h"

/*
 * AC, PN -> STR, CT, ST
 * zip -> CT, ST
 * id -> name, PR
 * CT, STR -> zip
 */
bool is_conflict(const order_t& order1, const order_t& order2) {
	if (!strcmp(order1.area_code, order2.area_code)
		&& !strcmp(order1.phone_number, order2.phone_number))
		if (strcmp(order1.street, order2.street) || strcmp(order1.city, order2.city)
			|| strcmp(order1.state, order2.state)) {
			// cout << "ac, pn -> str, ct, st" << endl;
			return true;
		}

	if (!strcmp(order1.zip, order2.zip))
		if (strcmp(order1.city, order2.city) || strcmp(order1.state, order2.state)) {
			// cout << "zip -> ct, st" << endl;
			return true;
		}

	if (!strcmp(order1.city, order2.city)
		&& !strcmp(order1.street, order2.street))
		if (strcmp(order1.zip, order2.zip)) {
			// cout << "ct, str -> zip" << endl;
			return true;
		}

	return false;
}

/*
 * title -> authors
 * ee -> title
 * year, publication, pages -> title, ee, url
 * url -> title
 */
bool is_conflict(const dblp_t& dblp1, const dblp_t& dblp2) {
	if (!strcmp(dblp1.title, dblp2.title))
		if (strcmp(dblp1.authors, dblp2.authors)) {
			// cout << "title -> authors" << endl;
			return true;
		}

	if (!strcmp(dblp1.ee, dblp2.ee))
		if (strcmp(dblp1.title, dblp2.title)) {
			// cout << "ee -> title" << endl;
			return true;
		}

	if (!strcmp(dblp1.year, dblp2.year) && !strcmp(dblp1.pages, dblp2.pages)
		&& !strcmp(dblp1.publication, dblp2.publication))
		if (strcmp(dblp1.title, dblp2.title) || strcmp(dblp1.ee, dblp2.ee)
			|| strcmp(dblp1.url, dblp2.url)) {
			// cout << "year, publication, pages -> title, ee, url" << endl;
			return true;
		}


	if (!strcmp(dblp1.url, dblp2.url))
		if (strcmp(dblp1.title, dblp2.title)) {
			// cout << "url -> title" << endl;
			return true;
		}

	return false;
}

bool is_conflict_fd1(const order_t& order1, const order_t& order2) {

	if (!strcmp(order1.area_code, order2.area_code)
		&& !strcmp(order1.phone_number, order2.phone_number))
		if (strcmp(order1.street, order2.street) || strcmp(order1.city, order2.city)
			|| strcmp(order1.state, order2.state)) {
			// cout << "ac, pn -> str, ct, st" << endl;
			return true;
		}

	return false;
}

bool is_conflict_fd2(const order_t& order1, const order_t& order2) {
	if (!strcmp(order1.zip, order2.zip))
		if (strcmp(order1.city, order2.city) || strcmp(order1.state, order2.state)) {
			// cout << "zip -> ct, st" << endl;
			return true;
		}

	return false;
}

bool is_conflict_fd3(const order_t& order1, const order_t& order2) {
	if (order1.id == order2.id)
		if (order1.key != order2.key ||
			strcmp(order1.name, order2.name)) {
			// cout << "id -> name, pr" << endl;
			return true;
		}

	return false;
}

bool is_conflict_fd4(const order_t& order1, const order_t& order2) {
	if (!strcmp(order1.city, order2.city)
		&& !strcmp(order1.street, order2.street))
		if (strcmp(order1.zip, order2.zip)) {
			// cout << "ct, str -> zip" << endl;
			return true;
		}

	return false;
}

bool is_conflict_fd1(const dblp_t& dblp1, const dblp_t& dblp2) {
	if (!strcmp(dblp1.title, dblp2.title))
		if (strcmp(dblp1.authors, dblp2.authors))
			return true;

	return false;
}

bool is_conflict_fd2(const dblp_t& dblp1, const dblp_t& dblp2) {
	if (!strcmp(dblp1.ee, dblp2.ee))
		if (strcmp(dblp1.title, dblp2.title))
			return true;

	return false;
}

bool is_conflict_fd3(const dblp_t& dblp1, const dblp_t& dblp2) {

	if (!strcmp(dblp1.year, dblp2.year) && !strcmp(dblp1.pages, dblp2.pages)
		&& !strcmp(dblp1.publication, dblp2.publication))
		if (strcmp(dblp1.title, dblp2.title) || strcmp(dblp1.ee, dblp2.ee)
			|| strcmp(dblp1.url, dblp2.url))
			return true;

	return false;
}

bool is_conflict_fd4(const dblp_t& dblp1, const dblp_t& dblp2) {

	if (!strcmp(dblp1.url, dblp2.url))
		if (strcmp(dblp1.title, dblp2.title))
			return true;

	return false;
}



int read_data(const char* path, const off_t& offset, const size_t& max, data_t* datas) {
	FILE* fp = fopen(path, "rb");
	fseek(fp, offset, SEEK_SET);
	data_t data;
	size_t i = 0;
	while (fread(&data, sizeof(data_t), 1, fp) && i < max)
		datas[i++] = data;

	fclose(fp);
	return i;
}

inline void load_file(const char* path, vector<string>& vec) {
	ifstream inFile(path, ios::in);
	string line;
	int i = 0;
	while (getline(inFile, line) && i < 20000) {
		if (line.length() != 0) {
			vec.push_back(line);
			i++;
		}
	}
	inFile.close();
}

void Generator::load_rawdata() {
	if (sizeof(data_t) == sizeof(order_t)) {
		load_file("raw_data/US.csv", US);
		load_file("raw_data/booklist.csv", books);

	} else
		load_file("raw_data/dblp.csv", dblps);
}

void Generator::parse(const string& content, vector<string>& tuple) {
	int left = 0, right = 0;
	for (; right < content.length(); right++) {
		if (content[right] == ',') {
			tuple.push_back(content.substr(left, right - left));
			left = right + 1;
		}
	}
	tuple.push_back(content.substr(left, right - left));
}

void Generator::populate(const size_t& Id, order_t* order) {
	string name = books[Id % books.size()];
	string content = US[Id % US.size()];
	vector<string> tuple;
	parse(content, tuple);
	// if (tuple.size() < 8) {
	//	cout << content << endl;
	//	return 0;
	//}
	order->id = Id;
	order->key = Id / 10.0 + rand() % 10 / 100.0;
	strcpy(order->name, name.c_str());
	strcpy(order->zip, tuple[0].c_str());
	strcpy(order->city, tuple[1].c_str());
	strcpy(order->state, tuple[2].c_str());
	strcpy(order->country, tuple[4].c_str());
	strcpy(order->area_code, tuple[5].c_str());
	strcpy(order->street, tuple[6].c_str());
	strcpy(order->phone_number, tuple[7].c_str());
	// return 1;
}

void Generator::pollute(const size_t& Id, const size_t& clean_Id, order_t* order) {
	string name = books[Id % books.size()];
	string content = US[clean_Id % US.size()];
	vector<string> tuple;
	parse(content, tuple);
	// if (tuple.size() < 7) {
	//	cout << content << endl;
	//	return 0;
	//}
	order->id = Id;
	order->key = Id / 10.0 + rand() % 10 / 100.0;
	strcpy(order->name, name.c_str());
	strcpy(order->zip, tuple[0].c_str());
	strcpy(order->city, tuple[1].c_str());
	strcpy(order->state, tuple[2].c_str());
	strcpy(order->country, tuple[4].c_str());
	strcpy(order->area_code, tuple[5].c_str());
	strcpy(order->street, tuple[6].c_str());
	strcpy(order->phone_number, tuple[7].c_str());
	// cout << data->zip << endl;

	content = US[rand() % US.size()];
	tuple.clear();
	parse(content, tuple);
	double pollute_b = rand() / (double)RAND_MAX; // 0 : STR, 1 : CT, 2 : ST, 3 : zip
	// cout << pollute_b << endl;
	if (pollute_b < 0.475)
		strcpy(order->street, tuple[6].c_str());
	else if (pollute_b < 0.5)
		strcpy(order->city, tuple[1].c_str());
	else if (pollute_b < 0.525)
		strcpy(order->state, tuple[2].c_str());
	else
		order->zip[rand() % 5] = '0' + rand() % 10;

}

void Generator::pollute_bd(const size_t& Id, const size_t& clean_Id, order_t* order) {
	string name = books[Id % books.size()];
	string content = US[clean_Id % US.size()];
	vector<string> tuple;
	parse(content, tuple);
	// if (tuple.size() < 7) {
	//	cout << content << endl;
	//	return 0;
	//}
	order->id = Id;
	order->key = Id / 10.0 + rand() % 10 / 100.0;
	strcpy(order->name, name.c_str());
	strcpy(order->zip, tuple[0].c_str());
	strcpy(order->city, tuple[1].c_str());
	strcpy(order->state, tuple[2].c_str());
	strcpy(order->country, tuple[4].c_str());
	strcpy(order->area_code, tuple[5].c_str());
	strcpy(order->street, tuple[6].c_str());
	strcpy(order->phone_number, tuple[7].c_str());

	content = US[rand() % US.size()];
	tuple.clear();
	parse(content, tuple);
	strcpy(order->street, tuple[6].c_str());
}

void Generator::populate(const size_t& Id, dblp_t* dblp) {
	string content = dblps[Id % dblps.size()];
	vector<string> tuple;
	parse(content, tuple);
	dblp->id = Id;
	dblp->key = Id;
	strcpy(dblp->title, tuple[0].c_str());
	strcpy(dblp->authors, tuple[1].c_str());
	strcpy(dblp->year, tuple[2].c_str());
	strcpy(dblp->publication, tuple[3].c_str());
	strcpy(dblp->pages, tuple[4].c_str());
	strcpy(dblp->ee, tuple[5].c_str());
	strcpy(dblp->url, tuple[6].c_str());

	// dblp->print();
}

void Generator::pollute(const size_t& Id, const size_t& clean_Id, dblp_t* dblp) {
	string content = dblps[clean_Id % dblps.size()];
	vector<string> tuple;
	parse(content, tuple);
	dblp->id = Id;
	dblp->key = Id;
	strcpy(dblp->title, tuple[0].c_str());
	strcpy(dblp->authors, tuple[1].c_str());
	strcpy(dblp->year, tuple[2].c_str());
	strcpy(dblp->publication, tuple[3].c_str());
	strcpy(dblp->pages, tuple[4].c_str());
	strcpy(dblp->ee, tuple[5].c_str());
	strcpy(dblp->url, tuple[6].c_str());

	content = dblps[rand() % dblps.size()];
	tuple.clear();
	parse(content, tuple);
	double pollute_b = rand() / (double)RAND_MAX;
	if (pollute_b < 0.25)
		strcpy(dblp->title, tuple[0].c_str());
	else if (pollute_b < 0.5)
		strcpy(dblp->authors, tuple[1].c_str());
	else if (pollute_b < 0.75)
		strcpy(dblp->ee, tuple[5].c_str());
	else
		strcpy(dblp->url, tuple[6].c_str());
}

void Generator::pollute_bd(const size_t& Id, const size_t& clean_Id, dblp_t* dblp) {
	string content = dblps[clean_Id % dblps.size()];
	vector<string> tuple;
	parse(content, tuple);
	dblp->id = Id;
	dblp->key = Id;
	strcpy(dblp->title, tuple[0].c_str());
	strcpy(dblp->authors, tuple[1].c_str());
	strcpy(dblp->year, tuple[2].c_str());
	strcpy(dblp->publication, tuple[3].c_str());
	strcpy(dblp->pages, tuple[4].c_str());
	strcpy(dblp->ee, tuple[5].c_str());
	strcpy(dblp->url, tuple[6].c_str());

	content = dblps[rand() % dblps.size()];
	tuple.clear();
	parse(content, tuple);
	strcpy(dblp->title, tuple[0].c_str());
}


void Generator::gen_data(const char* path, const size_t& number, const double& rho) {
	// load_rawdata();

	size_t clean = number * (1 - rho);
	//	size_t dirty = number - clean;
	size_t Id = 0;
	data_t data;
	FILE* fp = fopen(path, "wb");

	while (Id < clean) {
		populate(Id++, &data);
		fwrite(&data, sizeof(data_t), 1, fp);
	}

	while (Id < number) {
		pollute(Id++, rand() % clean, &data);
		fwrite(&data, sizeof(data_t), 1, fp);
	}

	// cout << dirty_count << " " << clean_count << endl;

	fclose(fp);
}

void Generator::gen_data_bd(const char* path, const size_t& number, const double& rho, const size_t& max_degree) {
	// load_rawdata();

	size_t clean = number * (1 - rho);
	size_t Id = 0, clean_Id = 0;
	data_t data;
	FILE* fp = fopen(path, "wb");

	while (Id < clean) {
		populate(Id++, &data);
		fwrite(&data, sizeof(data_t), 1, fp);
	}

	size_t repeat = ceil((double)clean / US.size());
	if (sizeof(data_t) == sizeof(dblp_t))
		repeat = ceil((double)clean / dblps.size());
	// cout << "r:" << repeat << endl;
	size_t degree = max_degree - repeat;
	if (repeat > max_degree)
		degree = 1;

	while (Id < number) {
		size_t conflicts = degree + 1;

		for (int i = 0; i < conflicts && Id < number; i++) {
			pollute_bd(Id++, clean_Id, &data);
			fwrite(&data, sizeof(data_t), 1, fp);
		}
		clean_Id++;
	}
	fclose(fp);
}



