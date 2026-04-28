// Copyright Epic Games, Inc. All Rights Reserved.

#include "VehicleDTWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UVehicleDTWheelFront::UVehicleDTWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}