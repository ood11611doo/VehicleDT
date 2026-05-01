#include "Sensor/Lidar/LidarBevRenderer.h"
#include "Engine/Texture2D.h"

void ULidarBevRenderer::Initialize(const FBevConfig& InConfig)
{
	Config = InConfig;
	ObstacleColor = InConfig.ObstacleColor.ToFColor(true);
	AllocateCanvas();
	BuildColorTable();
}

void ULidarBevRenderer::UpdateConfig(const FBevConfig& InConfig)
{
	const bool bResizeNeeded = (Config.ImageSize != InConfig.ImageSize);
	Config        = InConfig;
	ObstacleColor = InConfig.ObstacleColor.ToFColor(true);

	BuildColorTable();
	if (bResizeNeeded)
		AllocateCanvas();
}

void ULidarBevRenderer::AllocateCanvas()
{
	const int32 Size = Config.ImageSize;

	Canvas = UTexture2D::CreateTransient(Size, Size, PF_B8G8R8A8);
	Canvas->Filter = TF_Nearest;
	Canvas->SRGB   = true;
	Canvas->UpdateResource();

	PixelBuffer.SetNumUninitialized(Size * Size);
	UpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, Size, Size);
}

void ULidarBevRenderer::BuildColorTable()
{
	const FLinearColor Dark(0.0f, 0.25f, 0.0f, 1.0f);
	const FLinearColor Bright = Config.PointColor;
	for (int32 i = 0; i < 256; ++i)
	{
		ColorTable[i] = FMath::Lerp(Dark, Bright, static_cast<float>(i) / 255.f).ToFColor(true);
	}
}

void ULidarBevRenderer::Render(const FLidarScanResult& Scan, const FTransform& SensorTransform)
{
	if (!Canvas) return;

	const int32  ImgSize  = Config.ImageSize;
	const float  Half     = static_cast<float>(ImgSize) * 0.5f;
	const float  Scale    = Half / Config.ViewRange;
	const int32  PtSize   = static_cast<int32>(FMath::Max(Config.PointSize, 1.0f));
	const int32  PtHalf   = PtSize / 2;
	const bool   bObstHighlight = Config.ObstacleHighlightRange > 0.f;
	const float  ObstRangeSq    = Config.ObstacleHighlightRange * Config.ObstacleHighlightRange;

	const FColor BgColor = Config.BackgroundColor.ToFColor(true);
	FColor* RESTRICT Px  = PixelBuffer.GetData();
	const int32 TotalPx  = ImgSize * ImgSize;
	for (int32 i = 0; i < TotalPx; ++i)
		Px[i] = BgColor;

	const FTransform InvSensor = SensorTransform.Inverse();
	const int32      Count     = Scan.PointCount;
	const FVector*   RESTRICT Pts = Scan.Points.GetData();
	const float*     RESTRICT Int = Scan.Intensities.GetData();
	const int32      IntCount     = Scan.Intensities.Num();

	for (int32 i = 0; i < Count; ++i)
	{
		const FVector Local = InvSensor.TransformPosition(Pts[i]);

		const int32 CX = FMath::RoundToInt32(Half + Local.Y * Scale);
		const int32 CY = FMath::RoundToInt32(Half - Local.X * Scale);

		if (CX < PtHalf || CX >= ImgSize - PtHalf ||
			CY < PtHalf || CY >= ImgSize - PtHalf)
			continue;

		FColor Color;
		if (bObstHighlight && (Local.X * Local.X + Local.Y * Local.Y) < ObstRangeSq)
		{
			Color = ObstacleColor;
		}
		else
		{
			const float  Intensity = (i < IntCount) ? Int[i] : 0.5f;
			Color = ColorTable[static_cast<uint8>(FMath::Clamp(Intensity * 255.f, 0.f, 255.f))];
		}

		if (PtSize == 1)
		{
			Px[CY * ImgSize + CX] = Color;
		}
		else
		{
			for (int32 dy = -PtHalf; dy < PtSize - PtHalf; ++dy)
			{
				const int32 Row = (CY + dy) * ImgSize;
				for (int32 dx = -PtHalf; dx < PtSize - PtHalf; ++dx)
					Px[Row + CX + dx] = Color;
			}
		}
	}

	const int32  C     = FMath::RoundToInt32(Half);
	const FColor White(255, 255, 255, 255);
	for (int32 dy = -3; dy < 3; ++dy)
	{
		const int32 Row = (C + dy) * ImgSize;
		for (int32 dx = -3; dx < 3; ++dx)
			Px[Row + C + dx] = White;
	}

	Canvas->UpdateTextureRegions(
		0, 1, &UpdateRegion,
		ImgSize * sizeof(FColor), sizeof(FColor),
		reinterpret_cast<uint8*>(Px));
}
