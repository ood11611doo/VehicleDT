// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sensor/SensorTypes.h"
#include "SplineFollowerComponent.generated.h"

class AVehicleDTPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEHICLEDT_API USplineFollowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USplineFollowerComponent();

	FOnControlOutput OnControlOutput;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void    BuildPath();
	void    ResampleCatmullRom();
	FVector GetPointAhead(FVector& OutDirection, float Distance) const;
	float   EstimateCurvature(float AheadOffset) const;
	float   ComputeCurveSpeedLimit(float Curvature) const;
	void	DoThrottle(const float Value);
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float MaxSpeed = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float MinSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float ThrottleGain = 0.008f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float DecelRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float AccelRate = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(AllowPrivateAccess="true"))
	float LookAheadBase = 1800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(AllowPrivateAccess="true"))
	float LookAheadSpeedFactor = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(AllowPrivateAccess="true"))
	float MaxYawDelta = 16.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(ClampMin="0", ClampMax="1", AllowPrivateAccess="true"))
	float HeadingWeight = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(AllowPrivateAccess="true"))
	float CrosstrackGain = 0.0013f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature",
		meta=(ClampMin="0.1", ClampMax="2.0", AllowPrivateAccess="true"))
	float LateralFriction = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature",
		meta=(AllowPrivateAccess="true"))
	float CurvatureSampleSpan = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature",
		meta=(AllowPrivateAccess="true"))
	float BrakePreviewDist = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Path",
		meta=(AllowPrivateAccess="true"))
	float SearchRadius = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Path",
		meta=(AllowPrivateAccess="true"))
	float ResampleSpacing = 50.f;

	UPROPERTY()
	TWeakObjectPtr<AVehicleDTPawn> OwnerPawn;

private:
	TArray<FVector> PathPoints;
	int32 CurrentPointIndex = 0;
	int32 PrevPointIndex = 0;
	bool  bClosedLoop = false;
	float SmoothedTargetSpeed = 0.f;

	float LapStartTime = 0.f;
	bool  bLapStarted = false;
};
