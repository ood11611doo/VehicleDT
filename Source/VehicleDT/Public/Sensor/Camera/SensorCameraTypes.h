#pragma once

#include "CoreMinimal.h"
#include "VehicleDT/Public/Sensor/SensorShared.h"
#include "SensorCameraTypes.generated.h"

UENUM(BlueprintType)
enum class ECameraPreset : uint8
{
	Custom           UMETA(DisplayName = "Custom"),
	TeslaHW3_Wide    UMETA(DisplayName = "Tesla HW3 Wide (120 deg)"),
	TeslaHW3_Main    UMETA(DisplayName = "Tesla HW3 Main (50 deg)"),
	TeslaHW3_Narrow  UMETA(DisplayName = "Tesla HW3 Narrow (35 deg)"),
	TeslaHW4         UMETA(DisplayName = "Tesla HW4 (5MP)"),
	DroneFPV         UMETA(DisplayName = "Drone FPV (150 deg)"),
	Waymo            UMETA(DisplayName = "Waymo 5th Gen")
};

USTRUCT(BlueprintType)
struct FCameraSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSpec", meta = (ClampMin = "1", ClampMax = "7680"))
	int32 Width = 1280;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSpec", meta = (ClampMin = "1", ClampMax = "4320"))
	int32 Height = 960;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSpec", meta = (ClampMin = "1.0", ClampMax = "120.0"))
	float FrameRate = 36.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSpec", meta = (ClampMin = "1.0", ClampMax = "180.0"))
	float FOV = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSpec", meta = (ClampMin = "0.1"))
	float FocalLengthMM = 1.5f;
};

USTRUCT(BlueprintType)
struct FLensDistortion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distortion")
	float K1 = -0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distortion")
	float K2 = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distortion")
	float K3 = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distortion")
	float P1 = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distortion")
	float P2 = 0.0f;

	bool IsNonTrivial() const
	{
		return !FMath::IsNearlyZero(K1) || !FMath::IsNearlyZero(K2) || !FMath::IsNearlyZero(K3)
			|| !FMath::IsNearlyZero(P1) || !FMath::IsNearlyZero(P2);
	}
};

USTRUCT(BlueprintType)
struct FCameraNoiseParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise", meta = (EditCondition = "bEnabled"))
	float GaussianMean = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise", meta = (EditCondition = "bEnabled", ClampMin = "0.0", ClampMax = "0.5"))
	float GaussianStdDev = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise", meta = (EditCondition = "bEnabled", ClampMin = "0.0", ClampMax = "1.0"))
	float ShotNoiseIntensity = 0.01f;
};

USTRUCT(BlueprintType)
struct FCameraPostProcess
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float VignetteIntensity = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float LensFlareIntensity = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float ChromaticAberration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MotionBlurAmount = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float BloomIntensity = 0.0f;
};

USTRUCT(BlueprintType)
struct FAutoExposure
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exposure")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exposure", meta = (EditCondition = "bEnabled"))
	float MinEV = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exposure", meta = (EditCondition = "bEnabled"))
	float MaxEV = 14.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exposure", meta = (EditCondition = "bEnabled", ClampMin = "0.1", ClampMax = "20.0"))
	float SpeedUp = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exposure", meta = (EditCondition = "bEnabled", ClampMin = "0.1", ClampMax = "20.0"))
	float SpeedDown = 1.0f;
};
