#include "Dialogue.hpp"

#include <utility>

Dialogue::Dialogue(std::string dialogue_name, std::string speaker_name, FunctionType dialogue_text, FunctionType next_dialogue, bool player_option)
	: name(std::move(dialogue_name))
	, speaker(std::move(speaker_name))
	, text(std::move(dialogue_text))
	, next(std::move(next_dialogue))
	, player_option(player_option)
{};