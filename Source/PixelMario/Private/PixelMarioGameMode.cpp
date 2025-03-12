// Copyright Epic Games, Inc. All Rights Reserved.

#include "PixelMarioGameMode.h"
#include "ThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

APixelMarioGameMode::APixelMarioGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP/BP_PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
