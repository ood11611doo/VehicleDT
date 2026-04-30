// Copyright Epic Games, Inc. All Rights Reserved.

#include "System/VehicleDTGameMode.h"
#include "System/VehicleDTPlayerController.h"

AVehicleDTGameMode::AVehicleDTGameMode()
{
	PlayerControllerClass = AVehicleDTPlayerController::StaticClass();
}
