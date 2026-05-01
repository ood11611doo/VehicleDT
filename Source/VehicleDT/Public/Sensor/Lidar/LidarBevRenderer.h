#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sensor/Lidar/LidarTypes.h"
#include "LidarBevRenderer.generated.h"

class UTexture2D;

UCLASS()
class VEHICLEDT_API ULidarBevRenderer : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const FBevConfig& InConfig);
	void UpdateConfig(const FBevConfig& InConfig);
	void Render(const FLidarScanResult& Scan, const FTransform& SensorTransform);
	UTexture2D* GetRenderTarget() const { return Canvas; }

private:
	void AllocateCanvas();
	void BuildColorTable();

	UPROPERTY()
	TObjectPtr<UTexture2D> Canvas;

	TArray<FColor>         PixelBuffer;
	FUpdateTextureRegion2D UpdateRegion;
	FBevConfig             Config;
	FColor                 ColorTable[256];
	FColor                 ObstacleColor;
};
