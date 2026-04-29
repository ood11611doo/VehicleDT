// Fill out your copyright notice in the Description page of Project Settings.

#include "Sensor/SensorViewWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"

void USensorViewWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USensorViewWidget::SetRenderTarget(UTextureRenderTarget2D* InRenderTarget)
{
}

void USensorViewWidget::SetLidarRenderTarget(UTexture2D* InRenderTarget)
{
}

void USensorViewWidget::ToggleCameraView()
{
}

void USensorViewWidget::ToggleLidarView()
{
}

bool USensorViewWidget::IsCameraViewVisible() const
{
	return SensorBorder && SensorBorder->GetVisibility() == ESlateVisibility::Visible;
}

bool USensorViewWidget::IsLidarViewVisible() const
{
	return LidarBorder && LidarBorder->GetVisibility() == ESlateVisibility::Visible;
}
