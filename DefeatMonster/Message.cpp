#include"Class.h"

void Message::window_box() {
	DrawBox(message_window_x, message_window_y, window_width - 15, window_height - 59, GetColor(128, 128, 128), TRUE);
	DrawBox(message_window_x, message_window_y, window_width - 15, window_height - 59, GetColor(255, 255, 255), FALSE);
}

void Message::attack() {
	this->window_box();
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%sの攻撃！", this->attacker.c_str());
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%sのこうげき！", this->attacker.c_str());
	ScreenFlip();
}

void Message::magic_attack(const char* chant_magic, cint magic_sound_handle) {
	this->window_box();
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%sは%sを唱えた！", this->attacker.c_str(), chant_magic);
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%sは%sをとなえた！", this->attacker.c_str(), chant_magic);
	ScreenFlip();
	PlaySoundMem(magic_sound_handle, DX_PLAYTYPE_NORMAL);
}

void Message::skill_attack(const char* use_skill) {
	this->window_box();
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%sは%sを放った！", this->attacker.c_str(), use_skill);
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%sは%sをはなった！", this->attacker.c_str(), use_skill);
	ScreenFlip();
}

void Message::cure(int cure_point) {
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%sは%d回復した！", this->target.c_str(), cure_point);
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%sは%dかいふくした！", this->attacker.c_str(), cure_point);
	ScreenFlip();
}

void Message::resuscitation_success() {
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "なんと、%sが生き返った！", this->target.c_str());
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "なんと、%sがいきかえった！", this->target.c_str());
	ScreenFlip();
}

void Message::resuscitation_failure() {
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%sは生き返らなかった", this->target.c_str());
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%sはいきかえらなかった", this->target.c_str());
	ScreenFlip();
}

void Message::shortage_magic_power() {
	if (this->print_in_kanji) DrawString(this->message_window.x + 5, this->message_window.y + 21, "しかし、MPが足りなかった！", this->message_window.string_color);
	else DrawString(this->message_window.x + 5, this->message_window.y + 21, "しかし、MPが足りなかった！", this->message_window.string_color);
	ScreenFlip();
}

void Message::miss() {
	if (this->print_in_kanji)DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "ミス！%sはダメージを受けない！", this->target.c_str());
	else DrawFormatString(this->message_window.x + 5, this->message_window.y, this->message_window.string_color, "ミス！%sはダメージをうけない！", this->target.c_str());
	ScreenFlip();
}

void Message::damage_to_enemy(int damage) {
	if (damage != 0) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%sに%dのダメージ！", this->target.c_str(), damage);
	else this->miss();
	ScreenFlip();
}

void Message::damage_from_enemy(int damage) {
	if (damage != 0) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%sは%dのダメージを受けた！", this->target.c_str(), damage);
	else this->miss();
	ScreenFlip();
}
