#include <bits/stdc++.h>
#include <filesystem>

#include "lsof.hpp"

using namespace std;
using namespace std::filesystem;

const char *proc = "/proc/";

int main(int argc, char* argv[]) {
	if (argc%2 == 0) {
		cout << "Arguments Error!";
		return 0;
	}
	for (int i=0; i<argc; ++i)
		if (i>0) ARG.push_back(argv[i]);

	if (stat(proc, buf) == -1)
		perror("stat");

	vector<Basic*> process;
	for (auto e : directory_iterator(proc)) {
		if (e.status().type() == file_type::directory) {
			string dir = absolute(e);
			string file = e.path().filename();
			if (isdigit(file[0])) {
				stat(dir.c_str(), buf);
				Basic *info = new Basic(proc, file, buf->st_uid);
				process.push_back(info);
			}
		}
	}

	printf("COMMAND		PID		USER	FD			TYPE		NODE			NAME\n");
	vector<string> hw1{"cwd", "root", "exe", "maps", "fd"};
	for (auto info : process) {
		string pid = info->PID;
		directory_iterator detail;
		try {
			detail = directory_iterator(proc + pid);
		}
		catch (filesystem_error const& ex) {
			cout << proc + pid
				<< " DIR UNEXISTED!\n";
			continue;
		}
		for (auto e : hw1) {
			string dir = proc + pid + "/" + e;
			string file = e;

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
