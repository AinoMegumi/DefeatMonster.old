#include"Class.h"
#include"GameSystem.h"
#include"Settings.h"
#include<ctime>
#include<fstream>

int init() {
	SetMainWindowText("モンスター討伐ゲーム　アンチ核家族化");
	SetOutApplicationLogValidFlag(FALSE);
	SetGraphMode(window_width, window_height, 16);
	ChangeWindowMode(TRUE);
	SetBackgroundColor(153, 217, 234);
	SetAlwaysRunFlag(TRUE);
	if (DxLib_Init() == -1 || SetMouseDispFlag(TRUE) == -1) return -1;
	SetTransColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	return 0;
}

int end(int end_num) {
	DxLib_End();
	return end_num;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow) {
	int check_print_in_kanji_num = check_print_in_kanji();
	if (check_print_in_kanji_num == -1 || -1 == init()) return -1;
	bool print_in_kanji = check_print_in_kanji_num == 1;
	//const time_t main_process_begin = time(NULL);
	struct tm cu_time;
	COORDINATE enemy_GSize, status_graph = { 10, 10, 0 };//enemy_GSizeはenemy_introductionで値がセットされる
	IniStream graph("./Graphic/graphic.ini"), music("./Sound/sound.ini");
	Music MHandle;
	all_music_setting(music, MHandle);
	int battle_bg_graph_handle = LoadGraph(graph.get_string("BackGround", "battle", "").c_str());
	try {
		if (1 == opening(graph)) return end(0);
		StatusDataList status = all_settings(enemy_GSize, print_in_kanji, MHandle.setting);
		Battle bat(command, print_in_kanji, MHandle.magic_chant);
		bat.turn_arr = { player_turn, partner1_turn, partner2_turn, enemy_turn, check };
		int battle_result = 0;
		PlaySoundMem(MHandle.battle, DX_PLAYTYPE_LOOP);
		while (ProcessMessage() != -1) {
			ClearDrawScreen();
			print_status(status, status_graph);
			graph_enemy_and_background(
				{
					(static_cast<int>(window_width) - enemy_GSize.x) >> 1,
					(static_cast<int>(window_height) - enemy_GSize.y) >> 1,
					0
				}, battle_bg_graph_handle, status.player.name, status.enemy);
			battle_result = bat.battle_main(status, status_graph);
			ScreenFlip();
			if (battle_result != 0) break;
		}
		StopSoundMem(MHandle.battle);
		ClearDrawScreen();
		result(battle_result, print_in_kanji, MHandle.win, MHandle.lose);
		WaitKey();
		return end(0);
	}
	catch (const std::exception& er) {
		const time_t current_time = time(NULL);
		localtime_s(&cu_time, &current_time);
		std::string title = "\\log\\" + std::to_string(cu_time.tm_year + 1900) + std::to_string(cu_time.tm_mon + 1)
			+ std::to_string(cu_time.tm_mday) + "_" + std::to_string(cu_time.tm_hour) + std::to_string(cu_time.tm_min)
			+ std::to_string(cu_time.tm_sec) + ".log";
		std::ofstream output(title.c_str());
		output << er.what();
		output.close();
		return end(-1);
	}
}