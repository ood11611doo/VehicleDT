#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Sensor/Lidar/LidarTypes.h"
#include "LidarSensorComponent.generated.h"

class ULidarScanner;
class ULidarBevRenderer;

UCLASS(ClassGroup = (Sensor), meta = (BlueprintSpawnableComponent), BlueprintType)
class VEHICLEDT_API ULidarSensorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	ULidarSensorComponent();

	UFUNCTION(BlueprintCallable, Category = "LidarSensor")
	void StartScan();

	UFUNCTION(BlueprintCallable, Category = "LidarSensor")
	void StopScan();

	UFUNCTION(BlueprintPure, Category = "LidarSensor")
	UTexture2D* GetBevRenderTarget() const;

	UFUNCTION(BlueprintPure, Category = "LidarSensor")
	FLidarScanResult GetLastScan() const;

	UFUNCTION(BlueprintCallable, Category = "LidarSensor")
	void SetPreset(ELidarPreset NewPreset);

	UFUNCTION(BlueprintCallable, Category = "LidarSensor")
	void SetRotationRate(float Hz);

	UFUNCTION(BlueprintCallable, Category = "LidarSensor")
	void ApplySensorConfig();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	void InitSensor();
	void StartTimer();
	void StopTimer();
	void OnTimer();
	void ExportScan();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Config",
		meta=(AllowPrivateAccess="true"))
	ELidarPreset Preset = ELidarPreset::VelodyneVLP16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Config",
		meta=(AllowPrivateAccess="true"))
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Config",
		meta=(AllowPrivateAccess="true"))
	FLidarConfig Config;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|BEV",
		meta=(AllowPrivateAccess="true"))
	FBevConfig BevConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Export",
		meta=(AllowPrivateAccess="true"))
	bool bExportEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Export",
		meta=(AllowPrivateAccess="true"))
	FDataExportConfig ExportConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LidarSensor|Output",
		meta=(AllowPrivateAccess="true"))
	int64 ScanCount = 0;

	UPROPERTY()
	TObjectPtr<ULidarScanner> Scanner;

	UPROPERTY()
	TObjectPtr<ULidarBevRenderer> BevRenderer;

	FTimerHandle TimerHandle;
};
