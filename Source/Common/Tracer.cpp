#include "stdafx.h"
#include "Tracer.h"

PrintThread::~PrintThread()
{
	std::lock_guard<std::mutex> guard(cout_mtx);
	std::cout << this->str();
}

std::mutex PrintThread::cout_mtx{};