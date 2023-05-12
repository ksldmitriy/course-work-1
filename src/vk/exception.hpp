#pragma once
#include "../exception.hpp"

using namespace std;

namespace vk {

class CriticalException : public ::CriticalException {
private:
public:
  CriticalException(string message) : ::CriticalException(message){};
};

class MemoryTypeNotFoundException : public IException {
private:
protected:
  string Message() { return "suitable memory type not found"; };

public:
  MemoryTypeNotFoundException() : ::IException(){};
};

class AcquireNextImageFailedException : public IException {
private:
protected:
  string Message() { return "cant acquire next image"; };

public:
  AcquireNextImageFailedException() : IException(){};
};

class PresentFailedException: public IException {
private:
protected:
  string Message() { return "cant present image"; };

public:
  PresentFailedException() : IException(){};
};

class QueueFamilyNotFoundException : public IException {
private:
protected:
  string Message() { return "suitable queue family not found"; };

public:
  QueueFamilyNotFoundException() : IException(){};
};

} // namespace vk
