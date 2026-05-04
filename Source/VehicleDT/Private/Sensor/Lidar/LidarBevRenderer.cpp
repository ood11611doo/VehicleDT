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

	if (Config.bDrawObstacleBoxes)
		DrawObstacleBoundingBoxes(Scan, SensorTransform);

	Canvas->UpdateTextureRegions(
		0, 1, &UpdateRegion,
		ImgSize * sizeof(FColor), sizeof(FColor),
		reinterpret_cast<uint8*>(Px));
}

void ULidarBevRenderer::DrawObstacleBoundingBoxes(const FLidarScanResult& Scan,
                                                   const FTransform& SensorTransform)
{
	const int32 ImgSize  = Config.ImageSize;
	const float Half     = static_cast<float>(ImgSize) * 0.5f;
	const float Scale    = Half / Config.ViewRange;
	const int32 CellPx   = FMath::Max(Config.ClusterCellPx, 1);
	const int32 GridW    = ImgSize / CellPx;
	const int32 GridN    = GridW * GridW;

	// Build occupancy grid — only points above ground threshold and outside self-exclusion radius
	TArray<bool> Occupied;
	Occupied.Init(false, GridN);

	const FTransform InvSensor  = SensorTransform.Inverse();
	const float SelfExcludeSq   = 200.f * 200.f; // ignore within 2 m of sensor origin
	const float GroundThreshold = Config.ObstacleGroundHeight;

	for (int32 i = 0; i < Scan.PointCount && i < Scan.Points.Num(); ++i)
	{
		const FVector Local = InvSensor.TransformPosition(Scan.Points[i]);
		if (Local.Z < GroundThreshold) continue;
		if (Local.X * Local.X + Local.Y * Local.Y < SelfExcludeSq) continue;

		const int32 PX = FMath::RoundToInt32(Half + Local.Y * Scale);
		const int32 PY = FMath::RoundToInt32(Half - Local.X * Scale);
		const int32 GX = FMath::Clamp(PX / CellPx, 0, GridW - 1);
		const int32 GY = FMath::Clamp(PY / CellPx, 0, GridW - 1);
		Occupied[GY * GridW + GX] = true;
	}

	// Connected-component labeling (4-connected BFS)
	TArray<int32> Labels;
	Labels.Init(-1, GridN);
	int32 NumLabels = 0;

	struct FBox2i { int32 MinX, MinY, MaxX, MaxY; };
	TArray<FBox2i> Boxes;

	for (int32 Start = 0; Start < GridN; ++Start)
	{
		if (!Occupied[Start] || Labels[Start] >= 0) continue;

		const int32 Label = NumLabels++;
		Boxes.Add({ GridW, GridW, -1, -1 });
		FBox2i& Box = Boxes.Last();

		TArray<int32, TInlineAllocator<64>> Queue;
		Queue.Add(Start);
		Labels[Start] = Label;

		for (int32 Q = 0; Q < Queue.Num(); ++Q)
		{
			const int32 Cur = Queue[Q];
			const int32 CX  = Cur % GridW;
			const int32 CY  = Cur / GridW;

			Box.MinX = FMath::Min(Box.MinX, CX);
			Box.MinY = FMath::Min(Box.MinY, CY);
			Box.MaxX = FMath::Max(Box.MaxX, CX);
			Box.MaxY = FMath::Max(Box.MaxY, CY);

			const int32 Neighbors[4] = {
				CY > 0       ? Cur - GridW : -1,
				CY < GridW-1 ? Cur + GridW : -1,
				CX > 0       ? Cur - 1     : -1,
				CX < GridW-1 ? Cur + 1     : -1
			};
			for (int32 N : Neighbors)
			{
				if (N >= 0 && Occupied[N] && Labels[N] < 0)
				{
					Labels[N] = Label;
					Queue.Add(N);
				}
			}
		}
	}

	// Draw box outlines — skip single-cell noise
	static const FColor BoxPalette[] = {
		FColor(0,   255, 255, 255),  // cyan
		FColor(255, 165,   0, 255),  // orange
		FColor(255,   0, 255, 255),  // magenta
		FColor(0,   200,   0, 255),  // green
	};
	constexpr int32 NumColors = 4;

	FColor* Px = PixelBuffer.GetData();

	for (int32 L = 0; L < Boxes.Num(); ++L)
	{
		const FBox2i& B = Boxes[L];
		if (B.MaxX < B.MinX || B.MaxY < B.MinY) continue;
		if ((B.MaxX - B.MinX + 1) * (B.MaxY - B.MinY + 1) < 2) continue;

		const int32 X0 = B.MinX * CellPx;
		const int32 Y0 = B.MinY * CellPx;
		const int32 X1 = FMath::Min((B.MaxX + 1) * CellPx, ImgSize - 1);
		const int32 Y1 = FMath::Min((B.MaxY + 1) * CellPx, ImgSize - 1);

		const FColor Col = BoxPalette[L % NumColors];

		for (int32 X = X0; X <= X1; ++X)
		{
			if (X < 0 || X >= ImgSize) continue;
			if (Y0 >= 0 && Y0 < ImgSize) Px[Y0 * ImgSize + X] = Col;
			if (Y1 >= 0 && Y1 < ImgSize) Px[Y1 * ImgSize + X] = Col;
		}
		for (int32 Y = Y0 + 1; Y < Y1; ++Y)
		{
			if (Y < 0 || Y >= ImgSize) continue;
			if (X0 >= 0 && X0 < ImgSize) Px[Y * ImgSize + X0] = Col;
			if (X1 >= 0 && X1 < ImgSize) Px[Y * ImgSize + X1] = Col;
		}
	}
}
