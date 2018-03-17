echo on
git clone https://github.com/openssl/openssl.git .
rem Ǩ���ȶ��汾1.0.2j���������Լ����ط�֧b1.0.2j
git checkout OpenSSL_1_0_2j -b b1.0.2j

rem ����x86λ�ο�INSTALL.W32˵���ļ�
rem �����Ҫ���벻ͬ��visualStudio�汾��ÿ��vs�汾Ҫ������һ��cmd�����������������ܵ���һ��vs�汾����������Ӱ�졣
rem ʹ��vs2015 x86���뻷��������������仰����������vs2015��x86����
"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"

rem һ��Ҫע��INSTALL.W32���ĵ�˵����win32 release �汾��Ҫָ��no-asm������ʹ��ms\do_nasm.bat�ű�

rem ����win32/release��̬��lib
nmake -f ms\nt.mak clean
rmdir /S /Q out32
rmdir /S /Q tmp32
perl Configure VC-WIN32 no-asm --prefix=C:/install/openssl-1.0.2j/vs2015/lib/win32/release
ms\do_nasm.bat
nmake -f ms\nt.mak install

rem ����win32/release��̬��dll
nmake -f ms\ntdll.mak clean
rmdir /S /Q out32dll
rmdir /S /Q tmp32dll
perl Configure VC-WIN32 no-asm --prefix=C:/install/openssl-1.0.2j/vs2015/dll/win32/release
ms\do_nasm.bat
nmake -f ms\ntdll.mak install

rem ����win32/debug��̬��lib
nmake -f ms\nt.mak clean
rmdir /S /Q out32.dbg
rmdir /S /Q tmp32.dbg
perl Configure debug-VC-WIN32 no-asm --prefix=C:/install/openssl-1.0.2j/vs2015/lib/win32/debug
ms\do_ms.bat
nmake -f ms\nt.mak install

rem ����win32/debug��̬��dll
nmake -f ms\ntdll.mak clean
rmdir /S /Q out32dll.dbg
rmdir /S /Q tmp32dll.dbg
perl Configure debug-VC-WIN32 no-asm --prefix=C:/install/openssl-1.0.2j/vs2015/dll/win32/debug
ms\do_ms.bat
nmake -f ms\ntdll.mak install

rem ����x64λ�ο�INSTALL.W64˵���ļ�
rem �����Ҫ���벻ͬ��visualStudio�汾��ÿ��vs�汾Ҫ������һ��cmd�����������������ܵ���һ��vs�汾����������Ӱ�졣
rem ʹ��vs2015 x64���뻷�������ʹ������vs�汾��ֻ���޸�������л���
"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat"

rem ����win64/release��̬��lib
nmake -f ms\nt.mak clean
rmdir /S /Q out32
rmdir /S /Q tmp32
perl Configure VC-WIN64A --prefix=C:/install/openssl-1.0.2j/vs2015/lib/win64/release
ms\do_win64a.bat
nmake -f ms\nt.mak install

rem ����win64/release��̬��dll
nmake -f ms\ntdll.mak clean
rmdir /S /Q out32dll
rmdir /S /Q tmp32dll
perl Configure VC-WIN64A --prefix=C:/install/openssl-1.0.2j/vs2015/dll/win64/release
ms\do_win64a.bat
nmake -f ms\ntdll.mak install

rem ����win64/debug��̬��lib
nmake -f ms\nt.mak clean
rmdir /S /Q out32.dbg
rmdir /S /Q tmp32.dbg
perl Configure debug-VC-WIN64A --prefix=C:/install/openssl-1.0.2j/vs2015/lib/win64/debug
ms\do_win64a.bat
nmake -f ms\nt.mak install

rem ����win64/debug��̬��dll
nmake -f ms\ntdll.mak clean
rmdir /S /Q out32dll.dbg
rmdir /S /Q tmp32dll.dbg
perl Configure debug-VC-WIN64A --prefix=C:/install/openssl-1.0.2j/vs2015/dll/win64/debug
ms\do_win64a.bat
nmake -f ms\ntdll.mak install
