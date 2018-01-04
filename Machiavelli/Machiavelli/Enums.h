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
	condotierre
};

enum PlayerState
{
	eWait,
	eTakeCharacter,
	eDiscardCharacter,
	eTurnStart,
	eDiscardBuilding,
	eBuild,
	eTurnEnd,
	eUsingCharacterPower
};
