#pragma once
#include "precomp.h"

#include "Core/Core.h"
#include "Core/Editor/Editor.h"

int main(int argc, char** argv)
{
	Editor editor = Editor();

	editor.CreateEditor();
	editor.Start();

	return 0;
}