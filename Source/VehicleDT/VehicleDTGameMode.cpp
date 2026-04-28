// Copyright Epic Games, Inc. All Rights Reserved.

#include "VehicleDTGameMode.h"
#include "VehicleDTPlayerController.h"

AVehicleDTGameMode::AVehicleDTGameMode()
{
	PlayerControllerClass = AVehicleDTPlayerController::StaticClass();
}
