//
// Created by Akira Furui on 2018/10/25.
//
// Windows環境
#ifdef _WIN32
#define ClearConsole system("cls")
#pragma warning(disable:4996)
// Mac環境
#else
#define ClearConsole system("clear")
#endif
