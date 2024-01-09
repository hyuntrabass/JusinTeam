

xcopy /y/s	.\Engine\Public\*.*			.\Reference\Headers\
xcopy /y	.\Engine\Bin\Engine.lib		.\Reference\Lib\
xcopy /y	.\Engine\ThirdpartyLib\*.lib	.\Reference\Lib\
xcopy /y	.\Engine\Bin\Engine.dll		.\Client\Bin\
xcopy /y	.\Engine\Bin\Engine.dll		.\MapEditor\Bin\
xcopy /y	.\Engine\Bin\ShaderFiles\*.*		.\Client\Bin\ShaderFiles\
xcopy /y	.\Engine\Bin\ShaderFiles\*.*		.\MapEditor\Bin\ShaderFiles\