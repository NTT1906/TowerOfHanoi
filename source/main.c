#include <raylib.h>
#include <stdio.h>

#define COL_WIDTH 20

#define ROD_AMOUNT 3
#define DISK_AMOUNT 4
#define MAX 20

struct Disk{
	int x;
	int y;
};

struct Rod{
	int x;
	int current;
};

int STEPS[1 << MAX][3];
int move = 0;

void initSteps(int n, int from_rod, int to_rod, int aux_rod) {
	if (n == 1) {
		STEPS[move][0] = from_rod;
		STEPS[move][1] = to_rod;
		STEPS[move][2] = 0;
		move++;
		return;
	}
	initSteps(n - 1, from_rod, aux_rod, to_rod);
	STEPS[move][0] = from_rod;
	STEPS[move][1] = to_rod;
	STEPS[move][2] = n - 1;
	move++;
	initSteps(n - 1, aux_rod, to_rod, from_rod);
}

void printArr(int a[1 << MAX][3], int n) {
	for (int i = 0; i < (1 << n) - 1; i++) {
		printf("Step %d : (%d) %d -> %d\n", i + 1,  a[i][2], a[i][0], a[i][1]);
	}
}

int gcd(int a, int b) {
	if (b == 0) return a;
	return gcd(b, a % b);
}

int main(void) {
	struct Disk disks[MAX] = {};
	struct Rod rods[ROD_AMOUNT] = {0};
	int disk = DISK_AMOUNT - 1;

	initSteps(DISK_AMOUNT, 0, 1, 2);
	printArr(STEPS, 3);
	InitWindow(1200, 800, "Tower Of Hanoi");

	int offset = 20;
	int diskHeight = ((GetScreenHeight() - offset) / DISK_AMOUNT) / 5;
	int rodHeight = diskHeight * (DISK_AMOUNT + 5);
	int diskBorder = GetScreenHeight() - rodHeight - 30;
	int bottomDiskWidth = GetScreenWidth() / (ROD_AMOUNT + 1) + (DISK_AMOUNT * COL_WIDTH / (ROD_AMOUNT + 1));
	int diskWidthGrow = bottomDiskWidth / (DISK_AMOUNT + 1);

	int xSteps[] = {0, 1, 0};
	int ySteps[] = {-1, 0, 1};

	for (int i = 0; i < 3; i++) { //Draw pile
		rods[i].x = GetScreenWidth() * (i + 1) / (ROD_AMOUNT + 1) - COL_WIDTH / 2;
		rods[i].current = -1;
	}
	for (int i = 0; i < DISK_AMOUNT; i++) {
		disks[++rods[0].current].x = rods[0].x;
		disks[rods[0].current].y = GetScreenHeight() - ((i + 1) * diskHeight);
	}

	int source = STEPS[0][0];
	int target = STEPS[0][1];

	int step = 1;
	int m = 0;

	bool tempBoost = false;
	bool done = false;
	bool lockedBoost = false;
	bool lockClicked = false;
	bool displayMove = false;
	bool displayMoveClicked = false;

	int BOOST_FPS = 5000;
	int NORMAL_FPS = 10;
	SetTargetFPS(NORMAL_FPS);

	int loading_bar_width = GetScreenWidth() / 2;
	int loading_bar_height = COL_WIDTH * 2;
	int loading_bar_x = GetRenderWidth() / 2;
	int loading_bar_y = diskBorder - GetScreenHeight() / 2;

	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(WHITE);
		DrawRectangleLines(loading_bar_x - loading_bar_width / 2 - 1, loading_bar_y + loading_bar_height / 2 - 1, loading_bar_width + 2, loading_bar_height + 2, BLACK);
		DrawRectangle(loading_bar_x - loading_bar_width / 2, loading_bar_y + loading_bar_height / 2, loading_bar_width * m / move, loading_bar_height, done ? GREEN : YELLOW);
		char mStr[32];
		sprintf(mStr, "%d", m);
		DrawText(mStr, loading_bar_x - loading_bar_width / 2 + loading_bar_width * m / move - MeasureText(mStr, 15) / 2, loading_bar_y + loading_bar_height / 2 + loading_bar_height + 5, 15, BLACK);
		if (!done) {
			sprintf(mStr, "%d", move);
			DrawText(mStr, loading_bar_x - loading_bar_width / 2 + loading_bar_width + 1 - MeasureText(mStr, 15) / 2, loading_bar_y + loading_bar_height / 2 + loading_bar_height + 5, 15, BLACK);
			//DrawText(mStr, loading_bar_x - loading_bar_width / 2 + loading_bar_width * m / move - MeasureText(mStr, 15) / 2, loading_bar_y + loading_bar_height / 2 + loading_bar_height + 5, 15, BLACK);
		}
		if (done) {
			DrawText("DONE", loading_bar_x - loading_bar_width / 2 - 1 )
		}
		if (m == move) {
			done = true;
		}
		if (!displayMoveClicked && IsKeyDown(KEY_LEFT_SHIFT)) {
			displayMove = !displayMove;
			displayMoveClicked = true;
		}
		if (displayMoveClicked && IsKeyUp(KEY_LEFT_SHIFT)) {
			displayMoveClicked = false;
		}
		if (!lockClicked && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_SPACE)) {
			lockedBoost = !lockedBoost;
			if (lockedBoost) {
				SetTargetFPS(BOOST_FPS);
			}
			lockClicked = true;
		}
		if (lockClicked && ((IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_RIGHT_CONTROL)) || IsKeyUp(KEY_SPACE))) {
			lockClicked = false;
		}
		if (!tempBoost && IsKeyDown(KEY_SPACE)) {
			SetTargetFPS(BOOST_FPS);
			tempBoost = true;
		}
		if (tempBoost && IsKeyUp(KEY_SPACE)) {
			tempBoost = false;
			if (!lockedBoost) SetTargetFPS(NORMAL_FPS);
		}
		if (tempBoost || lockedBoost) {
			if (lockedBoost) {
				DrawText("Locked", GetRenderWidth() - 200, 40, 20, BLACK);
			}
			DrawText("Speed!1!", GetRenderWidth() - 90, 40, 20, BLACK);
			if (GetFPS() % 2 == 0) DrawCircle(GetScreenWidth() - 90 + MeasureText("Speed!1!", 20) / 2, 60, 3, RED);
		}

		if (!done && IsKeyPressed(KEY_S)) {
			step = 1;
			int skipAmount = (move - m) / DISK_AMOUNT;
			if (move - m < 10) skipAmount = move - m;
			for (int i = 0; i < skipAmount; i++) {
				int targetY = GetScreenHeight() - diskHeight * (rods[target].current + 2);
				m++;
				rods[source].current--;
				rods[target].current++;
				disks[disk].x = rods[target].x;
				disks[disk].y = targetY;
				source = STEPS[m][0];
				target = STEPS[m][1];
				disk = DISK_AMOUNT - STEPS[m][2] - 1;
			}
//			char num[100];
//			sprintf(num, "Skipped %d moves", skipAmount);
//			DrawText(num, GetRenderWidth() - 90 - MeasureText(num, 20), 100, 20, BLACK);
			EndDrawing();
		}
		for (int i = 0; i < ROD_AMOUNT; i++) { //Draw rod
			DrawRectangle(GetScreenWidth() * (i + 1) / (ROD_AMOUNT + 1) - COL_WIDTH / 2,
						  GetScreenHeight() - rodHeight, COL_WIDTH, rodHeight, GRAY);
			char num[100];
			sprintf(num, "%d", rods[i].x);
			DrawText(num, GetScreenWidth() * (i + 1) / (ROD_AMOUNT + 1) - COL_WIDTH / 2,
						  GetScreenHeight() - rodHeight - 10, 10, BLACK);
		}

		for (int i = 0; i < DISK_AMOUNT; i++) { //Draw disk
			int diskWidth = bottomDiskWidth - diskWidthGrow * (i + 1);
			DrawRectangle(disks[i].x - diskWidth / 2 + COL_WIDTH / 2, disks[i].y, diskWidth, diskHeight, RED);
			char num[100];
			sprintf(num, "%d", i + 1);
			DrawText(num, disks[i].x - MeasureText(num, diskHeight) / 2 + COL_WIDTH / 2, disks[i].y, diskHeight - 1, WHITE);
		}
		int targetY = GetScreenHeight() - diskHeight * (rods[target].current + 2);

		int debug = 0;
		char sStep[100];
		DrawFPS(GetRenderWidth() - 100, 0);
		sprintf(sStep, "Step: %d", step);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Disk amount: %d", DISK_AMOUNT);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Disk: %d", disk);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Disk x: %d", disks[disk].x);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Disk y: %d", disks[disk].y);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);

		sprintf(sStep, "Source rod: %d", source);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Target rod: %d", target);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Target x: %d", rods[target].x);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Target y: %d", targetY);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Rod 1's disks: %d", rods[0].current + 1);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Rod 2's disks: %d", rods[1].current + 1);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		sprintf(sStep, "Rod 3's disks: %d", rods[2].current + 1);
		DrawText(sStep, 0, 10 * debug++, 10, BLACK);
		if (displayMove) {
			sprintf(sStep, "Move disk %d from rod %d to rod %d", disk, source, target);
			DrawText(sStep, GetRenderWidth() / 2 - MeasureText(sStep, 20) / 2, diskBorder / 2, 20, BLACK);
		}

		if (target != source) {
			int speedX = (target - source) * xSteps[step - 1];
			int speedY = ySteps[step - 1];
			sprintf(sStep, "Speed x: %d", speedX);
			DrawText(sStep, 0, 10 * debug++, 10, BLACK);
			sprintf(sStep, "Speed y: %d", speedY);
			DrawText(sStep, 0, 10 * debug++, 10, BLACK);
			disks[disk].x += speedX;
			disks[disk].y += speedY;
		}

		if (step == 1 && disks[disk].y == diskBorder) {
			step = 2;
		}
		if (step == 2 && disks[disk].x == rods[target].x) {
			step = 3;
		}
		if (step == 3 && disks[disk].y == targetY) {
			step = 1;
			m++;
			rods[source].current--;
			rods[target].current++;
			source = STEPS[m][0];
			target = STEPS[m][1];
			disk = DISK_AMOUNT - STEPS[m][2] - 1;
		}

		int log = 0;
		char logs[100];
		for (int i = 0; i < DISK_AMOUNT; i++) {
			sprintf(logs, "Disk %-2d{x: %d, y: %d}", i + 1, disks[i].x, disks[i].y);
			DrawText(logs, 0, GetScreenHeight() / 3 + 10 * log++, 10, BLACK);
		}
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
