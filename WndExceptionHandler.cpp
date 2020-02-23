#include "WndExceptionHandler.h"
#include <sstream>
WndExceptionHandler::WndExceptionHandler(int line, const char* file)noexcept
	:
	m_file(file),
	m_line(line)
{
}

const char* WndExceptionHandler::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	m_whatBuffer = oss.str();

	return m_whatBuffer.c_str();
}

const char* WndExceptionHandler::GetType() const noexcept
{
	return "WndExceptionHndler";
}


const std::string& WndExceptionHandler::GetFile() const noexcept
{
	return m_file;
}

std::string WndExceptionHandler::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File]" << m_file << std::endl
		<< "[Line]" << m_line;

	return oss.str();
}