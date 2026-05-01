#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sensor/Lidar/LidarTypes.h"
#include "LidarScanner.generated.h"

UCLASS()
class VEHICLEDT_API ULidarScanner : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const FLidarConfig& InConfig, ELidarPreset InPreset);
	void PerformScan(UWorld* World, const FTransform& SensorOrigin, AActor* OwnerToIgnore);
	void UpdateConfig(const FLidarConfig& InConfig, ELidarPreset InPreset);

	const FLidarScanResult& GetLastScan() const { return LastScan; }

private:
	struct FChannelHits
	{
		TArray<FVector> Points;
		TArray<float>   Ranges;
	};

	void BuildElevationAngles(ELidarPreset InPreset);
	void FlattenChannels();

	FLidarConfig            Config;
	TArray<float>           ElevationAngles;
	TArray<FChannelHits>    ChannelBuffer;
	FLidarScanResult        LastScan;
	FCollisionQueryParams   QueryParams;
};
