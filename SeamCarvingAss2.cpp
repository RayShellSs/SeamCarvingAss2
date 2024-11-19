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
#include <Windows.h>

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

// Buttons
HWND hButtonLoad, hButtonResize, hButtonSave;

// RadioButtons
HWND hRadioDP, hRadioGreedy;

// Labels
HWND hWidth, hHeight, hFilePath, hIntro;

// Input Boxes
HWND hEditVertSeams, hEditHorizSeams;

Mat img, resizedImg;
HWND hWndImage;

int selectedAlgorithm = 0;  // 0 for DP, 1 for greedy

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

    // Introduction
    hIntro = CreateWindow(L"STATIC", L"Hello! To Reduce Seams, please include a - sign infront of your values!", WS_CHILD | WS_VISIBLE,
        10, 10, 480, 20, hWnd, NULL, hInstance, NULL);

    // Width
    hWidth = CreateWindow(L"STATIC", L"Width (px):", WS_CHILD | WS_VISIBLE,
        10, 40, 80, 20, hWnd, NULL, hInstance, NULL);

    hEditVertSeams = CreateWindow(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        100, 40, 50, 20, hWnd, (HMENU)4, hInstance, NULL);

    // Height
    hHeight = CreateWindow(L"STATIC", L"Height (px):", WS_CHILD | WS_VISIBLE,
        10, 80, 80, 20, hWnd, NULL, hInstance, NULL);

    hEditHorizSeams = CreateWindow(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        100, 80, 50, 20, hWnd, (HMENU)5, hInstance, NULL);
    
    hRadioDP = CreateWindow(L"BUTTON", L"Dynamic Programming", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
        10, 120, 180, 30, hWnd, (HMENU)1001, hInstance, NULL);

    hRadioGreedy = CreateWindow(L"BUTTON", L"Greedy Algorithm", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
        200, 120, 150, 30, hWnd, (HMENU)1002, hInstance, NULL);

    // Load Image
    hButtonLoad = CreateWindow(L"BUTTON", L"Load Image", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 160, 120, 30, hWnd, (HMENU)1, hInstance, NULL);

    // Create a static text control beside the "Load Image" button to display the file path
    hFilePath = CreateWindow(L"STATIC", L"File Path:", WS_CHILD | WS_VISIBLE | SS_LEFT,
        150, 160, 500, 30, hWnd, NULL, hInstance, NULL);

    hButtonResize = CreateWindow(L"BUTTON", L"Resize Image", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 200, 120, 30, hWnd, (HMENU)2, hInstance, NULL);

    hButtonSave = CreateWindow(L"BUTTON", L"Save Image", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 240, 120, 30, hWnd, (HMENU)3, hInstance, NULL);

    // Set the default selection for the radio button group
    SendMessage(hRadioDP, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);

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

HBITMAP MatToHBITMAP(const Mat& mat)
{
    if (mat.empty())
        return nullptr;

    Mat temp;
    cvtColor(mat, temp, COLOR_BGR2BGRA); // Convert OpenCV's BGR to BGRA for Windows

    HBITMAP hBitmap = CreateBitmap(temp.cols, temp.rows, 1, 32, temp.data);
    return hBitmap;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int n{}, m{};

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case 1001: // DP RadioBtn
        {
            if (HIWORD(wParam) == BN_CLICKED) {
                selectedAlgorithm = 0;  
            }
            break;
        }            
        case 1002: // Greedy RadioBtn
        {
            if (HIWORD(wParam) == BN_CLICKED) {
                selectedAlgorithm = 1;  
            }
            break;
        }           
        case 1: // Load Image
        {
            OpenImage(hWnd);
            break;
        }
        case 2: // Resize Image
        {
            // Retrieve vertical and horizontal seams input before calling ResizeImage
            wchar_t buffer[256];

            // Get vertical seams input (n)
            GetWindowText(hEditVertSeams, buffer, sizeof(buffer) / sizeof(wchar_t));
            n = _wtoi(buffer);  // Convert wide string to integer (use _wtoi for wide char to int)

            // Get horizontal seams input (m)
            GetWindowText(hEditHorizSeams, buffer, sizeof(buffer) / sizeof(wchar_t));
            m = _wtoi(buffer);  // Convert wide string to integer (use _wtoi for wide char to int)

            // Call ResizeImage with the parsed values for n and m
            ResizeImage(hWnd, n, m);
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

        // Choose which image to display: resizedImg (if not empty) or the original img
        Mat displayImg = resizedImg.empty() ? img : resizedImg;

        if (!displayImg.empty()) {
            // Convert Mat to HBITMAP
            HBITMAP hBitmap = MatToHBITMAP(displayImg);
            if (hBitmap) {
                HDC hMemDC = CreateCompatibleDC(hdc);
                SelectObject(hMemDC, hBitmap);

                // Draw the image on the window
                BITMAP bmp;
                GetObject(hBitmap, sizeof(BITMAP), &bmp);
                StretchBlt(hdc, 150, 10, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

                // Cleanup
                DeleteDC(hMemDC);
                DeleteObject(hBitmap);
            }
        }

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
        //if (!img.empty()) 
        //{
        //    // Update the file path text control with the selected file path
        //    SetWindowText(hFilePath, ofn.lpstrFile);

        //    MessageBox(hwnd, L"Image Loaded!", L"Info", MB_OK);
        //}
        //else {
        //    MessageBox(hwnd, L"Failed to Load Image!", L"Error", MB_ICONERROR);
        //}
        if (!img.empty())
        {
            // Update the file path text control with the selected file path
            SetWindowText(hFilePath, ofn.lpstrFile);

            MessageBox(hwnd, L"Image Loaded!", L"Info", MB_OK);

            // Trigger repaint to display the image
            InvalidateRect(hwnd, nullptr, TRUE);
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

/*
    - For each row in img, shift pixels to right starting from seam position
    - Insert the new pixel in the seam's place
*/
Mat insertVerticalSeam(const Mat& img, const std::vector<int>& seam)
{
    int rows = img.rows;
    int cols = img.cols;

    // Create a new image with one more column
    Mat output(rows, cols + 1, img.type());

    // For each row, insert the seam
    for (int i = 0; i < rows; i++) 
    {
        int seamPos = seam[i];
        for (int j = 0; j < cols; j++) 
        {
            if (j < seamPos) {
                // Copy the pixel from the original image to the new image (before the seam)
                output.at<Vec3b>(i, j) = img.at<Vec3b>(i, j);
            }
            else if (j == seamPos) {
                // Insert a new pixel (take the average or some other method to blend)
                output.at<Vec3b>(i, j) = img.at<Vec3b>(i, j); // You can choose to take an average, or simply copy one of the adjacent pixels
            }
            else {
                // Shift the pixels to the right (after the seam)
                output.at<Vec3b>(i, j) = img.at<Vec3b>(i, j - 1);
            }
        }
    }

    return output;
}

/*
    - Using Vertical Seam, transpose the image so that the rows become columns
*/
std::vector<int> findHorizontalSeam(const Mat& energyMap)
{
    // Transpose the energy map to make the rows become columns
    Mat transposedEnergyMap;
    transpose(energyMap, transposedEnergyMap);

    // Use the vertical seam function on the transposed image
    std::vector<int> verticalSeam = findVerticalSeam(transposedEnergyMap);

    // The vertical seam in the transposed image corresponds to the horizontal seam in the original image
    return verticalSeam;
}

/*
     Iterate Through Each Column:
    ----------------------------------
    For each column, skip the row corresponding to the seam index.

    Copy Remaining Pixels:
    ----------------------------------
    Create a new image with one less row by copying all pixels except the ones in the seam.
*/
Mat removeHorizontalSeam(const Mat& img, const std::vector<int>& seam)
{
    int rows = img.rows;
    int cols = img.cols;

    // Create a new image with one less row
    Mat output(rows - 1, cols, img.type());

    for (int j = 0; j < cols; j++) {
        int k = 0; // Index for the output image's row
        for (int i = 0; i < rows; i++) {
            if (i != seam[j]) {
                output.at<Vec3b>(k++, j) = img.at<Vec3b>(i, j);
            }
        }
    }

    return output;
}

/*
    - For each col in img, shift pixels downwards starting from seam position
    - Insert new pixel in seam's place
*/
Mat insertHorizontalSeam(const Mat& img, const std::vector<int>& seam)
{
    int rows = img.rows;
    int cols = img.cols;

    // Create a new image with one more row
    Mat output(rows + 1, cols, img.type());

    // For each column, insert the seam
    for (int j = 0; j < cols; j++) 
    {
        int seamPos = seam[j];
        for (int i = 0; i < rows; i++) 
        {
            if (i < seamPos) 
            {
                // Copy the pixel from the original image to the new image (before the seam)
                output.at<Vec3b>(i, j) = img.at<Vec3b>(i, j);
            }
            else if (i == seamPos) 
            {
                // Insert a new pixel 
                output.at<Vec3b>(i, j) = img.at<Vec3b>(i, j); 
            }
            else 
            {
                // Shift the pixels downward (after the seam)
                output.at<Vec3b>(i, j) = img.at<Vec3b>(i - 1, j);
            }
        }
    }

    return output;
}

void ResizeImage(HWND hwnd, int n, int m)
{
    if (img.empty()) 
    {
        MessageBox(hwnd, L"No image loaded!", L"Error", MB_ICONERROR);
        return;
    }

    // Initialize resizedImg to img first
    resizedImg = img.clone();

    // Dynamic Programming Algo
    if (selectedAlgorithm == 0) 
    {
        // Vertical
        if (n < 0)
        {
            // Reduce Width 
            for (int i = 0; i < -n; i++)
            {
                Mat energyMap = calculateEnergyMap(resizedImg);
                std::vector<int> verticalSeam = findVerticalSeam(energyMap);
                resizedImg = removeVerticalSeam(resizedImg, verticalSeam);
            }
        }
        else if (n > 0)
        {
            // Insert vertical seams
            for (int i = 0; i < n; i++)
            {
                Mat energyMap = calculateEnergyMap(resizedImg);
                std::vector<int> verticalSeam = findVerticalSeam(energyMap);
                resizedImg = insertVerticalSeam(resizedImg, verticalSeam);
            }
        }


        // Horizontal
        if (m < 0)
        {
            for (int i = 0; i < -m; i++)
            {
                // After removing the vertical seam, calculate the energy map again for the updated image
                Mat energyMap = calculateEnergyMap(resizedImg);

                // Remove horizontal seams (reduce height)
                std::vector<int> horizontalSeam = findHorizontalSeam(energyMap);
                resizedImg = removeHorizontalSeam(resizedImg, horizontalSeam);
            }
        }
        else if (m > 0)
        {
            // Insert horizontal seams (increase height)
            for (int i = 0; i < m; i++)
            {
                Mat energyMap = calculateEnergyMap(resizedImg);
                std::vector<int> horizontalSeam = findHorizontalSeam(energyMap);
                resizedImg = insertHorizontalSeam(resizedImg, horizontalSeam);
            }
        }
    }
    else if (selectedAlgorithm == 1) 
    {
        // Greedy Algo

    }

    MessageBox(hwnd, L"Image resized!", L"Info", MB_OK);

    // Trigger repaint to display the resized image
    InvalidateRect(hwnd, nullptr, TRUE);
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


