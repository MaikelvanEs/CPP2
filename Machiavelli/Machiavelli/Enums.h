#pragma once

enum Characters
{
	start,
	moordenaar,
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
	eBuildBuilding,
	eTurnEnd,
	eUsingCharacterPower,
	eBuildTwoBuildings,
	eBuildOneBuilding,
	eMagier
};
