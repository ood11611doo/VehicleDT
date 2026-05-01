// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "VehicleDTPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
class USplineFollowerComponent;
class UCameraSensorComponent;
class ULidarSensorComponent;
class UBEVVisualizationComponent;
class UAgentDataLoggerComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateVehicle, Log, All);

/**
 *  Vehicle Pawn class
 *  Handles common functionality for all vehicle types,
 *  including input handling and camera management.
 *  
 *  Specific vehicle configurations are handled in subclasses.
 */
UCLASS(abstract)
class AVehicleDTPawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	/** Spring Arm for the front camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FrontSpringArm;

	/** Front Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FrontCamera;

	/** Spring Arm for the back camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	/** Back Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	/** Cast pointer to the Chaos Vehicle movement component */
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineFollowerComponent> SplineFollower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraSensorComponent> CameraSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULidarSensorComponent> LidarSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBEVVisualizationComponent> BEVVisualization;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAgentDataLoggerComponent> DataLogger;

protected:

	/** Steering Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;

	/** Throttle Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	/** Brake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;

	/** Handbrake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;

	/** Look Around Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;

	/** Toggle Camera Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleCameraAction;

	/** Reset Vehicle Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ResetVehicleAction;

	/** Keeps track of which camera is active */
	bool bFrontCameraActive = false;

public:
	AVehicleDTPawn();

	// Begin Pawn interface

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// End Pawn interface

	// Begin Actor interface

	virtual void BeginPlay() override;
	virtual void Tick(float Delta) override;

	// End Actor interface

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetBEVTexture() const;
	
	/** SplineFollower가 직접 호출하는 제어 인터페이스 */
	void DoSteering(float Value);
	void DoThrottle(float Value);
	void DoBrake(float Value);
	void DoBrakeStart();
	void DoHandbrake(bool bActive);
	void DoToggleSensorView();

protected:

	/** Handles steering input */
	void Steering(const FInputActionValue& Value);

	/** Handles throttle input */
	void Throttle(const FInputActionValue& Value);

	/** Handles brake input */
	void Brake(const FInputActionValue& Value);

	/** Handles brake start/stop inputs */
	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	/** Handles handbrake start/stop inputs */
	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	/** Handles look around input */
	void LookAround(const FInputActionValue& Value);

	/** Handles toggle camera input */
	void ToggleCamera(const FInputActionValue& Value);

	/** Handles reset vehicle input */
	void ResetVehicle(const FInputActionValue& Value);

	/** Called when the brake lights are turned on or off */
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle")
	void BrakeLights(bool bBraking);

public:
	/** Returns the front spring arm subobject */
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	/** Returns the front camera subobject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }
	/** Returns the back spring arm subobject */
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	/** Returns the back camera subobject */
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	/** Returns the cast Chaos Vehicle Movement subobject */
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
};
