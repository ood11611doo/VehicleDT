#include "Sensor/UI/SensorViewWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"

void USensorViewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CameraPanel) CameraPanel->SetVisibility(ESlateVisibility::Collapsed);
	if (LidarPanel)  LidarPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void USensorViewWidget::SetRenderTarget(UTextureRenderTarget2D* InTarget)
{
	if (InTarget && CameraImage)
		CameraImage->SetBrushResourceObject(InTarget);
}

void USensorViewWidget::SetLidarTexture(UTexture2D* InTexture)
{
	if (InTexture && LidarImage)
		LidarImage->SetBrushResourceObject(InTexture);
}

void USensorViewWidget::ToggleCameraView()
{
	if (!CameraPanel) return;

	CameraPanel->SetVisibility(
		IsCameraVisible() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void USensorViewWidget::ToggleLidarView()
{
	if (!LidarPanel) return;

	LidarPanel->SetVisibility(
		IsLidarVisible() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

bool USensorViewWidget::IsCameraVisible() const
{
	return CameraPanel && CameraPanel->GetVisibility() != ESlateVisibility::Collapsed;
}

bool USensorViewWidget::IsLidarVisible() const
{
	return LidarPanel && LidarPanel->GetVisibility() != ESlateVisibility::Collapsed;
}
