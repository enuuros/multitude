@echo off

if not defined BOOST_DIR (
	@echo Please set BOOST_DIR environment variable to point to your Boost installation
	exit 1
)

set PWD=%~dp0
SET PATH=%PATH%;%PWD%lib;%PWD%Win32x\lib;%BOOST_DIR%\lib