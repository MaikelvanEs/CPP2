#pragma once

enum Characters
{
	moordenaar = 1,
	dief,
	magier,
	koning,
	prediker,
	koopman,
	bouwmeester,
	condottiere
};

enum PlayerState
{
	eWait,
	eTakeCharacter,
	eDiscardCharacter,
	eTurnStart,
	eBuild,
	eTurnEnd,
	eUsingCharacterPower
};
