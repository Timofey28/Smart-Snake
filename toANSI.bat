@echo off

set FILES= src\alert.cc src\alert.h src\algorithm.cc src\algorithm.h src\application.cc src\application.h src\caption.h src\caption.cc src\caption_init.h src\console.h src\console.cc src\draw.cc src\draw.h src\experiment.cc src\experiment.h src\file_handler.cc src\file_handler.h src\game.cc src\game.h src\interface.h src\interface.cc src\main.cc src\mouse_input.cc src\mouse_input.h src\playground.cc src\playground.h src\scrollable_list.cc src\scrollable_list.h src\table.cc src\table.h src\utils.cc src\utils.h src\validation.cc src\validation.h


set counter=1
for %%a in (%FILES%) do (
    call :convert2ansi %%a
)
echo:
echo Press any key to continue
pause > NUL
goto :eof


:convert2ansi <FILE>
set FILE=%~1
echo | set /p dummyName="%counter%) %FILE%... "
set /a counter+=1
IF NOT EXIST %FILE% (
    echo NO SUCH FILE
    goto :eof
)
iconv -f utf-8 -t windows-1251 %FILE% > SomeNameIWouldNeverGiveToAFileInTheProject 2> NUL
IF ERRORLEVEL 1 (
    del SomeNameIWouldNeverGiveToAFileInTheProject
    echo already in ANSI
) ELSE (
    copy SomeNameIWouldNeverGiveToAFileInTheProject %FILE% > NUL
    del SomeNameIWouldNeverGiveToAFileInTheProject
    echo converted to ANSI
)
goto :eof
