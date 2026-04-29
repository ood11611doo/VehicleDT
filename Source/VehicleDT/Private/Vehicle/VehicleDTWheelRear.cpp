// Copyright Epic Games, Inc. All Rights Reserved.

#include "Vehicle/VehicleDTWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UVehicleDTWheelRear::UVehicleDTWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}