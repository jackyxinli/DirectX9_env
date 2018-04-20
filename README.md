## Settings

- Confirmed on Visual Studio 2017.

- Copy SDK sources (not necessarily overwriting) and check include-path and library-path.

- Add "legacy_stdio_definitions.lib" to project_property->linker->input->depending_files.

- Make change project_property->general->encoding into Multi-byte characters.

- Set Project_src/* on workspace.