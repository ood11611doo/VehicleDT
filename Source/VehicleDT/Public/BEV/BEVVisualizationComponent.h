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

	/** Z값cm → ColorLUT 조회 → FColor 반환 */
	FColor MakeColorFromHeight(float Height) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	FBevRenderConfig Config;

	/** 주의: GridSpacingMeter는 메소드 내부에서 *100.f로 계산되어 미터(m) 단위임 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	float GridSpacingMeters = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	bool bShowGrid = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	bool bShowHeading = true;

	/** 높이 컬러맵 최솟값 cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	float HeightMinCm = -200.f;

	/** 높이 컬러맵 최댓값 cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BEV",
		meta=(AllowPrivateAccess="true"))
	float HeightMaxCm = 300.f;

	UPROPERTY()
	TObjectPtr<UTexture2D> DynamicTexture;

	TArray<FColor> PixelBuffer;
	FColor ColorLUT[256];
	FUpdateTextureRegion2D UpdateRegion;
};
