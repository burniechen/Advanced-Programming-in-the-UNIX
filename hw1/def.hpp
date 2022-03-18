#pragma once
#ifndef __def_hpp__
#define __def_hpp__

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

	Basic() : 
		PATH(""), COMMAND(""), PID(""), UID(0), USER("") {}

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

struct Info : Basic {
	Status *BUF;
	std::string FD;
	std::string TYPE;
	std::string NODE;
	std::string NAME;
	std::string PERM;

	Info(const Basic &src, 
		Status *buf) :
		Basic(src), 
		BUF(buf), FD(""), TYPE(""), NODE(""), NAME(""), PERM("") {};

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

	bool operator==(const Info &pre) {
		return (this->COMMAND == pre.COMMAND and
				this->PID == pre.PID and
				this->USER == pre.USER and
				this->FD == pre.FD and
				this->TYPE == pre.TYPE and
				this->NODE == pre.NODE and
				this->NAME == pre.NAME and
				this->PERM == pre.PERM);
	}

	void check(std::string &file);
	void list_MAPS();
	void list_FD();
	void print_ALL();
	std::string get_TYPE(std::filesystem::file_status s);
};

Status *buf = new Status;
Basic trash;
Info pre(trash, buf);
std::string jump_MAPS = "";

std::vector<std::string> ARG;

#endif
