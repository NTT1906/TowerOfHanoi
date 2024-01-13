#include <raylib.h>
#include <stdio.h>

#define COL_WIDTH 20

#define ROD_AMOUNT 3
#define DISK_AMOUNT 3
#define DISK_WIDTH_GROW 30
#define LOWEST_DISK_WIDTH (DISK_AMOUNT * DISK_WIDTH_GROW * 2)
#define MAX 20

struct Disk{
	int x;
	int y;
	int id;
	int rod;
};

struct Rod{
	int x;
	int current;
	int disks[DISK_AMOUNT];
};

int STEPS[1 << MAX][3];
int move = 0;

void initSteps(int n, int from_rod, int to_rod, int aux_rod) {
	if (n == 1) {
		STEPS[move][0] = from_rod;
		STEPS[move][1] = to_rod;
		STEPS[move][2] = 1;
		move++;
		return;
	}
	initSteps(n - 1, from_rod, aux_rod, to_rod);
	STEPS[move][0] = from_rod;
	STEPS[move][1] = to_rod;
	STEPS[move][2] = n;
	move++;
	initSteps(n - 1, aux_rod, to_rod, from_rod);
}

void printArr(int a[1 << MAX][3], int n) {
	for (int i = 0; i < (1 << n) - 1; i++) {
		printf("Step %d : (%d) %d -> %d\n", i + 1,  a[i][2], a[i][0], a[i][1]);
	}
}

int main(void) {
	struct Disk disks[MAX] = {};
	struct Rod rods[ROD_AMOUNT] = {0};
	int disk = DISK_AMOUNT - 1;

	initSteps(DISK_AMOUNT, 0, 1, 2);
	printArr(STEPS, 3);
	InitWindow(700, 500, "Tower Of Hanoi");
	SetTargetFPS(500);
//	SetTargetFPS(50);

	int diskHeight = GetScreenHeight() / DISK_AMOUNT / 8;
	int pileHeight = diskHeight * (DISK_AMOUNT + 5);
	int diskBorder = GetScreenHeight() - pileHeight - 30;

	int xSteps[] = {0, 1, 0};
	int ySteps[] = {-1, 0, 1};

	for (int i = 0; i < 3; i++) { //Draw pile
		rods[i].x = GetScreenWidth() * (i + 1) / (ROD_AMOUNT + 1) - COL_WIDTH / 2;
		rods[i].current = -1;
	}
	for (int i = 0; i < DISK_AMOUNT; i++) {
		disks[++rods[0].current].x = rods[0].x;
		disks[rods[0].current].y = GetScreenHeight() - ((i + 1) * diskHeight);
		disks[rods[0].current].id = i;
		disks[rods[0].current].rod = 0;
	}

	int source = STEPS[0][0];
	int target = STEPS[0][1];

	int step = 1;
	int m = 0;

	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(WHITE);

		for (int i = 0; i < 3; i++) { //Draw rod
			DrawRectangle(GetScreenWidth() * (i + 1) / (ROD_AMOUNT + 1) - COL_WIDTH / 2,
						  GetScreenHeight() - pileHeight, COL_WIDTH, pileHeight, GRAY);
		}

		for (int i = 0; i < DISK_AMOUNT; i++) { //Draw disk
			int diskWidth = LOWEST_DISK_WIDTH - DISK_WIDTH_GROW * (i + 1);
			DrawRectangle(disks[i].x - diskWidth / 2 + COL_WIDTH / 2, disks[i].y, diskWidth, diskHeight, RED);
			char num[100];
			sprintf(num, "%d", disks[i].id);
			DrawText(num, disks[i].x - MeasureText(num, diskHeight) / 2 + COL_WIDTH / 2, disks[i].y, diskHeight, WHITE);
		}
		int targetY = GetScreenHeight() - diskHeight * (rods[target].current + 2);

		int debug = 0;
		char sStep[100];
		DrawFPS(GetRenderWidth() - 100, 0);
		sprintf(sStep, "Move: %d", m);
		DrawText(sStep, GetRenderWidth() - 100, 100, 15, BLACK);
		sprintf(sStep, "Step: %d", step);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Disk x: %d", disks[disk].x);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Disk y: %d", disks[disk].y);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Current disk: %d", disk);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);

		sprintf(sStep, "Current: %d", source);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Target: %d", target);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Rod 1 current: %d", rods[0].current);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Rod 2 current: %d", rods[1].current);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Rod 3 current: %d", rods[2].current);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Move disk %d from rod %d to rod %d", disk, source, target);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Next: move disk %d from rod %d to rod %d", rods[STEPS[m + 1][0]].disks[rods[STEPS[m + 1][0]].current], STEPS[m + 1][0],  STEPS[m + 1][1]);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);

		if (target != source) {
			disks[disk].x += (target - source) * xSteps[step - 1];
			disks[disk].y += ySteps[step - 1];
		}

		if (step == 1 && disks[disk].y == diskBorder) {
			step = 2;
		}
		if (step == 2 && disks[disk].x == rods[target].x) {
			step = 3;
		}
		if (step == 3 && disks[disk].y == targetY) {
			step = 1;
			disks[disk].rod = target;
			rods[target].current++;
			rods[target].disks[rods[target].current] = disk;
			rods[source].current--;
			m++;
			source = STEPS[m][0];
//			disk = rods[source].disks[rods[source].current - 1];
			disk = STEPS[m][2] - 1;
			target = STEPS[m][1];
		}
		sprintf(sStep, "Disk %d", disk);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Disk id %d", disks[disk].id);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
