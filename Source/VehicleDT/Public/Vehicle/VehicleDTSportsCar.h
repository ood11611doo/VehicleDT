// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VehicleDTPawn.h"
#include "VehicleDTSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class VEHICLEDT_API AVehicleDTSportsCar : public AVehicleDTPawn
{
	GENERATED_BODY()
	
public:

	AVehicleDTSportsCar();
};
