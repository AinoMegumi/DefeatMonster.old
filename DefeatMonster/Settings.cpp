#include"Class.h"
#include"Common.h"
Status::Status(std::vector<status_data> arr, int i, bool is_player_setting) {
	this->name = arr[i].name;
	this->graph_handle = LoadGraph(arr[i].graph_name.c_str());
	this->is_male = arr[i].is_male;
	this->hp = this->max_hp = is_player_setting ? (this->is_male ? (arr[i].hp * 3) / 2 : (arr[i].hp * 9) / 10) : arr[i].hp;
	this->mp = this->max_mp = is_player_setting ? (this->is_male ? (arr[i].mp * 9) / 10 : (arr[i].mp * 3) / 2) : arr[i].mp;
	this->attack = is_player_setting ? (this->is_male ? (arr[i].attack * 6) / 5 : (arr[i].attack * 4) / 5) : arr[i].attack;
	this->defence = arr[i].defence;
	this->magic_attack = is_player_setting ? (this->is_male ? (arr[i].attack * 4) / 5 : (arr[i].attack * 6) / 5) : arr[i].magic_attack;
	this->magic_defence = arr[i].magic_defence;
	this->speed = arr[i].speed;
	this->graph_handle = LoadGraph(arr[i].graph_name.c_str());
}

void print_in_kanji_message(COORDINATE graph) {
	SetBackgroundColor(160, 216, 239);
	DrawString(graph.x, graph.y, "メッセージをひらがなで表示しますか？漢字で表示しますか？", graph.string_color);
	DrawString(graph.x + 18, graph.y + 16, "漢字", graph.string_color);
	DrawString(graph.x + 18, graph.y + 32, "ひらがな", graph.string_color);
	ScreenFlip();
}

int check_print_in_kanji() {
	switch (MessageBox(NULL, "メッセージを漢字で表示しますか？", "モンスター討伐ゲーム　アンチ核家族化", MB_YESNOCANCEL)) {
	case IDYES: return 1;
	case IDNO: return 0;
	default: return -1;
	}
}

std::string get_string(size_t max_letter_num) {
	std::string re(max_letter_num * 2 + 1, '\0');
	KeyInputString(0, 16, static_cast<int>(max_letter_num * 2 + 1), &re[0], FALSE);
	re.resize(DxLib::strlenDx(re.c_str()));
	return re;
}

std::string get_player_name(bool print_in_kanji, uint draw_string_color) {
	ClearDrawScreen();
	DrawString(0, 0,
		print_in_kanji ? "名前を入力してください(最大全角５文字　半角１０文字)"
		: "なまえをおしえてね(ひらがな、カタカナ、かんじで５もじ、アルファベット、すうじで１０もじまで)",
		draw_string_color);
	//ScreenFlip();//Before calling KeyInputString(), YOU MUST NOT CALL ScreenFlip().
	return get_string(5);
}

static void print_sex_check_message(bool print_in_kanji, int x, int y, cint color) {
	fill_background_color();
	if (print_in_kanji) {
		DrawString(0, 0, "あなたは男性ですか？女性ですか？", color);
		DrawString(x, y, "男性", color);
		DrawString(x, y + 16, "女性", color);
	}
	else {
		DrawString(0, 0, "キミはおとこのこ？それともおんなのこ？", color);
		DrawString(x, y, "おとこのこ", color);
		DrawString(x, y + 16, "おんなのこ", color);
	}
}

bool check_male(bool print_in_kanji, uint string_color) {
	ClearDrawScreen();
	auto screen = dxle::graph2d::MakeScreen(window_width, window_height);
	screen.drawn_on([print_in_kanji, string_color]() { print_sex_check_message(print_in_kanji, 16, 16, string_color); });
	KeyState key;
	return 0 == key.cursole(screen, 2, 0, 16, GetColor(255, 255, 0), GetColor(160, 216, 239));
}

void print_partner_list(std::vector<status_data> arr, int choosen_character_num, bool print_in_kanji, cint string_color) {
	fill_background_color();
	int t = 0;
	uint string_color2 = GetColor(255, 0, 255);
	DrawString(0, 0, print_in_kanji ? "パートナーを選択してください" : "パートナーをえらんでね", string_color);
	for (uint i = 0; i <= arr.size() - 1; i++) {
		if (i != choosen_character_num) {
			DrawString(18, (t + 1) * 16, arr[i].name.c_str(), t % 2 == 1 ? string_color : string_color2);
			DrawFormatString(18 + 96, (t + 1) * 16, t % 2 == 1 ? string_color : string_color2, " HP:%d", arr[i].hp);
			DrawFormatString(18 + 192, (t + 1) * 16, t % 2 == 1 ? string_color : string_color2, " MP:%d", arr[i].mp);
			DrawFormatString(18 + 288, (t + 1) * 16, t % 2 == 1 ? string_color : string_color2, "ATK:%d", arr[i].attack);
			DrawFormatString(18 + 384, (t + 1) * 16, t % 2 == 1 ? string_color : string_color2, "DEF:%d", arr[i].defence);
			DrawFormatString(18 + 480, (t + 1) * 16, t % 2 == 1 ? string_color : string_color2, "MAK:%d", arr[i].magic_attack);
			DrawFormatString(18 + 576, (t + 1) * 16, t % 2 == 1 ? string_color : string_color2, "MDF:%d", arr[i].magic_defence);
			DrawFormatString(18 + 672, (t + 1) * 16, t % 2 == 1 ? string_color : string_color2, "SPD:%d", arr[i].speed);
			t++;
		}
	}
}

void choose_partner_num(std::vector<status_data> arr, cint string_color, bool print_in_kanji, int &partner_num, int choosen_partner_num = -1) {
	auto screen = dxle::graph2d::MakeScreen(window_width, window_height);
	screen.drawn_on([&arr, choosen_partner_num, print_in_kanji, string_color]() { print_partner_list(arr, choosen_partner_num, print_in_kanji, string_color); });
	KeyState key;
	const int choose_num = choosen_partner_num == -1 ? static_cast<int>(arr.size() - 1) : static_cast<int>(arr.size() - 2);
	partner_num = key.cursole(screen, choose_num, 0, 16, GetColor(255, 255, 0), GetColor(160, 216, 239));
}

void choose_partner(std::vector<status_data> arr, int &partner1, int &partner2, const DWORD string_color, bool print_in_kanji) {
	choose_partner_num(arr, string_color, print_in_kanji, partner1);
	choose_partner_num(arr, string_color, print_in_kanji, partner2, partner1);
}
