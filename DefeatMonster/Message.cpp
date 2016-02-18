#include"Class.h"

void Message::window_box() {
	DrawBox(message_window_x, message_window_y, window_width - 15, window_height - 59, GetColor(128, 128, 128), TRUE);
	DrawBox(message_window_x, message_window_y, window_width - 15, window_height - 59, GetColor(255, 255, 255), FALSE);
}

void Message::attack() {
	this->window_box();
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%s�̍U���I", this->attacker.c_str());
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%s�̂��������I", this->attacker.c_str());
	ScreenFlip();
}

void Message::magic_attack(const char* chant_magic, cint magic_sound_handle) {
	this->window_box();
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%s��%s���������I", this->attacker.c_str(), chant_magic);
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%s��%s���ƂȂ����I", this->attacker.c_str(), chant_magic);
	ScreenFlip();
	PlaySoundMem(magic_sound_handle, DX_PLAYTYPE_NORMAL);
}

void Message::skill_attack(const char* use_skill) {
	this->window_box();
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%s��%s��������I", this->attacker.c_str(), use_skill);
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 5, this->message_window.string_color, "%s��%s���͂Ȃ����I", this->attacker.c_str(), use_skill);
	ScreenFlip();
}

void Message::cure(int cure_point) {
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%s��%d�񕜂����I", this->target.c_str(), cure_point);
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%s��%d�����ӂ������I", this->attacker.c_str(), cure_point);
	ScreenFlip();
}

void Message::resuscitation_success() {
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "�Ȃ�ƁA%s�������Ԃ����I", this->target.c_str());
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "�Ȃ�ƁA%s���������������I", this->target.c_str());
	ScreenFlip();
}

void Message::resuscitation_failure() {
	if (this->print_in_kanji) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%s�͐����Ԃ�Ȃ�����", this->target.c_str());
	else DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%s�͂���������Ȃ�����", this->target.c_str());
	ScreenFlip();
}

void Message::shortage_magic_power() {
	if (this->print_in_kanji) DrawString(this->message_window.x + 5, this->message_window.y + 21, "�������AMP������Ȃ������I", this->message_window.string_color);
	else DrawString(this->message_window.x + 5, this->message_window.y + 21, "�������AMP������Ȃ������I", this->message_window.string_color);
	ScreenFlip();
}

void Message::miss() {
	if (this->print_in_kanji)DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "�~�X�I%s�̓_���[�W���󂯂Ȃ��I", this->target.c_str());
	else DrawFormatString(this->message_window.x + 5, this->message_window.y, this->message_window.string_color, "�~�X�I%s�̓_���[�W�������Ȃ��I", this->target.c_str());
	ScreenFlip();
}

void Message::damage_to_enemy(int damage) {
	if (damage != 0) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%s��%d�̃_���[�W�I", this->target.c_str(), damage);
	else this->miss();
	ScreenFlip();
}

void Message::damage_from_enemy(int damage) {
	if (damage != 0) DrawFormatString(this->message_window.x + 5, this->message_window.y + 21, this->message_window.string_color, "%s��%d�̃_���[�W���󂯂��I", this->target.c_str(), damage);
	else this->miss();
	ScreenFlip();
}
