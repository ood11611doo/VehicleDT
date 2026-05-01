#pragma once

#include "CoreMinimal.h"
#include "SensorShared.generated.h"

USTRUCT(BlueprintType)
struct FSensorTimestamp
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sensor")
	double GameTime = 0.0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sensor")
	int64 FrameNumber = 0;

	static FSensorTimestamp Now()
	{
		FSensorTimestamp T;
		T.GameTime    = FPlatformTime::Seconds();
		T.FrameNumber = GFrameCounter;
		return T;
	}
};

USTRUCT(BlueprintType)
struct FDataExportConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	FString Label = TEXT("Sensor");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export", meta = (ClampMin = "1", ClampMax = "100"))
	int32 JPGQuality = 95;
};
