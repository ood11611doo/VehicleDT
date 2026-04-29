// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sensor/SensorTypes.h"
#include "BEVVisualizationComponent.generated.h"

class UTexture2D;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEHICLEDT_API UBEVVisualizationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBEVVisualizationComponent();

	/** LidarSensor의 OnLidarScanReady 델리게이트에 바인딩 */
	void HandleLidarScan(const FLidarPointCloudData& PointCloud);

	UFUNCTION(BlueprintPure, Category = "BEV")
	UTexture2D* GetRenderTarget() const { return DynamicTexture; }
	FOnBEVUpdate OnBEVUpdate;

protected:
	virtual void BeginPlay() override;

private:
	void CreateTexture();
	void BuildColorLUT();
	void UpdatePixelBuffer(const FLidarPointCloudData& PointCloud);
	void DrawGrid();
	void DrawHeading();
	FColor MakeColorFromHeight(float Height) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	FBevRenderConfig Config;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	float GridSpacingMeters = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	bool bShowGrid = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	bool bShowHeading = true;

	UPROPERTY()
	TObjectPtr<UTexture2D> DynamicTexture;

	TArray<FColor> PixelBuffer;
	FColor ColorLUT[256];
};
