#pragma once

#include "resource.h"
#include <vector>

void OpenImage(HWND hwnd);
void ResizeImage(HWND hwnd, int n, int m);
void SaveImage(HWND hwnd);

//std::vector<int> findVerticalSeamGreedy(const Mat& energyMap);
//Mat removeVerticalSeamGreedy(const Mat& image, const std::vector<int>& seam);
//std::vector<int> findHorizontalSeamGreedy(const Mat& energyMap);
//Mat removeHorizontalSeamGreedy(const Mat& image, const std::vector<int>& seam);