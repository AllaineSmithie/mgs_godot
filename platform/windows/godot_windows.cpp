/**************************************************************************/
/*  godot_windows.cpp                                                     */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "os_windows.h"

#include "main/main.h"

#include <locale.h>
#include <stdio.h>

// For export templates, add a section; the exporter will patch it to enclose
// the data appended to the executable (bundled PCK)
#ifndef TOOLS_ENABLED
#if defined _MSC_VER
#pragma section("pck", read)
__declspec(allocate("pck")) static char dummy[8] = { 0 };

// Dummy function to prevent LTO from discarding "pck" section.
extern "C" char *__cdecl pck_section_dummy_call() {
	return &dummy[0];
};
#if defined _AMD64_
#pragma comment(linker, "/include:pck_section_dummy_call")
#elif defined _X86_
#pragma comment(linker, "/include:_pck_section_dummy_call")
#endif

#elif defined __GNUC__
static const char dummy[8] __attribute__((section("pck"), used)) = { 0 };
#endif
#endif

PCHAR *
CommandLineToArgvA(
		PCHAR CmdLine,
		int *_argc) {
	PCHAR *argv;
	PCHAR _argv;
	ULONG len;
	ULONG argc;
	CHAR a;
	ULONG i, j;

	BOOLEAN in_QM;
	BOOLEAN in_TEXT;
	BOOLEAN in_SPACE;

	len = strlen(CmdLine);
	i = ((len + 2) / 2) * sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR *)GlobalAlloc(GMEM_FIXED,
			i + (len + 2) * sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv) + i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	a = CmdLine[i];
	while (a) {
		if (in_QM) {
			if (a == '\"') {
				in_QM = FALSE;
			} else {
				_argv[j] = a;
				j++;
			}
		} else {
			switch (a) {
				case '\"':
					in_QM = TRUE;
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					in_SPACE = FALSE;
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					if (in_TEXT) {
						_argv[j] = '\0';
						j++;
					}
					in_TEXT = FALSE;
					in_SPACE = TRUE;
					break;
				default:
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					_argv[j] = a;
					j++;
					in_SPACE = FALSE;
					break;
			}
		}
		i++;
		a = CmdLine[i];
	}
	_argv[j] = '\0';
	argv[argc] = nullptr;

	(*_argc) = argc;
	return argv;
}

char *wc_to_utf8(const wchar_t *wc) {
	int ulen = WideCharToMultiByte(CP_UTF8, 0, wc, -1, nullptr, 0, nullptr, nullptr);
	char *ubuf = new char[ulen + 1];
	WideCharToMultiByte(CP_UTF8, 0, wc, -1, ubuf, ulen, nullptr, nullptr);
	ubuf[ulen] = 0;
	return ubuf;
}


int utf8_char_main(int argc, char **argv_utf8) {
	OS_Windows os(nullptr);

	setlocale(LC_CTYPE, "");

	TEST_MAIN_PARAM_OVERRIDE(argc, argv_utf8)

	Error err = Main::setup(argv_utf8[0], argc - 1, &argv_utf8[1]);

	if (err != OK) {
		if (err == ERR_HELP) { // Returned by --help and --version, so success.
			return 0;
		}
		return 255;
	}

	if (Main::start()) {
		os.run();
	}
	Main::cleanup();

	return os.get_exit_code();
}

int widechar_main(int argc, wchar_t **argv) {
	char **argv_utf8 = new char *[argc];

	for (int i = 0; i < argc; ++i) {
		argv_utf8[i] = wc_to_utf8(argv[i]);
	}
	int returnCode = utf8_char_main(argc, argv_utf8);

	for (int i = 0; i < argc; ++i) {
		delete[] argv_utf8[i];
	}
	delete[] argv_utf8;

	return returnCode;
}

/*int widechar_main(int argc, wchar_t **argv) {
	OS_Windows os(nullptr);

	setlocale(LC_CTYPE, "");

	char **argv_utf8 = new char *[argc];

	for (int i = 0; i < argc; ++i) {
		argv_utf8[i] = wc_to_utf8(argv[i]);
	}

	TEST_MAIN_PARAM_OVERRIDE(argc, argv_utf8)

	Error err = Main::setup(argv_utf8[0], argc - 1, &argv_utf8[1]);

	if (err != OK) {
		for (int i = 0; i < argc; ++i) {
			delete[] argv_utf8[i];
		}
		delete[] argv_utf8;

		if (err == ERR_HELP) { // Returned by --help and --version, so success.
			return 0;
		}
		return 255;
	}

	if (Main::start()) {
		os.run();
	}
	Main::cleanup();

	for (int i = 0; i < argc; ++i) {
		delete[] argv_utf8[i];
	}
	delete[] argv_utf8;

	return os.get_exit_code();
}*/

int _main() {
	LPWSTR *wc_argv;
	int argc;
	int result;

	wc_argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (nullptr == wc_argv) {
		wprintf(L"CommandLineToArgvW failed\n");
		return 0;
	}

	result = widechar_main(argc, wc_argv);

	LocalFree(wc_argv);
	return result;
}

#if defined(LIBRARY_ENABLED)
 #ifdef MGS_VST_BUILD
	#undef DEVELOPMENT
	#define DEVELOPMENT DEBUG_ENABLED  // This avoids a Clang warning in Steinberg code about unused values
  #include "core/libgodot/plugins/vst3/vstentry.cpp"

	//==============================================================================
	#undef ASSERT
	#undef WARNING
	#undef PRINTSYSERROR
	#undef DEBUGSTR
	#undef DBPRT0
	#undef DBPRT1
	#undef DBPRT2
	#undef DBPRT3
	#undef DBPRT4
	#undef DBPRT5
	#undef min
	#undef max
	#undef MIN
	#undef MAX
	#undef calloc
	#undef free
	#undef malloc
	#undef realloc
	#undef NEW
	#undef NEWVEC
	#undef VERIFY
	#undef VERIFY_IS
	#undef VERIFY_NOT
	#undef META_CREATE_FUNC
	#undef CLASS_CREATE_FUNC
	#undef SINGLE_CREATE_FUNC
	#undef _META_CLASS
	#undef _META_CLASS_IFACE
	#undef _META_CLASS_SINGLE
	#undef META_CLASS
	#undef META_CLASS_IFACE
	#undef META_CLASS_SINGLE
	#undef SINGLETON
	#undef OBJ_METHODS
	#undef QUERY_INTERFACE
	#undef LICENCE_UID
	#undef BEGIN_FACTORY
	#undef DEF_CLASS
	#undef DEF_CLASS1
	#undef DEF_CLASS2
	#undef DEF_CLASS_W
	#undef END_FACTORY

	#ifdef atomic_thread_fence
	 #undef atomic_thread_fence
	#endif

  #if WINDOWS_ENABLED
	/*
	 * Disables missing prototype warnings as function definition
	 * will be present in BTF.
	 */
   #if __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmissing-prototypes"
   #endif


/*static void* current_handle = nullptr;

extern "C" BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		current_handle = instance;
		char* args[1];
		char buffer[MAX_PATH];
		args[0] = buffer;
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::string::size_type pos = std::string(buffer).find_last_of("\\/");
		godot_main_setup(1, args, instance);
	}
	return true;
}*/

   #if __GNUC__
 #pragma GCC diagnostic pop 
   #endif

   #define MGS_API 
  #endif // WINDOWS_ENABLED
 #else
  #define MGS_API extern "C" LIBGODOT_API
 #endif // MGS_VST_BUILD
#include "core/libgodot/libgodot.h"
MGS_API int godot_main(int argc, char* argv[]) {
//#ifndef MGS_VST_BUILD
	return utf8_char_main(argc, argv);
//#else
//#endif
}
OS_Windows* os = nullptr;
MGS_API int godot_main_setup(int argc, char* argv_utf8[], HINSTANCE handle) {
	os = memnew(OS_Windows(handle));

	setlocale(LC_CTYPE, "");

	TEST_MAIN_PARAM_OVERRIDE(argc, argv_utf8)

	Error err = Main::setup(argv_utf8[0], argc - 1, &argv_utf8[1]);

	Main::start();
	return 0;
}

MGS_API void godot_main_run_pre(int argc, char* argv[], HINSTANCE handle) {

	if (os->get_main_loop())
		return;

	os->get_main_loop()->initialize();
}

MGS_API void godot_main_run_iteration(int argc, char* argv[], HINSTANCE handle) {
	DisplayServer::get_singleton()->process_events(); // get rid of pending events
	Main::iteration();
}

MGS_API int godot_main_finalize(int argc, char* argv[], HINSTANCE handle) {
	os->get_main_loop()->finalize();
	Main::cleanup();
	int result = os->get_exit_code();
	memdelete(os);
	return result;
}

#else

int main(int argc, char **argv) {
	// override the arguments for the test handler / if symbol is provided
	// TEST_MAIN_OVERRIDE

	// _argc and _argv are ignored
	// we are going to use the WideChar version of them instead
#ifdef CRASH_HANDLER_EXCEPTION
	__try {
		return _main();
	} __except (CrashHandlerException(GetExceptionInformation())) {
		return 1;
	}
#else
	return _main();
#endif
}

HINSTANCE godot_hinstance = nullptr;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	godot_hinstance = hInstance;
	return main(0, nullptr);
}
#endif
