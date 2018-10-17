#pragma once
struct Rec {
	double x;
	double y;
	double width;
	double height;
	int id;
	int new_id;
	Rec() {
		new_id = 1;   //初始化new id为true
	}
};

struct recId {
	int id;
	int new_id;
	recId() {
		new_id = 1;   //初始化new id为true
	}

	bool operator < (const recId& g)const {
		if (id < g.id) {
			return true;
		}
		return false;
	}
};
