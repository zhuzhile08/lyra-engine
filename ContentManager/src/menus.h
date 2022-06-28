/*************************
 * @file   menus.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  A singleton containing all menu functions
 * 
 * @date   2022-26-6
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <imgui.h>
#include <lyra.h>

#include <vector>

class Menus {
public:
	~Menus() noexcept { }

	Menus() noexcept = delete;
	Menus(const Menus&) noexcept = delete;
	Menus operator=(const Menus&) const noexcept = delete;

	/**
	 * @brief show the window bar
	 */
	static void show_window_bar();

private:
	static void file_text_input(std::string openedType);

	// static void file_successful_load();
};
