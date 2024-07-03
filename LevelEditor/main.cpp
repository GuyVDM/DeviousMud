#pragma once
#include "precomp.h"

#include "Core/Core.hpp"

#include "Editor/Editor.h"

int main(int argc, char** argv)
{
	Editor editor = Editor();

	editor.CreateEditorWindow();
	editor.Start();

	return 0;
}