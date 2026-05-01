#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Sensor/Camera/SensorCameraTypes.h"
#include "CameraSensorComponent.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS(ClassGroup = (Sensor), meta = (BlueprintSpawnableComponent), BlueprintType)
class VEHICLEDT_API UCameraSensorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UCameraSensorComponent();

	UFUNCTION(BlueprintPure, Category = "CameraSensor")
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	UFUNCTION(BlueprintCallable, Category = "CameraSensor")
	void TriggerCapture();

	UFUNCTION(BlueprintCallable, Category = "CameraSensor")
	void SetPreset(ECameraPreset NewPreset);

	UFUNCTION(BlueprintCallable, Category = "CameraSensor")
	void SetFrameRate(float Hz);

	UFUNCTION(BlueprintCallable, Category = "CameraSensor")
	void ApplySensorConfig();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	void InitCapture();
	void BuildRenderTarget();
	void ConfigureCapture();
	void ApplyPostProcess();
	void ApplyDistortion();
	void StartTimer();
	void StopTimer();
	void OnTimer();
	void ExportFrame();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Config",
		meta=(AllowPrivateAccess="true"))
	ECameraPreset Preset = ECameraPreset::TeslaHW3_Wide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Config",
		meta=(AllowPrivateAccess="true"))
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Spec",
		meta=(AllowPrivateAccess="true"))
	FCameraSpec Spec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Distortion",
		meta=(AllowPrivateAccess="true"))
	FLensDistortion Distortion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Noise",
		meta=(AllowPrivateAccess="true"))
	FCameraNoiseParams Noise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|PostProcess",
		meta=(AllowPrivateAccess="true"))
	FCameraPostProcess PostProcess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Exposure",
		meta=(AllowPrivateAccess="true"))
	FAutoExposure Exposure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Export",
		meta=(AllowPrivateAccess="true"))
	bool bExportEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Export",
		meta=(AllowPrivateAccess="true"))
	FDataExportConfig ExportConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Distortion",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMaterialInterface> DistortionMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSensor|Output",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSensor|Output",
		meta=(AllowPrivateAccess="true"))
	int64 CaptureCount = 0;

	UPROPERTY()
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DistortionMID;

	FTimerHandle TimerHandle;
};
