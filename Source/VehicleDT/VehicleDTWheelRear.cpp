// Copyright Epic Games, Inc. All Rights Reserved.

#include "VehicleDTWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UVehicleDTWheelRear::UVehicleDTWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}