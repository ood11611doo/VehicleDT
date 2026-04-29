// Fill out your copyright notice in the Description page of Project Settings.

#include "Sensor/CameraSensorComponent.h"

UCameraSensorComponent::UCameraSensorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCameraSensorComponent::OnRegister()
{
	Super::OnRegister();
}

void UCameraSensorComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeCapture();
}

void UCameraSensorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	StopCaptureTimer();
}

#if WITH_EDITOR
void UCameraSensorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	RefreshSettings();
}
#endif

void UCameraSensorComponent::ApplyPreset(ECameraSensorPreset NewPreset)
{
}

void UCameraSensorComponent::SetCaptureRate(float Hz)
{
}

void UCameraSensorComponent::CaptureOnce()
{
}

void UCameraSensorComponent::RefreshSettings()
{
}

void UCameraSensorComponent::InitializeCapture()
{
}

void UCameraSensorComponent::CreateRenderTarget()
{
}

void UCameraSensorComponent::ConfigureSceneCapture()
{
}

void UCameraSensorComponent::ApplyPostProcessSettings()
{
}

void UCameraSensorComponent::ApplyLensDistortion()
{
}

void UCameraSensorComponent::StartCaptureTimer()
{
}

void UCameraSensorComponent::StopCaptureTimer()
{
}

void UCameraSensorComponent::OnCaptureTimer()
{
}

void UCameraSensorComponent::SaveCameraImage()
{
}
