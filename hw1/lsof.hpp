#pragma once
#ifndef __lsof_hpp__
#define __lsof_hpp__

#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <filesystem>

typedef struct stat Status;

struct Basic{
	std::string PATH;
	std::string COMMAND;
	std::string PID;
	long UID;
	std::string USER;

	Basic(std::string path, 
		std::string pid,
		int uid) : 
		PATH(path), COMMAND(""), PID(pid), UID(uid), USER("")
	{
		get_COMMAND();
		get_USER();
	};

	Basic(const Basic &src) :
		PATH(src.PATH), 
		COMMAND(src.COMMAND),
		PID(src.PID),
		UID(src.UID),
		USER(src.USER) {};

	void get_COMMAND();
	void get_USER();
};

void Basic::get_COMMAND() {
	std::ifstream file;
	file.open(PATH + PID + "/comm");
	getline(file, COMMAND);
	file.close();
}

void Basic::get_USER() {
	struct passwd *pws;
	pws = getpwuid(UID);
	USER = pws->pw_name;
}

struct Info : Basic {
	Status *BUF;
	std::string FD;
	std::string TYPE;
	std::string NODE;
	std::string NAME;
	std::string PERM;

	Info(const Basic &src, 
		Status *buf,
		std::string &dir,
		std::string &file) : 
		Basic(src), 
		BUF(buf), FD(""), TYPE(""), NODE(""), NAME(dir), PERM("") 
	{
		try {
			TYPE = get_TYPE(std::filesystem::status(NAME));
		}
		catch (std::filesystem::filesystem_error const& ex) {
			TYPE = "unknown";
		}
		check(file);
	};

	void check(std::string &file);
	void list_MAPS();
	void list_FD();
	void print_ALL();
	std::string get_TYPE(std::filesystem::file_status s);
};

void Info::check(std::string &file) {
	stat(NAME.c_str(), BUF);
	NODE = std::to_string(BUF->st_ino);
	if (file == "cwd") {
		FD = "cwd";
		try {
			std::filesystem::directory_iterator check(NAME);
			NAME = std::filesystem::read_symlink(NAME);
		}
		catch (std::filesystem::filesystem_error const& ex) {
			NODE = "";
			PERM = "(Permission denied)";
		}
	}
	else if (file == "root") {
		FD = "rtd";
		try {
			std::filesystem::directory_iterator check(NAME);
			NAME = std::filesystem::read_symlink(NAME);
		}
		catch (std::filesystem::filesystem_error const& ex) {
			NODE = "";
			PERM = "(Permission denied)";
		}
	}
	else if (file == "exe") {
		FD = "txt";
		try {
			std::filesystem::is_symlink(std::filesystem::status(NAME));
		}
		catch (std::filesystem::filesystem_error const& ex) {
			NODE = "";
			PERM = "(Permission denied)";
		}
		if (TYPE == "REG")
			NAME = std::filesystem::read_symlink(NAME);
	}
	else if (file == "fd") {
		FD = "NOFD";
		try {
			std::filesystem::directory_iterator check(NAME);
		}
		catch (std::filesystem::filesystem_error const& ex) {
			NODE = "";
			TYPE = "";
			PERM = "(Permission denied)";
		}
	}
}

void Info::list_MAPS() {
	FD = "mem";
	std::ifstream f;
	f.open(NAME);
	std::string target;
	while (getline(f, target)) {
		std::stringstream ss(target);
		std::string tmp;
		std::vector<std::string> v;
		while (ss >> tmp)
			v.push_back(tmp);
		int len = v.size();
		if (v[len-1][0] != '/')
			continue;
		NAME = v[len-1];
		NODE = v[len-2];

		print_ALL();
	}
	f.close();
}

void Info::list_FD() {
	std::filesystem::directory_iterator detail(NAME);
	for (auto e:detail) {
		// e: /proc/<pid>/fd/<number>
		std::string dir = std::filesystem::absolute(e);
		std::string file = e.path().filename();
		stat(dir.c_str(), BUF);

		mode_t mode = BUF->st_mode;
		if ( (mode&S_IRUSR) and (mode&S_IWUSR) )
			FD = file + "u";
		else if(mode & S_IRUSR)
			FD = file + "r";
		else if (mode & S_IWUSR)
			FD = file + "w";

		NAME = std::filesystem::read_symlink(e);
		TYPE = get_TYPE(std::filesystem::status(NAME));
		if (NAME.find("socket:") != std::string::npos)
			TYPE = "SOCK";
		if (NAME.find("pipe:") != std::string::npos)
			TYPE = "FIFO";
		if (NAME.find("anon_inode:") != std::string::npos)
			TYPE = "a_inode";

		NODE = std::to_string(BUF->st_ino);


		print_ALL();
	}
}

void Info::print_ALL() {
	printf("%s		%s		%s	%s			%s		%s			%s %s\n", 
		COMMAND.c_str(),
		PID.c_str(),
		USER.c_str(),
		FD.c_str(),
		TYPE.c_str(),
		NODE.c_str(),
		NAME.c_str(),
		PERM.c_str());
}

std::string Info::get_TYPE(std::filesystem::file_status s) {
    switch(s.type())
    {
        case std::filesystem::file_type::none: 
			return "NONE";
        case std::filesystem::file_type::not_found: 
			return "";
        case std::filesystem::file_type::regular: 
			return "REG";
        case std::filesystem::file_type::directory: 
			return "DIR";
        case std::filesystem::file_type::symlink: 
			return "SYM";
        case std::filesystem::file_type::block: 
			return "BLOCK";
        case std::filesystem::file_type::character: 
			return "CHR";
        case std::filesystem::file_type::fifo: 
			return "FIFO";
        case std::filesystem::file_type::socket: 
			return "IPC";
        case std::filesystem::file_type::unknown: 
			return "unknown";
        default: 
			return "default";
    }
}

#endif
