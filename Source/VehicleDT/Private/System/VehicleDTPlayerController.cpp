// Copyright Epic Games, Inc. All Rights Reserved.


#include "System/VehicleDTPlayerController.h"
#include "VehicleDTPawn.h"
#include "System/VehicleDTUI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Sensor/SensorViewWidget.h"
#include "BEV/BEVVisualizationComponent.h"

void AVehicleDTPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// spawn the UI widget and add it to the viewport
	VehicleUI = CreateWidget<UVehicleDTUI>(this, VehicleUIClass);

	check(VehicleUI);

	VehicleUI->AddToViewport();
	
	if (SensorViewClass)
	{
		SensorView = CreateWidget<USensorViewWidget>(this, SensorViewClass);
		if (SensorView)
		{
			SensorView->AddToViewport();
			SensorView->ToggleLidarView();   // BEV 켜기
			SensorView->ToggleCameraView();  // 카메라 켜기

			// BEV 텍스처 연결
			GetWorldTimerManager().SetTimerForNextTick([this]()
			{
				if (auto* P = Cast<AVehicleDTPawn>(GetPawn()))
				{
					if (auto* BEV = P->FindComponentByClass<UBEVVisualizationComponent>())
					{
						BEV->OnBEVUpdate.AddUObject(SensorView.Get(),
							&USensorViewWidget::SetLidarRenderTarget);
						SensorView->SetLidarRenderTarget(BEV->GetRenderTarget());
					}
				}
			});
		}
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
}
