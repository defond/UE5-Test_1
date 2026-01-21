// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_1GameMode.h"
#include "Test_1Character.h"
#include "UObject/ConstructorHelpers.h"

ATest_1GameMode::ATest_1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
