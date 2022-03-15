#include <bits/stdc++.h>
#include <filesystem>

#include "lsof.hpp"

using namespace std;
using namespace std::filesystem;

Status *buf = new Status;
const char *proc = "/proc/";

int main() {
	if (stat(proc, buf) == -1)
		perror("stat");

	vector<string> pids{"1", "2"};
	/*
	for (auto e : directory_iterator(proc))
		if (e.status().type() == file_type::directory) {
			string dir = e.path().filename();
			if (isdigit(dir[0]))
				pids.push_back(dir);
		}
		*/

	printf("COMMAND		PID		USER	FD			TYPE		NODE			NAME\n");
	for (auto pid:pids) {
		Basic *info = new Basic(proc, pid, buf->st_uid);

		directory_iterator sub_detail( proc+pid );
		for (auto e:sub_detail) {
			string dir = absolute(e);
			string file = e.path().filename();
			Info *all = new Info(*info, buf, dir, file);
			int state = stat(dir.c_str(), buf);

			if (file == "cwd" or file == "root" or file == "exe")
				all->print_ALL();

			if (file == "fd") {
				if (all->TYPE == "DIR")
					all->list_FD();
				else all->print_ALL();
			}

			if (state != -1 and file == "maps")
				all->list_MAPS();
		}
	}
	return 0;
}
