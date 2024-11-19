// SeamCarvingAss2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "SeamCarvingAss2.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp> // Required for Sobel and other image processing functions
#include <iostream>
#include <commdlg.h>
#include <string>
#include <vector>
#include <fstream>
#include <limits> // For numeric_limits

using namespace cv;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND hButtonLoad, hButtonResize, hButtonSave;
Mat img, resizedImg;
HWND hWndImage;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SEAMCARVINGASS2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SEAMCARVINGASS2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SEAMCARVINGASS2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SEAMCARVINGASS2);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Create buttons
    hButtonLoad = CreateWindow(L"BUTTON", L"Load Image", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 10, 120, 30, hWnd, (HMENU)1, hInstance, NULL);
    hButtonResize = CreateWindow(L"BUTTON", L"Resize Image", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 50, 120, 30, hWnd, (HMENU)2, hInstance, NULL);
    hButtonSave = CreateWindow(L"BUTTON", L"Save Image", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 90, 120, 30, hWnd, (HMENU)3, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case 1: // Load Image
        {
            OpenImage(hWnd);
            break;
        }
        case 2: // Resize Image
        {
            ResizeImage(hWnd);
            break;
        }
        case 3: // Save Image
        {
            SaveImage(hWnd);
            break;
        }

        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void OpenImage(HWND hwnd)
{
    OPENFILENAME ofn = {};
    wchar_t fileName[MAX_PATH] = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Image Files\0*.bmp;*.jpg;*.png\0All Files\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        img = imread(std::string(fileName, fileName + wcslen(fileName)));
        if (!img.empty()) {
            MessageBox(hwnd, L"Image Loaded!", L"Info", MB_OK);
        }
        else {
            MessageBox(hwnd, L"Failed to Load Image!", L"Error", MB_ICONERROR);
        }
    }
}

/*
    - Convert image to grayscale with cvtColor because enery maps are typically computed based on intensity differences (easier to handle if all grey)
    - Sobel filter to compute the gradients in X and Y directions to detect edges and intensity transitions
    - computing magnitude of the gradients to give overall edge intensity at each pixel by combining both X and Y functions
*/
Mat calculateEnergyMap(const Mat& img) 
{
    Mat gray, gradX, gradY, energyMap;
    cvtColor(img, gray, COLOR_BGR2GRAY);        
    Sobel(gray, gradX, CV_64F, 1, 0);
    Sobel(gray, gradY, CV_64F, 0, 1);
    magnitude(gradX, gradY, energyMap);
    return energyMap;
}


/*
    Dynamic Programming Table (dp):
    ----------------------------------
    Stores the minimum cumulative energy at each pixel in the image, starting from the top.
    Each pixel in dp is calculated as the energy of the pixel in the energyMap plus the minimum cumulative energy from the row above.

    Backtrack Matrix (backtrack):
    ----------------------------------
    Records the path of the minimum energy seam.
    Allows tracing the seam back from the last row to the first.

    Seam Construction:
    ----------------------------------
    Start from the minimum energy pixel in the last row and follow the backtrack matrix to reconstruct the seam.
*/
std::vector<int> findVerticalSeam(const Mat& energyMap)
{
    int rows = energyMap.rows;
    int cols = energyMap.cols;

    // Dynamic programming table to store cumulative energies
    Mat dp = Mat::zeros(rows, cols, CV_64F);

    // Backtrack matrix to store the path of the minimum seam
    Mat backtrack = Mat::zeros(rows, cols, CV_32S);

    // Initialize the first row of dp with the energy map values
    for (int j = 0; j < cols; j++) {
        dp.at<double>(0, j) = energyMap.at<double>(0, j);
    }

    // Fill the dp table and record backtrack information
    for (int i = 1; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Find the minimum cumulative energy from the previous row
            double minEnergy = dp.at<double>(i - 1, j);
            int offset = 0;

            if (j > 0 && dp.at<double>(i - 1, j - 1) < minEnergy) {
                minEnergy = dp.at<double>(i - 1, j - 1);
                offset = -1;
            }
            if (j < cols - 1 && dp.at<double>(i - 1, j + 1) < minEnergy) {
                minEnergy = dp.at<double>(i - 1, j + 1);
                offset = 1;
            }

            dp.at<double>(i, j) = energyMap.at<double>(i, j) + minEnergy;
            backtrack.at<int>(i, j) = j + offset;
        }
    }

    // Find the minimum energy in the last row
    double minEnergy = std::numeric_limits<double>::max();
    int minIndex = 0;
    for (int j = 0; j < cols; j++) {
        if (dp.at<double>(rows - 1, j) < minEnergy) {
            minEnergy = dp.at<double>(rows - 1, j);
            minIndex = j;
        }
    }

    // Backtrack to find the vertical seam
    std::vector<int> seam(rows);
    seam[rows - 1] = minIndex;
    for (int i = rows - 2; i >= 0; i--) {
        seam[i] = backtrack.at<int>(i + 1, seam[i + 1]);
    }

    return seam;
}


/*
    Iterate Through Each Row:
    ----------------------------------
    For each row, skip the column corresponding to the seam index.

    Copy Remaining Pixels:
    ----------------------------------
    Create a new image with one less column by copying all pixels except the ones in the seam.
*/
Mat removeVerticalSeam(const Mat& img, const std::vector<int>& seam)
{
    int rows = img.rows;
    int cols = img.cols;

    // Create a new image with one less column
    Mat output(rows, cols - 1, img.type());

    for (int i = 0; i < rows; i++) {
        int k = 0; // Index for the output image's column
        for (int j = 0; j < cols; j++) {
            if (j != seam[i]) {
                output.at<Vec3b>(i, k++) = img.at<Vec3b>(i, j);
            }
        }
    }

    return output;
}

void ResizeImage(HWND hwnd) {
    if (img.empty()) {
        MessageBox(hwnd, L"No image loaded!", L"Error", MB_ICONERROR);
        return;
    }

    // Seam carving logic
    Mat energyMap = calculateEnergyMap(img);
    std::vector<int> seam = findVerticalSeam(energyMap);
    resizedImg = removeVerticalSeam(img, seam);

    MessageBox(hwnd, L"Image resized!", L"Info", MB_OK);
}

void SaveImage(HWND hwnd)
{
    if (resizedImg.empty())
    {
        MessageBox(hwnd, L"No resized image to save!", L"Error", MB_ICONERROR);
        return;
    }

    OPENFILENAME ofn = {};
    wchar_t fileName[MAX_PATH] = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Image Files\0*.bmp;*.jpg;*.png\0All Files\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;

    if (GetSaveFileName(&ofn)) {
        std::wstring ws(fileName);
        std::string filePath(ws.begin(), ws.end());

        // Append default file extension if none exists
        if (filePath.find('.') == std::string::npos) {
            filePath += ".png";
        }

        // Save the image
        if (!imwrite(filePath, resizedImg))
        {
            MessageBox(hwnd, L"Failed to save the image!", L"Error", MB_ICONERROR);
        }
        else
        {
            MessageBox(hwnd, L"Image saved!", L"Info", MB_OK);
        }
    }
}


////////////////////////////////////////////// GREEDY ALGORITHM IMPLEMENTATION ////////////////////////////////////////////
std::vector<int> findVerticalSeamGreedy(const Mat& energyMap)
{
    int rows = energyMap.rows, cols = energyMap.cols;
    std::vector<int> seam(rows);

    // Start from the minimum energy pixel in the first row
    seam[0] = std::min_element(energyMap.row(0).begin<uchar>(), energyMap.row(0).end<uchar>()) - energyMap.row(0).begin<uchar>();

    // Select the minimum energy pixel in each subsequent row
    for (int i = 1; i < rows; i++) {
        int prevCol = seam[i - 1];
        int left = max(0, prevCol - 1);
        int right = min(cols - 1, prevCol + 1);

        // Find the column with minimum energy in the range [left, right]
        seam[i] = left + std::min_element(energyMap.ptr<uchar>(i) + left, energyMap.ptr<uchar>(i) + right + 1) - (energyMap.ptr<uchar>(i) + left);
    }

    return seam;
}

// Transpose Image for Horizontal Seam Support

Mat transposeImage(const Mat& image)
{
    Mat transposed;
    transpose(image, transposed);
    return transposed;
}