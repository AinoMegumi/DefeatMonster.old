#include"Class.h"
#include"Storage.h"
#include<boost/property_tree/ptree.hpp>
#include<boost/property_tree/xml_parser.hpp>
#include<ctime>
#include<codecvt>
#include<stdexcept>
#include<type_traits>
#include<Shlwapi.h>
#include<string>
#include<fstream>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Kernel32.lib")
constexpr size_t GetPrivateProfileStringBuf = 1024;
std::string getLastErrorText();
static inline void clearErr();

std::string WStringToString(std::wstring oWString) {
	int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, (char *)NULL, 0, NULL, NULL);
	char* cpMultiByte = new char[iBufferSize];
	WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, cpMultiByte, iBufferSize, NULL, NULL);
	std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);
	delete[] cpMultiByte;
	return oRet;
}

static std::string get_text_from_node(boost::property_tree::ptree& p, const std::string& path) {
	auto target_node = p.get_optional<std::string>(path);
	if (!target_node) throw std::runtime_error("xmlのnodeの読み込みに失敗しました");
	static_assert(sizeof(wchar_t) == 2, "In function 'read_xml' : Please check usage of 'std::codecvt_utf8_utf16'");
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
	auto str = convert.from_bytes(target_node.get());
	return WStringToString(str);
}

static int get_int_data_from_node(boost::property_tree::ptree& p, const std::string& path) {
	auto target_node = p.get_optional<int>(path);
	if (!target_node) throw std::runtime_error("xmlのnodeの読み込みに失敗しました");
	return target_node.get();
}

std::vector<status_data> read_xml_player_status_data(std::string player_name, bool is_male) {
	std::vector<status_data> re;
	boost::property_tree::ptree pt;
	read_xml("./Status/player.xml", pt);
	for (auto& i : pt.get_child("characterdatabook")) {
		re.emplace_back(
			player_name,
			"",
			is_male,
			get_int_data_from_node(i.second, "hp"),
			get_int_data_from_node(i.second, "mp"),
			get_int_data_from_node(i.second, "attack"),
			get_int_data_from_node(i.second, "defence"),
			get_int_data_from_node(i.second, "magicattack"),
			get_int_data_from_node(i.second, "magicdefence"),
			get_int_data_from_node(i.second, "speed")
			);
	}
	return re;
}

std::vector<status_data> read_xml_partner_status_data() {
	std::vector<status_data> re;
	boost::property_tree::ptree pt;
	read_xml("./Status/partner.xml", pt);
	for (auto& i : pt.get_child("characterdatabook")) {
		re.emplace_back(
			get_text_from_node(i.second, "name"),
			"",
			0 == get_int_data_from_node(i.second, "sex"),
			get_int_data_from_node(i.second, "hp"),
			get_int_data_from_node(i.second, "mp"),
			get_int_data_from_node(i.second, "attack"),
			get_int_data_from_node(i.second, "defence"),
			get_int_data_from_node(i.second, "magicattack"),
			get_int_data_from_node(i.second, "magicdefence"),
			get_int_data_from_node(i.second, "speed")
			);
	}
	return re;
}

std::vector<status_data> read_xml_enemy_status_data() {
	std::vector<status_data> re;
	boost::property_tree::ptree pt;
	read_xml("./Status/enemy.xml", pt);
	for (auto& i : pt.get_child("characterdatabook")) {
		re.emplace_back(
			get_text_from_node(i.second, "name"),
			get_text_from_node(i.second, "graph"),
			true,
			get_int_data_from_node(i.second, "hp"),
			get_int_data_from_node(i.second, "mp"),
			get_int_data_from_node(i.second, "attack"),
			get_int_data_from_node(i.second, "defence"),
			get_int_data_from_node(i.second, "magicattack"),
			get_int_data_from_node(i.second, "magicdefence"),
			get_int_data_from_node(i.second, "speed")
			);
	}
	return re;
}

IniStream::IniStream(std::string const& filepath_or_name) {
	if (':' != filepath_or_name.at(1)) {//2文字目が':'ならフルパス
		char dir[MAX_PATH];
		if (!GetCurrentDirectory(MAX_PATH, dir)) throw std::runtime_error("IniStream::IniStream : getcurrentdirctory error.");
		//".\\[filepath_or_name]だとファイルが無いときにcreate_UTF16_fileがCurrent Directoryに生成しない"
		this->filefullpath = dir + ("\\" + filepath_or_name);
	}
	else {
		this->filefullpath = filepath_or_name;
	}
	this->was_no_exist = (!PathFileExists(filefullpath.c_str()));
	if (was_no_exist) {
		create_UTF16_file();
	}
}

bool IniStream::is_now_created() const {
	return this->was_no_exist;
}

void IniStream::create_UTF16_file() {
	if (this->was_no_exist || !PathFileExists(this->filefullpath.c_str())) {
		constexpr int BOM = 0xFEFF;
		std::wofstream wfout;
		wfout.open(this->filefullpath, std::ios_base::binary);
		wfout << BOM;
	}
}

void IniStream::edit(std::string const& section, std::string const& key, std::string const& str) {
	clearErr();
	if (false == WritePrivateProfileString(section.c_str(), key.c_str(), str.c_str(), this->filefullpath.c_str())) {
		throw std::runtime_error("IniStream::edit : " + getLastErrorText());
	}
}

int IniStream::get_int(std::string const& section, std::string const& key, const int default_num) {
	return GetPrivateProfileInt(section.c_str(), key.c_str(), default_num, this->filefullpath.c_str());
}

std::string IniStream::get_string(std::string const& section, std::string const& key, std::string const& default_str) {
	char buf[GetPrivateProfileStringBuf];
	GetPrivateProfileString(section.c_str(), key.c_str(), default_str.c_str(), buf, GetPrivateProfileStringBuf, this->filefullpath.c_str());
	return std::string(buf);
}

static inline void clearErr() {
	SetLastError(NO_ERROR);
}

std::string getLastErrorText() {
	char* buf = nullptr;
	const auto lasterr = GetLastError();
	const auto len = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
		nullptr,
		lasterr,
		LANG_NEUTRAL,
		(LPSTR)&buf,
		0,
		nullptr
		);
	auto i = len - 3;
	for (; '\r' != buf[i] && '\n' != buf[i] && '\0' != buf[i]; i++);//改行文字削除
	buf[i] = '\0';
	std::string ret = buf + ("(" + std::to_string(lasterr)) + ")";//エラーメッセージ作成
	LocalFree(buf);//FormatMessageAでFORMAT_MESSAGE_ALLOCATE_BUFFERを指定したので必ず開放
	return ret;
}
