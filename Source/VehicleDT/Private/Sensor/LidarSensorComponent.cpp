// Fill out your copyright notice in the Description page of Project Settings.

#include "Sensor/LidarSensorComponent.h"

ULidarSensorComponent::ULidarSensorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULidarSensorComponent::StartScan()
{
}

void ULidarSensorComponent::StopScan()
{
}

void ULidarSensorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULidarSensorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	StopScan();
}

void ULidarSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#if WITH_EDITOR
void ULidarSensorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	RefreshSettings();
}
#endif

void ULidarSensorComponent::ApplyPreset(ELidarSensorPreset NewPreset)
{
}

void ULidarSensorComponent::SetScanRate(float Hz)
{
}

void ULidarSensorComponent::RefreshSettings()
{
}

void ULidarSensorComponent::InitializeSensor()
{
}

void ULidarSensorComponent::StartScanTimer()
{
}

void ULidarSensorComponent::StopScanTimer()
{
}

void ULidarSensorComponent::OnScanTimer()
{
}

void ULidarSensorComponent::FireAsyncTraces()
{
}

void ULidarSensorComponent::CollectAsyncResults()
{
	// 스캔 완료 후 구독자에게 브로드캐스트
	OnLidarScanReady.Broadcast(LastPointCloud);
}

void ULidarSensorComponent::RebuildDirectionCache()
{
}
