// DearGlfw.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#define __cpp_consteval

#include <iostream>
#include <math.h>
#include <memory>
#include <optional>
#include <source_location>
#include <string>
#include <tuple>
#include <variant>
#include <utility>

#include <fmt/format.h>

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>

constexpr uint32_t SCREEN_WIDTH = 640;
constexpr uint32_t SCREEN_HEIGHT = 480;

auto formatColor(SDL_Color color) -> std::string {
	return fmt::format("Color({},{},{},{})", color.r, color.g, color.b, color.a);
}

enum class LerpBehavior {
	Once,
	Cycle,
	PingPong,
};

auto lerpColor(SDL_Color startColor, SDL_Color endColor, uint32_t cycleTime, uint32_t currentTime, LerpBehavior behavior) -> SDL_Color;

class App {
public:
	App() = default;

	~App() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	std::optional<std::string> init() {
		int rendererFlags, windowFlags;

		rendererFlags = SDL_RENDERER_ACCELERATED;

		windowFlags = 0;

		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			return std::make_optional(fmt::format("Couldn't initialize SDL: {}\n", SDL_GetError()));
		}

		window = SDL_CreateWindow("Hello SDL!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

		if (!window) {
			return std::make_optional(fmt::format("Failed to open {} x {} window: {}\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError()));
		}

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

		renderer = SDL_CreateRenderer(window, -1, rendererFlags);

		if (!renderer) {
			return std::make_optional(fmt::format("Failed to create renderer: {}\n", SDL_GetError()));
		}

		IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

		return { };
	}

	void run() {
		lastTick = currentTick = SDL_GetTicks();
		while (!quitRequested_) {
			lastTick = currentTick;
			currentTick = SDL_GetTicks();
			prepareScene();
			setBgColor(currentTick);
			doInput();

			presentScene();

			SDL_Delay(16);
		}
	}

	void prepareScene() {
		SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
		SDL_RenderClear(renderer);
	}

	void setBgColor(uint32_t tick) {
		bgColor_ = lerpColor(color1_, color2_, cycleTime_, tick, lerpBehavior_);
	}

	void presentScene() {
		SDL_RenderPresent(renderer);
	}

	void doInput() {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: quitRequested_ = true;
				break;

			default: break;
			}
		}
	}


	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;
private:
	bool isInit_ { false };
	bool quitRequested_ { false };

	uint32_t currentTick { };
	uint32_t lastTick { };

	SDL_Color color1_ { 254, 204, 213, 255 };
	SDL_Color color2_ { 7, 255, 255, 255 };
	SDL_Color bgColor_ { color1_ };
	LerpBehavior lerpBehavior_ { LerpBehavior::PingPong };
	uint32_t cycleTime_ { 5000 };
};

auto shouldReverse(LerpBehavior behavior, uint32_t cycleTime, uint32_t currentTime) -> bool {
	return behavior == LerpBehavior::PingPong && (currentTime % (cycleTime * 2) >= cycleTime);
}

auto quickClamp(auto val, auto min, auto max) -> auto {
	return val > max ? max : val < min ? min : val;
}

auto makeByte(double val) -> uint8_t {
	return static_cast<uint8_t>(val);
}

auto getPointBetween(SDL_Color start, SDL_Color end, double percent) -> SDL_Color {

	double p = quickClamp(percent, 0.0, 1.0);



	uint8_t r = start.r == end.r ? start.r : makeByte(quickClamp(std::lerp(start.r, end.r, p), 0, 255));
	uint8_t g = start.g == end.g ? start.g : makeByte(quickClamp(std::lerp(start.g, end.g, p), 0, 255));
	uint8_t b = start.b == end.b ? start.b : makeByte(quickClamp(std::lerp(start.b, end.b, p), 0, 255));
	uint8_t a = start.a == end.a ? start.a : makeByte(quickClamp(std::lerp(start.a, end.a, p), 0, 255));

	return SDL_Color { r, g, b, a };
}

static SDL_Color ERROR_COLOR = SDL_Color { 255, 0, 0, 255 };

auto lerpColorOnce(SDL_Color startColor, SDL_Color endColor, uint32_t cycleTime, uint32_t currentTime) -> SDL_Color;

auto lerpColorCycle(SDL_Color startColor, SDL_Color endColor, uint32_t cycleTime, uint32_t currentTime) -> SDL_Color;

auto lerpColorPingPong(SDL_Color startColor, SDL_Color endColor, uint32_t cycleTime, uint32_t currentTime) -> SDL_Color;

auto lerpColor(SDL_Color startColor, SDL_Color endColor, uint32_t cycleTime, uint32_t currentTime, LerpBehavior behavior) -> SDL_Color {
	if (cycleTime < 1) {
		std::cerr << "Invalid cycle time." << '\n';
		return ERROR_COLOR;
	}

	switch (behavior) {
	case LerpBehavior::Once: return lerpColorOnce(startColor, endColor, cycleTime, currentTime);

	case LerpBehavior::Cycle: return lerpColorCycle(startColor, endColor, cycleTime, currentTime);

	case LerpBehavior::PingPong: return lerpColorPingPong(startColor, endColor, cycleTime, currentTime);

	default: auto loc = std::source_location::current();
		throw std::runtime_error(
			fmt::format(
				"Default switch statement. File = {} Function = {} Line = {}",
				loc.file_name(),
				loc.function_name(),
				loc.line())
		);
	}
}

auto lerpColorOnce(SDL_Color startColor, SDL_Color endColor, uint32_t cycleTime, uint32_t currentTime) -> SDL_Color {
	if (currentTime >= cycleTime) {
		return endColor;
	}

	if (currentTime <= cycleTime) {
		return startColor;
	}

	auto r = getPointBetween(startColor, endColor, ((double)currentTime) / cycleTime);
	fmt::print("LerpColorOnce returning color {}\n", formatColor(r));
	return r;
}

auto lerpColorCycle(SDL_Color startColor, SDL_Color endColor, uint32_t cycleTime, uint32_t currentTime) -> SDL_Color {
	auto r = getPointBetween(startColor, endColor, ((double)(currentTime % cycleTime)) / cycleTime);
	fmt::print("LerpColorCycle returning color {}\n", formatColor(r));
	return r;
}

auto lerpColorPingPong(SDL_Color startColor, SDL_Color endColor, uint32_t cycleTime, uint32_t currentTime) -> SDL_Color {
	auto rev = shouldReverse(LerpBehavior::PingPong, cycleTime, currentTime);
	auto r = rev ?
		getPointBetween(endColor, startColor, ((double)(currentTime % cycleTime)) / cycleTime) :
		getPointBetween(startColor, endColor, ((double)(currentTime % cycleTime)) / cycleTime);

	fmt::print("LerpColorPingPong {} Reversed and Returning {}\n", rev ? "IS" : "IS NOT", formatColor(r));
	return r;
}


// TODO: Reference additional headers your program requires here.
