/*

 Demonstrate how the timer feature works.

 */

#include "pebble.h"

static Window *window;

static AppTimer *timer;

int score = 0;
int highScorePersistKey = 2;
int highScore = 0;
float speed = 2.5f;
float kSpeed = 0.47f;
int numberofballs = 3;
float xs[] = { 0, -54, -108 };
int lines[] = { 0, 2, 1 };
int endX = 124;
int dX = 16;
int gameState = 0; // start, game, gameover
typedef struct {
	int t;
	float x, y, vx, vy, a;
} Sparkle;
float sparkleG = 0.3f;
int sparkleT = 17;
int sparklesN = 30;
Sparkle *sparkles;
int lastClickLane;
int lastClickTimer = 0;
int coolDownTimer = 0;
int mistakeCounter = 0;
InverterLayer *inverterLayer10;
InverterLayer *inverterLayer20;
InverterLayer *inverterLayer21;
InverterLayer *inverterLayer22;
InverterLayer *inverterLayer23;
InverterLayer *inverterLayer24;
InverterLayer *inverterLayer30;
InverterLayer *inverterLayer31;
InverterLayer *inverterLayer32;
InverterLayer *inverterLayer33;
InverterLayer *inverterLayer34;

static void showDigit(int d) {
	layer_remove_child_layers(window_get_root_layer(window));
	switch (d) {
	case 1:
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer10));
		break;
	case 2:
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer20));
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer21));
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer22));
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer23));
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer24));
		break;
	case 3:
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer30));
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer31));
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer32));
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer33));
		layer_add_child(window_get_root_layer(window),
				inverter_layer_get_layer(inverterLayer34));
		break;
	}
}

static void initializeDigits() {
	int sc = 6;
	inverterLayer10 = inverter_layer_create(
			GRect(65, 80 - 5 * sc, 2 * sc, 10 * sc));
	inverterLayer20 = inverter_layer_create(
			GRect(72 - 4 * sc, 80 - 5 * sc, 8 * sc, 2 * sc));
	inverterLayer21 = inverter_layer_create(
			GRect(72 - 4 * sc, 80 - 1 * sc, 8 * sc, 2 * sc));
	inverterLayer22 = inverter_layer_create(
			GRect(72 - 4 * sc, 80 + 3 * sc, 8 * sc, 2 * sc));
	inverterLayer23 = inverter_layer_create(
			GRect(72 + 2 * sc, 80 - 3 * sc, 2 * sc, 2 * sc));
	inverterLayer24 = inverter_layer_create(
			GRect(72 - 4 * sc, 80 + 1 * sc, 2 * sc, 2 * sc));
	inverterLayer30 = inverter_layer_create(
			GRect(72 - 4 * sc, 80 - 5 * sc, 8 * sc, 2 * sc));
	inverterLayer31 = inverter_layer_create(
			GRect(72 - 4 * sc, 80 - 1 * sc, 8 * sc, 2 * sc));
	inverterLayer32 = inverter_layer_create(
			GRect(72 - 4 * sc, 80 + 3 * sc, 8 * sc, 2 * sc));
	inverterLayer33 = inverter_layer_create(
			GRect(72 + 2 * sc, 80 - 3 * sc, 2 * sc, 2 * sc));
	inverterLayer34 = inverter_layer_create(
			GRect(72 + 2 * sc, 80 + 1 * sc, 2 * sc, 2 * sc));
}

static void sparkify(int x, int y) {
	int j = 0, n = 0;
	while (j < sparklesN && n < 7) {
		if (sparkles[j].t == 0) {
			sparkles[j].t = sparkleT;
			int dx = rand() % 20 - 10;
			int dy = rand() % 10;
			sparkles[j].x = x + dx;
			sparkles[j].y = y - dy;
			sparkles[j].vx = dx / 5.0f;
			sparkles[j].vy = -dy / 4.0f;
			++n;
		}
		++j;
	}
}

static void timer_callback(void *data) {
	if (gameState == 1) {
		for (int i = 0; i < numberofballs; i++) {
			xs[i] += speed;
		}
		for (int i = 0; i < numberofballs; ++i) {
			if (xs[i] > endX + dX) {
				vibes_long_pulse();
				gameState = 2;
				if (highScore < score) {
					persist_write_int(highScorePersistKey, score);
					highScore = score;
				}
				layer_mark_dirty(window_get_root_layer(window));
				return;
			}
		}
		for (int i = 0; i < sparklesN; i++) {
			if (sparkles[i].t != 0) {
				sparkles[i].x += sparkles[i].vx;
				sparkles[i].y += sparkles[i].vy;
				sparkles[i].vy += sparkleG;
				sparkles[i].t--;
			}
		}
		if (lastClickTimer != 0)
			lastClickTimer--;
		layer_mark_dirty(window_get_root_layer(window));
	} else if (gameState == 3) {
		if (coolDownTimer == 3 * 14)
			showDigit(3);
		if (coolDownTimer == 2 * 14)
			showDigit(2);
		if (coolDownTimer == 1 * 14)
			showDigit(1);
		if (coolDownTimer != 0) {
			coolDownTimer--;
		}
		if (coolDownTimer == 0) {
			gameState = 1;
			layer_remove_child_layers(window_get_root_layer(window));
		}
		layer_mark_dirty(window_get_root_layer(window));
	}
	timer = app_timer_register(50, timer_callback, NULL);
}

static void onRButton(int lane) {
	lastClickLane = lane;
	lastClickTimer = (int) (2.5f + 5 / speed);
	for (int i = 0; i < numberofballs; i++) {
		if (xs[i] > endX - dX && xs[i] < endX + dX) {
			if (lines[i] == lane) {
				speed += kSpeed / speed;
				score++;

				sparkify(xs[i], lines[i] * 48 + 24);

				xs[i] -= 162;
				lines[i] = rand() % 3;
				mistakeCounter = 0;
				return;
			}
		}
	}
	mistakeCounter++;
	if (mistakeCounter == 2) {
		app_timer_cancel(timer);
		vibes_long_pulse();
		gameState = 2;
		if (highScore < score) {
			persist_write_int(highScorePersistKey, score);
			highScore = score;
		}
		layer_mark_dirty(window_get_root_layer(window));
	}
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (gameState == 1) {
		onRButton(0);
	}
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (gameState == 0) {
		gameState = 1;
		speed = 2.5f;
		xs[0] = 0;
		xs[1] = -54;
		xs[2] = -108;
		lines[0] = rand() % 3;
		lines[1] = rand() % 3;
		lines[2] = rand() % 3;
		mistakeCounter = 0;
		lastClickTimer = 0;
		coolDownTimer = 0;
		for (int i = 0; i < sparklesN; i++)
			sparkles[i].t = 0;
		score = 0;
		timer = app_timer_register(50, timer_callback, NULL);
	} else if (gameState == 1) {
		onRButton(1);
	} else if (gameState == 3 && coolDownTimer == 0) {
		coolDownTimer = 42;
		timer = app_timer_register(50, timer_callback, NULL);
	}
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (gameState == 1) {
		onRButton(2);
	}

}
static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (gameState == 0) {
		window_stack_pop_all(1);
	} else if (gameState == 1) {
		gameState = 3;
		app_timer_cancel(timer);
		layer_mark_dirty(window_get_root_layer(window));
	} else if (gameState == 2) {
		gameState = 0;
		layer_mark_dirty(window_get_root_layer(window));
	} else if (gameState == 3) {
		gameState = 0;
		layer_remove_child_layers(window_get_root_layer(window));
		app_timer_cancel(timer);
		layer_mark_dirty(window_get_root_layer(window));
	}

}
static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

void draw_layer(Layer *layer, GContext *gctxt) {
	graphics_context_set_fill_color(gctxt, GColorBlack);
	GRect rect = GRect(0, 0, 144, 168);
	graphics_fill_rect(gctxt, rect, 0, GCornerNone);
	if (gameState == 0) {
		graphics_context_set_stroke_color(gctxt, GColorWhite);
		graphics_draw_text(gctxt, "Pebble Tiles!",
				fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
				GRect(0, 40, 144, 168), GTextOverflowModeWordWrap,
				GTextAlignmentCenter, NULL);
		graphics_draw_text(gctxt, "Press select to start",
				fonts_get_system_font(FONT_KEY_GOTHIC_18),
				GRect(0, 80, 144, 168), GTextOverflowModeWordWrap,
				GTextAlignmentCenter, NULL);
	} else if (gameState == 1 || gameState == 2 || gameState == 3) {
		for (int i = 0; i < numberofballs; i++) {
			GPoint point = GPoint((float) xs[i], lines[i] * 48 + 24);
			graphics_context_set_fill_color(gctxt, GColorWhite);
			graphics_fill_circle(gctxt, point, 16);
		}
		for (int i = 0; i < 3; i++) {
			GPoint point = GPoint(endX, i * 48 + 24);
			graphics_context_set_stroke_color(gctxt, GColorWhite);
			graphics_draw_circle(gctxt, point, 16);
		}

		if (lastClickTimer != 0) {
			graphics_context_set_stroke_color(gctxt, GColorWhite);
			switch (lastClickLane) {
			case 2:
				graphics_draw_line(gctxt,
						GPoint(endX - 7, lastClickLane * 48 + 24 - 7),
						GPoint(endX + 7, lastClickLane * 48 + 24 + 7));
				graphics_draw_line(gctxt,
						GPoint(endX - 7, lastClickLane * 48 + 24 + 7),
						GPoint(endX + 7, lastClickLane * 48 + 24 - 7));
				graphics_draw_line(gctxt,
						GPoint(endX - 7, lastClickLane * 48 + 24 - 6),
						GPoint(endX + 7, lastClickLane * 48 + 24 + 8));
				graphics_draw_line(gctxt,
						GPoint(endX - 7, lastClickLane * 48 + 24 + 8),
						GPoint(endX + 7, lastClickLane * 48 + 24 - 6));
				break;
			case 0:
				graphics_draw_line(gctxt,
						GPoint(endX, lastClickLane * 48 + 24 - 10),
						GPoint(endX + 9, lastClickLane * 48 + 24 + 7));
				graphics_draw_line(gctxt,
						GPoint(endX, lastClickLane * 48 + 24 - 10),
						GPoint(endX - 9, lastClickLane * 48 + 24 + 7));
				graphics_draw_line(gctxt,
						GPoint(endX - 9, lastClickLane * 48 + 24 + 7),
						GPoint(endX + 9, lastClickLane * 48 + 24 + 7));
				break;
			case 1:
				graphics_draw_line(gctxt,
						GPoint(endX - 8, lastClickLane * 48 + 24 - 8),
						GPoint(endX + 8, lastClickLane * 48 + 24 - 8));
				graphics_draw_line(gctxt,
						GPoint(endX - 8, lastClickLane * 48 + 24 + 8),
						GPoint(endX + 8, lastClickLane * 48 + 24 + 8));
				graphics_draw_line(gctxt,
						GPoint(endX - 8, lastClickLane * 48 + 24 - 8),
						GPoint(endX - 8, lastClickLane * 48 + 24 + 8));
				graphics_draw_line(gctxt,
						GPoint(endX + 8, lastClickLane * 48 + 24 - 8),
						GPoint(endX + 8, lastClickLane * 48 + 24 + 8));
				break;
			}
		}

		graphics_context_set_fill_color(gctxt, GColorWhite);
		for (int i = 0; i < sparklesN; i++) {
			if (sparkles[i].t != 0) {
				GRect rect = GRect((int) sparkles[i].x, (int) sparkles[i].y,
						(sparkles[i].t * 5) / sparkleT,
						(sparkles[i].t * 5) / sparkleT);
				graphics_fill_rect(gctxt, rect, 0, GCornerNone);
			}
		}

		if (gameState == 2 || (gameState == 3 && coolDownTimer == 0)) {
			graphics_context_set_fill_color(gctxt, GColorBlack);
			GRect rect = GRect(20, 45, 104, 28);
			graphics_fill_rect(gctxt, rect, 0, GCornerNone);

			graphics_context_set_stroke_color(gctxt, GColorWhite);
			graphics_draw_rect(gctxt, rect);
			graphics_context_set_stroke_color(gctxt, GColorWhite);

			char* buff = "Score:         ";
			snprintf(buff + 7, 16, "%d", score);
			graphics_draw_text(gctxt, buff,
					fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
					GRect(0, 42, 144, 168), GTextOverflowModeWordWrap,
					GTextAlignmentCenter, NULL);
		}
		if (gameState == 2) {
			graphics_context_set_fill_color(gctxt, GColorBlack);
			rect = GRect(10, 85, 124, 30);
			graphics_fill_rect(gctxt, rect, 0, GCornerNone);

			graphics_context_set_stroke_color(gctxt, GColorWhite);
			graphics_draw_rect(gctxt, rect);
			graphics_context_set_stroke_color(gctxt, GColorWhite);

			char* buff2 = "High Score:                ";
			snprintf(buff2 + 12, 16, "%d", highScore);
			graphics_draw_text(gctxt, buff2,
					fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
					GRect(0, 82, 144, 168), GTextOverflowModeWordWrap,
					GTextAlignmentCenter, NULL);

			graphics_context_set_fill_color(gctxt, GColorBlack);
			rect = GRect(0, 126, 144, 13);
			graphics_fill_rect(gctxt, rect, 0, GCornerNone);
			graphics_context_set_stroke_color(gctxt, GColorWhite);
			graphics_draw_text(gctxt, "Press back to go to menu",
					fonts_get_system_font(FONT_KEY_GOTHIC_14),
					GRect(0, 123, 144, 168), GTextOverflowModeWordWrap,
					GTextAlignmentCenter, NULL);
		}
		if (gameState == 3) {
			if (coolDownTimer == 0) {
				graphics_context_set_fill_color(gctxt, GColorBlack);
				rect = GRect(0, 126, 144, 13);
				graphics_fill_rect(gctxt, rect, 0, GCornerNone);
				graphics_context_set_stroke_color(gctxt, GColorWhite);
				graphics_draw_text(gctxt, "Press select to continue",
						fonts_get_system_font(FONT_KEY_GOTHIC_14),
						GRect(0, 123, 144, 168), GTextOverflowModeWordWrap,
						GTextAlignmentCenter, NULL);
			} else {
				/*graphics_context_set_fill_color(gctxt, GColorBlack);
				 GRect rect = GRect(52, 55, 40, 28);
				 graphics_fill_rect(gctxt, rect, 0, GCornerNone);

				 graphics_context_set_stroke_color(gctxt, GColorWhite);
				 graphics_draw_rect(gctxt, rect);
				 graphics_context_set_stroke_color(gctxt, GColorWhite);

				 char* buff = "                  ";
				 snprintf(buff, 16, "%d", (coolDownTimer / 14) + 1);
				 graphics_draw_text(gctxt, buff,
				 fonts_get_system_font(
				 FONT_KEY_BITHAM_34_MEDIUM_NUMBERS),
				 GRect(0, 52, 144, 168), GTextOverflowModeWordWrap,
				 GTextAlignmentCenter, NULL);*/
			}
		}
	}
}

int main(void) {
	window = window_create();
	window_stack_push(window, true /* Animated */);
	window_set_click_config_provider(window, click_config_provider);

	Layer *window_layer = window_get_root_layer(window);
	layer_set_update_proc(window_layer, draw_layer);

	initializeDigits();

	if (persist_exists(highScorePersistKey))
		highScore = persist_read_int(highScorePersistKey);

	sparkles = (Sparkle*) malloc(sparklesN * sizeof(Sparkle));
	for (int i = 0; i < sparklesN; i++) {
		sparkles[i].t = 0;
	}

	app_event_loop();

	vibes_cancel();
	window_destroy(window);
}