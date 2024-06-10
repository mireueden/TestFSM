// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestFSMGameMode.h"
#include "TestFSMCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATestFSMGameMode::ATestFSMGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
