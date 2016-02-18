#include"Class.h"
#include"Settings.h"
#include"Storage.h"

auto rand_engine() {
	std::random_device rnd;
	std::vector<std::uint_least32_t> v(10);// 初期化用ベクタ
	std::generate(v.begin(), v.end(), std::ref(rnd));// ベクタの初期化
	std::seed_seq seed(v.begin(), v.end());
	return std::mt19937(seed);// 乱数エンジン
}

std::mt19937& Random::make_mt() {
	static std::mt19937 mt = rand_engine();
	return mt;
}

void all_music_setting(IniStream ini, Music &music) {
	music = {
		LoadSoundMem(ini.get_string("Music", "title", "").c_str()),
		LoadSoundMem(ini.get_string("Music", "setting", "").c_str()),
		LoadSoundMem(ini.get_string("Music", "battle", "").c_str()),
		LoadSoundMem(ini.get_string("SE", "magic", "").c_str()),
		LoadSoundMem(ini.get_string("Music", "win", "").c_str()),
		LoadSoundMem(ini.get_string("Music", "lose", "").c_str())
	};
}

void music_fade_out(int music_handle, int sleep_time) {
	auto normal_con_f = []() ->bool { return -1 != ProcessMessage() && 0 == ScreenFlip() && 0 == ClearDrawScreen(); };
	volatile bool is_normal_state;
	for (int i = 100; is_normal_state = normal_con_f() && i > -1; i--) {
		ChangeVolumeSoundMem(255 * i / 100, music_handle);
		DrawFormatString(window_width - 156, window_height - 18, GetColor(0, 0, 0), "Now Loading %--3d％", 100 - i);
	}
	if(sleep_time > 1) WaitTimer(sleep_time - 1);
}

void opening_message(int GHandle) noexcept(false) {
	for (int i = 0; i <= 100 && -1 != ProcessMessage(); i++) {
		ClearDrawScreen();
		SetDrawBright(256 * i / 100, 256 * i / 100, 256 * i / 100);
		DrawGraph(0, 0, GHandle, FALSE);
		ScreenFlip();
	}
	WaitTimer(3000);
	for (int i = 100; i >= 0 && -1 != ProcessMessage(); i--) {
		ClearDrawScreen();
		SetDrawBright(256 * i / 100, 256 * i / 100, 256 * i / 100);
		DrawGraph(0, 0, GHandle, FALSE);
		ScreenFlip();
	}
	ClearDrawScreen();
}

int title_graph(int GHandle) noexcept(false) {
	char* KeyStateBuf = new char[256];
	auto flag = []() {return -1 != ProcessMessage(); };
	volatile bool is_normal_state;
	while (is_normal_state = flag()) {
		ClearDrawScreen();
		DrawGraph(0, 0, GHandle, FALSE);
		ScreenFlip();
		GetHitKeyStateAll(KeyStateBuf);
		if (KeyStateBuf[KEY_INPUT_SPACE] == 1)  break;
		if (KeyStateBuf[KEY_INPUT_ESCAPE] == 1) return 1;
	}
	delete KeyStateBuf;
	ClearDrawScreen();
	return 0;
}

int opening(IniStream ini) noexcept(false) {
	int GHandle[2] = {
		LoadGraph(ini.get_string("Opening", "launch", "").c_str()),
		LoadGraph(ini.get_string("Opening", "opening", "").c_str())
	};
	opening_message(GHandle[0]);
	ClearDrawScreen();
	SetDrawBright(255, 255, 255);
	WaitTimer(1000);
	return title_graph(GHandle[1]);
}

void enemy_introduction(Status enemy, COORDINATE status_graph, COORDINATE &GSize, bool print_in_kanji) {
	ClearDrawScreen();
	DrawString(status_graph.x, status_graph.y, print_in_kanji ? "今回の対戦相手はこちら" : "こんかいのあいてはこちら", status_graph.string_color);
	if (print_in_kanji) DrawFormatString(status_graph.x, status_graph.y + (2 * 16), status_graph.string_color, "名前:%s", enemy.name.c_str());
	else DrawFormatString(status_graph.x, status_graph.y + (2 * 16), status_graph.string_color, "なまえ:%s", enemy.name.c_str());
	DrawFormatString(status_graph.x, status_graph.y + (3 * 16), status_graph.string_color, " HP:%d", enemy.hp);
	DrawFormatString(status_graph.x, status_graph.y + (4 * 16), status_graph.string_color, " MP:%d", enemy.mp);
	DrawFormatString(status_graph.x, status_graph.y + (5 * 16), status_graph.string_color, "ATK:%d", enemy.attack);
	DrawFormatString(status_graph.x, status_graph.y + (6 * 16), status_graph.string_color, "DEF:%d", enemy.defence);
	DrawFormatString(status_graph.x, status_graph.y + (7 * 16), status_graph.string_color, "MAK:%d", enemy.magic_attack);
	DrawFormatString(status_graph.x, status_graph.y + (8 * 16), status_graph.string_color, "MDF:%d", enemy.magic_defence);
	DrawFormatString(status_graph.x, status_graph.y + (9 * 16), status_graph.string_color, "SPD:%d", enemy.speed);
	GetGraphSize(enemy.graph_handle, &GSize.x, &GSize.y);
	DrawGraph((static_cast<int>(window_width) - GSize.x) >> 1, (static_cast<int>(window_height) - GSize.y) >> 1, enemy.graph_handle, TRUE);
	ScreenFlip();
	WaitKey();
}

StatusDataList all_settings(COORDINATE &GSize, bool print_in_kanji, int setting_music_handle) {
	const auto black = GetColor(0, 0, 0);
	const auto white = GetColor(255, 255, 255);
	const auto blue = GetColor(51, 153, 255);
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_NORMAL_STR, black);//入力文字列の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_NORMAL_STR_EDGE, black);//入力文字列の縁の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_NORMAL_CURSOR, black);//ＩＭＥ非使用時のカーソルの色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_SELECT_STR, white);//入力文字列の選択部分( SHIFTキーを押しながら左右キーで選択 )の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_SELECT_STR_EDGE, white);//入力文字列の選択部分( SHIFTキーを押しながら左右キーで選択 )の縁の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_SELECT_STR_BACK, blue);//入力文字列の選択部分( SHIFTキーを押しながら左右キーで選択 )の周りの色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_STR, black);//ＩＭＥ使用時の入力文字列の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_STR_EDGE, black);//ＩＭＥ使用時の入力文字列の縁の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_STR_BACK, white);//ＩＭＥ使用時の入力文字列の周りの色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_CURSOR, black);//ＩＭＥ使用時のカーソルの色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_LINE, black);//ＩＭＥ使用時の変換文字列の下線の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_SELECT_STR, white);//ＩＭＥ使用時の選択対象の変換候補文字列の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_SELECT_STR_EDGE, white);//ＩＭＥ使用時の選択対象の変換候補文字列の縁の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_SELECT_STR_BACK, blue);//ＩＭＥ使用時の選択対象の変換候補文字列の周りの色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_CONV_WIN_STR, black);//ＩＭＥ使用時の変換候補ウインドウ内の文字列の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_CONV_WIN_STR_EDGE, black);//ＩＭＥ使用時の変換候補ウインドウ内の文字列の縁の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR, black);//ＩＭＥ使用時の変換候補ウインドウ内で選択している文字列の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR_EDGE, black);//ＩＭＥ使用時の変換候補ウインドウ内で選択している文字列の縁の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR_BACK, blue);//ＩＭＥ使用時の変換候補ウインドウ内で選択している文字列の周りの色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_CONV_WIN_EDGE, black);//ＩＭＥ使用時の変換候補ウインドウの縁の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_CONV_WIN_BACK, white);//ＩＭＥ使用時の変換候補ウインドウの下地の色
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_MODE_STR, black);//ＩＭＥ使用時の入力モード文字列の色(『全角ひらがな』等)
	SetKeyInputStringColor2(DX_KEYINPSTRCOLOR_IME_MODE_STR_EDGE, black);//ＩＭＥ使用時の入力モード文字列の縁の色
	COORDINATE graph = { 0, 0, GetColor(0, 0, 0) };
	PlaySoundMem(setting_music_handle, DX_PLAYTYPE_LOOP);

	//敵
	auto enemy_arr = read_xml_enemy_status_data();
	std::uniform_int_distribution<int> rand(0, enemy_arr.size() - 1);
	int rand_num = rand(Random::make_mt());
	Status enemy(enemy_arr, rand_num, false);
	enemy_introduction(enemy, { 0, 0, GetColor(0, 0, 0) }, GSize, print_in_kanji);
	//プレイヤー
	std::string player_name = get_player_name(print_in_kanji, black);
	bool is_male = check_male(print_in_kanji, black);
	int battle_style = 0;
	auto player_arr = read_xml_player_status_data(player_name, is_male);
	Status player(player_arr, battle_style, true);
	//パートナー
	int partner1_num, partner2_num;
	auto partner_arr = read_xml_partner_status_data();
	choose_partner(partner_arr, partner1_num, partner2_num, black, print_in_kanji);
	if (partner2_num >= partner1_num) partner2_num++;
	Status partner1(partner_arr, partner1_num, false);
	Status partner2(partner_arr, partner2_num, false);
	music_fade_out(setting_music_handle, 300);
	StopSoundMem(setting_music_handle);
	return { player, partner1, partner2, enemy };
}

auto color(Status status) {
	if (status.hp * 100 / status.max_hp > 50) return GetColor(255, 255, 255);
	else if (status.hp * 100 / status.max_hp > 20) return GetColor(255, 255, 0);
	else if (status.hp == 0) return GetColor(255, 0, 0);
	else return GetColor(255, 128, 0);
}

void print_status(StatusDataList sta, COORDINATE status_graph) {
	//ステータス表示カラー決め
	auto player_print_color = color(sta.player);
	auto partner1_print_color = color(sta.partner1);
	auto partner2_print_color = color(sta.partner2);

	//ステータス表示ボックス
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (70 * 255) / 100);
	DrawBox(status_graph.x, status_graph.y, status_graph.x + 280, status_graph.y + 56, GetColor(128, 128, 128), TRUE);
	DrawBox(status_graph.x, status_graph.y, status_graph.x + 280, status_graph.y + 56, GetColor(255, 255, 255), FALSE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	//プレイヤーのステータス表示
	DrawString(status_graph.x + 2, status_graph.y + 2, sta.player.name.c_str(), player_print_color);
	DrawFormatString(status_graph.x + 2, status_graph.y + 20, player_print_color, "HP: %d", sta.player.hp);
	DrawFormatString(status_graph.x + 2, status_graph.y + 38, player_print_color, "MP: %d", sta.player.mp);

	//パートナー１のステータス表示
	DrawString(status_graph.x + 98, status_graph.y + 2, sta.partner1.name.c_str(), partner1_print_color);
	DrawFormatString(status_graph.x + 98, status_graph.y + 20, partner1_print_color, "HP: %d", sta.partner1.hp);
	DrawFormatString(status_graph.x + 98, status_graph.y + 38, partner1_print_color, "MP: %d", sta.partner1.mp);

	//パートナー２のステータス表示
	DrawString(status_graph.x + 192, status_graph.y + 2, sta.partner2.name.c_str(), partner2_print_color);
	DrawFormatString(status_graph.x + 194, status_graph.y + 20, partner2_print_color, "HP: %d", sta.partner2.hp);
	DrawFormatString(status_graph.x + 194, status_graph.y + 38, partner2_print_color, "MP: %d", sta.partner2.mp);
}

void graph_enemy_and_background(COORDINATE enemy_graph, int bg_graphic_handle, std::string player_name, Status &enemy) {
	DrawGraph(0, 0, bg_graphic_handle, FALSE);
	if(enemy.hp != 0) DrawGraph(enemy_graph.x, enemy_graph.y, enemy.graph_handle, TRUE);
	if (player_name == "GM神御田") {
		DrawBox(window_width - 170, 0, window_width, 26, GetColor(128, 128, 128), TRUE);
		DrawBox(window_width - 170, 0, window_width, 26, GetColor(255, 255, 255), FALSE);
		DrawFormatString(window_width - 165, 5, GetColor(255, 255, 255), "HP:%---4d　MP:%--3d", enemy.hp, enemy.mp);
	}
	ScreenFlip();
}

void result(int battle_result, bool print_in_kanji, int win, int lose) {
	switch (battle_result) {
	case 1:
		PlaySoundMem(lose, DX_PLAYTYPE_BACK);
		DrawString(0, 0, !print_in_kanji ? "キミのまけだよ" : "あなたの負けです", GetColor(0, 0, 0));
		break;
	case 2:
		DrawString(0, 0, !print_in_kanji ? "ひきわけだよ" : "引き分けです", GetColor(0, 0, 0));
		break;
	case 3:
		PlaySoundMem(win, DX_PLAYTYPE_BACK);
		DrawString(0, 0, !print_in_kanji ? "キミのかちだよ" : "あなたの勝ちです", GetColor(0, 0, 0));
		break;
	default:
		DrawString(0, 0, "ゲームの途中でエラーが発生しました。", GetColor(0, 0, 0));
		break;
	}
	ScreenFlip();
}