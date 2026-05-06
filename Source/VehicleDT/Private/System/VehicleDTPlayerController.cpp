// Copyright Epic Games, Inc. All Rights Reserved.


#include "System/VehicleDTPlayerController.h"
#include "VehicleDTPawn.h"
#include "System/VehicleDTUI.h"
#include "Sensor/UI/SensorViewWidget.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"

void AVehicleDTPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// spawn the UI widget and add it to the viewport
	VehicleUI = CreateWidget<UVehicleDTUI>(this, VehicleUIClass);

	check(VehicleUI);

	VehicleUI->AddToViewport();
	
	
	// spawn the UI widget and add it to the viewport
	SensorViewWidget = CreateWidget<USensorViewWidget>(this, SensorViewWidgetClass);

	check(SensorViewWidget);

	SensorViewWidget->AddToViewport();
	SensorViewWidget->ToggleCameraView();
	SensorViewWidget->ToggleLidarView();
	if (VehiclePawn)
	{
		VehiclePawn->SetSensorViewWidget(SensorViewWidget);
	}
}

void AVehicleDTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);

		// optionally add the steering wheel context
		if (bUseSteeringWheelControls && SteeringWheelInputMappingContext)
		{
			Subsystem->AddMappingContext(SteeringWheelInputMappingContext, 1);
		}
	}
}

void AVehicleDTPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
	}
}

void AVehicleDTPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// get a pointer to the controlled pawn
	VehiclePawn = CastChecked<AVehicleDTPawn>(InPawn);
	VehiclePawn->SetSensorViewWidget(SensorViewWidget);
}
